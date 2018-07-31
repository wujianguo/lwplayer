//
//  media_asset_track.c
//  player
//
//  Created by Jianguo Wu on 2018/7/23.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "media_asset_track.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <assert.h>

void init_track(MediaAssetTrack *track, AVCodecContext *codec_ctx) {
    memset(track, 0, sizeof(MediaAssetTrack));
    
    track->packet_max_size = MAX_PACKET_NUM;
    track->packet_index = 0;
    track->packet_size = 0;
    track->packet_mutex = SDL_CreateMutex();

    track->frame_max_size = MAX_FRAME_NUM;
    track->frame_index = 0;
    track->frame_size = 0;
    track->frame_mutex = SDL_CreateMutex();    
}

int packet_queue_put(MediaAssetTrack *track, AVPacket *packet) {
    SDL_LockMutex(track->packet_mutex);
    if (track->packet_size == track->packet_max_size) {
        SDL_UnlockMutex(track->packet_mutex);
        return AVERROR(EAGAIN);
    }
    assert(track->packet_size < track->packet_max_size);
    unsigned int index = (track->packet_index + track->packet_size) % track->packet_max_size;
    av_packet_move_ref(&track->packets[index], packet);
    track->packet_size += 1;
    av_log(NULL, AV_LOG_ERROR, "packet_queue_put packet index: %d, size: %d\n", track->packet_index, track->packet_size);
    SDL_UnlockMutex(track->packet_mutex);
    return 0;
}

int packet_queue_get(MediaAssetTrack *track, AVPacket **packet) {
    SDL_LockMutex(track->packet_mutex);
    if (track->packet_size == 0) {
        SDL_UnlockMutex(track->packet_mutex);
        return AVERROR(EAGAIN);
    }
    assert(track->packet_size > 0);
    *packet = &track->packets[track->packet_index];

    av_log(NULL, AV_LOG_ERROR, "packet_queue_get packet index: %d, size: %d\n", track->packet_index, track->packet_size);
    SDL_UnlockMutex(track->packet_mutex);
    return 0;
}

int packet_queue_release(MediaAssetTrack *track) {
    SDL_LockMutex(track->packet_mutex);
    assert(track->packet_size > 0);
    av_packet_unref(&track->packets[track->packet_index]);
    track->packet_index = (track->packet_index + track->packet_max_size + 1) % track->packet_max_size;
    track->packet_size -= 1;
    SDL_UnlockMutex(track->packet_mutex);
    return 0;
}


int packet_queue_full(MediaAssetTrack *track) {
    return track->packet_size >= MAX_PACKET_NUM;
}


int frame_queue_put_video(struct SwsContext *sws_ctx, MediaAssetTrack *track, AVFrame *frame) {
    SDL_LockMutex(track->frame_mutex);
    if (track->frame_size == track->frame_max_size) {
        SDL_UnlockMutex(track->frame_mutex);
        return AVERROR(EAGAIN);
    }
    assert(track->frame_size < track->frame_max_size);
    unsigned int index = (track->frame_index + track->frame_size) % track->frame_max_size;
    AVFrame *frame_out = track->frames[index];
    sws_scale(sws_ctx, (uint8_t const *const *)frame->data,
              frame->linesize, 0, track->codec_context->height,
              frame_out->data, frame_out->linesize);
    track->frame_size += 1;
    av_log(NULL, AV_LOG_ERROR, "frame_queue_put_video  frame index: %d, size: %d\n", track->frame_index, track->frame_size);

    SDL_UnlockMutex(track->frame_mutex);
    return 0;
}

int frame_queue_put_audio(struct SwrContext *swr_ctx, MediaAssetTrack *track, AVFrame *frame) {
    SDL_LockMutex(track->frame_mutex);
    if (track->frame_size == track->frame_max_size) {
        SDL_UnlockMutex(track->frame_mutex);
        return AVERROR(EAGAIN);
    }
    assert(track->frame_size < track->frame_max_size);
    unsigned int index = (track->frame_index + track->frame_size) % track->frame_max_size;
    AVFrame *frame_out = track->frames[index];
    
//    swr_convert(swr_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);

//    sws_scale(sws_ctx, (uint8_t const *const *)frame->data,
//              frame->linesize, 0, track->codec_context->height,
//              frame_out->data, frame_out->linesize);
    track->frame_size += 1;
    av_log(NULL, AV_LOG_ERROR, "frame_queue_put_audio  frame index: %d, size: %d\n", track->frame_index, track->frame_size);
    
    SDL_UnlockMutex(track->frame_mutex);
    return 0;
}

int frame_queue_get(MediaAssetTrack *track, AVFrame **frame) {
    SDL_LockMutex(track->frame_mutex);
    if (track->frame_size == 0) {
        SDL_UnlockMutex(track->frame_mutex);
        return AVERROR(EAGAIN);
    }
    assert(track->frame_size > 0);
    *frame = track->frames[track->frame_index];
    av_log(NULL, AV_LOG_ERROR, "frame_queue_get  frame index: %d, size: %d\n", track->frame_index, track->frame_size);

    SDL_UnlockMutex(track->frame_mutex);
    return 0;
}

int frame_queue_release(MediaAssetTrack *track) {
    SDL_LockMutex(track->frame_mutex);
    assert(track->frame_size > 0);
//    av_frame_unref(track->frames[track->frame_index]);
    track->frame_index = (track->frame_index + track->frame_max_size + 1) % track->frame_max_size;
    track->frame_size -= 1;
    SDL_UnlockMutex(track->frame_mutex);
    return 0;
}


int frame_queue_full(MediaAssetTrack *track) {
    return track->frame_size >= MAX_FRAME_NUM;
}
