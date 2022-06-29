/**
 * Copyright (c) 2019 Parrot Drones SAS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Parrot Drones SAS Company nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE PARROT DRONES SAS COMPANY BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <video-defs/vdefs.h>


/* Build a raw format from:
 *  - the pixel format, order, layout and size
 *  - the data layout, padding, endianess and size */
#define VDEF_MAKE_RAW_FORMAT(_name,                                            \
			     _p_fmt,                                           \
			     _p_order,                                         \
			     _p_layout,                                        \
			     _p_size,                                          \
			     _d_layout,                                        \
			     _d_pad,                                           \
			     _d_end,                                           \
			     _d_size)                                          \
	const struct vdef_raw_format _name = {                                 \
		.pix_format = VDEF_RAW_PIX_FORMAT_##_p_fmt,                    \
		.pix_order = VDEF_RAW_PIX_ORDER_##_p_order,                    \
		.pix_layout = VDEF_RAW_PIX_LAYOUT_##_p_layout,                 \
		.pix_size = _p_size,                                           \
		.data_layout = VDEF_RAW_DATA_LAYOUT_##_d_layout,               \
		.data_pad_low = _d_pad,                                        \
		.data_little_endian = _d_end,                                  \
		.data_size = _d_size,                                          \
	}


/* RAW */
VDEF_MAKE_RAW_FORMAT(vdef_raw8, RAW, A, LINEAR, 8, PACKED, false, false, 8);
VDEF_MAKE_RAW_FORMAT(vdef_raw10_packed,
		     RAW,
		     A,
		     LINEAR,
		     10,
		     PACKED,
		     false,
		     true,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_raw10, RAW, A, LINEAR, 10, PACKED, true, true, 16);
VDEF_MAKE_RAW_FORMAT(vdef_raw12_packed,
		     RAW,
		     A,
		     LINEAR,
		     12,
		     PACKED,
		     false,
		     true,
		     12);
VDEF_MAKE_RAW_FORMAT(vdef_raw12, RAW, A, LINEAR, 12, PACKED, true, true, 16);
VDEF_MAKE_RAW_FORMAT(vdef_raw14_packed,
		     RAW,
		     A,
		     LINEAR,
		     14,
		     PACKED,
		     false,
		     true,
		     14);
VDEF_MAKE_RAW_FORMAT(vdef_raw14, RAW, A, LINEAR, 14, PACKED, true, true, 16);
VDEF_MAKE_RAW_FORMAT(vdef_raw16, RAW, A, LINEAR, 16, PACKED, false, true, 16);
VDEF_MAKE_RAW_FORMAT(vdef_raw32, RAW, A, LINEAR, 32, PACKED, false, true, 32);


/* GRAY */
VDEF_MAKE_RAW_FORMAT(vdef_gray, GRAY, A, LINEAR, 8, PACKED, false, false, 8);
VDEF_MAKE_RAW_FORMAT(vdef_gray16, GRAY, A, LINEAR, 16, PACKED, false, true, 16);


/* YUV422 planar (I420 / YV12) */
VDEF_MAKE_RAW_FORMAT(vdef_i420,
		     YUV420,
		     YUV,
		     LINEAR,
		     8,
		     PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_i420_10_16le,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     PLANAR,
		     false,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_i420_10_16be,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     PLANAR,
		     false,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_i420_10_16le_high,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     PLANAR,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_i420_10_16be_high,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     PLANAR,
		     true,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_yv12,
		     YUV420,
		     YVU,
		     LINEAR,
		     8,
		     PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_yv12_10_16le,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     PLANAR,
		     false,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_yv12_10_16be,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     PLANAR,
		     false,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_yv12_10_16le_high,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     PLANAR,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_yv12_10_16be_high,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     PLANAR,
		     true,
		     false,
		     16);


/* YUV420 semi-planar (NV12 / NV21) */
VDEF_MAKE_RAW_FORMAT(vdef_nv12,
		     YUV420,
		     YUV,
		     LINEAR,
		     8,
		     SEMI_PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_nv12_10_packed,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_nv12_10_16le,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv12_10_16be,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv12_10_16le_high,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv12_10_16be_high,
		     YUV420,
		     YUV,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     true,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv21,
		     YUV420,
		     YVU,
		     LINEAR,
		     8,
		     SEMI_PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_10_packed,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_10_16le,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_10_16be,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_10_16le_high,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_10_16be_high,
		     YUV420,
		     YVU,
		     LINEAR,
		     10,
		     SEMI_PLANAR,
		     true,
		     false,
		     16);


/* RGB24 */
VDEF_MAKE_RAW_FORMAT(vdef_rgb, RGB24, RGB, LINEAR, 8, PACKED, false, false, 8);
VDEF_MAKE_RAW_FORMAT(vdef_bgr, RGB24, BGR, LINEAR, 8, PACKED, false, false, 8);


/* RGBA32 */
VDEF_MAKE_RAW_FORMAT(vdef_rgba,
		     RGBA32,
		     RGBA,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_abgr,
		     RGBA32,
		     ABGR,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_bgra,
		     RGBA32,
		     BGRA,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);

/* Bayer 8-bits */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb,
		     BAYER,
		     RGGB,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr,
		     BAYER,
		     BGGR,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg,
		     BAYER,
		     GRBG,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg,
		     BAYER,
		     GBRG,
		     LINEAR,
		     8,
		     PACKED,
		     false,
		     false,
		     8);


