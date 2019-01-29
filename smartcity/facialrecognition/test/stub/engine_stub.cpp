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
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/engine.h"

namespace hiai {

Engine::Engine() {}

Engine::~Engine() {}

HIAI_StatusT Engine::InitQueue(const uint32_t& in_port_num,
                               const uint32_t& out_port_num) {
  return HIAI_OK;
}

HIAI_StatusT Engine::SendData(const uint32_t port_id,
                              const std::string& message_name,
                              const std::shared_ptr<void>& data_ptr,
                              const uint32_t time_out) {
  return HIAI_OK;
}

void Graph::ReleaseDataBuffer(void* ptr) { return; }

}  // namespace hiai

HIAIDataTypeFactory* HIAIDataTypeFactory::GetInstance() {
  static HIAIDataTypeFactory instance;
  return &instance;
}

void HIAI_ENGINE_LOG_IMPL(const char* _func_, const char* _file_, int _line_,
                          const char* format, ...) {
  return;
}

void HIAI_ENGINE_LOG_IMPL(const char* _func_, const char* _file_, int _line_,
                          const uint32_t error_code, const char* format, ...) {
  return;
}
