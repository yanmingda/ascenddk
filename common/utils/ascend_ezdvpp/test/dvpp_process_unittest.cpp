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
#define ALIGN_UP(x, align) ((((x) + ((align)-1))/(align))*(align))
#define private public
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"

void *MmapStub(void *addr, size_t length, int prot, int flags, int fd,
               off_t offset) {
  return malloc(length);

}

int MunmapStub(void *addr, size_t length) {
  free(addr);
  return 0;
}

TEST(DvppProcessUnitTest, dvppYuvChangeToJpegTestSuccess) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = 720;
  dvpp_to_jpg_para.resolution.width = 1280;
  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  sJpegeOut jpg_out_data;
  jpg_out_data.jpgSize = 100;
  jpg_out_data.jpgData = (unsigned char *) malloc(100);
  MOCKER_CPP(&ascend::utils::DvppProcess::DvppYuvChangeToJpeg).stubs()
  .with(any(),any(),outBoundP(&jpg_out_data, sizeof(sJpegeOut)))
  .will(returnValue(0));

  ret = dvpp_jpg_process.DvppOperationProc(in_buf, size, &dvpp_output);

  free(in_buf);
  free(jpg_out_data.jpgData);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToH264TestSuccess) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  shared_ptr<AutoBuffer> output_data_queue = make_shared<AutoBuffer>();
  output_data_queue->allocBuffer(300);
  MOCKER_CPP(&ascend::utils::DvppProcess::DvppYuvChangeToH264).stubs()
  .with(any(),any(),outBoundP(&output_data_queue,
          sizeof(shared_ptr<AutoBuffer> *)))
  .will(returnValue(0));

  ret = dvpp_h264_process.DvppOperationProc(in_buf, size, &dvpp_output);
  dvpp_h264_process.GetMode();
  dvpp_h264_process.PrintErrorInfo(ret);
  free(in_buf);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToH264UnitTest) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;
  shared_ptr<AutoBuffer> output_data_queue;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  dvpp_h264_process.DvppYuvChangeToH264(in_buf, size, &output_data_queue);

  free(in_buf);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppOperationProcH264Fail) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  int ret = 0;
  char buf[] = "sadasdasd";
  int size = sizeof(buf);
  ascend::utils::DvppOutput dvpp_out;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  MOCKER_CPP(&ascend::utils::DvppProcess::DvppYuvChangeToH264).stubs()
  .will(returnValue((int)ascend::utils::kDvppErrorDvppCtlFail));

  ret = dvpp_h264_process.DvppOperationProc(buf, size, &dvpp_out);

  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();

}

TEST(DvppProcessUnitTest, DvppOperationProcJpgFail) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char buf[] = "sadasdasd";
  int size = sizeof(buf);
  ascend::utils::DvppOutput dvpp_out;
  int ret = 0;

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = 720;
  dvpp_to_jpg_para.resolution.width = 1280;

  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);

  MOCKER_CPP(&ascend::utils::DvppProcess::DvppYuvChangeToJpeg).stubs()
  .will(returnValue((int)ascend::utils::kDvppErrorDvppCtlFail));

  ret = dvpp_jpg_process.DvppOperationProc(buf, size, &dvpp_out);

  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();

}

