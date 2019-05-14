// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>
#include <vector>

#include <mesos/mesos.hpp>

#include <process/owned.hpp>
#include <process/gtest.hpp>

#include <stout/gtest.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>

#include "common/parse.hpp"

#include "linux/ns.hpp"

#include "slave/containerizer/mesos/containerizer.hpp"

#include "tests/cluster.hpp"
#include "tests/mesos.hpp"

#include "tests/containerizer/docker_archive.hpp"

using process::Future;
using process::Owned;

using std::string;
using std::vector;

using testing::Values;
using testing::WithParamInterface;

using mesos::master::detector::MasterDetector;

namespace mesos {
namespace internal {
namespace tests {

class UserNamespaceIsolatorTest
  : public MesosTest,
    public WithParamInterface<ParamExecutorType>
{
public:
  slave::Flags CreateSlaveFlags() override
  {
      slave::Flags flags = MesosTest::CreateSlaveFlags();

#ifndef USE_SSL_SOCKET
  // Disable operator API authentication for the default executor. Executor
  // authentication currently has SSL as a dependency, so we cannot require
  // executors to authenticate with the agent operator API if Mesos was not
  // built with SSL support.
  flags.authenticate_http_readwrite = false;
#endif // USE_SSL_SOCKET

  flags.isolation = "namespaces/user";

  uid_t uid = os::getuid(os::getenv("SUDO_USER")).get();

  flags.userns_id_mapping = flags::parse<IDMapInfo>(
        strings::format(
    R"~(
    {
      "user_mapping": [
        {
          "host": %d,
          "container": %d,
          "length": 1
        }
      ],

      "group_mapping": [
        {
          "host": %d,
          "container": %d,
          "length": 1
        }
      ]
    }
    )~", uid, uid, uid, uid).get()).get();

    return flags;
  }

  slave::Flags CreateDockerSlaveFlags(const string& dockerRegistry)
  {
    slave::Flags flags = CreateSlaveFlags();

    // For now, enabling user namespaces also requires enabling PID
    // namespaces. This ensures that the PID namespace is created as
    // a child of the user namespace. The child relationship is needed
    // so that we can hold CAP_SYS_ADMIN over the PID namespace in
    // order to mount procfs.
    flags.isolation += ",namespaces/pid";

    flags.isolation += ",docker/runtime,filesystem/linux";
    flags.image_providers = "docker";
    flags.docker_registry = dockerRegistry;
    flags.docker_store_dir = path::join(os::getcwd(), "store");

    return flags;
  }

  Try<Nothing> acceptOffer(
      MesosSchedulerDriver& driver,
      const ParamExecutorType& executorType,
      const FrameworkID& frameworkId,
      const OfferID& offerId,
      const TaskInfo& task)
  {
    if (executorType.isCommandExecutor()) {
      driver.acceptOffers(
          {offerId},
          {LAUNCH({task})});
      return Nothing();
    }

    if (executorType.isDefaultExecutor()) {
      ExecutorInfo executor;
      executor.mutable_executor_id()->set_value("default");
      executor.set_type(ExecutorInfo::DEFAULT);
      executor.mutable_framework_id()->CopyFrom(frameworkId);
      executor.mutable_resources()->CopyFrom(task.resources());

      TaskGroupInfo taskGroup;
      taskGroup.add_tasks()->CopyFrom(task);

      driver.acceptOffers(
          {offerId},
          {LAUNCH_GROUP(executor, taskGroup)});
      return Nothing();
    }

    return Error("Unexpected executor type");
  }

  Try<string> checkCommand()
  {
    Result<ino_t> agentUserNamespace = ns::getns(::getpid(), "user");

    if (agentUserNamespace.isError()) {
      return Error(agentUserNamespace.error());
    }

    if (agentUserNamespace.isNone()) {
      return Error("Unable to determine agent user namespace");
    }

    return strings::format(R"~(
        #!/bin/sh

        echo user:[%lld] > agent.userns
        readlink /proc/self/ns/user > task.userns

        echo Agent is in user namespace `cat agent.userns`
        echo Task is in user namespace `cat task.userns`

        if [ "x`cat agent.userns`" = "x`cat task.userns`" ]; then
          exit 1
        fi
        )~",
        static_cast<long long>(agentUserNamespace.get()));
    }
};


INSTANTIATE_TEST_CASE_P(
    ExecutorType,
    UserNamespaceIsolatorTest,
    Values(
        ParamExecutorType::commandExecutor(),
        ParamExecutorType::defaultExecutor()),
    ParamExecutorType::Printer());


TEST_P(UserNamespaceIsolatorTest, ROOT_USERNS_BasicTask)
{
  Try<Owned<cluster::Master>> master = StartMaster();
  ASSERT_SOME(master);

  Owned<MasterDetector> detector = master.get()->createDetector();

  Try<Owned<cluster::Slave>> slave =
    StartSlave(detector.get(), CreateSlaveFlags());
  ASSERT_SOME(slave);

  MockScheduler sched;

  MesosSchedulerDriver driver(
      &sched,
      DEFAULT_FRAMEWORK_INFO,
      master.get()->pid,
      DEFAULT_CREDENTIAL);

  Future<FrameworkID> frameworkId;
  EXPECT_CALL(sched, registered(&driver, _, _))
    .WillOnce(FutureArg<1>(&frameworkId));

  Future<vector<Offer>> offers;
  EXPECT_CALL(sched, resourceOffers(&driver, _))
    .WillOnce(FutureArg<1>(&offers))
    .WillRepeatedly(Return()); // Ignore subsequent offers.

  driver.start();

  AWAIT_READY(offers);
  ASSERT_FALSE(offers->empty());

  const Offer& offer = offers.get()[0];

  Try<string> command = checkCommand();
  ASSERT_SOME(command);

  const Resources resources =
    Resources::parse("cpus:0.1;mem:32;disk:32").get();

  const TaskInfo task = createTask(offer.slave_id(), resources, command.get());

  ASSERT_SOME(
      acceptOffer(driver, GetParam(), frameworkId.get(), offer.id(), task));

  Future<TaskStatus> statusStarting;
  Future<TaskStatus> statusRunning;
  Future<TaskStatus> statusFinished;

  EXPECT_CALL(sched, statusUpdate(&driver, _))
    .WillOnce(FutureArg<1>(&statusStarting))
    .WillOnce(FutureArg<1>(&statusRunning))
    .WillOnce(FutureArg<1>(&statusFinished));

  AWAIT_READY(statusStarting);
  EXPECT_EQ(TASK_STARTING, statusStarting->state());

  AWAIT_READY(statusRunning);
  EXPECT_EQ(TASK_RUNNING, statusRunning->state());

  AWAIT_READY(statusFinished);
  EXPECT_EQ(TASK_FINISHED, statusFinished->state());

  driver.stop();
  driver.join();
}


TEST_P(UserNamespaceIsolatorTest, ROOT_USERNS_DockerTask)
{
  Try<Owned<cluster::Master>> master = StartMaster();
  ASSERT_SOME(master);

  const string dockerRegistry = path::join(os::getcwd(), "registry");

  Future<Nothing> testImage = DockerArchive::create(dockerRegistry, "alpine");
  AWAIT_READY(testImage);

  ASSERT_TRUE(os::exists(path::join(dockerRegistry, "alpine.tar")));

  Owned<MasterDetector> detector = master.get()->createDetector();

  Try<Owned<cluster::Slave>> slave =
    StartSlave(detector.get(), CreateDockerSlaveFlags(dockerRegistry));
  ASSERT_SOME(slave);

  Option<string> user = os::getenv("SUDO_USER");
  ASSERT_SOME(user);

  FrameworkInfo frameworkInfo = DEFAULT_FRAMEWORK_INFO;
  frameworkInfo.set_user(user.get());

  MockScheduler sched;

  MesosSchedulerDriver driver(
      &sched,
      frameworkInfo,
      master.get()->pid,
      DEFAULT_CREDENTIAL);

  Future<FrameworkID> frameworkId;
  EXPECT_CALL(sched, registered(&driver, _, _))
    .WillOnce(FutureArg<1>(&frameworkId));

  Future<vector<Offer>> offers;
  EXPECT_CALL(sched, resourceOffers(&driver, _))
    .WillOnce(FutureArg<1>(&offers))
    .WillRepeatedly(Return()); // Ignore subsequent offers.

  driver.start();

  AWAIT_READY(offers);
  ASSERT_FALSE(offers->empty());

  const Offer& offer = offers.get()[0];

  Result<ino_t> agentUserNamespace = ns::getns(::getpid(), "user");
  ASSERT_SOME(agentUserNamespace);

  Try<string> command = checkCommand();

  const Resources resources =
    Resources::parse("cpus:0.1;mem:32;disk:32").get();

  CommandInfo commandInfo = createCommandInfo(command.get());
  commandInfo.set_user(user.get());

  LOG(INFO) << "task user is " << user.get();
  TaskInfo task = createTask(offer.slave_id(), resources, commandInfo);

  Image image;
  image.set_type(Image::DOCKER);
  image.mutable_docker()->set_name("alpine");

  ContainerInfo* container = task.mutable_container();
  container->set_type(ContainerInfo::MESOS);
  container->mutable_mesos()->mutable_image()->CopyFrom(image);

  ASSERT_SOME(
      acceptOffer(driver, GetParam(), frameworkId.get(), offer.id(), task));

  Future<TaskStatus> statusStarting;
  Future<TaskStatus> statusRunning;
  Future<TaskStatus> statusFinished;

  EXPECT_CALL(sched, statusUpdate(&driver, _))
    .WillOnce(FutureArg<1>(&statusStarting))
    .WillOnce(FutureArg<1>(&statusRunning))
    .WillOnce(FutureArg<1>(&statusFinished));

  AWAIT_READY(statusStarting);
  EXPECT_EQ(TASK_STARTING, statusStarting->state());

  AWAIT_READY(statusRunning);
  EXPECT_EQ(TASK_RUNNING, statusRunning->state());

  AWAIT_READY(statusFinished);
  EXPECT_EQ(TASK_FINISHED, statusFinished->state());

  driver.stop();
  driver.join();
}

} // namespace tests {
} // namespace internal {
} // namespace mesos {
