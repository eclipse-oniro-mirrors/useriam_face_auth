/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "face_auth_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace UserIam {
namespace FaceAuth {
class FaceAuthServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FaceAuthServiceTest::SetUpTestCase()
{
    static const char *PERMS[] = {
        "ohos.permission.MANAGE_USER_IDM"
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = PERMS,
        .acls = nullptr,
        .processName = "face_auth_service_test",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void FaceAuthServiceTest::TearDownTestCase()
{
}

void FaceAuthServiceTest::SetUp()
{
}

void FaceAuthServiceTest::TearDown()
{
}

HWTEST_F(FaceAuthServiceTest, FaceAuthServiceTest_001, TestSize.Level0)
{
    auto service = FaceAuthService::GetInstance();
    service->OnStart();
    sptr<IBufferProducer> producer = nullptr;
    int32_t ret = service->SetBufferProducer(producer);
    EXPECT_EQ(ret, FACEAUTH_ERROR);
}
} // namespace FaceAuth
} // namespace UserIam
} // namespace OHOS