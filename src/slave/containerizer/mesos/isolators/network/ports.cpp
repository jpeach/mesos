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

#include "slave/containerizer/mesos/isolators/network/ports.hpp"

#include <process/id.hpp>

#include <stout/numify.hpp>
#include <stout/path.hpp>

#include "common/protobuf_utils.hpp"
#include "common/values.hpp"

using std::list;
using std::string;
using std::vector;

using process::Failure;
using process::Future;
using process::Owned;

using mesos::slave::ContainerConfig;
using mesos::slave::ContainerLaunchInfo;
using mesos::slave::ContainerLimitation;
using mesos::slave::ContainerState;
using mesos::slave::Isolator;

using mesos::internal::values::rangesToIntervalSet;

using namespace routing::diagnosis;

namespace mesos {
namespace internal {
namespace slave {

Try<Isolator*> NetworkPortsIsolatorProcess::create(const Flags& flags)
{
  return new MesosIsolator(process::Owned<MesosIsolatorProcess>(
      new NetworkPortsIsolatorProcess()));
}


NetworkPortsIsolatorProcess::NetworkPortsIsolatorProcess()
  : ProcessBase(process::ID::generate("network-ports-isolator"))
{
}


bool NetworkPortsIsolatorProcess::supportsNesting()
{
  return true;
}


Future<Option<ContainerLaunchInfo>> NetworkPortsIsolatorProcess::prepare(
    const ContainerID& containerId,
    const ContainerConfig& containerConfig)
{
  if (infos.contains(containerId)) {
    return Failure("Container has already been prepared");
  }

  infos.emplace(containerId, Owned<Info>(new Info()));

  return update(containerId, containerConfig.resources())
    .then([]() -> Future<Option<ContainerLaunchInfo>> {
      return None();
    });
}


process::Future<Nothing> NetworkPortsIsolatorProcess::update(
    const ContainerID& containerId,
    const Resources& resources)
{
  if (!infos.contains(containerId)) {
    LOG(INFO) << "Ignoring update for unknown container " << containerId;
    return Nothing();
  }

  Option<Value::Ranges> ports = resources.ports();
  if (ports.isSome()) {
    const Owned<Info>& info = infos.at(containerId);
    info->ports = rangesToIntervalSet<uint16_t>(ports.get()).get();
  }

  return Nothing();
}


process::Future<ContainerLimitation> NetworkPortsIsolatorProcess::watch(
    const ContainerID& containerId)
{
  if (!infos.contains(containerId)) {
    return Failure(
        "Failed to watch ports for unknown container " +
        stringify(containerId));
  }

  return infos.at(containerId)->limitation.future();
}


process::Future<Nothing> NetworkPortsIsolatorProcess::cleanup(
    const ContainerID& containerId)
{
  if (!infos.contains(containerId)) {
    LOG(INFO) << "Ignoring cleanup for unknown container " << containerId;
    return Nothing();
  }

  infos.erase(containerId);

  return Nothing();
}


Future<Nothing> NetworkPortsIsolatorProcess::recover(
    const list<ContainerState>& states,
    const hashset<ContainerID>& orphans)
{
  foreach (const auto& state, states) {
    CHECK(!infos.contains(state.container_id()))
      << "Duplicate ContainerID " << state.container_id();

    infos.emplace(state.container_id(), Owned<Info>(new Info()));
  }

  return Nothing();
}


Try<hashmap<uint32_t, socket::Info>>
NetworkPortsIsolatorProcess::getListeningSockets()
{
  Try<vector<socket::Info>> socketInfos = socket::infos(
      AF_INET,
      socket::state::LISTEN);

  if (socketInfos.isError()) {
    return Error(socketInfos.error());
  }

  hashmap<uint32_t, socket::Info> inodes;

  foreach (const socket::Info& info, socketInfos.get()) {
    // The inode should never be 0. This would only happen if the kernel
    // didn't return the inode in the sockdiag response, which would imply
    // a very old kernel or a problem between the kernel and libnl.
    if (info.inode != 0) {
      inodes.emplace(info.inode, info);
    }
  }

  return inodes;
}


// Extract the inode field from a /proc/$PID/fd entry. The format of
// the socket entry is "socket:[nnnn]" where nnnn is the numberic inode
// number of the socket.
static uint32_t extractSocketInode(const string& sock)
{
  const size_t s = sizeof("socket:[]") - 1;
  const string val = sock.substr(s - 1, sock.size() - s);

  Try<uint32_t> value = numify<uint32_t>(val);
  CHECK_SOME(value);

  return value.get();
}


Try<vector<uint32_t>> NetworkPortsIsolatorProcess::getProcessSockets(pid_t pid)
{
  const string fdPath = path::join("/proc", stringify(pid), "fd");

  DIR* dir = opendir(fdPath.c_str());
  if (dir == nullptr) {
    return ErrnoError("Failed to open directory '" + fdPath + "'");
  }

  errno = 0;

  vector<uint32_t> inodes;
  struct dirent* entry;
  char target[NAME_MAX];

  while ((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    ssize_t nbytes = readlinkat(
        dirfd(dir), entry->d_name, target, sizeof(target) - 1);
    if (nbytes == -1) {
      int saved = errno;
      closedir(dir);
      return ErrnoError(
          saved,
          "Failed to read link '" + path::join(fdPath, entry->d_name) + "'");
    }

    target[nbytes] = '\0';

    if (strings::startsWith(target, "socket:[")) {
      inodes.push_back(extractSocketInode(target));
    }
  }

  closedir(dir);
  return inodes;
}

} // namespace slave {
} // namespace internal {
} // namespace mesos {
