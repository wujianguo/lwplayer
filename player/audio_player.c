//
//  audio_player.c
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#include "audio_player.h"
#include <libavutil/time.h>
#include "media_asset.h"

void audio_callback(void *data, Uint8 *stream, int len) {
    MediaAsset *asset = (MediaAsset *)data;
    AudioAssetTrack *track = asset->audio;
    
    int ret = 0;
    AVFrame *frame = NULL;
    
    while ((ret = frame_queue_get(&track->track, &frame)) == AVERROR(EAGAIN)) {
        av_usleep(1000);
    }
    frame_queue_release(&track->track);
}
