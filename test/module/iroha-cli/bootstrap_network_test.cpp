/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../../iroha-cli/bootstrap_network.hpp"
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <gtest/gtest.h>
#include <fstream>
#include <memory>
#include "../../../iroha-cli/genesis_block_client.hpp"
#include "ametsuchi/block_serializer.hpp"
#include "common/types.hpp"
#include "crypto/crypto.hpp"
#include "endpoint.grpc.pb.h"
#include "model/block.hpp"
#include "model/model_hash_provider_impl.hpp"
#include "main/genesis_block_server/genesis_block_service.hpp"

using ::testing::Return;

class GenesisBlockClientMock : public iroha_cli::GenesisBlockClient {
  /*
  MOCK_METHOD2(set_channel, void(const std::string &ip, const int port));
  MOCK_METHOD2(
      send_genesis_block,
      grpc::Status(const iroha::model::Block &iroha_block,
                   iroha::protocol::ApplyGenesisBlockResponse &response));
  MOCK_METHOD1(send_abort_genesis_block,
               void(const iroha::model::Block &block));
               */

  void set_channel(const std::string &ip, const int port) {}
  grpc::Status send_genesis_block(const iroha::model::Block &iroha_block,
                                  iroha::protocol::ApplyGenesisBlockResponse &response) {
    response.set_applied(iroha::protocol::APPLY_SUCCESS);
    return grpc::Status::OK;
  }

  void send_abort_genesis_block(const iroha::model::Block &block) {}
};

TEST(iroha_cli, NormalWhenParseTrustedPeers) {
  auto test_path = "/tmp/_boot_strap_test_target.conf";
  std::ofstream ofs(test_path);
  ofs << R"({"ip":["192.168.0.3","192.168.0.4","192.168.0.5","192.168.0.6"]})";
  ofs.close();

  GenesisBlockClientMock client_mock;
  iroha_cli::BootstrapNetwork bootstrap(client_mock);
  auto peers = bootstrap.parse_trusted_peers(test_path);
  ASSERT_EQ(peers.size(), 4);
  ASSERT_STREQ(peers[0].c_str(), "192.168.0.3");
  ASSERT_STREQ(peers[1].c_str(), "192.168.0.4");
  ASSERT_STREQ(peers[2].c_str(), "192.168.0.5");
  ASSERT_STREQ(peers[3].c_str(), "192.168.0.6");
  ASSERT_TRUE(remove(test_path) == 0);
}

TEST(iroha_cli, WrongIPNameWhenParseTrustedPeers) {
  auto test_path = "/tmp/_bootstrap_test_target.conf";
  std::ofstream ofs(test_path);
  ofs << R"({"IP":["192.168.0.3","192.168.0.4","192.168.0.5","192.168.0.6"]})";
  ofs.close();

  GenesisBlockClientMock client_mock;
  iroha_cli::BootstrapNetwork bootstrap(client_mock);
  ASSERT_ANY_THROW({ bootstrap.parse_trusted_peers(test_path); });
  ASSERT_TRUE(remove(test_path) == 0);
}

TEST(iroha_cli, InvalidIPValueWhenParseTrustedPeers) {
  auto test_path = "/tmp/_bootstrap_test_target.conf";
  std::ofstream ofs(test_path);
  ofs << R"({"ip":["192.168.256.3","192.168.0.4","192.168.0.5","192.168.0.6"]})";
  ofs.close();

  GenesisBlockClientMock client_mock;
  iroha_cli::BootstrapNetwork bootstrap(client_mock);
  ASSERT_ANY_THROW({ bootstrap.parse_trusted_peers(test_path); });
  ASSERT_TRUE(remove(test_path) == 0);
}

TEST(iroha_cli, NormalWhenParseGenesisBlock) {
  auto test_path = "/tmp/_bootstrap_test_genesis.json";
  std::ofstream ofs(test_path);

  ofs << std::string(R"({
  "transactions":[
    {
      "signatures":[{"signature":"blob", "pubkey":"blob"}],
      "created_ts":1111111111,
      "creator_account_id":"first_account_id",
      "tx_counter":1234567,
      "commands":[
        {
          "command_type":"CreateAccount",
          "pubkey":"blob",
          "account_name":"someone",
          "domain_id":"soramitsu"
        },
        {
          "command_type":"CreateDomain",
          "domain_name":"soramitsu"
        }
      ]
    },
    {
      "signatures":[{"signature":"blob","pubkey":"blob"}],
      "created_ts":1111111111,
      "creator_account_id":"second_account_id",
      "tx_counter":9876543,
      "commands":[
        {
          "command_type":"CreateDomain",
          "domain_name":"mydomain"
        }
      ]
    }
  ]
}
)");
  ofs.close();

  GenesisBlockClientMock client_mock;
  iroha_cli::BootstrapNetwork bootstrap(client_mock);
  auto genesis = bootstrap.parse_genesis_block(test_path);
  ASSERT_TRUE(remove(test_path) == 0);
}

TEST(iroha_cli, NormalWhenRunNetwork) {
  iroha::model::Transaction tx;
  tx.creator_account_id = "user1";
  iroha::model::CreateDomain createDomain;
  createDomain.domain_name = "ja";
  tx.commands.push_back(
      std::make_shared<iroha::model::CreateDomain>(createDomain));
  iroha::model::CreateAccount createAccount;
  createAccount.account_name = "user2";
  createAccount.domain_id = "ja";
  tx.commands.push_back(
      std::make_shared<iroha::model::CreateAccount>(createAccount));

  iroha::model::Block block;
  block.transactions.push_back(tx);
  block.height = 1;
  block.prev_hash.fill(0);
  block.txs_number = block.transactions.size();
  iroha::model::HashProviderImpl hash_provider;
  block.hash = hash_provider.get_hash(block);

  GenesisBlockClientMock client_mock;
  iroha::protocol::ApplyGenesisBlockResponse response;
  //EXPECT_CALL(send_genesis_block, send_genesis_block(block, response))
  //  .WillRepeatedly(Return(grpc::Status::OK));

  iroha_cli::BootstrapNetwork bootstrap(client_mock);
  
  ASSERT_NO_THROW({
    bootstrap.run_network(
        {"192.168.256.3", "192.168.0.4"}, block);
  });
}
