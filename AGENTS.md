# 人脸认证组件指引

## 项目定位

本仓库对应 OpenHarmony `useriam` 子系统的人脸认证能力（FaceAuth）上游基础组件，位于 `base/useriam/face_auth`。作为协同认证框架（UserAuth）的人脸认证执行器，负责将人脸资源注册到协同认证框架，并按框架调度调用人脸认证 HDI 完成录入/认证/删除等。优先按这些目录定位问题：

- `services/`：人脸认证 SA（SystemAbility 942）核心服务，包括 HDI 驱动封装、认证执行器、SA 命令处理、屏幕亮度管理。
- `services_ex/`：SA 扩展逻辑（以动态库方式按需加载），包括有限状态机（FSM）、屏幕亮度提升任务、单线程处理器。
- `frameworks/`：客户端侧代码，包括 NAPI（`frameworks/js/napi`）、IPC 代理/存根（`frameworks/ipc`）、ETS 动画接口（`frameworks/ets`）。
- `common/`：共享日志、工具宏、基础定义（`face_auth_defines.h`）。
- `sa_profile/`：SA 配置文件（SAID 942）。
- `ui/`：人脸录入界面实现（`Settings_FaceAuth`，ETS 应用）。
- `test/`：单元测试（`test/unittest`）与 fuzz 目标（`test/fuzztest`）。

### 按任务类型定位代码

| 任务类型 | 目录 | 关键文件 |
| --- | --- | --- |
| 修改 SA 启动/停止、IPC 入口、权限校验 | `services/` | `face_auth_service.h/cpp`, `face_auth_stub.h/cpp` |
| 修改 HDI 驱动封装（executor 注册、命令转换） | `services/` | `face_auth_driver_hdi.h/cpp`, `face_auth_all_in_one_executor_hdi.h/cpp`, `face_auth_interface_adapter.h/cpp` |
| 修改 HDI 结果回包/振动反馈 | `services/` | `face_auth_executor_callback_hdi.h/cpp` |
| 修改 SA 命令分发 | `services/` | `sa_command_manager.h/cpp`, `isa_command_processor.h`, `face_auth_hdi.h` |
| 修改屏幕亮度提升逻辑 | `services/`, `services_ex/` | `screen_brightness_manager.h/cpp`, `screen_brightness_task.h/cpp`, `iscreen_brightness_task.h` |
| 修改有限状态机 | `services_ex/` | `finite_state_machine.h/cpp`, `finite_state_machine_impl.h/cpp`, `finite_state_machine_builder.h/cpp` |
| 修改 NAPI 绑定 | `frameworks/js/napi/` | `face_auth_napi.cpp` |
| 修改 ETS 动画接口 | `frameworks/ets/ani/` | `idl/ohos.userIAM.faceAuth.taihe`, `src/*.cpp` |
| 修改客户端 IPC | `frameworks/ipc/` | `face_auth_client_impl.cpp`, `face_auth_proxy.h/cpp`, `face_auth_stub.h/cpp`, `iface_auth.h` |
| 修改人脸录入 UI | `ui/Settings_FaceAuth/` | `entry/src/main/ets/pages/*.ets`, `entry/src/main/ets/model/*.ts`, `entry/src/main/ets/controller/*.ts` |

### 嵌套指引

本仓库无目录级别的嵌套指引。任务级指导以各模块代码内注释、`bundle.json` 组件依赖和 `README_ZH.md` 为准。

## 构建和验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。本组件编译产物目标名见 `bundle.json` 的 `build` 字段（`faceauthservice`、`faceauthservice_ex`、`faceauth`、`face_auth_ani`、`faceauth_framework`、`faceauth_sa_profile` 等）。

```sh
./build.sh --product-name rk3568 --build-target face_auth --ccache
```

