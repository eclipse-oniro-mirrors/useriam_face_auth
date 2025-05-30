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

import window from '@ohos.window';

import BackComponent from '../component/backComponent';
import Log from '../utils/log';
import EntryViewController from '../controller/entryViewController';
import CommonController from '../controller/commonController';

@Entry
@Component
struct Index {
  private readonly TAG: string = 'Index';
  @State resultString: string = '';
  @StorageLink('SYSTEM_STATUS_BAR_HEIGHT') SYSTEM_STATUS_BAR_HEIGHT: number = 0;
  @StorageLink('SYSTEM_NAVIGATION_BAR_HEIGHT') SYSTEM_NAVIGATION_BAR_HEIGHT: number = 0;

  build() {
    Column() {
      BackComponent({ prevTag: this.TAG })
      Text('临时认证界面')
        .fontColor($r('sys.color.ohos_id_color_text_primary'))
        .fontSize($r('sys.float.ohos_id_text_size_headline7'))
        .fontWeight(FontWeight.Bold)
      Text('请将锁屏密码设置为123456后，点击认证按钮')
        .fontColor($r('sys.color.ohos_id_color_text_primary'))
        .fontSize($r('sys.float.ohos_id_text_size_sub_title1'))
        .fontWeight(FontWeight.Medium)
        .margin({
          top: '12',
          bottom: '24'
        })
      Button({ type: ButtonType.Capsule, stateEffect: true }) {
        Text('认证')
          .fontColor($r('sys.color.ohos_id_color_foreground_contrary'))
          .fontSize($r('sys.float.ohos_id_text_size_button1'))
          .fontWeight(FontWeight.Medium)
      }
      .backgroundColor($r('sys.color.ohos_id_color_activated'))
      .onClick(()=>{
        EntryViewController.doPINAuth()
          .catch(()=>{this.resultString = '认证失败，请将锁屏密码设置为123456后重试'})
      })
      .width('30%')
      Text(this.resultString)
      .fontColor($r('sys.color.ohos_id_color_text_primary'))
      .fontSize($r('sys.float.ohos_id_text_size_sub_title1'))
      .fontWeight(FontWeight.Medium)
    }
    .alignItems(HorizontalAlign.Center)
    .width('100%')
    .height('100%')
    .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
    .onAreaChange((_, newArea) => {
      try {
        window.getLastWindow(getContext(this))
          .then((windowClass) => {
            let type = window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR;
            let avoidArea = windowClass.getWindowAvoidArea(type);
            this.SYSTEM_NAVIGATION_BAR_HEIGHT = px2vp(avoidArea.bottomRect.height);

            type = window.AvoidAreaType.TYPE_SYSTEM;
            avoidArea = windowClass.getWindowAvoidArea(type);
            this.SYSTEM_STATUS_BAR_HEIGHT = px2vp(avoidArea.topRect.height);

            Log.info(this.TAG, `SYSTEM_STATUS_BAR_HEIGHT:${this.SYSTEM_STATUS_BAR_HEIGHT},
              SYSTEM_NAVIGATION_BAR_HEIGHT:${this.SYSTEM_NAVIGATION_BAR_HEIGHT}`);

            Log.info(this.TAG, 'new display area :' + JSON.stringify(newArea));
            CommonController.setDisplaySize(Number(newArea.width), Number(newArea.height));
            Log.info(this.TAG, 'total area w: ' + newArea.width + ' h: ' + newArea.height);
          })
          .catch(() => {
            Log.error(this.TAG, 'getLastWindow Fail');
          })
      } catch (err) {
        Log.error(this.TAG, `getLastWindow error, errCode:${err?.code}, errMessage:${err?.message}`);
      }
    })
  }

  onBackPress(): boolean {
    Log.info(this.TAG, 'onBackPress+');
    CommonController.routeBack();
    Log.info(this.TAG, 'onBackPress-');
    return true;
  }
}