TEST(DvppProcessUnitTest, DvppYuvChangeToJpegFunctionError) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;
  sJpegeOut out_data;

  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = 720;
  dvpp_to_jpg_para.resolution.width = 1280;

  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  ret = dvpp_jpg_process.DvppYuvChangeToJpeg(in_buf, 0, NULL);
  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);

  MOCKER(mmap).stubs()
  .will(returnValue((void *)NULL));

  ret = dvpp_jpg_process.DvppYuvChangeToJpeg(in_buf, size, &out_data);
  EXPECT_EQ(ascend::utils::kDvppErrorMallocFail, ret);

  free(in_buf);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToJpegCreatedDvppError) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  MOCKER(CreateDvppApi).stubs()
  .will(returnValue(-1));

  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = 720;
  dvpp_to_jpg_para.resolution.width = 1280;

  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  ret = dvpp_jpg_process.DvppOperationProc(in_buf, size, &dvpp_output);

  free(in_buf);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppErrorCreateDvppFail, ret);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToJpegCreateCtrlError) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  MOCKER(DvppCtl).stubs()
  .will(returnValue(-1));

  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = 720;
  dvpp_to_jpg_para.resolution.width = 1280;

  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  ret = dvpp_jpg_process.DvppOperationProc(in_buf, size, &dvpp_output);

  free(in_buf);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToH264CtlError) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  MOCKER(DvppCtl).stubs()
  .will(returnValue(-1));

  ret = dvpp_h264_process.DvppOperationProc(in_buf, size, &dvpp_output);
  dvpp_h264_process.PrintErrorInfo(ret);
  free(in_buf);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);

  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToH264CreateError) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  MOCKER(CreateDvppApi).stubs()
  .will(returnValue(-1));

  ret = dvpp_h264_process.DvppOperationProc(in_buf, size, &dvpp_output);
  dvpp_h264_process.PrintErrorInfo(ret);
  free(in_buf);
  delete[] dvpp_output.buffer;
  EXPECT_EQ(ascend::utils::kDvppErrorCreateDvppFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcUnitTest,invalidPara) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  sJpegeIn in_data;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);
  dvpp_h264_process.PrintErrorInfo(0x100);
  int ret = dvpp_h264_process.DvppProc(in_data, nullptr);
  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuvChangeToH264OutError) {
  ascend::utils::DvppToH264Para dvpp_to_h264_para;
  ascend::utils::DvppOutput dvpp_output = { 0 };
  char *in_buf = NULL;
  int size = 0;
  int ret = 0;

  dvpp_to_h264_para.coding_type = ascend::utils::kH264High;
  dvpp_to_h264_para.yuv_store_type = ascend::utils::kYuv420sp;
  dvpp_to_h264_para.resolution.width = 1280;
  dvpp_to_h264_para.resolution.height = 720;

  ascend::utils::DvppProcess dvpp_h264_process(dvpp_to_h264_para);

  size = 1280 * 720 * 3 / 2;
  in_buf = (char *) malloc(size);
  memset(in_buf, 1, size);

  venc_in_msg venc_msg;
  venc_msg.output_data_queue = NULL;

  dvppapi_ctl_msg dvpp_api_ctl_msg;
  dvpp_api_ctl_msg.in = (void*) (&venc_msg);
  dvpp_api_ctl_msg.in_size = sizeof(venc_in_msg);

  MOCKER(DvppCtl).stubs()
  .with(any(),any(),outBoundP(&dvpp_api_ctl_msg, sizeof(dvppapi_ctl_msg)))
  .will(returnValue((int)0));

  ret = dvpp_h264_process.DvppOperationProc(in_buf, size, &dvpp_output);
  dvpp_h264_process.PrintErrorInfo(ret);

  EXPECT_EQ(ascend::utils::kDvppErrorNoOutputInfo, ret);

  dvpp_api_ctl_msg.in = NULL;

  MOCKER(DvppCtl).stubs()
  .with(any(),any(),outBoundP(&dvpp_api_ctl_msg, sizeof(dvppapi_ctl_msg)))
  .will(returnValue((int)0));

  ret = dvpp_h264_process.DvppOperationProc(in_buf, size, &dvpp_output);
  dvpp_h264_process.PrintErrorInfo(ret);

  EXPECT_EQ(ascend::utils::kDvppErrorNoOutputInfo, ret);

  free(in_buf);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppBGRChangeToYUVParamCheckError) {
  int width = 1280;
  int height = 720;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = 0;

  char *input_image = nullptr;

  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);
  if (input_image != nullptr) {
    delete[] input_image;
  }

  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
}

