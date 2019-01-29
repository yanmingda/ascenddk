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
#include "object_detection_post.h"

using namespace hiai;
using namespace std;
using ascend::utils::DvppCropOrResizePara;
using ascend::utils::DvppProcess;

class ObjectDetectionPostUTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() { GlobalMockObject::verify(); }
};

TEST_F(ObjectDetectionPostUTest, init_success) {
  ObjectDetectionPostProcess post_engine;
  vector<hiai::AIModelDescription> od_model_descs;
  AIConfig config;
  AIConfigItem* model_path = config.add_items();
  if (model_path == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path->set_name("Confidence");
  model_path->set_value("0.8");
  HIAI_StatusT ret = post_engine.Init(config, od_model_descs);
  EXPECT_EQ(HIAI_OK, ret);
}
TEST_F(ObjectDetectionPostUTest, init_not_float) {
  ObjectDetectionPostProcess post_engine;
  vector<hiai::AIModelDescription> od_model_descs;
  AIConfig config;
  AIConfigItem* model_path = config.add_items();
  if (model_path == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path->set_name("Confidence");
  model_path->set_value("efr3r");
  HIAI_StatusT ret = post_engine.Init(config, od_model_descs);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, init_invalid_value) {
  ObjectDetectionPostProcess post_engine;
  vector<hiai::AIModelDescription> od_model_descs;
  AIConfig config;
  AIConfigItem* model_path = config.add_items();
  if (model_path == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path->set_name("Confidence");
  model_path->set_value("-1");
  HIAI_StatusT ret = post_engine.Init(config, od_model_descs);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, send_data_success) {
  ObjectDetectionPostProcess post_engine;
  MOCKER_CPP(&Engine::SendData)
      .stubs()
      .will(returnValue(HIAI_QUEUE_FULL))
      .then(returnValue(HIAI_OK))
      .then(returnValue(3));
  shared_ptr<BatchCroppedImageParaT> croppedImage =
      make_shared<BatchCroppedImageParaT>();
  HIAI_StatusT ret = post_engine.SendResults(
      0, "BatchCroppedImageParaT", static_pointer_cast<void>(croppedImage));
  EXPECT_EQ(HIAI_OK, ret);
  ret = post_engine.SendResults(0, "BatchCroppedImageParaT",
                                static_pointer_cast<void>(croppedImage));
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, dvpp_failed) {
  ObjectDetectionPostProcess post_engine;

  ImageData<u_int8_t> img;
  ImageData<u_int8_t> target_img;
  img.height = 10;
  img.width = 10;
  img.format = IMAGEFORMAT::YUV420SP;
  img.size = 10 * 10 * 3 / 2;
  img.data = shared_ptr<u_int8_t>(new u_int8_t[img.size],
                                  default_delete<u_int8_t[]>());
  BoundingBox bbox = {0, 0, 2, 2};

  MOCKER_CPP(&DvppProcess::DvppOperationProc).stubs().will(returnValue(-1));

  HIAI_StatusT ret = post_engine.CropObjectFromImage(img, target_img, bbox);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, dvpp_success) {
  ObjectDetectionPostProcess post_engine;

  ImageData<u_int8_t> img;
  ImageData<u_int8_t> target_img;
  img.height = 10;
  img.width = 10;
  img.format = IMAGEFORMAT::YUV420SP;
  img.size = 10 * 10 * 3 / 2;
  img.data = shared_ptr<u_int8_t>(new u_int8_t[img.size],
                                  default_delete<u_int8_t[]>());
  BoundingBox bbox = {0, 0, 2, 2};

  HIAI_StatusT ret = post_engine.CropObjectFromImage(img, target_img, bbox);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(ObjectDetectionPostUTest, send_obj_img) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<DetectionEngineTransT> input_data =
      make_shared<DetectionEngineTransT>();
  vector<ObjectImageParaT> cropped_images;
  VideoImageInfoT video_image_info = {1, 1, "channel_1", false};
  post_engine.SendCroppedImages(0, cropped_images, video_image_info);

  MOCKER_CPP(&ObjectDetectionPostProcess::SendResults)
      .stubs()
      .will(returnValue(3))
      .then(returnValue(HIAI_OK));
  ObjectImageParaT object_image;

  object_image.object_info = {"car_1", 0.8};
  object_image.img.height = 10;
  object_image.img.width = 10;
  object_image.img.format = IMAGEFORMAT::YUV420SP;
  object_image.img.size = 10 * 10 * 3 / 2;
  object_image.img.data = shared_ptr<u_int8_t>(
      new u_int8_t[object_image.img.size], default_delete<u_int8_t[]>());

  cropped_images.push_back(object_image);
  post_engine.SendCroppedImages(0, cropped_images, video_image_info);

  post_engine.SendCroppedImages(0, cropped_images, video_image_info);
}

TEST_F(ObjectDetectionPostUTest, bbox_filter) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<VideoDetectionImageParaT> detection_image =
      make_shared<VideoDetectionImageParaT>();
  vector<ObjectImageParaT> car_type_imgs;
  vector<ObjectImageParaT> car_color_imgs;
  vector<ObjectImageParaT> person_imgs;

  int32_t res_size = 7;
  float res_failed[res_size] = {2.0, 0.0, 1.1, 0.0, 0.0, 0.5, 0.5};
  float res_car[res_size] = {1.0, 3, 0.92, 0.0, -0.1, 0.5, 1.1};
  float res_person[res_size] = {1.0, 1, 0.92, 0.0, -0.1, 0.5, 1.1};
  float res_bus[res_size] = {1.0, 6, 0.93, 0.0, -0.1, 0.5, 1.1};

  post_engine.FilterBoundingBox(res_failed, res_size, detection_image,
                                car_type_imgs, car_color_imgs, person_imgs);

  post_engine.FilterBoundingBox(res_car, res_size, detection_image,
                                car_type_imgs, car_color_imgs, person_imgs);
  post_engine.FilterBoundingBox(res_person, res_size, detection_image,
                                car_type_imgs, car_color_imgs, person_imgs);
  post_engine.FilterBoundingBox(res_bus, res_size, detection_image,
                                car_type_imgs, car_color_imgs, person_imgs);
  MOCKER_CPP(&ObjectDetectionPostProcess::CropObjectFromImage)
      .stubs()
      .will(returnValue(3));
  post_engine.FilterBoundingBox(res_person, res_size, detection_image,
                                car_type_imgs, car_color_imgs, person_imgs);
  EXPECT_EQ(1, car_type_imgs.size());
  EXPECT_EQ(2, car_color_imgs.size());
  EXPECT_EQ(1, person_imgs.size());
}

TEST_F(ObjectDetectionPostUTest, input_null) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<void> arg0 = nullptr;
  HIAI_StatusT ret = post_engine.Process(arg0);

  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, input_finished) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<DetectionEngineTransT> input_data =
      make_shared<DetectionEngineTransT>();
  input_data->video_image.video_image_info.is_finished = true;
  shared_ptr<void> arg0 = static_pointer_cast<void>(input_data);
  HIAI_StatusT ret = post_engine.Process(arg0);
  EXPECT_EQ(HIAI_OK, ret);
  input_data->video_image.video_image_info.is_finished = false;
  input_data->status = false;
  shared_ptr<void> arg1 = static_pointer_cast<void>(input_data);
  ret = post_engine.Process(arg1);
  EXPECT_EQ(HIAI_ERROR, ret);
  input_data->status = true;
  shared_ptr<void> arg2 = static_pointer_cast<void>(input_data);
  ret = post_engine.Process(arg2);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, inference_nullptr) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<DetectionEngineTransT> input_data =
      make_shared<DetectionEngineTransT>();
  input_data->status = true;
  input_data->video_image.video_image_info.is_finished = false;

  OutputT out_bbox = {0, "bbox", nullptr};
  OutputT out_num = {0, "num", nullptr};
  input_data->output_datas.push_back(out_bbox);
  input_data->output_datas.push_back(out_num);
  shared_ptr<void> arg0 = static_pointer_cast<void>(input_data);
  HIAI_StatusT ret = post_engine.Process(arg0);

  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(ObjectDetectionPostUTest, inference_success) {
  ObjectDetectionPostProcess post_engine;
  shared_ptr<DetectionEngineTransT> input_data =
      make_shared<DetectionEngineTransT>();
  input_data->status = true;
  input_data->video_image.video_image_info.is_finished = false;
  float bbox_num = 1;
  OutputT out_bbox = {
      7, "bbox",
      shared_ptr<uint8_t>(new uint8_t[7], default_delete<uint8_t[]>())};
  OutputT out_num = {
      1, "num",
      shared_ptr<uint8_t>(new uint8_t[4], default_delete<uint8_t[]>())};
  input_data->output_datas.push_back(out_bbox);
  input_data->output_datas.push_back(out_num);
  shared_ptr<void> arg0 = static_pointer_cast<void>(input_data);
  MOCKER_CPP(&ObjectDetectionPostProcess::SendResults)
      .stubs()
      .will(returnValue(3))
      .then(returnValue(HIAI_OK));
  HIAI_StatusT ret = post_engine.Process(arg0);
  EXPECT_EQ(HIAI_OK, ret);
  ret = post_engine.Process(arg0);
  EXPECT_EQ(HIAI_OK, ret);
}