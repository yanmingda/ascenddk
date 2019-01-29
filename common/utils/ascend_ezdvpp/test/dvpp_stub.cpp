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


#define VPC_UT
#define private public
#include "dvpp/dvpp_config.h"
#undef private

#include "dvpp/idvppapi.h"

int CreateDvppApi(IDVPPAPI *&dvpp_api) {
    dvpp_api = new IDVPPAPI();
    return 0;
}

int DvppCtl(IDVPPAPI *&dvpp_api, int cmd, dvppapi_ctl_msg *msg) {
    if (cmd == DVPP_CTL_VPC_PROC) {
        vpc_in_msg *vpc_input_msg = (vpc_in_msg *) msg->in;
        int dest_width = (vpc_input_msg->hmax - vpc_input_msg->hmin + 1) * vpc_input_msg->hinc;
        int dest_height = (vpc_input_msg->vmax - vpc_input_msg->vmin + 1) * vpc_input_msg->vinc;
        int width = ALIGN_UP(dest_width, 128);
        int height = ALIGN_UP(dest_height, 16);
        int total_size = width * height * 3 / 2;
        char *output_image = new char[total_size];
        memset_s(output_image, total_size, 1, total_size);
        char *dst = vpc_input_msg->auto_out_buffer_1->allocBuffer(total_size);
        memcpy_s(dst, total_size, output_image, total_size);
        vpc_input_msg->out_buffer_1_size = total_size;
        delete[] output_image;
    }

    if (cmd == DVPP_CTL_TOOL_CASE_GET_RESIZE_PARAM) {
        resize_param_in_msg *resize_input_msg = (resize_param_in_msg *) msg->in;
        resize_param_out_msg *resize_output_msg = (resize_param_out_msg *) msg->out;

        resize_output_msg->hmax = resize_input_msg->hmax;
        resize_output_msg->hmin = resize_input_msg->hmin;
        resize_output_msg->vmax = resize_input_msg->vmax;
        resize_output_msg->vmin = resize_input_msg->vmin;
        int a = resize_input_msg->hmax - resize_input_msg->hmin + 1;
        resize_output_msg->hinc = (double)resize_input_msg->dest_width / (resize_input_msg->hmax - resize_input_msg->hmin + 1);
        resize_output_msg->vinc = (double)resize_input_msg->dest_high / (resize_input_msg->vmax - resize_input_msg->vmin + 1);
    }

    return 0;
}

int DestroyDvppApi(IDVPPAPI *&dvpp_api) {
    delete dvpp_api;
    return 0;
}
