#pragma once

#if defined(__linux__) && defined(PLATFORM_DESKTOP)

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
}

namespace ffmpeg::decoder {

struct LinuxHardwareState {
    AVHWDeviceType deviceType = AV_HWDEVICE_TYPE_NONE;
    AVPixelFormat hwPixelFormat = AV_PIX_FMT_NONE;
    AVPixelFormat transferPixelFormat = AV_PIX_FMT_NONE;
    bool hardwareFormatSelected = false;
    bool softwareFallbackSelected = false;
};

void resetLinuxHardwareState(LinuxHardwareState& state);
int initializeLinuxHardwareDevice(LinuxHardwareState& state,
                                  const AVCodec* decoder,
                                  AVBufferRef*& hw_device_ctx);
void configureLinuxDecoderContext(LinuxHardwareState& state,
                                  AVCodecContext* decoderContext);
int prepareLinuxHardwareTransfer(LinuxHardwareState& state,
                                 AVFrame* softwareFrame,
                                 const AVFrame* hardwareFrame,
                                 int videoFormat);
const char* linuxHardwareDeviceName(const LinuxHardwareState& state);

} // namespace ffmpeg::decoder

#endif
