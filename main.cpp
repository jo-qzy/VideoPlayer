//
// Created by BoringWednesday on 2021/1/5.
//

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#ifdef __cplusplus
}
#endif

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    AVCodec            *codec       = nullptr;
    AVFrame            *frame_raw   = nullptr;
    AVFrame            *frame_yuv   = nullptr;
    AVPacket           *packet      = nullptr;
    AVFormatContext    *fmt_ctx     = nullptr;
    AVCodecContext     *codec_ctx   = nullptr;
    AVCodecParameters  *codec_par   = nullptr;

    uint8_t            *buffer      = nullptr;
    int                 buf_size    = 0;
    int                 idx         = -1;
    int                 ret         = 0;
    SwsContext         *sws_ctx     = nullptr;
    SDL_Window         *sdl_window  = nullptr;
    SDL_Renderer       *sdl_render  = nullptr;
    SDL_Texture        *sdl_texture = nullptr;
    SDL_Rect            sdl_rect;

    // 打开视频文件，读取文件头，存储在fmt_ctx中
    ret = avformat_open_input(&fmt_ctx, argv[1], nullptr, nullptr);
    if (ret != 0) {
        av_log(nullptr, AV_LOG_ERROR, "Open input failed.\n");
        return ret;
    }

    // 尝试读取一段视频数据，尝试解码，成功则填入fmp_ctx->streams
    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Find stream info failed.\n");
        return ret;
    }

    // 打印相关文件信息
    av_dump_format(fmt_ctx, 0, argv[1], 0);

    // 查找视频流
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            idx = i;
            av_log(nullptr, AV_LOG_INFO, "Find video stream index %d.\n", idx);
            break;
        }
    }

    if (idx == -1) {
        av_log(nullptr, AV_LOG_ERROR, "Find video stream index failed.\n");
        return -1;
    }

    // 查找解码器
    codec_par = fmt_ctx->streams[idx]->codecpar;
    codec = avcodec_find_decoder(codec_par->codec_id);
    if (codec == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "Find codec failed.\n");
        return -1;
    }

    // 构建解码器
    codec_ctx = avcodec_alloc_context3(codec);
    ret = avcodec_parameters_to_context(codec_ctx, codec_par);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "AVCodec parameters to context failed.\n");
        return ret;
    }

    // 使用codec初始化codec_ctx
    ret = avcodec_open2(codec_ctx, codec, nullptr);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "AVCodec open2 failed.\n");
        return ret;
    }

    // 分配AVFrame
    frame_raw = av_frame_alloc();
    frame_yuv = av_frame_alloc();

    // frm_raw的data_buffer由av_read_frame()分配，因此不需手工分配
    // frm_yuv的data_buffer无处分配，因此在此处手工分配
    buf_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height, 1);
    buffer = 0;

    return 0;
}
