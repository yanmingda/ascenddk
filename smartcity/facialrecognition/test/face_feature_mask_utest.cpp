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
#include "face_recognition_params.h"
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/types_c.h"
#define private public
#define protected public
#include "face_feature_mask.h"
using namespace hiai;
using namespace std;
using namespace cv;



/**
 * @brief: custom data serialize UT test
 */
class FaceFeatureMaskUTest : public testing::Test {
 protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {
    GlobalMockObject::verify();
  }
};

AIStatus MockProcessSuccessStub(
  hiai::AIModelManager *model_manager, AIContext &context,
  const std::vector<std::shared_ptr<IAITensor>> &in_data,
  std::vector<std::shared_ptr<IAITensor>> &out_data, uint32_t timeout) {
  std::shared_ptr<AISimpleTensor> data = std::make_shared<AISimpleTensor>();
  out_data.emplace_back(data);
  return hiai::SUCCESS;
}

/*
 * @berif: init
 */
TEST_F(FaceFeatureMaskUTest, init) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  std::vector<AIModelDescription> modelDesc;
  AIConfig config;

  // model_path item
  AIConfigItem *ai_config_model = config.add_items();
  if (ai_config_model == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }
  ai_config_model->set_name("model_path");
  ai_config_model->set_value("/path/test");

  AIConfigItem *ai_config_batch_size = config.add_items();
  if (ai_config_batch_size == nullptr) {
    EXPECT_EQ(0, HIAI_OK);
  }

  ai_config_batch_size->set_name("batch_size");
  ai_config_batch_size->set_value("4");
  hiai::AIModelManager managerObj;

  // init failed or success
  MOCKER_CPP_VIRTUAL(&managerObj, &hiai::AIModelManager::Init)
  .stubs()
  .will(returnValue(hiai::FAILED))
  .then(returnValue(hiai::SUCCESS));

  // first time mock AIModelManager::Init failed
  // should return HIAI_ERROR
  HIAI_StatusT ret = faceFeatureMaskProcessTest.Init(config, modelDesc);
  EXPECT_EQ(HIAI_ERROR, ret);

  // second time mock AIModelManager::Init success
  // should return HIAI_OK
  ret = faceFeatureMaskProcessTest.Init(config, modelDesc);
  EXPECT_EQ(HIAI_OK, ret);

  /*Mat mat;
  MOCK_METHOD(&mat, &Mat::empty).stubs()
  .will(returnValue(false));
  ret = faceFeatureMaskProcessTest.Init(config, modelDesc);
  EXPECT_EQ(HIAI_ERROR, ret);*/
}

/*
 * @berif: input parameter is null
 */
TEST_F(FaceFeatureMaskUTest, arg0Check) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  std::shared_ptr<void> inferenceNull = nullptr;
  std::shared_ptr<void> arg0 = nullptr; // not use
  HIAI_StatusT ret = faceFeatureMaskProcessTest.Process(inferenceNull, arg0);
  EXPECT_EQ(HIAI_ERROR, ret);
}

/*
 * @berif: input parameter is not null,but the data from
 *   last node is not correct
 */
TEST_F(FaceFeatureMaskUTest, handleDataError) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;

  shared_ptr<FaceRecognitionInfo> face_recognition_info = make_shared <
      FaceRecognitionInfo > ();
  face_recognition_info->err_info.err_code = AppErrorCode::kRegister;
  std::shared_ptr<void> arg0 = std::static_pointer_cast <
                               void > (face_recognition_info);
  std::shared_ptr<void> arg1 = nullptr; // not use
  HIAI_StatusT ret = faceFeatureMaskProcessTest.Process(arg0, arg1);
  EXPECT_EQ(HIAI_ERROR, ret);
}

/*
 * @berif: Handle success, but face size is 0
 */
