//
//  reader_thread.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "reader_thread.h"
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include "audio_decoder.h"
#include "video_decoder.h"


int reader_thread(void *data) {

    MediaAsset *asset = (MediaAsset *)data;
    int ret = 0;

    AVPacket packet = {0};
//    SDL_mutex *wait_mutex = SDL_CreateMutex();
    
    while (1) {
        ret = av_read_frame(asset->ifmt_ctx, &packet);
        if (ret < 0) {
            break;
        }
//        av_log(NULL, AV_LOG_ERROR, "read success");
        if (packet.stream_index == asset->video->track.stream_index) {
            while ((ret = packet_queue_put(&asset->video->track, &packet)) == AVERROR(EAGAIN)) {
                av_usleep(50);
            }
        } else if (packet.stream_index == asset->audio->track.stream_index) {
//            while ((ret = packet_queue_put(&asset->audio->track, &packet)) == AVERROR(EAGAIN)) {
//                av_usleep(50);
//            }
        }

        while (packet_queue_full(&asset->video->track) || packet_queue_full(&asset->audio->track)) {
            av_usleep(50);
        }

    }
//    SDL_DestroyMutex(wait_mutex);
    
    return ret;
}

