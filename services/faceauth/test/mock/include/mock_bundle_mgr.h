/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_AAFWK_ABILITY_MOCK_BUNDLE_MANAGER_H
#define OHOS_AAFWK_ABILITY_MOCK_BUNDLE_MANAGER_H

#include <vector>
#include "ability_info.h"
#include "application_info.h"
#include "i_bundle_mgr_mock.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrProxy : public IRemoteProxy<IBundleMgrMock> {
public:
    explicit BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgrMock>(impl)
    {}
};
class BundleMgrStub : public IRemoteStub<IBundleMgrMock> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IBundleMgrMock");
};
class BundleMgrService : public BundleMgrStub {
public:
    bool GetApplicationInfo(const std::string &appName, const ApplicationFlag flag, const int userId,
        ApplicationInfo &appInfo) override;
    bool GetApplicationInfos(const ApplicationFlag flag, const int userId,
        std::vector<ApplicationInfo> &appInfos) override;
    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo) override;
    bool GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos) override;
    int GetUidByBundleName(const std::string &bundleName, const int userId) override;
    std::string GetAppIdByBundleName(const std::string &bundleName, const int userId) override;
    bool GetBundleNameForUid(const int uid, std::string &bundleName) override;
    bool GetBundlesForUid(const int uid, std::vector<std::string> &bundleNames) override;
    bool GetNameForUid(const int uid, std::string &name) override;
    bool GetBundleGids(const std::string &bundleName, std::vector<int> &gids) override;
    std::string GetAppType(const std::string &bundleName) override;
    bool CheckIsSystemAppByUid(const int uid) override;
    bool GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos) override;
    bool QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo) override;
    bool QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo) override;
    bool QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos) override;
    std::string GetAbilityLabel(const std::string &bundleName, const std::string &className) override;
    bool GetBundleArchiveInfo(const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo) override;
    bool GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo) override;
    bool GetLaunchWantForBundle(const std::string &bundleName, Want &want) override;
    int CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName) override;
    int CheckPermission(const std::string &bundleName, const std::string &permission) override;
    bool GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef) override;
    bool GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs) override;
    bool GetAppsGrantedPermissions(const std::vector<std::string> &permissions,
        std::vector<std::string> &appNames) override;
    bool HasSystemCapability(const std::string &capName) override;
    bool GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps) override;
    bool IsSafeMode() override;
    bool CleanBundleCacheFiles(const std::string &bundleName,
        const sptr<ICleanCacheCallback> &cleanCacheCallback) override;
    bool CleanBundleDataFiles(const std::string &bundleName) override;
    bool RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback) override;
    bool ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback) override;
    bool UnregisterBundleStatusCallback() override;
    bool DumpInfos(const MockDumpFlag flag, const std::string &bundleName, std::string &result) override;
    bool IsApplicationEnabled(const std::string &bundleName) override;
    bool SetApplicationEnabled(const std::string &bundleName, bool isEnable) override;
    bool IsAbilityEnabled(const AbilityInfo &abilityInfo) override;
    bool SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnabled) override;
    std::string GetAbilityIcon(const std::string &bundleName, const std::string &className) override;
    bool CanRequestPermission(const std::string &bundleName, const std::string &permissionName,
        const int userId) override;
    bool RequestPermissionFromUser(const std::string &bundleName, const std::string &permission,
        const int userId) override;
    bool RegisterAllPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback) override;
    bool RegisterPermissionsChanged(const std::vector<int> &uids,
        const sptr<OnPermissionChangedCallback> &callback) override;
    bool UnregisterPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback) override;
    sptr<IBundleInstaller> GetBundleInstaller() override;
    bool GetAllFormsInfo(std::vector<FormInfo> &formInfos) override;
    bool GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos) override;
    bool GetFormsInfoByModule(const std::string &bundleName, const std::string &moduleName,
        std::vector<FormInfo> &formInfos) override;
    bool GetShortcutInfos(const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfos) override;
    BundleMgrService()
    {}
    ~BundleMgrService()
    {}
    void DefPermission(const std::string &bundleName, BundleInfo &bundleInfo, ApplicationInfo applicationInfo);
};
} // namespace AppExecFwk
} // namespace OHOS

#endif // OHOS_AAFWK_ABILITY_MOCK_BUNDLE_MANAGER_H
