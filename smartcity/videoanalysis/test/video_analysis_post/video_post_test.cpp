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
#include <mockcpp/mockcpp.hpp>

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "hiaiengine/status.h"
#include "presenter_agent_stub.h"

#define private public
#define protected public
#include "video_analysis_post.h"

using hiai::Engine;
using namespace hiai;
using namespace std;
using namespace ascend::presenter;
using namespace ascend::presenter::video_analysis;

class VideoPostSTest : public testing::Test {
 protected:
  virtual void SetUp() {}

  virtual void TearDown() { GlobalMockObject::verify(); }
};

PresenterErrorCode SendMessageMock(
    ChannelMock* This, const google::protobuf::Message& message,
    std::unique_ptr<google::protobuf::Message>& response) {
  CommonResponse* resp = new CommonResponse();
  resp->set_ret(kErrorNone);
  response.reset(resp);
  return PresenterErrorCode::kNone;
}

PresenterErrorCode SendMessageMock2(
    ChannelMock* This, const google::protobuf::Message& message,
    std::unique_ptr<google::protobuf::Message>& response) {
  CommonResponse* resp = new CommonResponse();
  resp->set_ret(kErrorOther);
  response.reset(resp);
  return PresenterErrorCode::kNone;
}

TEST_F(VideoPostSTest, Init_Normal) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  // none
  hiai::AIConfigItem* none_item = config.add_items();
  none_item->set_name("none");
  none_item->set_value("none");

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(invoke(SendMessageMock));

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(VideoPostSTest, Init_FailureIp) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("000.168.4.32");

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailurePort) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("");

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);

  server_port_item->set_value("700ac");
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailureAppName) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video&&app1");

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailureChannel) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(&obj, &ChannelMock::Open)
      .stubs()
      .will(returnValue(PresenterErrorCode::kConnection));

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailureSendMessage) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(returnValue(PresenterErrorCode::kConnection));

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;

  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailureResponseNull) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;

  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, Init_FailureResponse) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(invoke(SendMessageMock2));

  HIAI_StatusT ret = HIAI_OK;
  VideoAnalysisPost video_post_test;
  ret = video_post_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(VideoPostSTest, SendDetectionImage_Normal) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("video_analysis");

  VideoAnalysisPost video_post_test;
  video_post_test.Init(config, model_desc);

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(invoke(SendMessageMock));

  shared_ptr<VideoDetectionImageParaT> image_para =
      make_shared<VideoDetectionImageParaT>();

  ObjectImageParaT obj_img;
  obj_img.object_info = {"person_1", 0.92};
  obj_img.img.height = 10;
  obj_img.img.width = 10;
  obj_img.img.format = IMAGEFORMAT::YUV420SP;
  obj_img.img.size = 10 * 10 * 3 / 2;
  obj_img.img.data = shared_ptr<u_int8_t>(new u_int8_t[obj_img.img.size],
                                          default_delete<u_int8_t[]>());
  image_para->obj_imgs.push_back(obj_img);
  // image_para->image = obj_img;
  OperationCode ret = kOperationOk;
  ret = video_post_test.SendDetectionImage(image_para);
  EXPECT_EQ(kOperationOk, ret);
}

TEST_F(VideoPostSTest, SendCarInfo) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;

  VideoAnalysisPost video_post_test;
  video_post_test.Init(config, model_desc);
  video_post_test.app_config_ = make_shared<RegisterAppParam>();
  video_post_test.app_config_->app_name = "video1";
  shared_ptr<BatchCarInfoT> car_info = nullptr;
  OperationCode ret = video_post_test.SendCarInfo(car_info);
  EXPECT_EQ(kInvalidParam, ret);
  car_info = make_shared<BatchCarInfoT>();
  car_info->video_image_info.is_finished = true;
  ret = video_post_test.SendCarInfo(car_info);
  EXPECT_EQ(kExitApp, ret);

  car_info->video_image_info.is_finished = false;
  CarInfoT info = {"car_1", 3, ::CarInferenceType::kCarColor, "color", 0.92};
  car_info->car_infos.push_back(info);

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(returnValue(PresenterErrorCode::kConnection));
  ret = video_post_test.SendCarInfo(car_info);
  //EXPECT_EQ(kExitApp, ret);
}

