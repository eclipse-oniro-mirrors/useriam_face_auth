/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screen_brightness_manager.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "sa_command_manager.h"
#include "service_ex_manager.h"

#define LOG_TAG "FACE_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace {
std::shared_ptr<ScreenBrightnessManager> CreateScreenBrightnessManager()
{
    auto manager = Common::MakeShared<ScreenBrightnessManager>();
    IF_FALSE_LOGE_AND_RETURN_VAL(manager != nullptr, nullptr);

    std::vector<SaCommandId> commandIds = { SaCommandId::BEGIN_SCREEN_BRIGHTNESS_INCREASE,
        SaCommandId::END_SCREEN_BRIGHTNESS_INCREASE };
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, manager);
    return manager;
}
} // namespace

std::shared_ptr<ScreenBrightnessManager> ScreenBrightnessManager::GetInstance()
{
    static auto manager = CreateScreenBrightnessManager();
    if (manager == nullptr) {
        IAM_LOGE("ScreenBrightnessManager is null");
    }
    return manager;
}

std::shared_ptr<IScreenBrightnessTask> ScreenBrightnessManager::GetCurrentTask()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return taskInProc_;
}

UserAuth::ResultCode ScreenBrightnessManager::ProcessSaCommand(std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor,
    const SaCommand &command)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executor != nullptr, UserAuth::GENERAL_ERROR);

    std::lock_guard<std::mutex> lock(mutex_);

    UserAuth::ResultCode result = UserAuth::GENERAL_ERROR;
    switch (command.id) {
        case SaCommandId::BEGIN_SCREEN_BRIGHTNESS_INCREASE:
            result = ProcessScreenBrightnessIncreaseBegin(executor, command.param);
            break;
        case SaCommandId::END_SCREEN_BRIGHTNESS_INCREASE:
            result = ProcessScreenBrightnessIncreaseEnd(executor, command.param);
            break;
        default:
            IAM_LOGE("command id %{public}d not match", command.id);
    }
    return result;
}

void ScreenBrightnessManager::OnHdiDisconnect(std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor)
{
    IF_FALSE_LOGE_AND_RETURN(executor != nullptr);

    std::lock_guard<std::mutex> lock(mutex_);

    if (executorInProc_ != executor) {
        return;
    }

    IAM_LOGI("start");
    executorInProc_ = nullptr;

    if (taskInProc_ != nullptr) {
        taskInProc_->Stop();
        taskInProc_ = nullptr;
    }
    IAM_LOGI("success");
}

UserAuth::ResultCode ScreenBrightnessManager::ProcessScreenBrightnessIncreaseBegin(
    std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != nullptr) {
        IAM_LOGE("another executor is using this module");
        return UserAuth::GENERAL_ERROR;
    }

    if (taskInProc_ != nullptr) {
        IAM_LOGE("another task is running");
        return UserAuth::GENERAL_ERROR;
    }

    IAM_LOGI("start");
    auto acquireRet = ServiceExManager::GetInstance().Acquire();
    IF_FALSE_LOGE_AND_RETURN_VAL(acquireRet == UserAuth::SUCCESS, UserAuth::GENERAL_ERROR);

    auto taskInProc = ServiceExManager::GetInstance().GetScreenBrightnessTask();
    if (taskInProc == nullptr) {
        ServiceExManager::GetInstance().Release();
        IAM_LOGE("failed to get task");
        return UserAuth::GENERAL_ERROR;
    }
    taskInProc->RegisterDestructCallback([]() {
        IAM_LOGI("task destruct");
        ServiceExManager::GetInstance().Release();
    });
    taskInProc->Start();

    executorInProc_ = executor;
    taskInProc_ = taskInProc;
    IAM_LOGI("success");
    return UserAuth::SUCCESS;
}

UserAuth::ResultCode ScreenBrightnessManager::ProcessScreenBrightnessIncreaseEnd(
    std::shared_ptr<FaceAuthAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != executor) {
        IAM_LOGE("another executor is using this module");
        return UserAuth::GENERAL_ERROR;
    }

    IAM_LOGI("start");
    executorInProc_ = nullptr;

    if (taskInProc_ != nullptr) {
        taskInProc_->Stop();
        taskInProc_ = nullptr;
    }

    IAM_LOGI("success");
    return UserAuth::SUCCESS;
}

} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS