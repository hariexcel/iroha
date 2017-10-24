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

#ifndef IROHA_ED25519_SIGNER_HPP_
#define IROHA_ED25519_SIGNER_HPP_

#include "crypto/ed25519/ed25519.hpp"
#include "crypto_provider/signer.hpp"

namespace iroha {
  namespace crypto {

    namespace ed25519 {

      class Ed25519Signer final : public Signer<Ed25519Signer> {
       public:
        Ed25519Signer(ed25519::Keypair &&kp) : keypair(std::move(kp)) {}

        using pubkey_t = ed25519::PublicKey;
        using privkey_t = ed25519::PrivateKey;

        ed25519::Signature sign(const message_t &m) const noexcept;

       private:
        ed25519::Keypair keypair;
      };
    }
  }
}

#endif  //  IROHA_ED25519_SIGNER_HPP_