TEST_F(FaceFeatureMaskUTest, handleSizeZero) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;

  shared_ptr<FaceRecognitionInfo> face_recognition_info = make_shared <
      FaceRecognitionInfo > ();
  face_recognition_info->err_info.err_code = AppErrorCode::kNone;
  std::shared_ptr<void> arg0 = std::static_pointer_cast <
                               void > (face_recognition_info);
  std::shared_ptr<void> arg1 = nullptr; // not use
  HIAI_StatusT ret = faceFeatureMaskProcessTest.Process(arg0, arg1);
  EXPECT_EQ(HIAI_OK, ret);
}

/*
 * @berif: Crop Image
 */
TEST_F(FaceFeatureMaskUTest, cropImage) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  shared_ptr<FaceRecognitionInfo> face_recognition_info = make_shared <
      FaceRecognitionInfo > ();
  face_recognition_info->err_info.err_code = AppErrorCode::kNone;

  ImageData<u_int8_t> image;
  image.size = 19200;
  image.width = 80;
  image.height = 80;
  image.data = make_shared<u_int8_t>();
  image.format = hiai::IMAGEFORMAT::RGB565;

  vector<FaceImage> face_imgs;
  FaceImage face_img1;
  face_img1.rectangle.lt.x = 20;
  face_img1.rectangle.lt.y = 20;
  face_img1.rectangle.rb.x = 60;
  face_img1.rectangle.rb.y = 60;
  face_img1.image = image;

  face_imgs.push_back(face_img1);
  MOCKER_CPP(&ascend::utils::DvppProcess::DvppOperationProc)
  .stubs()
  .will(returnValue(-1))
  .then(returnValue(0));

  // Crop Success
  bool ret = faceFeatureMaskProcessTest.Crop(face_recognition_info, image, face_imgs);
  EXPECT_EQ(false, ret);
  face_imgs.clear();
  FaceImage face_img2;
  face_img2.rectangle.lt.x = 20;
  face_img2.rectangle.lt.y = 20;
  face_img2.rectangle.rb.x = 60;
  face_img2.rectangle.rb.y = 60;
  face_img2.image = image;
  face_imgs.push_back(face_img2);

  // Crop Failed
  ret = faceFeatureMaskProcessTest.Crop(face_recognition_info, image, face_imgs);
  EXPECT_EQ(true, ret);
  GlobalMockObject::reset();
}

/*
 * @berif: Resize image
 */
TEST_F(FaceFeatureMaskUTest, resizeImage) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  FaceRecognitionInfo face_recognition_info_temp;
  face_recognition_info_temp.err_info.err_code = AppErrorCode::kNone;
  face_recognition_info_temp.org_img.format = hiai::IMAGEFORMAT::RGB565;
  shared_ptr<FaceRecognitionInfo> face_recognition_info = make_shared <
      FaceRecognitionInfo > (face_recognition_info_temp);

  vector<ImageData<u_int8_t>> resized_imgs;
  ImageData<u_int8_t> image;
  image.size = 0;
  image.width = 80;
  image.height = 80;
  u_int8_t *data = new u_int8_t[19200];
  image.data = make_shared<u_int8_t>(*data);;
  image.format = hiai::IMAGEFORMAT::RGB565;

  vector<FaceImage> face_imgs;
  FaceImage face_img1;
  face_img1.rectangle.lt.x = 20;
  face_img1.rectangle.lt.y = 20;
  face_img1.rectangle.rb.x = 60;
  face_img1.rectangle.rb.y = 60;
  face_img1.image = image;

  face_imgs.push_back(face_img1);
  bool ret = faceFeatureMaskProcessTest.Resize(face_imgs, resized_imgs);
  EXPECT_EQ(false, ret);

  face_imgs[0].image.size = 19200;
  MOCKER_CPP(&ascend::utils::DvppProcess::DvppOperationProc)
  .stubs()
  .will(returnValue(-1));

  // Resize failed
  ret = faceFeatureMaskProcessTest.Resize(face_imgs, resized_imgs);
  EXPECT_EQ(false, ret);
  resized_imgs.clear();
  GlobalMockObject::reset();

  // Resize success
  ret = faceFeatureMaskProcessTest.Resize(face_imgs, resized_imgs);
  EXPECT_EQ(true, ret);
  delete [] data;
  data = nullptr;
}

/*
 * @berif: Transform YUV to BGR
 */
