/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
import Log from '../utils/log';

@CustomDialog
export default struct ConfirmDialog {
  private readonly TAG: string = 'ConfirmDialog';
  private controller?: CustomDialogController;
  private title: string | Resource = '';
  private content: string | Resource = '';
  private confirmText: string | Resource = '';
  private cancelText: string | Resource = '';
  private confirmOperation?: () => void;
  @StorageLink('POP_TYPE_WIDTH') POP_TYPE_WIDTH: number = 0;

  build() {
    Column() {
      Flex({ alignItems: ItemAlign.Center }) {
        Text(this.title)
          .fontColor($r('sys.color.ohos_id_color_text_primary'))
          .fontSize($r('sys.float.ohos_id_text_size_dialog_tittle'))
          .fontWeight(FontWeight.Medium)
      }
        .height('64vp')
        .margin({
          left: '32vp',
          right: '32vp',
        })
      Text(this.content)
        .fontColor($r('sys.color.ohos_id_color_text_primary'))
        .fontSize($r('sys.float.ohos_id_text_size_body1'))
        .margin({
          left: '32vp',
          right: '32vp',
          bottom: '8vp'
        })
      Flex({ justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Text(this.cancelText)
          .fontColor($r('sys.color.ohos_id_color_emphasize_transparent'))
          .fontSize($r('sys.float.ohos_id_text_size_button1'))
          .fontWeight(FontWeight.Medium)
          .flexGrow(1)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            Log.info(this.TAG, 'click cancel+')
            if (this.controller !== null && this.controller !== undefined) {
              this.controller.close()
            }
            Log.info(this.TAG, 'click cancel-')
          })
        Divider()
          .vertical(true)
          .strokeWidth('2vp')
          .color($r('sys.color.ohos_id_color_list_separator'))
          .height('24dp')
          .margin({ left: '4vp', right: '4vp'})
        Text(this.confirmText)
          .fontColor($r('sys.color.ohos_id_color_warning'))
          .fontSize($r('sys.float.ohos_id_text_size_button1'))
          .fontWeight(FontWeight.Medium)
          .flexGrow(1)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            Log.info(this.TAG, 'click confirm+')
            if (this.confirmOperation !== undefined) {
              this.confirmOperation()
            }
            Log.info(this.TAG, 'click confirm-')
            if (this.controller !== undefined) {
              this.controller.close()
            }
          })
      }
      .height('40vp')
      .margin({
        left: '16vp',
        right: '16vp',
        bottom : '16vp'
      })
    }
    .borderRadius('24vp')
    .backgroundColor($r('sys.color.ohos_id_color_dialog_bg'))
    .alignItems(HorizontalAlign.Start)
  }
}