TEST(DvppProcessUnitTest, callDvppBGRChangeToYUVError) {
  int width = 1280;
  int height = 720;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = width * height * 3;

  char *input_image = new char[input_size];

  memset(input_image, 1, input_size);

  MOCKER_CPP(&ascend::utils::DvppProcess::DvppBgrChangeToYuv).stubs()
  .will(returnValue((int)ascend::utils::kDvppErrorDvppCtlFail));

  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);
  if (input_image != nullptr) {
    delete[] input_image;
  }

  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppBGRChangeToYUVCreateDvppApiError) {
  int width = 720;
  int height = 404;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  MOCKER(CreateDvppApi).stubs()
  .will(returnValue(-1));

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = width * height * 3;

  char *input_image = new char[input_size];

  memset(input_image, 1, input_size);
  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;

  EXPECT_EQ(ascend::utils::kDvppErrorCreateDvppFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppBGRChangeToYUVExcuteDvppCtlError) {
  int width = 720;
  int height = 404;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  MOCKER(DvppCtl).stubs()
  .will(returnValue(-4));

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = width * height * 3;

  char *input_image = new char[input_size];

  memset(input_image, 1, input_size);
  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;

  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppBGRChangeToYUVNoAlignIsSuccess) {
  int width = 1280;
  int height = 720;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = width * height * 3;

  char *input_image = new char[input_size];

  memset(input_image, 1, input_size);
  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;
  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
}

TEST(DvppProcessUnitTest, dvppBGRChangeToYUVAlignIsSuccess) {
  int width = 720;
  int height = 404;
  ascend::utils::DvppToYuvPara dvpp_to_yuv;
  dvpp_to_yuv.image_type = 5;  //RGB888
  dvpp_to_yuv.rank = 7;  // convert yuv420spnv12
  dvpp_to_yuv.bit_width = 8;
  dvpp_to_yuv.cvdr_or_rdma = 1;
  dvpp_to_yuv.resolution.width = width;
  dvpp_to_yuv.resolution.height = height;
  dvpp_to_yuv.horz_max = width - 1;
  dvpp_to_yuv.horz_min = 0;
  dvpp_to_yuv.vert_max = height - 1;
  dvpp_to_yuv.vert_min = 0;
  dvpp_to_yuv.horz_inc = 1;
  dvpp_to_yuv.vert_inc = 1;

  ascend::utils::DvppProcess dvpp_process(dvpp_to_yuv);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = width * height * 3;

  char *input_image = new char[input_size];

  memset(input_image, 1, input_size);
  //call dvpp vpc interface to convert image from bgr to yuv420sp_nv12
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;
  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
}

TEST(DvppProcessUnitTest, dvppCheckCropOrResizeParamError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_output_align_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = 0;
  char *input_image = nullptr;

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_output_align_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  if(input_image != nullptr){
    delete[] input_image;
  }

  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);

  dvpp_crop_resize_para.is_output_align = false;
  ascend::utils::DvppProcess dvpp_output_not_align_process(dvpp_crop_resize_para);

  input_image = nullptr;
  ret = dvpp_output_not_align_process.DvppOperationProc(input_image, input_size,
                                                        &dvpp_out_info);

  if(input_image != nullptr){
    delete[] input_image;
  }

  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
}

TEST(DvppProcessUnitTest, dvppCheckDataSizeError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  MOCKER_CPP(&ascend::utils::DvppUtils::CheckDataSize).stubs()
  .will(returnValue((int)ascend::utils::kDvppErrorCheckMemorySizeFail));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorCheckMemorySizeFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeCreateDvppApiError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  MOCKER(CreateDvppApi).stubs()
  .will(returnValue(-1));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorCreateDvppFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeDvppCtlError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  MOCKER(DvppCtl).stubs()
  .will(returnValue(-1));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppCheckIncreaseParamError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 3000;
  int dest_height = 3000;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeAllocBufferError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 10;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeSecondDvppCtlError) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  MOCKER(DvppCtl).stubs()
  .will(returnValue(0))
  .then(returnValue(-1));

  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppResizeOutputNotAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 0;  // yuv420sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;
  dvpp_crop_resize_para.is_output_align = false;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuv422spResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 1;  // yuv422sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 2;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuv444spResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 2;  // yuv444packed
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 3;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuv422packedResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 3;  // yuv422packed
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 2;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuv444packedResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 4;  // yuv444packed
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 3;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppRgbpackedResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 5;  // rgb444packed
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 3;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppXrgbpackedResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 6;  // xrgb444packed
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 4;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppYuv400spResizeOutputAlignSuccess) {
  int src_width = 720;
  int src_height = 404;
  int dest_width = 300;
  int dest_height = 300;
  ascend::utils::DvppCropOrResizePara  dvpp_crop_resize_para;
  dvpp_crop_resize_para.image_type = 7;  // yuv400sp
  dvpp_crop_resize_para.rank = 1;  // convert yuv420spnv12
  dvpp_crop_resize_para.bit_width = 8;
  dvpp_crop_resize_para.cvdr_or_rdma = 1;
  dvpp_crop_resize_para.src_resolution.width = src_width;
  dvpp_crop_resize_para.src_resolution.height = src_height;
  dvpp_crop_resize_para.horz_max = src_width - 1;
  dvpp_crop_resize_para.horz_min = 0;
  dvpp_crop_resize_para.vert_max = src_height - 1;
  dvpp_crop_resize_para.vert_min = 0;
  dvpp_crop_resize_para.dest_resolution.width = dest_width;
  dvpp_crop_resize_para.dest_resolution.height = dest_height;

  ascend::utils::DvppProcess dvpp_process(dvpp_crop_resize_para);

  ascend::utils::DvppOutput dvpp_out_info;
  MOCKER_CPP(&ascend::utils::DvppUtils::CheckIncreaseParam).stubs()
  .will(returnValue((int)ascend::utils::kDvppOperationOk));

  int input_size = src_width * src_height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppOperationProc(input_image, input_size,
                                           &dvpp_out_info);

  delete[] input_image;
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppCheckJpegDParamError) {
  ascend::utils::DvppJpegDInPara  dvpp_jpegd_in_para;
  ascend::utils::DvppProcess dvpp_process(dvpp_jpegd_in_para);

  ascend::utils::DvppJpegDOutput dvpp_out_info;

  int input_size = 0;
  char *input_image = nullptr;

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppJpegDProc(input_image, input_size,
                                        &dvpp_out_info);

  if(input_image != nullptr){
    delete[] input_image;
  }

  EXPECT_EQ(ascend::utils::kDvppErrorInvalidParameter, ret);
}