/* Bayer 10-bits packed */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_10_packed,
		     BAYER,
		     RGGB,
		     LINEAR,
		     10,
		     PACKED,
		     false,
		     true,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_10_packed,
		     BAYER,
		     BGGR,
		     LINEAR,
		     10,
		     PACKED,
		     false,
		     true,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_10_packed,
		     BAYER,
		     GRBG,
		     LINEAR,
		     10,
		     PACKED,
		     false,
		     true,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_10_packed,
		     BAYER,
		     GBRG,
		     LINEAR,
		     10,
		     PACKED,
		     false,
		     true,
		     10);


/* Bayer 10-bits */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_10,
		     BAYER,
		     RGGB,
		     LINEAR,
		     10,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_10,
		     BAYER,
		     BGGR,
		     LINEAR,
		     10,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_10,
		     BAYER,
		     GRBG,
		     LINEAR,
		     10,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_10,
		     BAYER,
		     GBRG,
		     LINEAR,
		     10,
		     PACKED,
		     true,
		     true,
		     16);


/* Bayer 12-bits packed */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_12_packed,
		     BAYER,
		     RGGB,
		     LINEAR,
		     12,
		     PACKED,
		     false,
		     true,
		     12);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_12_packed,
		     BAYER,
		     BGGR,
		     LINEAR,
		     12,
		     PACKED,
		     false,
		     true,
		     12);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_12_packed,
		     BAYER,
		     GRBG,
		     LINEAR,
		     12,
		     PACKED,
		     false,
		     true,
		     12);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_12_packed,
		     BAYER,
		     GBRG,
		     LINEAR,
		     12,
		     PACKED,
		     false,
		     true,
		     12);


/* Bayer 12-bits */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_12,
		     BAYER,
		     RGGB,
		     LINEAR,
		     12,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_12,
		     BAYER,
		     BGGR,
		     LINEAR,
		     12,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_12,
		     BAYER,
		     GRBG,
		     LINEAR,
		     12,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_12,
		     BAYER,
		     GBRG,
		     LINEAR,
		     12,
		     PACKED,
		     true,
		     true,
		     16);


/* Bayer 14-bits packed */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_14_packed,
		     BAYER,
		     RGGB,
		     LINEAR,
		     14,
		     PACKED,
		     false,
		     true,
		     14);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_14_packed,
		     BAYER,
		     BGGR,
		     LINEAR,
		     14,
		     PACKED,
		     false,
		     true,
		     14);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_14_packed,
		     BAYER,
		     GRBG,
		     LINEAR,
		     14,
		     PACKED,
		     false,
		     true,
		     14);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_14_packed,
		     BAYER,
		     GBRG,
		     LINEAR,
		     14,
		     PACKED,
		     false,
		     true,
		     14);


/* Bayer 14-bits */
VDEF_MAKE_RAW_FORMAT(vdef_bayer_rggb_14,
		     BAYER,
		     RGGB,
		     LINEAR,
		     14,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_bggr_14,
		     BAYER,
		     BGGR,
		     LINEAR,
		     14,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_grbg_14,
		     BAYER,
		     GRBG,
		     LINEAR,
		     14,
		     PACKED,
		     true,
		     true,
		     16);
VDEF_MAKE_RAW_FORMAT(vdef_bayer_gbrg_14,
		     BAYER,
		     GBRG,
		     LINEAR,
		     14,
		     PACKED,
		     true,
		     true,
		     16);

/* Hardware specific formats */
VDEF_MAKE_RAW_FORMAT(vdef_nv21_hisi_tile,
		     YUV420,
		     YVU,
		     HISI_TILE_64x16,
		     8,
		     SEMI_PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_hisi_tile_compressed,
		     YUV420,
		     YVU,
		     HISI_TILE_64x16_COMPRESSED,
		     8,
		     SEMI_PLANAR,
		     false,
		     false,
		     8);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_hisi_tile_10_packed,
		     YUV420,
		     YVU,
		     HISI_TILE_64x16,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_nv21_hisi_tile_compressed_10_packed,
		     YUV420,
		     YVU,
		     HISI_TILE_64x16_COMPRESSED,
		     10,
		     SEMI_PLANAR,
		     false,
		     false,
		     10);
VDEF_MAKE_RAW_FORMAT(vdef_mmal_opaque,
		     UNKNOWN,
		     UNKNOWN,
		     UNKNOWN,
		     8,
		     OPAQUE,
		     false,
		     false,
		     8);


/* Build a coded format from:
 *  - the encoding
 *  - the data format */
#define VDEF_MAKE_CODED_FORMAT(_name, _enc, _fmt)                              \
	const struct vdef_coded_format _name = {                               \
		.encoding = VDEF_ENCODING_##_enc,                              \
		.data_format = VDEF_CODED_DATA_FORMAT_##_fmt,                  \
	}


VDEF_MAKE_CODED_FORMAT(vdef_h264_raw_nalu, H264, RAW_NALU);


VDEF_MAKE_CODED_FORMAT(vdef_h264_byte_stream, H264, BYTE_STREAM);


VDEF_MAKE_CODED_FORMAT(vdef_h264_avcc, H264, AVCC);


VDEF_MAKE_CODED_FORMAT(vdef_h265_raw_nalu, H265, RAW_NALU);


VDEF_MAKE_CODED_FORMAT(vdef_h265_byte_stream, H265, BYTE_STREAM);


VDEF_MAKE_CODED_FORMAT(vdef_h265_hvcc, H265, HVCC);


VDEF_MAKE_CODED_FORMAT(vdef_jpeg_jfif, JPEG, JFIF);
