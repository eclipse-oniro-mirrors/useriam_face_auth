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
import router from '@system.router';
import CommonController from '../controller/commonController';
import UserIdmModel from '../model/userIdmModel';
import FaceAuthModel from '../model/faceAuthModel';

class EnrollingController {
  private readonly TAG: string = 'EnrollingController';
  private readonly ANIMATION_TIME: number = 2000;
  private enrolling: boolean = false;

  constructor() {
    Log.info(this.TAG, 'constructor+');
    Log.info(this.TAG, 'constructor-');
  }

  init(): void {
    Log.info(this.TAG, 'init+');
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Hidden);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('stackProgressValue', 0);
    AppStorage.Set('stackShelterHeight', AppStorage.Get('stackShelterHeightBegin'));
    AppStorage.Set('enrollTip', $r('app.string.face_intro'));
    AppStorage.Set('enrollTipSize', $r('sys.float.ohos_id_text_size_sub_title1'));
    AppStorage.Set('stackVideoBlurRadius', 0);

    AppStorage.Set('enrollStatus', $r('app.string.enrolling'));
    this.startEnroll();
    Log.info(this.TAG, 'init-');
  }

  async startEnroll(): Promise<void> {
    Log.info(this.TAG, 'startEnroll+');
    if (AppStorage.Get('xComponentSurfaceId') === '') {
      Log.info(this.TAG, 'surface id is not set, skip start enroll');
      return;
    }
    if (this.enrolling) {
      Log.info(this.TAG, 'face is enrolling, skip start enroll');
      return;
    }
    await FaceAuthModel.setSurfaceId(AppStorage.Get('xComponentSurfaceId'));
    UserIdmModel.setOnAcquireCallback(this.onAcquire.bind(this));
    UserIdmModel.enrollFace().then(this.processResult.bind(this));
    this.enrolling = true;
    Log.info(this.TAG, 'startEnroll-');
  }

  async processResult(result : number): Promise<void> {
    await this.faceDetected();
    await this.enrollProcessing();
    if (result === 0) {
      await this.enrollSuccess();
    } else {
      await this.enrollFail();
    }
    this.enrolling = false;
  }

  async onAcquire(result: number): Promise<void> {
    Log.info(this.TAG, 'onAcquire+ result: ' + result);
    const NEED_FACE_DETECTED = 25;
    if (result === NEED_FACE_DETECTED) {
      Log.info(this.TAG, 'onAcquire face detected+');
      this.faceDetected();
      Log.info(this.TAG, 'onAcquire face detected-');
    }
    Log.info(this.TAG, 'onAcquire-');
  }

  async faceDetected(): Promise<void> {
    Log.info(this.TAG, 'faceDetected+');
    AppStorage.Set('animationTime', this.ANIMATION_TIME);
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('stackShelterHeight', AppStorage.Get('stackShelterHeightEnd'));
    await CommonController.sleepMS(this.ANIMATION_TIME);
    AppStorage.Set('animationTime', 0);
    Log.info(this.TAG, 'faceDetected-');
  }

  async enrollProcessing(): Promise<void> {
    Log.info(this.TAG, 'enrollProcessing+');
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Hidden);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);
    
    const PERCENTAGE_100 = 100;
    for (let i = 0; i <= PERCENTAGE_100; i += 1) {
      const SLEEP_TIME = 30;
      await CommonController.sleepMS(SLEEP_TIME);
      AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
      AppStorage.Set('stackProgressValue', i);
      AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    }

    Log.info(this.TAG, 'enrollProcessing-');
  }

  async enrollSuccess(): Promise<void> {
    Log.info(this.TAG, 'enrollSuccess+');
    AppStorage.Set('stackVideoVisibility', Visibility.Hidden);
    AppStorage.Set('stackShelterVisibility', Visibility.Hidden);
    AppStorage.Set('stackProgressVisibility', Visibility.Hidden);
    AppStorage.Set('stackSuccessVisibility', Visibility.Visible);
    AppStorage.Set('enrollTipVisibility', Visibility.Hidden);
    AppStorage.Set('enrollButtonVisibility', Visibility.Hidden);

    AppStorage.Set('enrollStatus', $r('app.string.enroll_success'));
    const SLEEP_TIME = 3000;
    await CommonController.sleepMS(SLEEP_TIME);
    router.replace({uri: 'pages/faceConfig'});
    Log.info(this.TAG, 'enrollSuccess-');
  }

  async enrollFail(): Promise<void> {
    Log.info(this.TAG, 'enrollFail+');
    AppStorage.Set('stackVideoVisibility', Visibility.Visible);
    AppStorage.Set('stackShelterVisibility', Visibility.Visible);
    AppStorage.Set('stackProgressVisibility', Visibility.Visible);
    AppStorage.Set('stackSuccessVisibility', Visibility.Hidden);
    AppStorage.Set('enrollTipVisibility', Visibility.Visible);
    AppStorage.Set('enrollButtonVisibility', Visibility.Visible);

    AppStorage.Set('enrollStatus', $r('app.string.enrolling_fail'));
    AppStorage.Set('enrollTip', $r('app.string.enroll_info_fail'));
    AppStorage.Set('enrollTipSize', $r('sys.float.ohos_id_text_size_body1'));
    const STACK_VIDEO_BLUR_RADIUS = 30;
    AppStorage.Set('stackVideoBlurRadius', STACK_VIDEO_BLUR_RADIUS);
    AppStorage.Set('stackProgressValue', 0);

    Log.info(this.TAG, 'enrollFail-');
  }

  async clear(): Promise<void> {
    Log.info(this.TAG, 'clear+');
    await FaceAuthModel.clearSurfaceId();
    Log.info(this.TAG, 'clear-');
  }
}

let enrollingController = new EnrollingController();
export default enrollingController as EnrollingController;