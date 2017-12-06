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
    return flags;
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

  slave::Flags flags = CreateSlaveFlags();
  Owned<MasterDetector> detector = master.get()->createDetector();

  Try<Owned<cluster::Slave>> slave = StartSlave(detector.get(), flags);
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

  Result<ino_t> agentUserNamespace = ns::getns(::getpid(), "user");
  ASSERT_SOME(agentUserNamespace);

  const string checkCommand =
    strings::format(R"~(
        #!/bin/sh

        echo user:[%lld] > agent.userns
        readlink /proc/self/ns/user > task.userns

        echo Agent is in user namespace `cat agent.userns`
        echo Task is in user namespace `cat task.userns`

        if [ "x`cat agent.userns`" = "x`cat task.userns`" ]; then
          exit 1
        fi
        )~",
        (long long)agentUserNamespace.get()).get();

  const Resources resources =
    Resources::parse("cpus:0.1;mem:32;disk:32").get();

  const TaskInfo task = createTask(offer.slave_id(), resources, checkCommand);

  if (GetParam().isCommandExecutor()) {
    driver.acceptOffers(
        {offer.id()},
        {LAUNCH({task})});
  } else if (GetParam().isDefaultExecutor()) {
    ExecutorInfo executor;
    executor.mutable_executor_id()->set_value("default");
    executor.set_type(ExecutorInfo::DEFAULT);
    executor.mutable_framework_id()->CopyFrom(frameworkId.get());
    executor.mutable_resources()->CopyFrom(resources);

    TaskGroupInfo taskGroup;
    taskGroup.add_tasks()->CopyFrom(task);

    driver.acceptOffers(
        {offer.id()},
        {LAUNCH_GROUP(executor, taskGroup)});
  } else {
    FAIL() << "Unexpected executor type";
  }

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
