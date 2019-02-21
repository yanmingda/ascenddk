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

#include "video_decode.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/prctl.h>

#include <fstream>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <thread>

#include "hiaiengine/log.h"
#include "hiaiengine/data_type_reg.h"

using namespace std;

namespace {
const int kWait10Milliseconds = 10000; // wait 10ms

const int kWait100Milliseconds = 100000; // wait 100ms

const int kKeyFrameInterval = 5; // key fram interval

const int kImageDataQueueSize = 10; // the queue default size

const int kMaxRetryNumber = 100; // the maximum number of retry

const int kCompareEqual = 0; // string compare equal

const int kNoFlag = 0; // no flag

const int kHandleSuccessful = 0; // the process handled successfully

const string kStrChannelId1 = "channel1"; // channle id 1 string

const string kStrChannelId2 = "channel2";  // channle id 2 string

const string kVideoTypeH264 = "h264"; // video type h264

const string kVideoTypeH265 = "h265"; // video type h265

const string kNeedRemoveStr = " \r\n\t"; // the string need remove

const string kVideoImageParaType = "VideoImageParaT"; // video image para type

const int kIntChannelId1 = 1; // channel id 1 integer

const int kIntChannelId2 = 2; // channel id 2 integer

const int kVideoFormatLength = 5; // video format string length

const int kInvalidVideoIndex = -1; // invalid video index

const string kImageFormatNv12 = "nv12"; // image format nv12

const string kRtspTransport = "rtsp_transport"; // rtsp transport

const string kUdp = "udp"; // video format udp

const string kBufferSize = "buffer_size"; // buffer size string

const string kMaxBufferSize = "425984"; // maximum buffer size

const string kMaxDelayStr = "max_delay"; // maximum delay string

const string kMaxDelayValue = "1000000"; // maximum delay time value

const string kTimeoutStr = "stimeout"; // timeout string

const string kTimeoutValue = "1000000"; // timeout value

const string kThreadNameHead = "handle_" // thread name head string

const int kErrorBufferSize = 1024; // buffer size for error info

const int kThreadNameLength = 32; // thread name string length

const string kRegexSpace = "^[ ]*$"; // regex for check string is empty

// regex for verify .mp4 file name
const string kRegexMp4File = "^/((?!\\.\\.).)*\\.(mp4)$";

// regex for verify RTSP rtsp://ip:port/channelname
const string kRegexRtsp =
    "^rtsp://(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[0-9])\\."
        "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
        "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
        "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)"
        ":([1-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|"
        "6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])/"
        "(.{1,100})$";

uint32_t frame_id_1 = 0; // frame id used for channle1

uint32_t frame_id_2 = 0; // frame id used for channle2

// the queue record image data from channel1
ThreadSafeQueue<shared_ptr<VideoImageParaT>> channel1_queue(
    kImageDataQueueSize);

// the queue record image data from channel2
ThreadSafeQueue<shared_ptr<VideoImageParaT>> channel2_queue(
    kImageDataQueueSize);
}

HIAI_REGISTER_DATA_TYPE("VideoImageParaT", VideoImageParaT);

bool IsKeyFrame(uint32_t frame_id) {
  // the 1, 6, 11, 16... frame is key frame
  if (frame_id % kKeyFrameInterval == 1) {
    return true;
  }

  return false;
}

VideoDecode::VideoDecode() {
  channel1_ = ""; // initialize channel1 to empty string
  channel2_ = ""; // initialize channel2 to empty string
}

VideoDecode::~VideoDecode() {
}

void VideoDecode::SendFinishedData() {
  VideoImageInfoT video_image_info;
  video_image_info.is_finished = true;

  hiai::ImageData<unsigned char> image_data;
  shared_ptr<VideoImageParaT> video_image_para = make_shared<VideoImageParaT>();
  video_image_para->img = image_data;
  video_image_para->video_image_info = video_image_info;

  HIAI_StatusT hiai_ret = HIAI_OK;

  // send finished data to next engine, use output port:0
  do {
    hiai_ret = SendData(0, kVideoImageParaType,
                        static_pointer_cast<void>(video_image_para));
    if (hiai_ret == HIAI_QUEUE_FULL) { // check hiai queue is full
      HIAI_ENGINE_LOG("Queue full when send finished data, sleep 10ms");
      usleep(kWait10Milliseconds);  // sleep 10 
