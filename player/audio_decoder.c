//
//  audio_decoder.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "audio_decoder.h"
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


int audio_decoder_thread(void *data) {
    MediaAsset *asset = (MediaAsset *)data;
    AudioAssetTrack *track = asset->audio;
    
    AVPacket *packet = NULL;
    AVFrame *frame = av_frame_alloc();
    int ret = 0;
    
    while (1) {
        ret = packet_queue_get(&track->track, &packet);
        if (ret == AVERROR(EAGAIN)) {
            av_usleep(1000);
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
        
        while ((ret = frame_queue_put_audio(track->swr_ctx, &track->track, frame)) == AVERROR(EAGAIN)) {
            av_usleep(50);
        }
    }
    
    return ret;
}


int open_audio_track(AVFormatContext *ifmt_ctx, unsigned int stream_index, AudioAssetTrack **track) {
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
    
    AudioAssetTrack *track_ptr = av_malloc(sizeof(AudioAssetTrack));
    
    init_track(&track_ptr->track, codec_ctx);
    
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    //nb_samples: AAC-1024 MP3-1152
    int out_nb_samples = codec_ctx->frame_size;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = 44100;
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
    
//    out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);

    int64_t in_channel_layout = av_get_default_channel_layout(codec_ctx->channels);

    struct SwrContext *swr_ctx = NULL;
    swr_ctx = swr_alloc();
    swr_ctx = swr_alloc_set_opts(swr_ctx, out_channel_layout,
                               out_sample_fmt, out_sample_rate,
                               in_channel_layout, codec_ctx->sample_fmt,
                               codec_ctx->sample_rate, 0, NULL);
    swr_init(swr_ctx);

    track_ptr->swr_ctx = swr_ctx;
    track_ptr->track.codec_context = codec_ctx;
    track_ptr->track.stream_index = stream_index;
    
    
    *track = track_ptr;
    return ret;
}

void close_audio_track(AudioAssetTrack **track) {
    
}
