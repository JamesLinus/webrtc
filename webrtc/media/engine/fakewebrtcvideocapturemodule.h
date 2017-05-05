/*
 *  Copyright (c) 2004 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MEDIA_ENGINE_FAKEWEBRTCVIDEOCAPTUREMODULE_H_
#define WEBRTC_MEDIA_ENGINE_FAKEWEBRTCVIDEOCAPTUREMODULE_H_

#include <vector>

#include "webrtc/api/video/i420_buffer.h"
#include "webrtc/media/base/testutils.h"
#include "webrtc/media/engine/webrtcvideocapturer.h"

class FakeWebRtcVcmFactory;

// Fake class for mocking out webrtc::VideoCaptureModule.
class FakeWebRtcVideoCaptureModule : public webrtc::VideoCaptureModule {
 public:
  FakeWebRtcVideoCaptureModule(FakeWebRtcVcmFactory* factory)
      : factory_(factory),
        callback_(NULL),
        running_(false) {
  }
  ~FakeWebRtcVideoCaptureModule();
  void RegisterCaptureDataCallback(
      rtc::VideoSinkInterface<webrtc::VideoFrame>* callback) override {
    callback_ = callback;
  }
  void DeRegisterCaptureDataCallback() override { callback_ = NULL; }
  int32_t StartCapture(const webrtc::VideoCaptureCapability& cap) override {
    if (running_) return -1;
    cap_ = cap;
    running_ = true;
    return 0;
  }
  int32_t StopCapture() override {
    running_ = false;
    return 0;
  }

  bool SuspendCapture() override {
    // not implemented
    return false;
  }

  bool ResumeCapture() override {
    // not implemented
    return false;
  }

  bool IsSuspended() override {
    // not implemented
    return false;
  }

  const char* CurrentDeviceName() const override {
    return NULL;  // not implemented
  }
  bool CaptureStarted() override { return running_; }
  int32_t CaptureSettings(webrtc::VideoCaptureCapability& settings) override {
    if (!running_) return -1;
    settings = cap_;
    return 0;
  }

  int32_t SetCaptureRotation(webrtc::VideoRotation rotation) override {
    return -1;  // not implemented
  }
  bool SetApplyRotation(bool enable) override {
    return true;  // ignored
  }
  bool GetApplyRotation() override {
    return true;  // Rotation compensation is turned on.
  }
  void SendFrame(int w, int h) {
    if (!running_) return;

    rtc::scoped_refptr<webrtc::I420Buffer> buffer =
        webrtc::I420Buffer::Create(w, h);
    // Initialize memory to satisfy DrMemory tests. See
    // https://bugs.chromium.org/p/libyuv/issues/detail?id=377
    buffer->InitializeData();
    if (callback_) {
      callback_->OnFrame(
          webrtc::VideoFrame(buffer, 0, 0, webrtc::kVideoRotation_0));
    }
  }

  const webrtc::VideoCaptureCapability& cap() const {
    return cap_;
  }

 private:
  FakeWebRtcVcmFactory* factory_;
  rtc::VideoSinkInterface<webrtc::VideoFrame>* callback_;
  bool running_;
  webrtc::VideoCaptureCapability cap_;
};

#endif  // WEBRTC_MEDIA_ENGINE_FAKEWEBRTCVIDEOCAPTUREMODULE_H_
