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

#ifndef MOCK_IFACE_AUTH_INTERFACE_H
#define MOCK_IFACE_AUTH_INTERFACE_H

#include "gmock/gmock.h"

#include "face_auth_hdi.h"

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class MockIFaceAuthInterface : public IFaceAuthInterface {
public:
    virtual ~MockIFaceAuthInterface() = default;

    MOCK_METHOD1(GetExecutorList, int32_t(std::vector<sptr<IAllInOneExecutor>> &executorList));
    MOCK_METHOD1(SetBufferProducer, int32_t(const sptr<BufferProducerSequenceable> &bufferProducer));

    MOCK_METHOD1(SetCameraController, int32_t(const sptr<ICameraControllerCallback>& cameraController));
    MOCK_METHOD3(GetCameraSettings, int32_t(uint64_t scheduleId, const std::vector<uint8_t>& cameraAbility,
         std::vector<uint8_t>& cameraSettings));
    MOCK_METHOD2(SetCameraSecureSeqId, int32_t(uint64_t scheduleId, uint64_t secureSeqId));
    MOCK_METHOD2(OnCameraError, int32_t(uint64_t scheduleId, int32_t resultCode));
};
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_IFACE_AUTH_INTERFACE_MOCK_H