/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FACE_AUTH_ALL_IN_ONE_EXECUTOR_HDI_H
#define FACE_AUTH_ALL_IN_ONE_EXECUTOR_HDI_H

#include <cstdint>
#include <map>
#include <vector>

#include "nocopyable.h"

#include "face_auth_hdi.h"
#include "iam_executor_framework_types.h"
#include "iam_executor_iauth_executor_hdi.h"
#include "iam_executor_iexecute_callback.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class FaceAuthAllInOneExecutorHdi : public std::enable_shared_from_this<FaceAuthAllInOneExecutorHdi>,
                                    public UserAuth::IAuthExecutorHdi,
                                    public NoCopyable {
public:
    explicit FaceAuthAllInOneExecutorHdi(sptr<IAllInOneExecutor> executorProxy);
    ~FaceAuthAllInOneExecutorHdi() override = default;

    UserAuth::ResultCode GetExecutorInfo(UserAuth::ExecutorInfo &info) override;
    UserAuth::ResultCode OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo) override;
    UserAuth::ResultCode SendMessage(uint64_t scheduleId, int32_t srcRole, const std::vector<uint8_t> &msg) override;
    UserAuth::ResultCode Enroll(uint64_t scheduleId, const UserAuth::EnrollParam &param,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserAuth::ResultCode Authenticate(uint64_t scheduleId, const UserAuth::AuthenticateParam &param,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserAuth::ResultCode Identify(uint64_t scheduleId, const UserAuth::IdentifyParam &param,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserAuth::ResultCode Delete(const std::vector<uint64_t> &templateIdList) override;
    UserAuth::ResultCode Cancel(uint64_t scheduleId) override;
    UserAuth::ResultCode SendCommand(UserAuth::PropertyMode commandId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) override;
    UserAuth::ResultCode GetProperty(const std::vector<uint64_t> &templateIdList,
        const std::vector<UserAuth::Attributes::AttributeKey> &keys, UserAuth::Property &property) override;
    UserAuth::ResultCode SetCachedTemplates(const std::vector<uint64_t> &templateIdList) override;
    void OnHdiDisconnect();

private:
    class SaCommandCallback;

    UserAuth::ResultCode MoveHdiExecutorInfo(ExecutorInfo &in, UserAuth::ExecutorInfo &out);
    void MoveHdiProperty(Property &in, UserAuth::Property &out,
        const std::vector<UserAuth::Attributes::AttributeKey> &keys);
    UserAuth::ResultCode ConvertCommandId(const UserAuth::PropertyMode in, int32_t &out);
    UserAuth::ResultCode ConvertAuthType(const int32_t in, UserAuth::AuthType &out);
    UserAuth::ResultCode ConvertExecutorRole(const int32_t in, UserAuth::ExecutorRole &out);
    UserAuth::ResultCode ConvertExecutorSecureLevel(const int32_t in, UserAuth::ExecutorSecureLevel &out);
    UserAuth::ResultCode ConvertResultCode(const int32_t in);
    UserAuth::ResultCode ConvertAttributeKeyToPropertyType(const UserAuth::Attributes::AttributeKey in, int32_t &out);
    UserAuth::ResultCode ConvertAttributeKeyVectorToPropertyType(
        const std::vector<UserAuth::Attributes::AttributeKey> inVector, std::vector<int32_t> &outVector);
    UserAuth::ResultCode RegisterSaCommandCallback();

    sptr<IAllInOneExecutor> executorProxy_;
    sptr<SaCommandCallback> saCommandCallback_;
};

class FaceAuthAllInOneExecutorHdi::SaCommandCallback : public ISaCommandCallback, public NoCopyable {
public:
    explicit SaCommandCallback(std::shared_ptr<FaceAuthAllInOneExecutorHdi> executorHdi) : executorHdi_(executorHdi) {};
    ~SaCommandCallback() override {};
    int32_t OnSaCommands(const std::vector<SaCommand> &commands) override;

private:
    std::shared_ptr<FaceAuthAllInOneExecutorHdi> executorHdi_;
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // FACE_AUTH_ALL_IN_ONE_EXECUTOR_HDI_H