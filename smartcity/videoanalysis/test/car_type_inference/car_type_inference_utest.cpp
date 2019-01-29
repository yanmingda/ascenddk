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
#include "mockcpp/mockcpp.hpp"

#include <memory>
#include "hiaiengine/data_type.h"
#include "hiaiengine/status.h"

#define private public
#define protected public
#include "car_type_inference.h"

using namespace hiai;

/**
 * @brief: face detection inference engine UT test
 */
class CarTypeInferenceUTest : public testing::Test {
protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {
    GlobalMockObject::verify();
  }
};

/**
 * @brief: test Init function
 * model_path
 */
TEST_F(CarTypeInferenceUTest, init_modelpath) {
  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //model_path item
  hiai::AIConfigItem* model_path_item = config.add_items();
  if (nullptr == model_path_item) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");

  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::FAILED)).then(returnValue(hiai::SUCCESS));

  // first time mock AIModelManager::Init failed
  // should return HIAI_ERROR
  HIAI_StatusT ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);

  // second time mock AIModelManager::Init success
  // should return HIAI_OK
  ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_OK, ret);
}

/**
 * @brief: test Init function
 * model_path
 */
TEST_F(CarTypeInferenceUTest, init_passcode) {
  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //model_path item
  hiai::AIConfigItem* model_path_item = config.add_items();
  if (nullptr == model_path_item) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path_item->set_name("passcode");
  model_path_item->set_value("");

  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::FAILED)).then(returnValue(hiai::SUCCESS));

  // first time mock AIModelManager::Init failed
  // should return HIAI_ERROR
  HIAI_StatusT ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);

  // second time mock AIModelManager::Init success
  // should return HIAI_OK
  ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_OK, ret);
}

/**
 * @brief: test Init function
 * batch_size
 */
TEST_F(CarTypeInferenceUTest, init_batch_size) {
  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //model_path item
  hiai::AIConfigItem* model_path_item = config.add_items();
  if (nullptr == model_path_item) {
    EXPECT_EQ(0, HIAI_OK);
  }
  model_path_item->set_name("batch_size");
  model_path_item->set_value("10");

  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::FAILED)).then(returnValue(hiai::SUCCESS));

  // first time mock AIModelManager::Init failed
  // should return HIAI_ERROR
  HIAI_StatusT ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);

  // second time mock AIModelManager::Init success
  // should return HIAI_OK
  ret = cartype_inference_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_OK, ret);
}

/**
 * @brief: test SendResultData function
 */
TEST_F(CarTypeInferenceUTest, SendResultData) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  tran_data->video_image_info.channel_id = "car1";
  tran_data->video_image_info.frame_id = 1;
  tran_data->video_image_info.channel_name = "car.mp4";
  tran_data->video_image_info.is_finished = false;

  MOCKER_CPP(&Engine::SendData).stubs().will(returnValue(HIAI_QUEUE_FULL)).then(
      returnValue(HIAI_OK)).then(returnValue(3));

  HIAI_StatusT ret = cartype_inference_test.SendResultData(tran_data);
  EXPECT_EQ(HIAI_OK, ret);

  ret = cartype_inference_test.SendResultData(tran_data);
  EXPECT_EQ(HIAI_ERROR, ret);
}

/**
 * @brief: test BatchImageResize function
 */
TEST_F(CarTypeInferenceUTest,BatchImageResize) {

  std::shared_ptr<BatchCroppedImageParaT> image_output = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCroppedImageParaT> image_input = std::make_shared<
      BatchCroppedImageParaT>();
  image_input->video_image_info.channel_id = "car1";
  image_input->video_image_info.frame_id = 1;
  image_input->video_image_info.channel_name = "car.mp4";
  image_input->video_image_info.is_finished = false;

  for (int i = 0; i < 10; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_input->obj_imgs.push_back(obj_image);
  }
  CarTypeInferenceEngine cartype_inference_test;

  cartype_inference_test.BatchImageResize(image_input, image_output);

  EXPECT_EQ(image_input->video_image_info.channel_id,
            image_output->video_image_info.channel_id);
  EXPECT_EQ(image_input->video_image_info.frame_id,
            image_output->video_image_info.frame_id);
  EXPECT_EQ(image_input->video_image_info.channel_name,
            image_output->video_image_info.channel_name);
  EXPECT_EQ(image_input->video_image_info.is_finished,
            image_output->video_image_info.is_finished);
  EXPECT_EQ(image_input->obj_imgs.size(), image_input->obj_imgs.size());
}

/**
 * @brief: test BatchImageResize function
 */