TEST_F(VideoPostSTest, SendPedestrianInfo) {
  std::vector<AIModelDescription> model_desc;
  AIConfig config;
  VideoAnalysisPost video_post_test;
  video_post_test.Init(config, model_desc);

  video_post_test.app_config_ = make_shared<RegisterAppParam>();
  video_post_test.app_config_->app_name = "video1";

  shared_ptr<BatchPedestrianInfoT> pedestrian_info = nullptr;
  OperationCode ret = video_post_test.SendPedestrianInfo(pedestrian_info);
  EXPECT_EQ(kInvalidParam, ret);
  pedestrian_info = make_shared<BatchPedestrianInfoT>();
  pedestrian_info->video_image_info.is_finished = true;
  ret = video_post_test.SendPedestrianInfo(pedestrian_info);
  EXPECT_EQ(kExitApp, ret);
  PedestrianInfoT info;
  info.object_id = "person_1";
  info.attribute_name = "pedestrian";
  info.pedestrian_attribute_map.insert({"hair", 0.9});
  pedestrian_info->pedestrian_info.push_back(info);
  pedestrian_info->video_image_info.is_finished = false;
  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::SendMessage,
                     PresenterErrorCode(ChannelMock::*)(
                         const google::protobuf::Message&,
                         std::unique_ptr<google::protobuf::Message>&))
      .stubs()
      .will(returnValue(PresenterErrorCode::kConnection));
  ret = video_post_test.SendPedestrianInfo(pedestrian_info);
}

/*TEST_F(FaceRegisterSTest, Init_FailureChannel) {
  std::vector < AIModelDescription > model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7004");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("video_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("facial_recognition");

  Channel *chan = nullptr;
  MOCKER_CPP(&PresenterChannels::GetChannel)
  .stubs()
  .will(returnValue(chan));

  HIAI_StatusT ret = HIAI_OK;
  FaceRegister face_register_test;
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(FaceRegisterSTest, DoRegisterProcess_Normal) {
  HIAI_StatusT ret = HIAI_OK;
  FaceRegister face_register_test;

  MOCKER_CPP(&Engine::SendData)
  .stubs()
  .will(returnValue(HIAI_OK))
  .then(returnValue(1))
  .then(returnValue(1))
  .then(returnValue(1))
  .then(returnValue(1))
  .then(returnValue(1));

  ret = face_register_test.DoRegisterProcess();
  EXPECT_FALSE(ret);
}

TEST_F(FaceRegisterSTest, Process_Noraml) {
  HIAI_StatusT ret = HIAI_OK;
  std::shared_ptr<void> arg0 = nullptr;

  MOCKER_CPP(&FaceRegister::DoRegisterProcess)
  .stubs()
  .will(returnValue(true));

  FaceRegister face_register_test;
  ret = face_register_test.Process(arg0);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(FaceRegisterSTest, Process_Fail) {
  HIAI_StatusT ret = HIAI_OK;
  std::shared_ptr<void> arg0 = nullptr;

  MOCKER_CPP(&FaceRegister::DoRegisterProcess)
  .stubs()
  .will(returnValue(false));

  FaceRegister face_register_test;
  ret = face_register_test.Process(arg0);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(FaceRegisterSTest, InitMsgQueue) {
  HIAI_StatusT ret = HIAI_OK;
  FaceRegister face_register_test;

  ret = face_register_test.InitMsgQueue();
  EXPECT_EQ(HIAI_OK, ret);
}*/
TEST_F(VideoPostSTest, Marco) {
  VideoAnalysisPost video_post_test;

  shared_ptr<VideoDetectionImageParaT> detection_img =
      make_shared<VideoDetectionImageParaT>();
  shared_ptr<BatchCarInfoT> car_type = make_shared<BatchCarInfoT>();
  shared_ptr<BatchCarInfoT> car_color = make_shared<BatchCarInfoT>();
  shared_ptr<BatchPedestrianInfoT> person = make_shared<BatchPedestrianInfoT>();

  shared_ptr<void> arg0 = static_pointer_cast<void>(detection_img);
  shared_ptr<void> arg1 = static_pointer_cast<void>(car_type);
  shared_ptr<void> arg2 = static_pointer_cast<void>(car_color);
  shared_ptr<void> arg3 = static_pointer_cast<void>(person);
  MOCKER_CPP(&VideoAnalysisPost::SendDetectionImage)
      .stubs()
      .will(returnValue(0))
      .then(returnValue(1))
      .then(returnValue(2));
  MOCKER_CPP(&VideoAnalysisPost::SendCarInfo)
      .stubs()
      .will(returnValue(0))
      .then(returnValue(1))
      .then(returnValue(2));
  MOCKER_CPP(&VideoAnalysisPost::SendPedestrianInfo)
      .stubs()
      .will(returnValue(0))
      .then(returnValue(1))
      .then(returnValue(2));

  HIAI_StatusT ret = video_post_test.Process(arg0, arg1, arg2, arg3);
  ret = video_post_test.Process(arg0, arg1, arg2, arg3);
  ret = video_post_test.Process(arg0, arg1, arg2, arg3);
}
