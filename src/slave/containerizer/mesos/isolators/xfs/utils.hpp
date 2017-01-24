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

#ifndef __XFS_UTILS_HPP__
#define __XFS_UTILS_HPP__

#include <string>

#include <stout/bytes.hpp>
#include <stout/interval.hpp>
#include <stout/nothing.hpp>
#include <stout/try.hpp>

#include <xfs/xfs_types.h>

namespace mesos {
namespace internal {
namespace xfs {

struct QuotaInfo
{
  Bytes limit;
  Bytes used;
};


// Quota operations are defined in terms of basic blocks (512 byte units).
class BasicBlocks
{
public:
  // Convert from Bytes to basic blocks. Note that we round up since a partial
  // block costs a full block to store on disk.
  explicit BasicBlocks(const Bytes& b)
    : count((b.bytes() + block_size() - 1) / block_size()) {}

  explicit BasicBlocks(uint64_t c) : count(c) {}

  operator Bytes() const { return bytes(); }
  bool operator==(const BasicBlocks& that) const { return count == that.count; }
  bool operator!=(const BasicBlocks& that) const { return count != that.count; }

  uint64_t blocks() const { return count; }
  Bytes bytes() const { return Bytes(block_size()) * count; }

  static constexpr uint64_t block_size() { return 512u;}

private:
  uint64_t count;
};


enum QuotaPolicy {
  QUOTA_POLICY_ENFORCING,
  QUOTA_POLICY_ACCOUNTING,
};


inline bool operator==(const QuotaInfo& left, const QuotaInfo& right)
{
  return left.limit == right.limit && left.used == right.used;
}


Option<Error> validateProjectIds(const IntervalSet<prid_t>& projectRange);


bool isPathXfs(const std::string& path);


// Test whether XFS project quotas are enabled on the filesystem at the
// given path. This does not imply that quotas are being enforced, just
// that they are enabled.
Try<bool> isQuotaEnabled(const std::string& path);


Result<QuotaInfo> getProjectQuota(
    const std::string& path,
    prid_t projectId,
    QuotaPolicy quotaPolicy);


Try<Nothing> setProjectQuota(
    const std::string& path,
    prid_t projectId,
    QuotaPolicy quotaPolicy,
    Bytes limit);


Try<Nothing> clearProjectQuota(
    const std::string& path,
    prid_t projectId);


Result<prid_t> getProjectId(
    const std::string& directory);


Try<Nothing> setProjectId(
    const std::string& directory,
    prid_t projectId);


Try<Nothing> clearProjectId(
    const std::string& directory);

} // namespace xfs {
} // namespace internal {
} // namespace mesos {

#endif // __XFS_UTILS_HPP__
