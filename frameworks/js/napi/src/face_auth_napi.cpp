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

#include <cstdint>
#include <sstream>
#include <string>

#include "napi/native_common.h"
#include "node_api.h"

#include "surface.h"
#include "surface_utils.h"

#include "iam_logger.h"

#include "face_auth_client.h"
#include "face_auth_defines.h"

#define LOG_TAG "FACE_AUTH_NAPI"

using namespace std;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
namespace {
enum ResultCode : int32_t {
    OHOS_CHECK_PERMISSION_FAILED = 201,
    OHOS_CHECK_SYSTEM_PERMISSION_FAILED = 202,
    RESULT_CODE_FAIL = 12700001,
};

const std::map<int32_t, std::string> g_result2Str = {
    { OHOS_CHECK_PERMISSION_FAILED, "Permission verification failed." },
    { OHOS_CHECK_SYSTEM_PERMISSION_FAILED, "The caller is not a system application." },
    { RESULT_CODE_FAIL, "The service is unavailable." },
};

napi_value FaceAuthManagerConstructor(napi_env env, napi_callback_info info)
{
    IAM_LOGI("start");
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    return thisVar;
}

napi_value GenerateBusinessError(napi_env env, int32_t result)
{
    napi_value code;
    std::string msgStr;
    auto res = g_result2Str.find(result);
    if (res == g_result2Str.end()) {
        IAM_LOGE("result %{public}d not found", result);
        msgStr = g_result2Str.at(RESULT_CODE_FAIL);
        NAPI_CALL(env, napi_create_int32(env, RESULT_CODE_FAIL, &code));
    } else {
        msgStr = res->second;
        NAPI_CALL(env, napi_create_int32(env, result, &code));
    }
    IAM_LOGI("get msg %{public}s", msgStr.c_str());

    napi_value msg;
    NAPI_CALL(env, napi_create_string_utf8(env, msgStr.c_str(), NAPI_AUTO_LENGTH, &msg));

    napi_value businessError;
    NAPI_CALL(env, napi_create_error(env, nullptr, msg, &businessError));
    NAPI_CALL(env, napi_set_named_property(env, businessError, "code", code));

    return businessError;
}

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

napi_value SetSurfaceId(napi_env env, napi_callback_info info)
{
    static constexpr size_t argsOne = 1;
    size_t argc = argsOne;
    napi_value argv;
    napi_status ret = napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);
    if (ret != napi_ok || argc != argsOne) {
        IAM_LOGE("napi_get_cb_info fail:%{public}d", ret);
        napi_throw(env, GenerateBusinessError(env, RESULT_CODE_FAIL));
        return nullptr;
    }
    static constexpr int maxLen = 25;
    char buf[maxLen] = { '\0' };
    size_t len;
    ret = napi_get_value_string_utf8(env, argv, buf, maxLen, &len);
    if (ret != napi_ok) {
        IAM_LOGE("napi_get_value_string_utf8 fail:%{public}d", ret);
        napi_throw(env, GenerateBusinessError(env, RESULT_CODE_FAIL));
        return nullptr;
    }
    buf[maxLen - 1] = '\0';
    std::string strSurfaceId = buf;
    std::istringstream surfaceIdStream(strSurfaceId);
    uint64_t surfaceId = 0;
    surfaceIdStream >> surfaceId;
    if (surfaceIdStream.fail()) {
        IAM_LOGE("invalid surfaceId format");
        napi_throw(env, GenerateBusinessError(env, RESULT_CODE_FAIL));
        return nullptr;
    }

    sptr<IBufferProducer> bufferProducer(nullptr);
    if (!GetBufferProducerBySurfaceId(surfaceId, bufferProducer)) {
        IAM_LOGE("GetBufferProducerBySurfaceId fail");
        napi_throw(env, GenerateBusinessError(env, RESULT_CODE_FAIL));
        return nullptr;
    }
    int32_t result = FaceAuthClient::GetInstance().SetBufferProducer(bufferProducer);
    if (result != FACE_AUTH_SUCCESS) {
        IAM_LOGE("SetBufferProducer fail");
        napi_throw(env, GenerateBusinessError(env, result));
        return nullptr;
    }
    return nullptr;
}

napi_value GetFaceAuthManagerConstructor(napi_env env)
{
    IAM_LOGI("start");
    napi_property_descriptor methods[] = { DECLARE_NAPI_FUNCTION("setSurfaceId", SetSurfaceId) };
    napi_value result = nullptr;
    NAPI_CALL(env,
        napi_define_class(env, "FaceAuth", NAPI_AUTO_LENGTH, FaceAuthManagerConstructor, nullptr,
            sizeof(methods) / sizeof(napi_property_descriptor), methods, &result));
    return result;
}

napi_value GetResultCodeConstructor(napi_env env)
{
    IAM_LOGI("start");
    napi_value resultCode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &resultCode));
    napi_value fail = nullptr;
    NAPI_CALL(env, napi_create_int32(env, RESULT_CODE_FAIL, &fail));
    NAPI_CALL(env, napi_set_named_property(env, resultCode, "FAIL", fail));
    return resultCode;
}

napi_value ModuleInit(napi_env env, napi_value exports)
{
    IAM_LOGI("start");
    NAPI_CALL(env, napi_set_named_property(env, exports, "FaceAuthManager", GetFaceAuthManagerConstructor(env)));

    napi_property_descriptor enums[] = {
        DECLARE_NAPI_PROPERTY("ResultCode", GetResultCodeConstructor(env)),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(enums) / sizeof(napi_property_descriptor), enums));

    return exports;
}
} // namespace

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module module = { .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = ModuleInit,
        .nm_modname = "userIAM.faceAuth",
        .nm_priv = nullptr,
        .reserved = {} };
    napi_module_register(&module);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS