//
//  audio_decoder.h
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#ifndef audio_decoder_h
#define audio_decoder_h

#include "media_asset.h"

int open_audio_track(AVFormatContext *ifmt_ctx, unsigned int stream_index, AudioAssetTrack **track);

void close_audio_track(AudioAssetTrack **track);

int audio_decoder_thread(void *data);

#endif /* audio_decoder_h */
