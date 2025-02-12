/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thread>

#include "PartitionOutputData.h"
#include "folly/io/IOBuf.h"
#include "gtest/gtest.h"

class PartitionOutputDataTest : public ::testing::Test {};

TEST(PartitionOutputDataTest, basic) {
  io::trino::bridge::PartitionOutputData output;
  for (int i = 0; i < 10; ++i) {
    std::vector<std::unique_ptr<folly::IOBuf>> data;
    data.emplace_back(std::make_unique<folly::IOBuf>());

    ASSERT_EQ(output.getSequence(), i);
    output.enqueue(i, data);
    ASSERT_EQ(output.getSequence(), i + 1);
    ASSERT_EQ(output.getDataSize(i), 0);
  }

  ASSERT_EQ(output.getSequence(), 10);
  ASSERT_EQ(output.getOutputDataNum(), 10);

  auto result = output.pop(10);
  ASSERT_EQ(result.size(), 10);
  for (auto& buf : result) {
    ASSERT_EQ(buf->computeChainDataLength(), 0);
  }
  ASSERT_EQ(output.getOutputDataNum(), 0);

  std::vector<std::unique_ptr<folly::IOBuf>> poison;
  poison.emplace_back(nullptr);
  output.enqueue(10, poison);
  ASSERT_EQ(output.getOutputDataNum(), 0);
}
