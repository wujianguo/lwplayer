//
//  video_decoder.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "video_decoder.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>


int video_decoder_thread(void *data) {

    MediaAsset *asset = (MediaAsset *)data;
    VideoAssetTrack *track = asset->video;
    
    AVPacket *packet = NULL;
    AVFrame *frame = av_frame_alloc();
    int ret = 0;
    
    while (1) {
        ret = packet_queue_get(&track->track, &packet);
        if (ret == AVERROR(EAGAIN)) {
            av_usleep(1000);
//            SDL_AddTimer(50, sdl_refresh_timer_cb, display_context);
            continue;
        }

        ret = avcodec_send_packet(track->track.codec_context, packet);
        if (ret != 0) {
            packet_queue_release(&track->track);
            break;
        }
        ret = avcodec_receive_frame(track->track.codec_context, frame);
        packet_queue_release(&track->track);

        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret != 0) {
            break;
        }

        while ((ret = frame_queue_put_video(track->sws_ctx, &track->track, frame)) == AVERROR(EAGAIN)) {
            av_usleep(50);
        }
        if (track->track.packet_size <= 5) {
            SDL_CondSignal(asset->read_cond);
        }
    }
    
    return ret;
}

int open_video_track(AVFormatContext *ifmt_ctx, unsigned int stream_index, VideoAssetTrack **track) {
    int ret = 0;
    AVStream *stream = ifmt_ctx->streams[stream_index];
    AVCodec *p_codec = NULL;
    if ((p_codec = avcodec_find_decoder(stream->codecpar->codec_id)) == NULL) {
        return -1;
    }
    
    AVCodecContext *codec_ctx = NULL;
    codec_ctx = avcodec_alloc_context3(p_codec);
    avcodec_parameters_to_context(codec_ctx, stream->codecpar);
    if ((ret = avcodec_open2(codec_ctx, p_codec, NULL)) != 0) {
        return ret;
    }
    
    VideoAssetTrack *track_ptr = av_malloc(sizeof(VideoAssetTrack));
    
    int numBytes = 0;
    uint8_t *buffer = NULL;
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height, 1);
    buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    
    init_track(&track_ptr->track, codec_ctx);
    for (int i=0; i<track_ptr->track.frame_max_size; ++i) {
        track_ptr->track.frames[i] = av_frame_alloc();
        av_image_fill_arrays(track_ptr->track.frames[i]->data, track_ptr->track.frames[i]->linesize, buffer, AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height, 1);
    }


    track_ptr->sws_ctx = sws_getContext(codec_ctx->width,
                                        codec_ctx->height,
                                        codec_ctx->pix_fmt,
                                        codec_ctx->width,
                                        codec_ctx->height,
                                        AV_PIX_FMT_YUV420P,
                                        SWS_BILINEAR,
                                        NULL,
                                        NULL,
                                        NULL
                                        );
    
    track_ptr->track.codec_context = codec_ctx;
    track_ptr->track.stream_index = stream_index;


    *track = track_ptr;
    return ret;
}

void close_video_track(VideoAssetTrack **track) {
    avcodec_close((*track)->track.codec_context);
    sws_freeContext((*track)->sws_ctx);
    av_free(*track);
    *track = NULL;
}


