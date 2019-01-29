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
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"
#include "dvpp/dvpp_config.h"
#include "dvpp/idvppapi.h"

int CreateDvppApi(IDVPPAPI *&pIDVPPAPI) { return 0; }

int DvppCtl(IDVPPAPI *&pIDVPPAPI, int CMD, dvppapi_ctl_msg *MSG) { return 0; }

int DestroyDvppApi(IDVPPAPI *&pIDVPPAPI) { return 0; }

namespace ascend {
namespace utils {
DvppProcess::DvppProcess(const DvppToJpgPara &para) {}

DvppProcess::DvppProcess(const DvppJpegDInPara &para) {}
DvppProcess::DvppProcess(const DvppCropOrResizePara &para) {}

DvppProcess::~DvppProcess() {}

int DvppProcess::DvppOperationProc(const char *input_buf, int input_size,
                                   DvppOutput *out_data) {
  int total_size=10;
  char *output_image = new char[total_size];
  memset_s(output_image, total_size, 1, total_size);
  out_data->buffer=(unsigned char*)output_image;
  out_data->size=total_size;
  return 0;
}

int DvppProcess::DvppJpegDProc(const char *input_buf, int input_size,
                               DvppJpegDOutput *output_data) {}

}  // namespace utils
}  // namespace ascend