单元测试与 fuzz 测试目标（`test/unittest/BUILD.gn`、`test/fuzztest/BUILD.gn`）：

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 face_auth_service_test
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 faceauth_fuzztest
```

### 完成标准

任务被认为完成，当且仅当：

1. **代码改动已提交** - 使用 `git commit -s`，多代理协作时添加 `Co-Authored-By: Agent`
2. **本地构建通过** - 执行上述构建命令
3. **相关测试通过** - 对应单元测试（`test/unittest`）与 fuzz 测试（`test/fuzztest`）通过
4. **板侧验证（如适用）** - 涉及人脸录入/认证/亮度/UI 等设备相关改动需提供验证证据
5. **文档更新（如适用）** - 公共 NAPI/ETS 接口或 README 修改需更新注释和文档

### 如果无法运行验证

明确说明无法运行的原因，列出推荐的验证步骤供人工执行，标记需要人工验证的部分。

### 完成报告格式

报告应包含：改动摘要（文件列表、改动点）、验证结果（构建/测试输出）、风险评估（API 兼容性、性能风险、隐私安全）、未完成事项。

## 架构概览

人脸认证能力以 SA（SystemAbility ID 942，进程 `useriam`）形式运行，主要链路如下：

- **上层接入**：应用/系统 UI 通过 NAPI（`@ohos.userIAM.faceAuth`）调用 `FaceAuthManager.setSurfaceId` 设置预览 surface；ETS 动画接口（`@ohos.userIAM.faceAuth` taihe IDL）提供 `setSurfaceId` 动画能力；`FaceAuthClient` 供客户端通过 `SetBufferProducer` 与 SA 交互。
- **IPC 层**：`face_auth_client_impl` 通过 `IFaceAuth` 代理访问 SA；SA 端 `face_auth_stub` 分发 `FACE_AUTH_SET_BUFFER_PRODUCER`（1）命令。
- **SA 服务**：`FaceAuthService` 启动时注册 HDI 驱动（`FaceAuthDriverHdi` + `FaceAuthInterfaceAdapter` 封装 `drivers_interface_face_auth` V3_0 HDI），并初始化 `ScreenBrightnessManager` 单例。
- **HDI/认证执行**：`FaceAuthAllInOneExecutorHdi` 实现 UserAuth 框架的 `IAuthExecutorHdi`，负责录入/认证/识别/删除/命令等，并将 HDI 结果通过 `FaceAuthExecutorCallbackHdi` 回调给 UserAuth 框架（含振动反馈）。
- **SA 命令链**：HDI 通过 `SaCommandCallback` 上报 SA 命令，`SaCommandManager` 按命令 ID 分发给已注册的 `ISaCommandProcessor`（如 `ScreenBrightnessManager`）。
- **屏幕亮度提升**：`ScreenBrightnessManager` 通过 `ServiceExManager` 按需加载 `services_ex` 动态库，`ScreenBrightnessTask` 基于 `FiniteStateMachine` 在单线程处理器（`ThreadHandler`）上执行亮度提升流程。
- **录入 UI**：`ui/Settings_FaceAuth` 提供人脸录入界面（entryView/enrollIntro/enrollDeclare/enrolling/faceConfig 等页面），通过 NAPI 的 `setSurfaceId` 将预览 surface 传给 SA。

## 项目约束

### 性能约束

- 摄像头预览/亮度调节/认证回包是高频路径，不要在每次回调或状态变化中增加全量扫描、字符串格式化或 INFO 日志。
- `Utils::Timer` 等定时器任务使用后必须释放（`Stop`/注销），避免资源泄漏。

### 架构约束

- `services`、`services_ex` 职责分离：`services_ex` 以按需加载的动态库方式提供（见 `ServiceExManager`），不直接耦合 SA 核心。
- HDI 类型转换（`ConvertXxx` 系列）集中在 `face_auth_all_in_one_executor_hdi`，不要在其它模块重复实现。
- 各 Manager 采用单例 + `NoCopyable`（如 `ScreenBrightnessManager`、`SaCommandManager`），遵循现有模式。

### 编码约定

- C++ 改动优先复用附近的 `IAM_LOG*`（`iam_logger.h`）、`IF_FALSE_LOGE_AND_RETURN*`（`iam_check.h`）、`MakeShared`/`MakeUnique`（`iam_ptr.h`）等项目宏与约定。
- 每个源文件需包含 `#define LOG_TAG "..."`，日志通过 `iam_logger.h` 的 `IAM_LOG*` 宏输出（自动携带文件名与行号）。
- 新增改动进行指针判空时，不要使用`CHKPV*`、`CHKPR*`、`CHKPC*`等改变代码逻辑的宏。