TEST_F(CarTypeInferenceUTest,BatchImageResize_DVPP_fail) {

  std::shared_ptr<BatchCroppedImageParaT> image_output = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCroppedImageParaT> image_input = std::make_shared<
      BatchCroppedImageParaT>();
  image_input->video_image_info.channel_id = "car1";
  image_input->video_image_info.frame_id = 1;
  image_input->video_image_info.channel_name = "car.mp4";
  image_input->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_input->obj_imgs.push_back(obj_image);
  }
  CarTypeInferenceEngine cartype_inference_test;

  MOCKER_CPP(&ascend::utils::DvppProcess::DvppOperationProc).stubs().will(
      returnValue(-1));

  cartype_inference_test.BatchImageResize(image_input, image_output);
}

/**
 * @brief: test ConstructBatchBuffer function
 */
TEST_F(CarTypeInferenceUTest,ConstructBatchBuffer) {
  int batch_index = 0;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  uint8_t* buffer = new uint8_t[10 * 224 * 224];

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER(memcpy_s).stubs()
  .will(returnValue(EOK))
  .then(returnValue(-1));

  CarTypeInferenceEngine cartype_inference_test;

  bool ret = cartype_inference_test.ConstructBatchBuffer(batch_index,
                                                         image_handle, buffer);
  EXPECT_EQ(true, ret);

  ret = cartype_inference_test.ConstructBatchBuffer(batch_index, image_handle,
                                                    buffer);
  EXPECT_EQ(false, ret);

  MOCKER(memset_s).stubs()
  .will(returnValue(-1));

  ret = cartype_inference_test.ConstructBatchBuffer(batch_index, image_handle,
                                                    buffer);
  EXPECT_EQ(false, ret);

  delete[] buffer;
}

/**
 * @brief: test ConstructBatchBuffer function
 */
TEST_F(CarTypeInferenceUTest,memset_s_fail) {
  int batch_index = 0;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  uint8_t* buffer = new uint8_t[10 * 224 * 224];

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER(memset_s).stubs()
  .will(returnValue(-1));

  CarTypeInferenceEngine cartype_inference_test;

  bool ret = cartype_inference_test.ConstructBatchBuffer(batch_index,
                                                         image_handle, buffer);
  EXPECT_EQ(false, ret);

  delete[] buffer;
}

/**
 * @brief: test ConstructInferenceResult function
 */
TEST_F(CarTypeInferenceUTest,ConstructInferenceResult) {
  int batch_index = 0;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::vector<std::shared_ptr<hiai::IAITensor> > output_data_vec;
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  uint8_t* buffer = new uint8_t[4310 * sizeof(float)];

  MOCKER_CPP(&hiai::AINeuralNetworkBuffer::GetSize).stubs().will(
      returnValue(4310 * sizeof(float)));

  MOCKER_CPP(&hiai::AINeuralNetworkBuffer::GetBuffer).stubs().will(
      returnValue((void *) buffer));

  for (int j = 0; j < 1; j++) {
    std::shared_ptr<hiai::AINeuralNetworkBuffer> output_data = std::make_shared<
        hiai::AINeuralNetworkBuffer>();
    output_data_vec.push_back(output_data);
  }

  CarTypeInferenceEngine cartype_inference_test;

  bool ret = cartype_inference_test.ConstructInferenceResult(output_data_vec,
                                                             batch_index,
                                                             image_handle,
                                                             tran_data);
  EXPECT_EQ(true, ret);

  delete[] buffer;
}

/**
 * @brief: test ConstructInferenceResult function
 */
TEST_F(CarTypeInferenceUTest,output_data_vec_null) {
  int batch_index = 0;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::vector<std::shared_ptr<hiai::IAITensor> > output_data_vec;
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  uint8_t* buffer = new uint8_t[4310 * sizeof(float)];

  MOCKER_CPP(&hiai::AINeuralNetworkBuffer::GetSize).stubs().will(
      returnValue(4310 * sizeof(float)));

  MOCKER_CPP(&hiai::AINeuralNetworkBuffer::GetBuffer).stubs().will(
      returnValue((void *) buffer));

  CarTypeInferenceEngine cartype_inference_test;

  bool ret = cartype_inference_test.ConstructInferenceResult(output_data_vec,
                                                             batch_index,
                                                             image_handle,
                                                             tran_data);
  EXPECT_EQ(false, ret);

  delete[] buffer;
}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest,BatchInferenceProcess) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true)).then(returnValue(false));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_OK, ret);

  ret = cartype_inference_test.BatchInferenceProcess(image_handle, tran_data);
}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest,transdata_null) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = nullptr;

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_OK, ret);

}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest,CreateOutputTensorFail) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::CreateOutputTensor)
      .stubs().will(returnValue(hiai::FAILED));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_ERROR, ret);
}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest,ProcessFail) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Process).stubs().will(
      returnValue(hiai::FAILED));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_ERROR, ret);

}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest,ConstructInferenceResultFail) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(false));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_ERROR, ret);

}

/**
 * @brief: test BatchInferenceProcess function
 */
