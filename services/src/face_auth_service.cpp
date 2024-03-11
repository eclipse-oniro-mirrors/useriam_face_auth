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

#include "face_auth_service.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "accesstoken_kit.h"
#include "iam_executor_idriver_manager.h"
#include "ibuffer_producer.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "refbase.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_para2str.h"
#include "iam_ptr.h"

#include "face_auth_defines.h"
#include "face_auth_driver_hdi.h"
#include "face_auth_interface_adapter.h"
#include "screen_brightness_manager.h"

#define LOG_TAG "FACE_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
using namespace OHOS::UserIam;
namespace {
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(FaceAuthService::GetInstance().get());
const uint16_t FACE_AUTH_DEFAULT_HDI_ID = 1;
const auto FACE_AUTH_DEFAULT_HDI_ADAPTER = Common::MakeShared<FaceAuthInterfaceAdapter>();
auto FACE_AUTH_DEFAULT_HDI = Common::MakeShared<FaceAuthDriverHdi>(FACE_AUTH_DEFAULT_HDI_ADAPTER);
// serviceName and HdiConfig.id must be globally unique
const std::map<std::string, UserAuth::HdiConfig> HDI_NAME_2_CONFIG = {
    { "face_auth_interface_service", { FACE_AUTH_DEFAULT_HDI_ID, FACE_AUTH_DEFAULT_HDI } },
};
const std::vector<std::shared_ptr<FaceAuthDriverHdi>> FACE_AUTH_DRIVER_HDIS = { FACE_AUTH_DEFAULT_HDI };
// setup brightness manager
const auto screenBrightnessManager = ScreenBrightnessManager::GetInstance();
} // namespace
std::mutex FaceAuthService::mutex_;
std::shared_ptr<FaceAuthService> FaceAuthService::instance_ = nullptr;

FaceAuthService::FaceAuthService() : SystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FACEAUTH, true)
{
}

std::shared_ptr<FaceAuthService> FaceAuthService::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> guard(mutex_);
        if (instance_ == nullptr) {
            instance_ = Common::MakeShared<FaceAuthService>();
            if (instance_ == nullptr) {
                IAM_LOGE("make share failed");
            }
        }
    }
    return instance_;
}

void FaceAuthService::OnStart()
{
    IAM_LOGI("start");
    StartDriverManager();
    Publish(this);
    IAM_LOGI("success");
}

void FaceAuthService::OnStop()
{
    IAM_LOGE("service is persistent, OnStop is not implemented");
}

bool FaceAuthService::IsPermissionGranted(const std::string &permission)
{
    IAM_LOGI("start");
    uint32_t tokenId = this->GetCallingTokenID();
    int ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, permission);
    if (ret != Security::AccessToken::RET_SUCCESS) {
        IAM_LOGE("failed to verify access token, code = %{public}d", ret);
        return false;
    }
    return true;
}

int32_t FaceAuthService::SetBufferProducer(sptr<IBufferProducer> &producer)
{
    const std::string MANAGE_USER_IDM_PERMISSION = "ohos.permission.MANAGE_USER_IDM";
    std::lock_guard<std::mutex> guard(mutex_);
    IAM_LOGI("set buffer producer %{public}s", Common::GetPointerNullStateString(producer).c_str());
    using namespace Security::AccessToken;
    uint64_t fullTokenId = IPCSkeleton::GetCallingFullTokenID();
    bool checkRet = TokenIdKit::IsSystemAppByFullTokenID(fullTokenId);
    uint32_t tokenId = this->GetCallingTokenID();
    ATokenTypeEnum callingType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (callingType == TOKEN_HAP && !checkRet) {
        IAM_LOGE("the caller is not a system application");
        return FACE_AUTH_CHECK_SYSTEM_PERMISSION_FAILED;
    }
    if (!IsPermissionGranted(MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        return FACE_AUTH_CHECK_PERMISSION_FAILED;
    }
    for (auto hdi : FACE_AUTH_DRIVER_HDIS) {
        IF_FALSE_LOGE_AND_RETURN_VAL(hdi != nullptr, FACE_AUTH_ERROR);
        int ret = hdi->SetBufferProducer(producer);
        if (ret != FACE_AUTH_SUCCESS) {
            IAM_LOGE("SetBufferProducer fail");
            return FACE_AUTH_ERROR;
        }
    }
    return FACE_AUTH_SUCCESS;
}

void FaceAuthService::StartDriverManager()
{
    IAM_LOGI("start");
    int32_t ret = UserAuth::IDriverManager::Start(HDI_NAME_2_CONFIG);
    if (ret != FACE_AUTH_SUCCESS) {
        IAM_LOGE("start driver manager failed");
    }
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS
