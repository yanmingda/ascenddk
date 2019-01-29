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

#include <string>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

#include "hiaiengine/status.h"
#include "presenter_agent_stub.h"

#define private public
#define protected public
#include "face_register.h"

using hiai::Engine;
using namespace hiai;
using namespace std;
using namespace ascend::presenter;
using namespace ascend::presenter::facial_recognition;

class FaceRegisterSTest : public testing::Test {
protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
    GlobalMockObject::verify();
  }
};

PresenterErrorCode SendMessageMock(
    ChannelMock* This, const google::protobuf::Message& message,
    std::unique_ptr<google::protobuf::Message>& response) {
  CommonResponse* resp = new CommonResponse();
  resp->set_ret(kErrorNone);
  response.reset(resp);
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ReceiveMessageMock(
  ChannelMock* This,
  std::unique_ptr<google::protobuf::Message>& message) {
  FaceInfo *face_info = new FaceInfo();
  face_info->set_id("yliwei33");
  face_info->set_image("imageimagejpgimageimage");
  message.reset(face_info);
  
  return PresenterErrorCode::kNone;
}
  
TEST_F(FaceRegisterSTest, Init_Normal) {
  std::vector < AIModelDescription > model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7008");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("face_app1");

  // app_type
  hiai::AIConfigItem* app_type_item = config.add_items();
  app_type_item->set_name("app_type");
  app_type_item->set_value("facial_recognition");

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
  FaceRegister face_register_test;
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_OK, ret);
}

TEST_F(FaceRegisterSTest, Init_FailureIp) {
  std::vector < AIModelDescription > model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("000.168.4.32");
  
  HIAI_StatusT ret = HIAI_OK;
  FaceRegister face_register_test;
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(FaceRegisterSTest, Init_FailurePort) {
  std::vector < AIModelDescription > model_desc;
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
  FaceRegister face_register_test;
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);

  server_port_item->set_value("700ac");
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(FaceRegisterSTest, Init_FailureAppName) {
  std::vector < AIModelDescription > model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7008");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("face&&app1");
  
  HIAI_StatusT ret = HIAI_OK;
  FaceRegister face_register_test;
  ret = face_register_test.Init(config, model_desc);
  EXPECT_EQ(HIAI_ERROR, ret);
}

TEST_F(FaceRegisterSTest, Init_FailureChannel) {
  std::vector < AIModelDescription > model_desc;
  AIConfig config;

  // presenter_server_ip
  hiai::AIConfigItem* server_ip_item = config.add_items();
  server_ip_item->set_name("presenter_server_ip");
  server_ip_item->set_value("192.168.4.32");

  // presenter_server_port
  hiai::AIConfigItem* server_port_item = config.add_items();
  server_port_item->set_name("presenter_server_port");
  server_port_item->set_value("7008");

  // app_name
  hiai::AIConfigItem* app_name_item = config.add_items();
  app_name_item->set_name("app_name");
  app_name_item->set_value("face_app1");

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

  ChannelMock obj;
  MOCKER_CPP_VIRTUAL(obj, &ChannelMock::ReceiveMessage)
  .stubs()
  .will(invoke(ReceiveMessageMock));
  
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
}

