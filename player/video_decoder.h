//
//  video_decoder.h
//  player
//
//  Created by Jianguo Wu on 2018/7/20.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#ifndef video_decoder_h
#define video_decoder_h

#include "media_asset.h"

int open_video_track(AVFormatContext *ifmt_ctx, unsigned int stream_index, VideoAssetTrack **track);

void close_video_track(VideoAssetTrack **track);


int video_decoder_thread(void *data);

#endif /* video_decoder_h */