TEST_F(FaceFeatureMaskUTest, ImageYUV2BGR) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  vector<ImageData<u_int8_t>> resized_imgs;
  vector<Mat> bgr_imgs;

  ImageData<u_int8_t> img;
  img.height = 60;
  img.width = 60;
  u_int8_t *data = new u_int8_t[5400];
  memset(data, 10, 5400);
  shared_ptr<u_int8_t> data_temp(data, [](u_int8_t* p) {delete [] p;});
  img.data = data_temp;
  resized_imgs.push_back(img);
  bool ret = faceFeatureMaskProcessTest.ImageYUV2BGR(resized_imgs, bgr_imgs);
  EXPECT_EQ(true, ret);

  bgr_imgs.clear();
  MOCKER(memcpy_s).stubs()
  .will(returnValue(-1));
  ret = faceFeatureMaskProcessTest.ImageYUV2BGR(resized_imgs, bgr_imgs);
  EXPECT_EQ(false, ret);

}

/*
 * @berif: NormalizeData
 */
TEST_F(FaceFeatureMaskUTest, NormalizeData) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  vector<Mat> normalized_datas;
  Mat normalized_data1 = Mat::zeros(40, 40, CV_32FC3);
  normalized_datas.push_back(normalized_data1);
  faceFeatureMaskProcessTest.InitNormlizedData();

  // Normalize data success
  bool ret = faceFeatureMaskProcessTest.NormalizeData(normalized_datas);
  EXPECT_EQ(true, ret);
  normalized_datas.clear();
}

/*
 * @berif: Inference
 */
TEST_F(FaceFeatureMaskUTest, Inference) {
  FaceFeatureMaskProcess faceFeatureMaskProcessTest;
  std::vector<AIModelDescription> modelDesc;
  AIConfig config;

  // model_path item
  AIConfigItem *ai_config_model = config.add_items();
  ai_config_model->set_name("model_path");
  ai_config_model->set_value("/path/test");
  AIConfigItem *ai_config_batch_size = config.add_items();
  ai_config_batch_size->set_name("batch_size");
  ai_config_batch_size->set_value("4");
  hiai::AIModelManager managerObj;

  // init failed or success
  MOCKER_CPP_VIRTUAL(&managerObj, &hiai::AIModelManager::Init)
  .stubs()
  .will(returnValue(hiai::SUCCESS));

  // first time mock AIModelManager::Init failed
  // should return HIAI_ERROR
  HIAI_StatusT ret = faceFeatureMaskProcessTest.Init(config, modelDesc);
  EXPECT_EQ(HIAI_OK, ret);

  vector<Mat> normalized_datas;
  Mat normalized_data1 = Mat::zeros(40, 40, CV_32FC3);
  normalized_datas.push_back(normalized_data1);

  // Normalize data success
  bool ret1 = faceFeatureMaskProcessTest.NormalizeData(normalized_datas);
  EXPECT_EQ(true, ret1);

  MOCKER_CPP_VIRTUAL(&managerObj, &AIModelManager::Process)
  .stubs()
  .will(invoke(MockProcessSuccessStub));

  MOCKER_CPP(&hiai::AISimpleTensor::GetSize).stubs()
  .will(returnValue(40));


  float res_tensor[10] = { 0.234,0.321,0.452,-0.124,-0.222,0.231,-0.124,-0.222,0.231 };
  MOCKER_CPP(&hiai::AISimpleTensor::GetBuffer).stubs()
  .will(returnValue((void *)res_tensor));

  vector<FaceImage> face_imgs;
  FaceImage face_img;
  face_imgs.push_back(face_img);

  // Handle success
  ret1 = faceFeatureMaskProcessTest.Inference(normalized_datas, face_imgs);
  EXPECT_EQ(true, ret1);

  MOCKER(memcpy_s).stubs()
  .will(returnValue(-1));
 
  // Handle error
  ret1 = faceFeatureMaskProcessTest.Inference(normalized_datas, face_imgs);
  EXPECT_EQ(false, ret1);

  GlobalMockObject::reset();

}