### 公共 API 约束

**Do not（禁止）：**
- 修改已发布的 NAPI、ETS、客户端接口（`frameworks/ipc/face_auth_client.h`）的签名、参数类型、返回值类型
- 修改已有 API 的错误码（`common/inc/face_auth_defines.h` 中 `FaceAuthRet`、`ResultCode`），除非明确标注为废弃
- 删除或重命名已有公共 API
- 修改已有 API 的行为语义（如异步变同步、阻塞变非阻塞）

**Ask before（修改前必须确认）：**
- 新增公共 API：确认是否需要 DFX 日志、事件追踪、权限检查
- 修改内部接口：评估是否影响跨模块兼容性（如 UserAuth 框架、协同认证框架）
- 修改错误处理逻辑：确认是否影响应用层的错误码兼容性

### 安全与权限边界

**Do not（禁止）：**
- 绕过已有的权限检查逻辑（如 `FaceAuthService::IsPermissionGranted`、`AccessTokenKit`/`TokenIdKit` 验证、`MANAGE_USER_IDM` 权限校验）
- 在未验证的情况下直接使用跨进程传递的 `IBufferProducer`/surface 句柄或共享内存
- 将敏感的人脸信息、surface 内容或生物特征数据写入非安全的日志（日志格式统一使用 `%{public}s`/`%{private}s` 标注）

**Ask before（修改前必须确认）：**
- 涉及 `Security::AccessToken` 相关代码的改动
- 涉及跨设备数据传输的改动
- 涉及认证结果/错误码语义变化的改动

### 协议与数据格式兼容性

**Do not（禁止）：**
- 修改 IPC 接口签名（`iface_auth.h` 的 `IFaceAuth`、`iface_auth_ipc_interface_code.h` 的命令码）与 Parcel 序列化顺序
- 修改跨进程传递的数据结构布局（如 `CaptureParam`、`ScheduleParam` 等 HDI 结构字段顺序）
- 修改已有的 HDI 交互协议与命令 ID（`SaCommandId`、`FaceAuthExtendCommandId` 等）

**Ask before（修改前必须确认）：**
- 新增 IPC/HDI 接口或命令：确认是否需要跨版本兼容性处理
- 修改事件/错误码：确认是否影响上层 UserAuth 框架或应用的兼容性

### 生成代码边界

**Do not（禁止）：**
- 直接修改 IDL 编译器生成的 C++ 代码文件（`frameworks/ets/ani` 的 taihe 生成产物、`drivers_interface_face_auth` 的 HDI 生成代码）
- 手动编辑生成的 IPC Proxy/Stub 代码

**正确做法：**
- 修改 IDL 定义文件（`frameworks/ets/ani/idl/*.taihe` 或 HDI 的 `.idl`）
- 重新运行 IDL 编译器生成代码
- 如果生成代码不满足需求，考虑调整 IDL 定义或使用回调机制

### 设备操作约束

**涉及真实设备时的注意事项：**
- 不执行可能影响设备正常运行的破坏性操作（如强制关闭摄像头、修改亮度到危险值）
- 需要在真实设备上验证的改动，必须提供板侧证据（日志、截图、hdc 输出）
- 摄像头节点/预览的打开关闭依赖南向 HDI 与系统相机能力，改动需遵循 `FaceAuthAllInOneExecutorHdi`/`ScreenBrightnessManager` 的生命周期管理
