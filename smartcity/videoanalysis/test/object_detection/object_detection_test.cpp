/**
 * ============================================================================
 *
 * Copyright (C) 2018, Hisilicon Technologies Co., Ltd. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1 Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   2 Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   3 Neither the names of the copyright holders nor the names of the
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */
#include <gtest/gtest.h>
#include <memory>
#include <mockcpp/mockcpp.hpp>
#include "hiaiengine/data_type.h"
#include "hiaiengine/status.h"
#define private public
#define protected public
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"
#include "object_detection.h"

using namespace hiai;
using namespace std;
using ascend::utils::DvppCropOrResizePara;
using ascend::utils::DvppProcess;

class ObjectDetectionUTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() { GlobalMockObject::verify(); }
};

TEST_F(ObjectDetectionUTest, init) {
  ObjectDetectionInferenceEngine object_detection;
  vector<hiai::AIModelDescription> od_model_descs;
  AIConfig config;
  AIConfigItem *model_path = config.add_items();
  if (model_path == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path->set_name("model_path");
  model_path->set_value("/path/mock");

  AIModelManager model_manager;
  MOCKER_CPP_VIRTUAL(&model_manager, &AIModelManager::Init)
      .stubs()
      .will(returnValue(hiai::FAILED))
      .then(returnValue(hiai::SUCCESS));

  HIAI_StatusT ret = object_detection.Init(config, od_model_descs);
  EXPECT_EQ(HIAI_ERROR, ret);
  ret = object_detection.Init(config, od_model_descs);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionUTest, dvpp_failed) {
  ObjectDetectionInferenceEngine object_detection;

  ImageData<u_int8_t> src_img;
  ImageData<u_int8_t> resized_img;
  src_img.format = IMAGEFORMAT::RGB888;
  HIAI_StatusT ret = object_detection.ImagePreProcess(src_img, resized_img);
  EXPECT_EQ(HIAI_ERROR, ret);
  src_img.format = IMAGEFORMAT::YUV420SP;
  MOCKER_CPP(&DvppProcess::DvppOperationProc).stubs().will(returnValue(-1));
  ret = object_detection.ImagePreProcess(src_img, resized_img);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionUTest, preprocess_success) {
  ObjectDetectionInferenceEngine object_detection;

  ImageData<u_int8_t> src_img;
  ImageData<u_int8_t> resized_img;
  src_img.format = IMAGEFORMAT::YUV420SP;
  HIAI_StatusT ret = object_detection.ImagePreProcess(src_img, resized_img);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionUTest, inference_failed) {
  ObjectDetectionInferenceEngine object_detection;
  object_detection.ai_model_manager_ = make_shared<AIModelManager>();
  shared_ptr<DetectionEngineTransT> detection_trans =
      make_shared<DetectionEngineTransT>();
  ImageData<u_int8_t> img;
  img.width = 10;
  img.height = 10;
  img.size = 10 * 10 * 3 / 2;
  img.data = shared_ptr<u_int8_t>(new u_int8_t[img.size],
                                  default_delete<u_int8_t[]>());

  AIModelManager model_manager;
  MOCKER_CPP_VIRTUAL(&model_manager, &AIModelManager::CreateOutputTensor)
      .stubs()
      .will(returnValue(hiai::FAILED))
      .then(returnValue(hiai::SUCCESS));

  HIAI_StatusT ret = object_detection.PerformInference(detection_trans, img);
  EXPECT_EQ(HIAI_ERROR, ret);

  MOCKER_CPP_VIRTUAL(&model_manager, &AIModelManager::Process)
      .stubs()
      .will(returnValue(hiai::FAILED));
  ret = object_detection.PerformInference(detection_trans, img);
  EXPECT_EQ(HIAI_ERROR, ret);
  ret = object_detection.PerformInference(detection_trans, img);
  EXPECT_EQ(HIAI_ERROR, ret);
}

AIStatus MockProcessSuccessStub(hiai::AIModelManager *model_manager,
                                AIContext &context,
                                const vector<shared_ptr<IAITensor>> &in_data,
                                vector<shared_ptr<IAITensor>> &out_data,
                                uint32_t timeout) {
  shared_ptr<AISimpleTensor> data = make_shared<AISimpleTensor>();
  out_data.emplace_back(data);
  return hiai::SUCCESS;
}

TEST_F(ObjectDetectionUTest, inference_success) {
  ObjectDetectionInferenceEngine object_detection;
  object_detection.ai_model_manager_ = make_shared<AIModelManager>();
  shared_ptr<DetectionEngineTransT> detection_trans =
      make_shared<DetectionEngineTransT>();
  ImageData<u_int8_t> img;
  img.width = 10;
  img.height = 10;
  img.size = 10 * 10 * 3 / 2;
  img.data = shared_ptr<u_int8_t>(new u_int8_t[img.size],
                                  default_delete<u_int8_t[]>());

  AIModelManager model_manager;
  MOCKER_CPP_VIRTUAL(&model_manager, &AIModelManager::CreateOutputTensor)
      .stubs()
      .then(returnValue(hiai::SUCCESS));

  MOCKER_CPP_VIRTUAL(&model_manager, &AIModelManager::Process)
      .stubs()
      .will(invoke(MockProcessSuccessStub));

  MOCKER(memcpy_s).stubs().will(returnValue(-1)).then(returnValue(EOK));

  HIAI_StatusT ret = object_detection.PerformInference(detection_trans, img);
  EXPECT_EQ(HIAI_ERROR, ret);
  ret = object_detection.PerformInference(detection_trans, img);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionUTest, send_data_success) {
  ObjectDetectionInferenceEngine object_detection;

  shared_ptr<DetectionEngineTransT> detection_trans =
      make_shared<DetectionEngineTransT>();

  MOCKER_CPP(&Engine::SendData)
      .stubs()
      .will(returnValue(HIAI_QUEUE_FULL))
      .then(returnValue(HIAI_OK));

  HIAI_StatusT ret =
      object_detection.SendDetectionResult(detection_trans, false, "error msg");
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionUTest, send_data_error) {
  ObjectDetectionInferenceEngine object_detection;

  shared_ptr<DetectionEngineTransT> detection_trans =
      make_shared<DetectionEngineTransT>();

  MOCKER_CPP(&Engine::SendData).stubs().will(returnValue(3));
  HIAI_StatusT ret =
      object_detection.SendDetectionResult(detection_trans, false, "error msg");
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionUTest, init_queue) {
  ObjectDetectionInferenceEngine object_detection;
  object_detection.InitMsgQueue();
}

TEST_F(ObjectDetectionUTest, process_void) {
  ObjectDetectionInferenceEngine object_detection;
  shared_ptr<void> arg0 = nullptr;
  shared_ptr<void> arg1 = nullptr;
  HIAI_StatusT ret = object_detection.Process(arg0, arg1);

  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionUTest, finished) {
  ObjectDetectionInferenceEngine object_detection;
  shared_ptr<VideoImageParaT> input_img = make_shared<VideoImageParaT>();
  input_img->video_image_info.is_finished = true;
  shared_ptr<void> arg1 = nullptr;
  shared_ptr<void> arg0 = static_pointer_cast<void>(input_img);
  HIAI_StatusT ret = object_detection.Process(arg0, arg1);
  MOCKER_CPP(&ObjectDetectionInferenceEngine::SendDetectionResult)
      .stubs()
      .will(returnValue(HIAI_OK));
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionUTest, resize_failed) {
  ObjectDetectionInferenceEngine object_detection;
  shared_ptr<VideoImageParaT> input_img = make_shared<VideoImageParaT>();
  input_img->video_image_info.is_finished = false;
  shared_ptr<void> arg1 = nullptr;
  shared_ptr<void> arg0 = static_pointer_cast<void>(input_img);
  HIAI_StatusT ret = object_detection.Process(arg0, arg1);
  MOCKER_CPP(&ObjectDetectionInferenceEngine::ImagePreProcess)
      .stubs()
      .will(returnValue(3));
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionUTest, process_success) {
  ObjectDetectionInferenceEngine object_detection;
  shared_ptr<void> arg0 = make_shared<VideoImageParaT>();
  shared_ptr<void> arg1 = nullptr;
  MOCKER_CPP(&ObjectDetectionInferenceEngine::ImagePreProcess)
      .stubs()
      .will(returnValue(HIAI_OK));
  MOCKER_CPP(&ObjectDetectionInferenceEngine::PerformInference)
      .stubs()
      .will(returnValue(HIAI_OK));

  HIAI_StatusT ret = object_detection.Process(arg0, arg1);

  EXPECT_EQ(HIAI_OK, ret);
}