TEST_F(CarTypeInferenceUTest, SendResultDataFail) {
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();
  std::shared_ptr<BatchCarInfoT> tran_data = std::make_shared<BatchCarInfoT>();

  CarTypeInferenceEngine cartype_inference_test;
  std::vector<AIModelDescription> model_desc;
  hiai::AIConfig config;

  //call init, avoid ai_model_manager_ is null
  hiai::AIConfigItem* model_path_item = config.add_items();
  model_path_item->set_name("model_path");
  model_path_item->set_value("/path/test");
  hiai::AIModelManager manager_obj;
  MOCKER_CPP_VIRTUAL(&manager_obj, &hiai::AIModelManager::Init).stubs().will(
      returnValue(hiai::SUCCESS));
  HIAI_StatusT ret_init = cartype_inference_test.Init(config, model_desc);

  //
  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::ConstructInferenceResult).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::ConstructBatchBuffer).stubs().will(
      returnValue(true));
  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(2));

  HIAI_StatusT ret = cartype_inference_test.BatchInferenceProcess(image_handle,
                                                                  tran_data);
  EXPECT_EQ(HIAI_OK, ret);
}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, agg0_null) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<void> inference_null = nullptr;
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(inference_null, arg1);
  EXPECT_EQ(HIAI_ERROR, ret);

}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, process) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();

  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::BatchInferenceProcess).stubs().will(
      returnValue(HIAI_OK));

  std::shared_ptr<void> arg0 = std::static_pointer_cast<void>(image_handle);
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(arg0, arg1);
  EXPECT_EQ(HIAI_OK, ret);

}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, isfinished_true) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();

  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = true;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::SendResultData).stubs().will(
      returnValue(HIAI_OK));

  std::shared_ptr<void> arg0 = std::static_pointer_cast<void>(image_handle);
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(arg0, arg1);
  EXPECT_EQ(HIAI_OK, ret);

}

void MockBatchImageResizeStub(CarTypeInferenceEngine *This,
    std::shared_ptr<BatchCroppedImageParaT>& batch_image_input,
    std::shared_ptr<BatchCroppedImageParaT>& batch_image_output) {
  batch_image_output->video_image_info.channel_id = "car1";
}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, resize_out_null) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();

  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&CarTypeInferenceEngine::BatchImageResize)
  .stubs()
  .will(invoke(MockBatchImageResizeStub));

  std::shared_ptr<void> arg0 = std::static_pointer_cast<void>(image_handle);
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(arg0, arg1);
  EXPECT_EQ(HIAI_ERROR, ret);

}


bool MockPopAllData(MultiTypeQueue *This, std::shared_ptr<BatchCroppedImageParaT>& arg1) {

  arg1 = nullptr;
  return true;
}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, popalldata_null) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();

  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&hiai::MultiTypeQueue::PopAllData, bool(MultiTypeQueue::*)(std::shared_ptr<BatchCroppedImageParaT>&))
  .stubs()
  .will(invoke(MockPopAllData));

  std::shared_ptr<void> arg0 = std::static_pointer_cast<void>(image_handle);
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(arg0, arg1);
  EXPECT_EQ(HIAI_ERROR, ret);

}

bool MockPopAllDataFail(MultiTypeQueue *This, std::shared_ptr<BatchCroppedImageParaT>& arg1) {
  return false;
}

/**
 * @brief: test HIAI_IMPL_ENGINE_PROCESS function
 */
TEST_F(CarTypeInferenceUTest, popalldata_fail) {
  CarTypeInferenceEngine cartype_inference_test;
  std::shared_ptr<BatchCroppedImageParaT> image_handle = std::make_shared<
      BatchCroppedImageParaT>();

  image_handle->video_image_info.channel_id = "car1";
  image_handle->video_image_info.frame_id = 1;
  image_handle->video_image_info.channel_name = "car.mp4";
  image_handle->video_image_info.is_finished = false;

  for (int i = 0; i < 1; i++) {
    ObjectImageParaT obj_image;
    obj_image.object_info.object_id = i;
    obj_image.object_info.score = 0.9;
    obj_image.img.format = RGB565;
    obj_image.img.width = 224;
    obj_image.img.height = 224;
    obj_image.img.channel = 3;
    obj_image.img.depth = 0;
    obj_image.img.size = 224 * 224;
    uint8_t* temp = new uint8_t[obj_image.img.size];
    obj_image.img.data.reset(temp, std::default_delete<uint8_t[]>());
    image_handle->obj_imgs.push_back(obj_image);
  }

  MOCKER_CPP(&hiai::MultiTypeQueue::PopAllData, bool(MultiTypeQueue::*)(std::shared_ptr<BatchCroppedImageParaT>&))
  .stubs()
  .will(invoke(MockPopAllDataFail));

  std::shared_ptr<void> arg0 = std::static_pointer_cast<void>(image_handle);
  std::shared_ptr<void> arg1 = nullptr;  // not use
  HIAI_StatusT ret = cartype_inference_test.Process(arg0, arg1);
  EXPECT_EQ(HIAI_ERROR, ret);

}
