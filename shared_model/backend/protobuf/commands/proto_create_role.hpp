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

#include "interfaces/commands/create_role.hpp"

#ifndef IROHA_PROTO_CREATE_ROLE_HPP
#define IROHA_PROTO_CREATE_ROLE_HPP

namespace shared_model {
  namespace proto {
    class CreateRole final : public interface::CreateRole {
     private:
      using RefCreateRole =
          detail::ReferenceHolder<iroha::protocol::Command,
                                  const iroha::protocol::CreateRole &>;

     public:
      explicit CreateRole(const iroha::protocol::Command &command)
          : CreateRole(
                RefCreateRole(command,
                              detail::makeReferenceGetter(
                                  &iroha::protocol::Command::create_role))) {}

      explicit CreateRole(iroha::protocol::Command &&command)
          : CreateRole(
                RefCreateRole(std::move(command),
                              detail::makeReferenceGetter(
                                  &iroha::protocol::Command::create_role))) {}

      const interface::types::RoleIdType &roleName() const override {
        return create_role_->role_name();
      }

      const PermissionsType &rolePermissions() const override {
        return *role_permissions_;
      }

      ModelType *copy() const override {
        iroha::protocol::Command command;
        *command.mutable_create_role() = *create_role_;
        return new CreateRole(std::move(command));
      }

     private:
      // ----------------------------| private API |----------------------------
      explicit CreateRole(RefCreateRole &&ref)
          : create_role_(std::move(ref)), role_permissions_([this] {
              std::set<std::string> perms;

              std::for_each(
                  create_role_->permissions().begin(),
                  create_role_->permissions().end(),
                  [&perms, this](auto perm) {
                    perms.insert(iroha::protocol::RolePermission_Name(
                        static_cast<iroha::protocol::RolePermission>(perm)));
                  });
              return perms;
            }) {}

      RefCreateRole create_role_;

      template <typename Value>
      using Lazy = detail::LazyInitializer<Value>;

      Lazy<PermissionsType> role_permissions_;
    };  // namespace proto
  }     // namespace proto
}  // namespace shared_model

#endif  // IROHA_PROTO_CREATE_ROLE_HPP