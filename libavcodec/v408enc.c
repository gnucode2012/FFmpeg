/*
 * v408 encoder
 *
 * Copyright (c) 2012 Carl Eugen Hoyos
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/intreadwrite.h"
#include "avcodec.h"

static av_cold int v408_encode_init(AVCodecContext *avctx)
{
    avctx->coded_frame = avcodec_alloc_frame();

    if (!avctx->coded_frame) {
        av_log(avctx, AV_LOG_ERROR, "Could not allocate frame.\n");
        return AVERROR(ENOMEM);
    }

    return 0;
}

static int v408_encode_frame(AVCodecContext *avctx, uint8_t *buf,
                             int buf_size, void *data)
{
    AVFrame *pic = data;
    uint8_t *dst = buf;
    uint8_t *y, *u, *v, *a;
    int i, j;
    int output_size = 0;

    if (buf_size < avctx->width * avctx->height * 4) {
        av_log(avctx, AV_LOG_ERROR, "Out buffer is too small.\n");
        return AVERROR(ENOMEM);
    }

    avctx->coded_frame->reference = 0;
    avctx->coded_frame->key_frame = 1;
    avctx->coded_frame->pict_type = AV_PICTURE_TYPE_I;

    y = pic->data[0];
    u = pic->data[1];
    v = pic->data[2];
    a = pic->data[3];

    for (i = 0; i < avctx->height; i++) {
        for (j = 0; j < avctx->width; j++) {
           if (avctx->codec_id==CODEC_ID_AYUV) {
                *dst++ = v[j];
                *dst++ = u[j];
                *dst++ = y[j];
                *dst++ = a[j];
            } else {
                *dst++ = u[j];
                *dst++ = y[j];
                *dst++ = v[j];
                *dst++ = a[j];
            }
            output_size += 4;
        }
        y += pic->linesize[0];
        u += pic->linesize[1];
        v += pic->linesize[2];
        a += pic->linesize[3];
    }

    return output_size;
}

static av_cold int v408_encode_close(AVCodecContext *avctx)
{
    av_freep(&avctx->coded_frame);

    return 0;
}

AVCodec ff_ayuv_encoder = {
    .name         = "ayuv",
    .type         = AVMEDIA_TYPE_VIDEO,
    .id           = CODEC_ID_AYUV,
    .init         = v408_encode_init,
    .encode       = v408_encode_frame,
    .close        = v408_encode_close,
    .pix_fmts     = (const enum PixelFormat[]){ PIX_FMT_YUVA444P, PIX_FMT_NONE },
    .long_name    = NULL_IF_CONFIG_SMALL("Uncompressed packed MS 4:4:4:4"),
};

AVCodec ff_v408_encoder = {
    .name         = "v408",
    .type         = AVMEDIA_TYPE_VIDEO,
    .id           = CODEC_ID_V408,
    .init         = v408_encode_init,
    .encode       = v408_encode_frame,
    .close        = v408_encode_close,
    .pix_fmts     = (const enum PixelFormat[]){ PIX_FMT_YUVA444P, PIX_FMT_NONE },
    .long_name    = NULL_IF_CONFIG_SMALL("Uncompressed packed QT 4:4:4:4"),
};