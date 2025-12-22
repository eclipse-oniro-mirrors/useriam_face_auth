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

#include "face_auth_stub_fuzzer.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include "parcel.h"

#include "face_auth_service.h"
#include "iam_fuzz_test.h"
#include "iam_logger.h"

#define LOG_TAG "FACE_AUTH_SA"

using namespace std;

namespace OHOS {
namespace UserIam {
namespace UserAuth {
namespace {
const std::u16string FACE_AUTH_INTERFACE_TOKEN = u"ohos.faceauth.IFaceAuth";

bool FaceAuthStubFuzzTest(const uint8_t *rawData, size_t size)
{
    IAM_LOGI("start");
    if (rawData == nullptr) {
        return false;
    }
    Parcel parcel;
    parcel.WriteBuffer(rawData, size);
    parcel.RewindRead(0);
    static uint32_t faceAuthStubCodes[] = {0, 1};
    uint32_t pos = parcel.ReadUint32() % (sizeof(faceAuthStubCodes) / sizeof(uint32_t));
    uint32_t code = faceAuthStubCodes[pos];
    MessageOption option = MessageOption::TF_SYNC;
    if (parcel.ReadBool()) {
        option = MessageOption::TF_ASYNC;
    }
    auto faceAuthservice = FaceAuth::FaceAuthService::GetInstance();
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(FACE_AUTH_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    (void)faceAuthservice->OnRemoteRequest(code, data, reply, option);
    return true;
}
} // namespace
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::UserIam::UserAuth::FaceAuthStubFuzzTest(data, size);
    return 0;
}
