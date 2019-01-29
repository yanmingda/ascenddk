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

#include "presenter_agent_stub.h"

namespace ascend {
namespace presenter {
	
ChannelMock* ChannelMock::NewChannel(const std::string& host_ip, uint16_t port,
                             std::shared_ptr<InitChannelHandler> handler) {
  return new ChannelMock();
}

PresenterErrorCode ChannelMock::Open() {
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ChannelMock::SendMessage(const google::protobuf::Message& message) {
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ChannelMock::SendMessage(const PartialMessageWithTlvs& message) {
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ChannelMock::SendMessage(
  const google::protobuf::Message& message,
  std::unique_ptr<google::protobuf::Message>& response) {
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ChannelMock::SendMessage(
  const PartialMessageWithTlvs& message,
  std::unique_ptr<google::protobuf::Message>& response) {
  return PresenterErrorCode::kNone;
}

PresenterErrorCode ChannelMock::ReceiveMessage(
  std::unique_ptr<google::protobuf::Message>& message) {
  return PresenterErrorCode::kNone;
}

const std::string& ChannelMock::GetDescription() const {
  return "";
}

PresenterErrorCode OpenChannel(Channel *&channel,
                               const OpenChannelParam &param) {
  channel = new ChannelMock();
  return PresenterErrorCode::kNone;
}

PresenterErrorCode PresentImage(Channel *channel, const ImageFrame &image) {
  return PresenterErrorCode::kNone;
}

Channel* ChannelFactory::NewChannel(const std::string& host_ip, uint16_t port) {
  return ChannelMock::NewChannel(host_ip, port, nullptr);
}

}  // namespace presenter
}  // namespace ascend