TEST(DvppProcessUnitTest, dvppJpegDYuv420spSuccess) {
  ascend::utils::DvppJpegDInPara  dvpp_jpegd_in_para;
  ascend::utils::DvppProcess dvpp_process(dvpp_jpegd_in_para);

  ascend::utils::DvppJpegDOutput dvpp_out_info = {0};

  int width = 720;
  int height = 404;
  int input_size = width * height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  jpegd_yuv_data_info jpgd_out_data;
  jpgd_out_data.yuv_data_size = 479232;
  jpgd_out_data.yuv_data = (unsigned char *) malloc(479232);
  jpgd_out_data.img_width = 720;
  jpgd_out_data.img_height = 404;
  jpgd_out_data.img_width_aligned = 768;
  jpgd_out_data.img_height_aligned = 416;
  jpgd_out_data.out_format = DVPP_JPEG_DECODE_OUT_YUV420;

  MOCKER_CPP(&ascend::utils::DvppProcess::DvppJpegChangeToYuv).stubs()
  .with(any(),any(),outBoundP(&jpgd_out_data, sizeof(jpegd_yuv_data_info)))
  .will(returnValue(0));

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppJpegDProc(input_image, input_size,
                                        &dvpp_out_info);

  delete[] input_image;
  free(jpgd_out_data.yuv_data);
  delete[] dvpp_out_info.buffer;

  EXPECT_EQ(ascend::utils::kDvppOperationOk, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppJpegDCreateDvppApiError) {
  ascend::utils::DvppJpegDInPara  dvpp_jpegd_in_para;
  ascend::utils::DvppProcess dvpp_process(dvpp_jpegd_in_para);

  ascend::utils::DvppJpegDOutput dvpp_out_info = {0};

  int width = 720;
  int height = 404;
  int input_size = width * height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  MOCKER(CreateDvppApi).stubs()
  .will(returnValue(-1));

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppJpegDProc(input_image, input_size,
                                        &dvpp_out_info);

  delete[] input_image;

  EXPECT_EQ(ascend::utils::kDvppErrorCreateDvppFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppJpegDDvppCtlError) {
  ascend::utils::DvppJpegDInPara  dvpp_jpegd_in_para;
  ascend::utils::DvppProcess dvpp_process(dvpp_jpegd_in_para);

  ascend::utils::DvppJpegDOutput dvpp_out_info = {0};

  int width = 720;
  int height = 404;
  int input_size = width * height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  MOCKER(mmap).stubs()
  .will(invoke(MmapStub));

  MOCKER(munmap).stubs()
  .will(invoke(MunmapStub));

  MOCKER(DvppCtl).stubs()
  .will(returnValue(-1));

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppJpegDProc(input_image, input_size,
                                        &dvpp_out_info);

  delete[] input_image;

  EXPECT_EQ(ascend::utils::kDvppErrorDvppCtlFail, ret);
  GlobalMockObject::reset();
}

TEST(DvppProcessUnitTest, dvppJpegDmmapError) {
  ascend::utils::DvppJpegDInPara  dvpp_jpegd_in_para;
  ascend::utils::DvppProcess dvpp_process(dvpp_jpegd_in_para);

  ascend::utils::DvppJpegDOutput dvpp_out_info = {0};

  int width = 720;
  int height = 404;
  int input_size = width * height * 1.5;
  char *input_image = new char[input_size];
  memset(input_image, 1, input_size);

  MOCKER(mmap).stubs()
  .will(returnValue ((void *) nullptr));

  //call dvpp vpc interface to crop or resize image
  int ret = dvpp_process.DvppJpegDProc(input_image, input_size,
                                        &dvpp_out_info);

  delete[] input_image;

  EXPECT_EQ(ascend::utils::kDvppErrorMallocFail, ret);
  GlobalMockObject::reset();
}