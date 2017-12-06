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

#include "slave/containerizer/mesos/isolators/namespaces/user.hpp"

#include <string>
#include <vector>

#include <process/id.hpp>

#include "linux/ns.hpp"

#include "slave/flags.hpp"

using std::string;
using std::vector;

using process::Future;

using mesos::slave::ContainerConfig;
using mesos::slave::ContainerLaunchInfo;
using mesos::slave::Isolator;

namespace mesos {
namespace internal {
namespace slave {

Try<Isolator*> NamespacesUserIsolatorProcess::create(const Flags& flags)
{
  // Check for root permission.
  if (geteuid() != 0) {
    return Error("The user namespace isolator requires root permissions");
  }

  // Verify that user namespaces are available on this kernel.
  Try<bool> usernsSupported = ns::supported(CLONE_NEWUSER);
  if (usernsSupported.isError() || !usernsSupported.get()) {
    return Error("user namespaces are not supported by this kernel");
  }

  // Make sure the 'linux' launcher is used because only 'linux' launcher
  // supports cloning namespaces for the container.
  if (flags.launcher != "linux") {
    return Error(
        "The 'linux' launcher must be used to enable user namespaces");
  }

  return new MesosIsolator(process::Owned<MesosIsolatorProcess>(
      new NamespacesUserIsolatorProcess()));
}


NamespacesUserIsolatorProcess::NamespacesUserIsolatorProcess()
  : ProcessBase(process::ID::generate("user-namespace-isolator")) {}


bool NamespacesUserIsolatorProcess::supportsNesting()
{
  return true;
}


Future<Option<ContainerLaunchInfo>> NamespacesUserIsolatorProcess::prepare(
    const ContainerID& containerId,
    const ContainerConfig& containerConfig)
{
  ContainerLaunchInfo launchInfo;

  if (containerId.has_parent()) {
    launchInfo.add_enter_namespaces(CLONE_NEWUSER);
  } else {
    launchInfo.add_clone_namespaces(CLONE_NEWUSER);
  }

  return launchInfo;
}

} // namespace slave {
} // namespace internal {
} // namespace mesos {
