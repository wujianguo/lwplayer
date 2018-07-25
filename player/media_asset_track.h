//
//  media_asset_track.h
//  player
//
//  Created by Jianguo Wu on 2018/7/23.
//  Copyright © 2018年 wujianguo. All rights reserved.
//

#ifndef media_asset_track_h
#define media_asset_track_h

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <SDL2/SDL.h>

#define MAX_PACKET_NUM   (20)
#define MAX_FRAME_NUM    (20)

typedef struct MediaAssetTrack {
    unsigned int        stream_index;
    AVCodecContext      *codec_context;
    
    AVPacket            packets[MAX_PACKET_NUM];
    unsigned int        packet_index;
    unsigned int        packet_size;
    unsigned int        packet_max_size;
    SDL_mutex           *packet_mutex;
    
    AVFrame             *frames[MAX_FRAME_NUM];
    unsigned int        frame_index;
    unsigned int        frame_size;
    unsigned int        frame_max_size;
    SDL_mutex           *frame_mutex;
} MediaAssetTrack;


typedef struct VideoAssetTrack {
    
    MediaAssetTrack     track;
    
    struct SwsContext   *sws_ctx;
//    AVFrame             *frame;
} VideoAssetTrack;


typedef struct AudioAssetTrack {

    MediaAssetTrack     track;

} AudioAssetTrack;


void init_track(MediaAssetTrack *track, uint8_t *buffer, AVCodecContext *codec_ctx);

//int packet_queue_init(MediaAssetTrack *track);

int packet_queue_put(MediaAssetTrack *track, AVPacket *packet);

int packet_queue_get(MediaAssetTrack *track, AVPacket **packet);

int packet_queue_release(MediaAssetTrack *track);

int packet_queue_full(MediaAssetTrack *track);


int frame_queue_init(MediaAssetTrack *track, uint8_t *buffer, AVCodecContext *codec_ctx);

int frame_queue_put_video(struct SwsContext *sws_ctx, MediaAssetTrack *track, AVFrame *frame);

int frame_queue_get(MediaAssetTrack *track, AVFrame **frame);

int frame_queue_release(MediaAssetTrack *track);

int frame_queue_full(MediaAssetTrack *track);


#endif /* media_asset_track_h */
