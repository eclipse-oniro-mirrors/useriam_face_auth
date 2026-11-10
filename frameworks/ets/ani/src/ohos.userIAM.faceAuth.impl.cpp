/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.userIAM.faceAuth.impl.hpp"

#include <sstream>

#include "ohos.userIAM.faceAuth.FaceAuthManager.proj.2.hpp"
#include "taihe/runtime.hpp"

#include "iam_check.h"
#include "iam_logger.h"

#include "face_auth_client.h"
#include "face_auth_defines.h"
#include "surface_utils.h"

using namespace OHOS;
using namespace taihe;

#define LOG_TAG "FACE_AUTH_ANI"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace {
bool GetBufferProducerBySurfaceId(uint64_t surfaceId, sptr<IBufferProducer> &bufferProducer)
{
    if (surfaceId == 0) {
        IAM_LOGI("surface id is 0, get buffer producer null");
        bufferProducer = nullptr;
        return true;
    }

    auto surfaceUtils = SurfaceUtils::GetInstance();
    if (surfaceUtils == nullptr) {
        IAM_LOGE("Get SurfaceUtils failed!");
        return false;
    }

    sptr<Surface> previewSurface = surfaceUtils->GetSurface(surfaceId);
    if (previewSurface == nullptr) {
        IAM_LOGE("GetSurface failed!");
        return false;
    }

    bufferProducer = previewSurface->GetProducer();
    if (bufferProducer == nullptr) {
        IAM_LOGE("GetProducer Failed!");
        return false;
    }

    IAM_LOGI("get buffer producer success");
    return true;
}

int32_t SetSurfaceId(const std::string &surfaceIdStr)
{
    IAM_LOGI("SetSurfaceId start");

    std::istringstream surfaceIdStream(surfaceIdStr);
    uint64_t surfaceId = 0;
    surfaceIdStream >> surfaceId;
    if (surfaceIdStream.fail()) {
        IAM_LOGE("invalid surfaceId format");
        return FACE_AUTH_ERROR;
    }

    sptr<IBufferProducer> bufferProducer(nullptr);
    if (!GetBufferProducerBySurfaceId(surfaceId, bufferProducer)) {
        IAM_LOGE("GetBufferProducerBySurfaceId fail");
        return FACE_AUTH_ERROR;
    }

    int32_t result = FaceAuthClient::GetInstance().SetBufferProducer(bufferProducer);
    if (result != FACE_AUTH_SUCCESS) {
        IAM_LOGE("SetBufferProducer fail, result: %{public}d", result);
        return result;
    }

    IAM_LOGI("SetSurfaceId success");
    return FACE_AUTH_SUCCESS;
}

void ThrowBusinessError(int32_t result)
{
    const int32_t napiGeneralError = 12700001;
    const std::map<int32_t, std::string> errorCode2Str = {
        { FACE_AUTH_CHECK_PERMISSION_FAILED, "Permission verification failed." },
        { FACE_AUTH_CHECK_SYSTEM_PERMISSION_FAILED, "The caller is not a system application." },
        { napiGeneralError, "The operation is failed." },
    };

    if (result == FACE_AUTH_SUCCESS) {
        return;
    }

    auto pair = errorCode2Str.find(result);
    if (pair == errorCode2Str.end()) {
        pair = errorCode2Str.find(napiGeneralError);
    }
    IF_FALSE_LOGE_AND_RETURN(pair != errorCode2Str.end());
    IAM_LOGE("ThrowBusinessError, result: %{public}d, errorCode: %{public}d, errmsg: %{public}s", result, pair->first,
        pair->second.c_str());
    set_business_error(pair->first, pair->second);
}

class FaceAuthManagerImpl {
public:
    FaceAuthManagerImpl()
    {
    }
    void setSurfaceId(string_view surfaceId)
    {
        std::string surfaceIdStr(surfaceId);
        int32_t ret = SetSurfaceId(surfaceIdStr);
        if (ret != FACE_AUTH_SUCCESS) {
            IAM_LOGE("SetSurfaceId fail, result: %{public}d", ret);
            ThrowBusinessError(ret);
            return;
        }
    }
};
} // namespace
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

namespace {
::ohos::userIAM::faceAuth::FaceAuthManager CreateFaceAuthManager()
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<OHOS::UserIam::FaceAuth::FaceAuthManagerImpl, ohos::userIAM::faceAuth::FaceAuthManager>();
}
} // namespace

TH_EXPORT_CPP_API_CreateFaceAuthManager(CreateFaceAuthManager);
