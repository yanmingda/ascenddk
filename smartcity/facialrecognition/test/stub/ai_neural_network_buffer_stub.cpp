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
#include "hiaiengine/ai_tensor.h"
#include "hiaiengine/status.h"

namespace hiai {
const char* const AINeuralNetworkBuffer::GetTypeName() {
  return (const char*)0x12;
}

uint32_t AINeuralNetworkBuffer::ByteSizeLong() { return 0; }

bool AINeuralNetworkBuffer::Init(const AITensorDescription& tensor_desc) {
  return true;
}

AIStatus AINeuralNetworkBuffer::SerializeTo(void* buffer, uint32_t size_) {
  return HIAI_OK;
}

AIStatus AINeuralNetworkBuffer::DeserializeFrom(const void* buffer,
                                                uint32_t size_) {
  return HIAI_OK;
}

int32_t AINeuralNetworkBuffer::GetNumber() { return 0; }

void AINeuralNetworkBuffer::SetNumber(int32_t number) {}

int32_t AINeuralNetworkBuffer::GetChannel() { return 0; }

void AINeuralNetworkBuffer::SetChannel(int32_t channel) {}

}  // namespace hiai
