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

#ifndef _VDEFS_H_
#define _VDEFS_H_

#include <stdbool.h>
#include <stdint.h>

#include <h264/h264.h>
#include <h265/h265.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* To be used for all public API */
#ifdef VDEF_API_EXPORTS
#	ifdef _WIN32
#		define VDEF_API __declspec(dllexport)
#	else /* !_WIN32 */
#		define VDEF_API __attribute__((visibility("default")))
#	endif /* !_WIN32 */
#else /* !VDEF_API_EXPORTS */
#	define VDEF_API
#endif /* !VDEF_API_EXPORTS */


/* Forward declaration */
struct json_object;


/* Alignment macros */
#define VDEF_ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define VDEF_ALIGN(x, a) VDEF_ALIGN_MASK(x, (typeof(x))(a)-1)
#define VDEF_IS_ALIGNED(x, a) (((x) & ((typeof(x))(a)-1)) == 0)


/* Rounding macros */
#define VDEF_ROUND_UP(n, d)                                                    \
	({                                                                     \
		typeof(n) _n = (n);                                            \
		typeof(d) _d = (d);                                            \
		((_n + _d - 1) / _d);                                          \
	})
#define VDEF_ROUND(n, d)                                                       \
	({                                                                     \
		typeof(n) _n = (n);                                            \
		typeof(d) _d = (d);                                            \
		((_n + (_d / 2)) / _d);                                        \
	})
#define VDEF_ROUND_DOWN(n, d) ((n) / (d))


/**
 * General definitions
 */

#define VDEF_RAW_MIME_TYPE "video/raw"


/* Frame type */
enum vdef_frame_type {
	/* Unknown frame type */
	VDEF_FRAME_TYPE_UNKNOWN = 0,

	/* Raw frame type */
	VDEF_FRAME_TYPE_RAW,

	/* Coded frame type */
	VDEF_FRAME_TYPE_CODED,
};


/* Color primaries */
enum vdef_color_primaries {
	/* Unknown color primaries */
	VDEF_COLOR_PRIMARIES_UNKNOWN = 0,

	/* Rec. ITU-R BT.601-7 525-line color primaries */
	VDEF_COLOR_PRIMARIES_BT601_525,

	/* Rec. ITU-R BT.601-7 625-line color primaries */
	VDEF_COLOR_PRIMARIES_BT601_625,

	/* Rec. ITU-R BT.709-6 / IEC 61966-2-1 sRGB color primaries */
	VDEF_COLOR_PRIMARIES_BT709,
	VDEF_COLOR_PRIMARIES_SRGB = VDEF_COLOR_PRIMARIES_BT709,

	/* Rec. ITU-R BT.2020-2 / Rec. ITU-R BT.2100-1 color primaries */
	VDEF_COLOR_PRIMARIES_BT2020,
	VDEF_COLOR_PRIMARIES_BT2100 = VDEF_COLOR_PRIMARIES_BT2020,

	/* SMPTE RP 431-2 "DCI-P3" color primaries */
	VDEF_COLOR_PRIMARIES_DCI_P3,

	/* SMPTE RP 432-1 "Display-P3" color primaries */
	VDEF_COLOR_PRIMARIES_DISPLAY_P3,

	VDEF_COLOR_PRIMARIES_MAX,
};


/* Color primaries and white point normalized chromacity coordinates */
struct vdef_color_primaries_value {
	/* Color primaries normalized chromacity coordinates;
	 * indexes: 0 = green, 1 = blue, 2 = red */
	struct {
		float x;
		float y;
	} color_primaries[3];

	/* White point normalized chromacity coordinates */
	struct {
		float x;
		float y;
	} white_point;
};


/* Color primaries values; see the corresponding specification documents */
extern VDEF_API const struct vdef_color_primaries_value
	vdef_color_primaries_values[VDEF_COLOR_PRIMARIES_MAX];


/* Transfer functions */
enum vdef_transfer_function {
	/* Unknown transfer function */
	VDEF_TRANSFER_FUNCTION_UNKNOWN = 0,

	/* Rec. ITU-R BT.601-7 525-line or 625-line transfer function */
	VDEF_TRANSFER_FUNCTION_BT601,

	/* Rec. ITU-R BT.709-6 transfer function */
	VDEF_TRANSFER_FUNCTION_BT709,

	/* Rec. ITU-R BT.2020-2 transfer function */
	VDEF_TRANSFER_FUNCTION_BT2020,

	/* SMPTE ST 2084 / Rec. ITU-R BT.2100-1 perceptual quantization
	 * transfer function */
	VDEF_TRANSFER_FUNCTION_PQ,

	/* Rec. ITU-R BT.2100-1 hybrid log-gamma transfer function */
	VDEF_TRANSFER_FUNCTION_HLG,

	/* IEC 61966-2-1 sRGB transfer function */
	VDEF_TRANSFER_FUNCTION_SRGB,

	VDEF_TRANSFER_FUNCTION_MAX,
};


/* Matrix coefficients */
enum vdef_matrix_coefs {
	/* Unknown matrix coefficients */
	VDEF_MATRIX_COEFS_UNKNOWN = 0,

	/* Identity / IEC 61966-2-1 sRGB matrix coefficients */
	VDEF_MATRIX_COEFS_IDENTITY,
	VDEF_MATRIX_COEFS_SRGB = VDEF_MATRIX_COEFS_IDENTITY,

	/* Rec. ITU-R BT.601-7 525-line matrix coefficients */
	VDEF_MATRIX_COEFS_BT601_525,

	/* Rec. ITU-R BT.601-7 625-line matrix coefficients */
	VDEF_MATRIX_COEFS_BT601_625,

	/* Rec. ITU-R BT.709-6 matrix coefficients */
	VDEF_MATRIX_COEFS_BT709,

	/* Rec. ITU-R BT.2020 non-constant luminance system / Rec. ITU-R
	 * BT.2100 matrix coefficients */
	VDEF_MATRIX_COEFS_BT2020_NON_CST,
	VDEF_MATRIX_COEFS_BT2100 = VDEF_MATRIX_COEFS_BT2020_NON_CST,

	/* Rec. ITU-R BT.2020 constant luminance system */
	VDEF_MATRIX_COEFS_BT2020_CST,

	VDEF_MATRIX_COEFS_MAX,
};


/* Dynamic range */
enum vdef_dynamic_range {
	/* Unknown dynamic range */
	VDEF_DYNAMIC_RANGE_UNKNOWN = 0,

	/* Standard dynamic range */
	VDEF_DYNAMIC_RANGE_SDR,

	/* High dynamic range: Parrot 8bit HDR */
	VDEF_DYNAMIC_RANGE_HDR8,

	/* High dynamic range: standard 10bit HDR10
	 * (Rec. ITU-R BT.2020 color primaries,
	 * SMPTE ST 2084 perceptual quantization transfer function
	 * and SMPTE ST 2086 metadata) */
	VDEF_DYNAMIC_RANGE_HDR10,

	VDEF_DYNAMIC_RANGE_MAX,
};


/* Tone mapping */
enum vdef_tone_mapping {
	/* Unknown tone mapping */
	VDEF_TONE_MAPPING_UNKNOWN = 0,

	/* Standard tone mapping */
	VDEF_TONE_MAPPING_STANDARD,

	/* Parrot P-log tone mapping */
	VDEF_TONE_MAPPING_P_LOG,

	VDEF_TONE_MAPPING_MAX,
};


/* RGB to YUV conversion matrix for normalized values for both limited and
 * full range.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 * Note: the matrix multiplication must be done before applying the offset:
 * YUV = (RGB * mat) + off */
extern VDEF_API const float vdef_rgb_to_yuv_norm_offset[VDEF_MATRIX_COEFS_MAX]
						       [2][3];
extern VDEF_API const float vdef_rgb_to_yuv_norm_matrix[VDEF_MATRIX_COEFS_MAX]
						       [2][9];

/* YUV to RGB conversion offsets and matrices for normalized values
 * for both limited and full range.
 * The matrix is in column-major order, which makes it usable in OpenGL.
 * Note: the matrix multiplication must be done after applying the offset:
 * RGB = (YUV + off) * mat */
extern VDEF_API const float vdef_yuv_to_rgb_norm_offset[VDEF_MATRIX_COEFS_MAX]
						       [2][3];
extern VDEF_API const float vdef_yuv_to_rgb_norm_matrix[VDEF_MATRIX_COEFS_MAX]
						       [2][9];

/* BT.709 to BT.2020 and BT.2020 to BT.709 conversion matrices for linearly
 * represented, normalized RGB values.
 * See Rec. ITU-R BT.2087 and Rep. ITU-R BT.2407.
 * The matrices are in column-major order, which makes them usable in OpenGL. */
extern VDEF_API const float vdef_bt709_to_bt2020_matrix[9];
extern VDEF_API const float vdef_bt2020_to_bt709_matrix[9];


/* Frame info flags */
enum vdef_frame_flag {
	/* The frame has no flags set */
	VDEF_FRAME_FLAG_NONE = 0,

	/* The frame memory is not mapped to user-space
	 * Some frame memory cannot be mapped to user-space (e.g. DMA buffer
	 * memory). In such case, this flag is set to specify the application
	 * that memory won't be accessible for read and/or write. If this flag
	 * is not set, the frame is considered readable / writable by CPU. */
	VDEF_FRAME_FLAG_NOT_MAPPED = (1 << 0),

	/* The frame has encountered a data error
	 * When this flag is set, the frame memory data might be corrupted and
	 * should not be read by the application. */
	VDEF_FRAME_FLAG_DATA_ERROR = (1 << 1),

	/* The frame memory data should not be invalidated
	 * When a frame is returned to the library or device with this flag set,
	 * the next generated frame memory should not be invalidated in the CPU
	 * cache. It is only applicable for memory written by DMA-capable
	 * devices and when frame memory is mapped as cached. */
	VDEF_FRAME_FLAG_NO_CACHE_INVALIDATE = (1 << 2),

	/* The frame memory data should not be flushed and cleaned
	 * When a frame is sent to the library or device with this flag set, the
	 * library / device should not flush and clean the CPU cache for the
	 * frame memory. It is only applicable for memory read by DMA-capable
	 * devices and when the buffer is mapped as cached and written by CPU.
	 */
	VDEF_FRAME_FLAG_NO_CACHE_CLEAN = (1 << 3),

	/* The frame contains a visual error
	 * This flag notifies that the frame is valid, but can contain visual
	 * errors. A frame with this flag can be displayed, but should not be
	 * used for vision algorithms. */
	VDEF_FRAME_FLAG_VISUAL_ERROR = (1 << 4),

	/* The frame is not intended to be displayed
	 * This flag is usually applied during a pipeline initialization to the
	 * first few frames. Such frames are not to be displayed nor used for
	 * vision algorithms. */
	VDEF_FRAME_FLAG_SILENT = (1 << 5),

	/* Whether the frame contains a slice for which there is a long-term
	 * reference picture list modification */
	VDEF_FRAME_FLAG_USES_LTR = (1 << 6),
};


/* Dimension */
struct vdef_dim {
	/* Dimension width */
	unsigned int width;

	/* Dimension height */
	unsigned int height;
};


/* Rectangle */
struct vdef_rect {
	/* Horizontal offset of the top-left corner of the rectangle (negative
	 * value means the rectangle is centered horizontally) */
	int left;

	/* Vertical offset of the top-left corner of the rectangle (negative
	 * value means the rectangle is centered vertically) */
	int top;

	/* Rectangle width */
	unsigned int width;

	/* Rectangle height */
	unsigned int height;
};


/* Fraction */
struct vdef_frac {
	/* Numerator of the fraction */
	unsigned int num;

	/* Denominator of the fraction */
	unsigned int den;
};


/* Format information common to raw and coded formats */
struct vdef_format_info {
	/* Video frame rate */
	struct vdef_frac framerate;

	/* Bit depth */
	unsigned int bit_depth;

	/* Full color range */
	bool full_range;

	/* Color primaries */
	enum vdef_color_primaries color_primaries;

	/* Transfer function */
	enum vdef_transfer_function transfer_function;

	/* Matrix coefficients */
	enum vdef_matrix_coefs matrix_coefs;

	/* Dynamic range */
	enum vdef_dynamic_range dynamic_range;

	/* Tone mapping */
	enum vdef_tone_mapping tone_mapping;

	/* Video frame resolution (in pixel units) */
	struct vdef_dim resolution;

	/* Sample aspect ratio (no unit; full
	 * aspect ratio is sar.width / sar.height) */
	struct vdef_dim sar;

	/* Mastering display colour volume */
	struct {
		/* Color primaries */
		enum vdef_color_primaries display_primaries;

		/* Color primaries and white point normalized
		 * chromacity coordinates, if display_primaries is
		 * VDEF_COLOR_PRIMARIES_UNKNOWN (values can be 0 otherwise) */
		struct vdef_color_primaries_value display_primaries_val;

		/* Luminance range maximum value (in cd/m2, 0 if unknown) */
		float max_display_mastering_luminance;

		/* Luminance range minimum value (in cd/m2, 0 if unknown) */
		float min_display_mastering_luminance;
	} mdcv;

	/* Content light level */
	struct {
		/* Maximum content light level (in cd/m2, 0 if unknown) */
		unsigned int max_cll;

		/* Maximum frame average light level (in cd/m2, 0 if unknown) */
		unsigned int max_fall;
	} cll;
};


/* Frame information common to raw and coded frames */
struct vdef_frame_info {
	/* Frame timestamp in units of time scale */
	uint64_t timestamp;

	/* Time scale in Hz */
	uint32_t timescale;

	/* Frame original capture timestamp in microseconds on the
	 * monotonic clock (or 0 if unknown) */
	uint64_t capture_timestamp;

	/* Frame index */
	uint32_t index;

	/* Bit depth */
	unsigned int bit_depth;

	/* Full color range */
	bool full_range;

	/* Color primaries */
	enum vdef_color_primaries color_primaries;

	/* Transfer function */
	enum vdef_transfer_function transfer_function;

	/* Matrix coefficients */
	enum vdef_matrix_coefs matrix_coefs;

	/* Dynamic range */
	enum vdef_dynamic_range dynamic_range;

	/* Tone mapping */
	enum vdef_tone_mapping tone_mapping;

	/* Video frame resolution (in pixel units) */
	struct vdef_dim resolution;

	/* Sample aspect ratio (no unit; full
	 * aspect ratio is sar.width / sar.height) */
	struct vdef_dim sar;

	/* Frame flags (bit-field of enum vdef_frame_flag) */
	uint64_t flags;
};


/**
 * Raw format and frame definitions
 */

/* Raw pixel format */
enum vdef_raw_pix_format {
	/* Unknown or raw pixel format */
	VDEF_RAW_PIX_FORMAT_UNKNOWN = 0,
	VDEF_RAW_PIX_FORMAT_RAW = VDEF_RAW_PIX_FORMAT_UNKNOWN,

	/* YUV/YCbCr 4:2:0 pixel format */
	VDEF_RAW_PIX_FORMAT_YUV420,

	/* YUV/YCbCr 4:2:2 pixel format */
	VDEF_RAW_PIX_FORMAT_YUV422,

	/* YUV/YCbCr 4:4:4 pixel format */
	VDEF_RAW_PIX_FORMAT_YUV444,

	/* Gray pixel format */
	VDEF_RAW_PIX_FORMAT_GRAY,

	/* RGB pixel format */
	VDEF_RAW_PIX_FORMAT_RGB24,

	/* RGB + alpha pixel format */
	VDEF_RAW_PIX_FORMAT_RGBA32,

	/* Bayer pixel format */
	VDEF_RAW_PIX_FORMAT_BAYER,

	/* Depth map pixel format */
	VDEF_RAW_PIX_FORMAT_DEPTH,

	/* Depth map (float data) pixel format */
	VDEF_RAW_PIX_FORMAT_DEPTH_FLOAT,
};


/* Raw pixel ordering */
enum vdef_raw_pix_order {
	/* Unknown pixel ordering */
	VDEF_RAW_PIX_ORDER_UNKNOWN = 0,

	/* A is first pixel */
	VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_ABC = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_AB = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_A = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_RGB = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_RGBA = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_YUYV = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_YUV = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_RGGB = VDEF_RAW_PIX_ORDER_ABCD,
	VDEF_RAW_PIX_ORDER_ABDC,
	VDEF_RAW_PIX_ORDER_ACBD,
	VDEF_RAW_PIX_ORDER_ACB = VDEF_RAW_PIX_ORDER_ACBD,
	VDEF_RAW_PIX_ORDER_YVYU = VDEF_RAW_PIX_ORDER_ACBD,
	VDEF_RAW_PIX_ORDER_YVU = VDEF_RAW_PIX_ORDER_ACBD,
	VDEF_RAW_PIX_ORDER_ACDB,
	VDEF_RAW_PIX_ORDER_ADBC,
	VDEF_RAW_PIX_ORDER_ADCB,

	/* B is first pixel */
	VDEF_RAW_PIX_ORDER_BACD,
	VDEF_RAW_PIX_ORDER_BAC = VDEF_RAW_PIX_ORDER_BACD,
	VDEF_RAW_PIX_ORDER_BA = VDEF_RAW_PIX_ORDER_BACD,
	VDEF_RAW_PIX_ORDER_BADC,
	VDEF_RAW_PIX_ORDER_GRBG = VDEF_RAW_PIX_ORDER_BADC,
	VDEF_RAW_PIX_ORDER_BCAD,
	VDEF_RAW_PIX_ORDER_BCA = VDEF_RAW_PIX_ORDER_BCAD,
	VDEF_RAW_PIX_ORDER_BCDA,
	VDEF_RAW_PIX_ORDER_BDAC,
	VDEF_RAW_PIX_ORDER_BDCA,

	/* C is first pixel */
	VDEF_RAW_PIX_ORDER_CABD,
	VDEF_RAW_PIX_ORDER_CAB = VDEF_RAW_PIX_ORDER_CABD,
	VDEF_RAW_PIX_ORDER_CADB,
	VDEF_RAW_PIX_ORDER_CBAD,
	VDEF_RAW_PIX_ORDER_CBA = VDEF_RAW_PIX_ORDER_CBAD,
	VDEF_RAW_PIX_ORDER_BGR = VDEF_RAW_PIX_ORDER_CBAD,
	VDEF_RAW_PIX_ORDER_BGRA = VDEF_RAW_PIX_ORDER_CBAD,
	VDEF_RAW_PIX_ORDER_CBDA,
	VDEF_RAW_PIX_ORDER_CDAB,
	VDEF_RAW_PIX_ORDER_GBRG = VDEF_RAW_PIX_ORDER_CDAB,
	VDEF_RAW_PIX_ORDER_CDBA,

	/* D is first pixel */
	VDEF_RAW_PIX_ORDER_DABC,
	VDEF_RAW_PIX_ORDER_DACB,
	VDEF_RAW_PIX_ORDER_DBAC,
	VDEF_RAW_PIX_ORDER_DBCA,
	VDEF_RAW_PIX_ORDER_DCAB,
	VDEF_RAW_PIX_ORDER_DCBA,
	VDEF_RAW_PIX_ORDER_ABGR = VDEF_RAW_PIX_ORDER_DCBA,
	VDEF_RAW_PIX_ORDER_BGGR = VDEF_RAW_PIX_ORDER_DCBA,
};


/* Raw pixel layout */
enum vdef_raw_pix_layout {
	/* Unknown pixel layout */
	VDEF_RAW_PIX_LAYOUT_UNKNOWN = 0,

	/* Linear pixel layout */
	VDEF_RAW_PIX_LAYOUT_LINEAR,

	/* HiSilicon tiled pixel layout (tiles of 64x16) */
	VDEF_RAW_PIX_LAYOUT_HISI_TILE_64x16,

	/* HiSilicon tiled pixel layout (tiles of 64x16) - compressed */
	VDEF_RAW_PIX_LAYOUT_HISI_TILE_64x16_COMPRESSED,
};


/* Raw data layout */
enum vdef_raw_data_layout {
	/* Unknown data layout */
	VDEF_RAW_DATA_LAYOUT_UNKNOWN = 0,

	/* Packed data layout */
	VDEF_RAW_DATA_LAYOUT_PACKED,
	VDEF_RAW_DATA_LAYOUT_RGB24 = VDEF_RAW_DATA_LAYOUT_PACKED,
	VDEF_RAW_DATA_LAYOUT_RGBA32 = VDEF_RAW_DATA_LAYOUT_PACKED,

	/* Planar data layout */
	VDEF_RAW_DATA_LAYOUT_PLANAR,
	VDEF_RAW_DATA_LAYOUT_PLANAR_R_G_B = VDEF_RAW_DATA_LAYOUT_PLANAR,
	VDEF_RAW_DATA_LAYOUT_PLANAR_R_G_B_A = VDEF_RAW_DATA_LAYOUT_PLANAR,
	VDEF_RAW_DATA_LAYOUT_PLANAR_Y_U_V = VDEF_RAW_DATA_LAYOUT_PLANAR,

	/* Semi-planar data layout */
	VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR,
	VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR_Y_UV =
		VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR,

	/* Interleaved data layout */
	VDEF_RAW_DATA_LAYOUT_INTERLEAVED,
	VDEF_RAW_DATA_LAYOUT_YUYV = VDEF_RAW_DATA_LAYOUT_INTERLEAVED,

	/* Opaque data layout */
	VDEF_RAW_DATA_LAYOUT_OPAQUE,
};


/* Maximum plane count for all raw data layouts */
#define VDEF_RAW_MAX_PLANE_COUNT 4


/* Raw format */
struct vdef_raw_format {
	/* Pixel format */
	enum vdef_raw_pix_format pix_format;

	/* Pixel order */
	enum vdef_raw_pix_order pix_order;

	/* Pixel layout */
	enum vdef_raw_pix_layout pix_layout;

	/* Pixel value size in bits (excluding padding) */
	unsigned int pix_size;

	/* Data layout */
	enum vdef_raw_data_layout data_layout;

	/* Data padding: true is padding in lower bits,
	 * false is padding in higher bits */
	bool data_pad_low;

	/* Data endianness: true is little-endian, false is big-endian */
	bool data_little_endian;

	/* Data size in bits including padding */
	unsigned int data_size;
};


/* Commonly used raw formats */
/* RAW formats */
extern VDEF_API const struct vdef_raw_format vdef_raw8;
extern VDEF_API const struct vdef_raw_format vdef_raw10_packed;
extern VDEF_API const struct vdef_raw_format vdef_raw10;
extern VDEF_API const struct vdef_raw_format vdef_raw12_packed;
extern VDEF_API const struct vdef_raw_format vdef_raw12;
extern VDEF_API const struct vdef_raw_format vdef_raw14_packed;
extern VDEF_API const struct vdef_raw_format vdef_raw14;
extern VDEF_API const struct vdef_raw_format vdef_raw16;
extern VDEF_API const struct vdef_raw_format vdef_raw32;
/* GRAY formats */
extern VDEF_API const struct vdef_raw_format vdef_gray;
extern VDEF_API const struct vdef_raw_format vdef_gray16;
/* YUV422 planar formats */
extern VDEF_API const struct vdef_raw_format vdef_i420;
extern VDEF_API const struct vdef_raw_format vdef_i420_10_16le;
extern VDEF_API const struct vdef_raw_format vdef_i420_10_16be;
extern VDEF_API const struct vdef_raw_format vdef_i420_10_16le_high;
extern VDEF_API const struct vdef_raw_format vdef_i420_10_16be_high;
extern VDEF_API const struct vdef_raw_format vdef_yv12;
extern VDEF_API const struct vdef_raw_format vdef_yv12_10_16le;
extern VDEF_API const struct vdef_raw_format vdef_yv12_10_16be;
extern VDEF_API const struct vdef_raw_format vdef_yv12_10_16le_high;
extern VDEF_API const struct vdef_raw_format vdef_yv12_10_16be_high;
/* YUV420 semi-planar formats */
extern VDEF_API const struct vdef_raw_format vdef_nv12;
extern VDEF_API const struct vdef_raw_format vdef_nv12_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_nv12_10_16be;
extern VDEF_API const struct vdef_raw_format vdef_nv12_10_16le;
extern VDEF_API const struct vdef_raw_format vdef_nv12_10_16be_high;
extern VDEF_API const struct vdef_raw_format vdef_nv12_10_16le_high;
extern VDEF_API const struct vdef_raw_format vdef_nv21;
extern VDEF_API const struct vdef_raw_format vdef_nv21_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_nv21_10_16le;
extern VDEF_API const struct vdef_raw_format vdef_nv21_10_16be;
extern VDEF_API const struct vdef_raw_format vdef_nv21_10_16le_high;
extern VDEF_API const struct vdef_raw_format vdef_nv21_10_16be_high;
/* RGB24 formats */
extern VDEF_API const struct vdef_raw_format vdef_rgb;
extern VDEF_API const struct vdef_raw_format vdef_bgr;
/* RGBA32 formats */
extern VDEF_API const struct vdef_raw_format vdef_rgba;
extern VDEF_API const struct vdef_raw_format vdef_bgra;
extern VDEF_API const struct vdef_raw_format vdef_abgr;
/* Bayer formats */
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_10;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_10;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_10;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_10;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_12_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_12_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_12_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_12_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_12;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_12;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_12;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_12;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_14_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_14_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_14_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_14_packed;
extern VDEF_API const struct vdef_raw_format vdef_bayer_rggb_14;
extern VDEF_API const struct vdef_raw_format vdef_bayer_bggr_14;
extern VDEF_API const struct vdef_raw_format vdef_bayer_grbg_14;
extern VDEF_API const struct vdef_raw_format vdef_bayer_gbrg_14;
/* Hardware specific formats */
extern VDEF_API const struct vdef_raw_format vdef_nv21_hisi_tile;
extern VDEF_API const struct vdef_raw_format vdef_nv21_hisi_tile_compressed;
extern VDEF_API const struct vdef_raw_format vdef_nv21_hisi_tile_10_packed;
extern VDEF_API const struct vdef_raw_format
	vdef_nv21_hisi_tile_compressed_10_packed;
extern VDEF_API const struct vdef_raw_format vdef_mmal_opaque;


/* Raw frame */
struct vdef_raw_frame {
	/* Raw format */
	struct vdef_raw_format format;

	/* Frame information */
	struct vdef_frame_info info;

	/* Planes stride in bytes (the planes count depends on the
	 * data layout; 0 for unused planes) */
	size_t plane_stride[VDEF_RAW_MAX_PLANE_COUNT];
};


/**
 * Coded format and frame definitions
 */

/* Video encoding */
enum vdef_encoding {
	/* Unknown encoding */
	VDEF_ENCODING_UNKNOWN = 0,

	/* ISO/IEC 10918-1 / ITU T.81 Baseline Sequential DCT-based
	 * i.e. JPEG or MJPEG */
	VDEF_ENCODING_JPEG,
	VDEF_ENCODING_MJPEG = VDEF_ENCODING_JPEG,

	/* ISO/IEC 14496-10 AVC / ITU-T H.264 */
	VDEF_ENCODING_H264,
	VDEF_ENCODING_AVC = VDEF_ENCODING_H264,

	/* ISO/IEC 23008-2 HEVC / ITU-T H.265 */
	VDEF_ENCODING_H265,
	VDEF_ENCODING_HEVC = VDEF_ENCODING_H265,
};


/* Coded data format */
enum vdef_coded_data_format {
	/* Unknown coded data format */
	VDEF_CODED_DATA_FORMAT_UNKNOWN = 0,

	/* ITU T.81 Annex B JPEG File Interchange Format (JFIF) */
	VDEF_CODED_DATA_FORMAT_JFIF,

	/* H.264 or H.265 raw NAL units or raw JPEG */
	VDEF_CODED_DATA_FORMAT_RAW_NALU,

	/* H.264 or H.265 Annex B byte stream format */
	VDEF_CODED_DATA_FORMAT_BYTE_STREAM,

	/* H.264 or H.265 AVCC/HVCC format (4-bytes NALU length
	 * in network order) */
	VDEF_CODED_DATA_FORMAT_AVCC,
	VDEF_CODED_DATA_FORMAT_HVCC = VDEF_CODED_DATA_FORMAT_AVCC,
};


/* Coded format */
struct vdef_coded_format {
	/* Video encoding */
	enum vdef_encoding encoding;

	/* Data format */
	enum vdef_coded_data_format data_format;
};


/* Commonly used coded formats */
extern VDEF_API const struct vdef_coded_format vdef_h264_raw_nalu;
extern VDEF_API const struct vdef_coded_format vdef_h264_byte_stream;
extern VDEF_API const struct vdef_coded_format vdef_h264_avcc;
extern VDEF_API const struct vdef_coded_format vdef_h265_raw_nalu;
extern VDEF_API const struct vdef_coded_format vdef_h265_byte_stream;
extern VDEF_API const struct vdef_coded_format vdef_h265_hvcc;
extern VDEF_API const struct vdef_coded_format vdef_jpeg_jfif;


/* Coded frame type */
enum vdef_coded_frame_type {
	/* Unknown coded frame type */
	VDEF_CODED_FRAME_TYPE_UNKNOWN = 0,

	/* Not coded frame (e.g. discarded by an encoder) */
	VDEF_CODED_FRAME_TYPE_NOT_CODED,

	/* H.264/H.265 IDR I-frame or coded JPEG frame */
	VDEF_CODED_FRAME_TYPE_IDR,
	VDEF_CODED_FRAME_TYPE_CODED = VDEF_CODED_FRAME_TYPE_IDR,

	/* H.264/H.265 I-frame (irrelevant in other encodings) */
	VDEF_CODED_FRAME_TYPE_I,

	/* H.264/H.265 P-frame, intra refresh start (irrelevant
	 * in other encodings) */
	VDEF_CODED_FRAME_TYPE_P_IR_START,

	/* H.264/H.265 P-frame (irrelevant in other encodings) */
	VDEF_CODED_FRAME_TYPE_P,

	/* H.264/H.265 Non-reference P-frame (irrelevant in other encodings) */
	VDEF_CODED_FRAME_TYPE_P_NON_REF,
};


/* NAL unit information */
struct vdef_nalu {
	/* NAL unit size in bytes */
	size_t size;

	union {
		/* H.264 NAL unit information */
		struct {
			/* NAL unit type */
			enum h264_nalu_type type;

			/* Slice type (or H264_SLICE_TYPE_UNKNOWN
			 * if not a slice) */
			enum h264_slice_type slice_type;

			/* Slice macroblock count (or 0 if not a slice) */
			unsigned int slice_mb_count;
		} h264;

		/* H.265 NAL unit information */
		struct {
			/* NAL unit type */
			enum h265_nalu_type type;
		} h265;
	};
};


/* Coded frame */
struct vdef_coded_frame {
	/* Coded format */
	struct vdef_coded_format format;

	/* Frame information */
	struct vdef_frame_info info;

	/* Frame type */
	enum vdef_coded_frame_type type;
};


/**
 * Check the validity of a raw format.
 * @param format: raw format
 * @return true if the format is valid, or false otherwise
 */
VDEF_API bool vdef_is_raw_format_valid(const struct vdef_raw_format *format);


/**
 * Compare two vdef_raw_format structs.
 * The components of both formats are compared and if one of them is
 * different, the function will return false.
 * @param f1: the first format to compare
 * @param f2: the second format to compare
 * @return true if both format are identical, false otherwise
 */
VDEF_API bool vdef_raw_format_cmp(const struct vdef_raw_format *f1,
				  const struct vdef_raw_format *f2);


/**
 * Check the intersection of a raw format against capabilities.
 * @param format: raw format
 * @param caps: raw capabilities array
 * @param count: raw capabilities count in array
 * @return true if the format and the capabilities intersect, or false otherwise
 */
VDEF_API bool vdef_raw_format_intersect(const struct vdef_raw_format *format,
					const struct vdef_raw_format *caps,
					unsigned int count);


/**
 * Check the validity of a coded format.
 * @param format: coded format
 * @return true if the format is valid, or false otherwise
 */
VDEF_API bool
vdef_is_coded_format_valid(const struct vdef_coded_format *format);


/**
 * Compare two vdef_coded_format structs.
 * The components of both formats are compared and if one of them is
 * different, the function will return false.
 * @param f1: the first format to compare
 * @param f2: the second format to compare
 * @return true if both format are identical, false otherwise
 */
VDEF_API bool vdef_coded_format_cmp(const struct vdef_coded_format *f1,
				    const struct vdef_coded_format *f2);


/**
 * Check the intersection of a coded format against capabilities.
 * @param format: coded format
 * @param caps: coded capabilities array
 * @param count: coded capabilities count in array
 * @return true if the format and the capabilities intersect, or false otherwise
 */
VDEF_API bool
vdef_coded_format_intersect(const struct vdef_coded_format *format,
			    const struct vdef_coded_format *caps,
			    unsigned int count);


/**
 * Get an enum vdef_frame_type value from a string.
 * Valid strings are only the suffix of the frame type name (eg. 'RAW').
 * The case is ignored.
 * @param str: frame type name to convert
 * @return the enum vdef_frame_type value or VDEF_FRAME_TYPE_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_frame_type vdef_frame_type_from_str(const char *str);


/**
 * Get a string from an enum vdef_frame_type value.
 * @param type: frame type value to convert
 * @return a string description of the frame type
 */
VDEF_API const char *vdef_frame_type_to_str(enum vdef_frame_type type);


/**
 * Get an enum vdef_raw_pix_format value from a string.
 * The case is ignored.
 * @param str: pixel format name to convert
 * @return the enum vdef_raw_pix_format value or VDEF_RAW_PIX_FORMAT_UNKNOWN if
 *         unknown
 */
VDEF_API enum vdef_raw_pix_format vdef_raw_pix_format_from_str(const char *str);


/**
 * Get a string from an enum vdef_raw_pix_format value.
 * @param format: pixel format value to convert
 * @return a string description of the pixel format
 */
VDEF_API const char *
vdef_raw_pix_format_to_str(enum vdef_raw_pix_format format);


/**
 * Get an enum vdef_raw_pix_order value from a string.
 * The case is ignored.
 * @param str: pixel order name to convert
 * @return the enum vdef_raw_pix_order value or VDEF_RAW_PIX_ORDER_UNKNOWN if
 *         unknown
 */
VDEF_API enum vdef_raw_pix_order vdef_raw_pix_order_from_str(const char *str);


/**
 * Get a string from an enum vdef_raw_pix_order value.
 * @param order: pixel order value to convert
 * @return a string description of the pixel order
 */
VDEF_API const char *vdef_raw_pix_order_to_str(enum vdef_raw_pix_order order);


/**
 * Get an enum vdef_raw_pix_layout value from a string.
 * The case is ignored.
 * @param str: pixel layout name to convert
 * @return the enum vdef_raw_pix_layout value or VDEF_RAW_PIX_LAYOUT_UNKNOWN if
 *         unknown
 */
VDEF_API enum vdef_raw_pix_layout vdef_raw_pix_layout_from_str(const char *str);


/**
 * Get a string from an enum vdef_raw_pix_layout value.
 * @param layout: pixel layout value to convert
 * @return a string description of the pixel layout
 */
VDEF_API const char *
vdef_raw_pix_layout_to_str(enum vdef_raw_pix_layout layout);


/**
 * Get an enum vdef_raw_data_layout value from a string.
 * The case is ignored.
 * @param str: data layout name to convert
 * @return the enum vdef_raw_data_layout value or VDEF_RAW_PIX_LAYOUT_UNKNOWN if
 *         unknown
 */
VDEF_API enum vdef_raw_data_layout
vdef_raw_data_layout_from_str(const char *str);


/**
 * Get a string from an enum vdef_raw_data_layout value.
 * @param layout: data layout value to convert
 * @return a string description of the data layout
 */
VDEF_API const char *
vdef_raw_data_layout_to_str(enum vdef_raw_data_layout layout);


/* Helper macros for printing a raw format as a string from a struct
 * vdef_raw_format */
#define VDEF_RAW_FORMAT_TO_STR_FMT "%s/%s/%s/%u/%s/%s/%s/%u"
/* codecheck_ignore[COMPLEX_MACRO] */
#define VDEF_RAW_FORMAT_TO_STR_ARG(_raw_format)                                \
	vdef_raw_pix_format_to_str((_raw_format)->pix_format),                 \
		vdef_raw_pix_order_to_str((_raw_format)->pix_order),           \
		vdef_raw_pix_layout_to_str((_raw_format)->pix_layout),         \
		(_raw_format)->pix_size,                                       \
		vdef_raw_data_layout_to_str((_raw_format)->data_layout),       \
		(_raw_format)->data_pad_low ? "LOW" : "HIGH",                  \
		(_raw_format)->data_little_endian ? "LE" : "BE",               \
		(_raw_format)->data_size


/**
 * Fill a struct vdef_raw_format from a string.
 * The case is ignored.
 * @param str: raw format name to convert
 * @param format: raw format to fill
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API int vdef_raw_format_from_str(const char *str,
				      struct vdef_raw_format *format);


/**
 * Get a string from a struct enum vdef_raw_format.
 * @param format: raw format to convert
 * @return a string description of the raw format, the string should be freed
 *         after usage
 */
VDEF_API char *vdef_raw_format_to_str(const struct vdef_raw_format *format);


/**
 * Get the raw frame plane count for a given raw frame format.
 * @param format: raw frame format
 * @return the plane count for the format or 0 if unknown
 */
VDEF_API unsigned int
vdef_get_raw_frame_plane_count(const struct vdef_raw_format *format);


/**
 * Calculate the raw frame plane stride and size for a given raw frame format.
 * This function can be used to calculate the stride and / or size of raw frame
 * planes.
 * If the plane_stride is set to NULL, the default stride value is calculated
 * and used for size calculation.
 * If the plane_stride_align is not NULL and values are different from 0, the
 * plane strides will be aligned.
 * If the plane_size is set to NULL, only stride calculation will be performed.
 * If the plane_size_align is not NULL and values are different from 0, the
 * plane size will be aligned.
 * @param format: raw frame format
 * @param resolution: the resolution of the frame in pixels
 * @param plane_stride: an array of VDEF_RAW_MAX_PLANE_COUNT plane strides in
 *        bytes, can be NULL (for NULL or value of 0, default stride value is
 *        calculated)
 * @param plane_stride_align: an array of VDEF_RAW_MAX_PLANE_COUNT plane stride
 *        alignment constraints, can be NULL (for NULL or value of 0, no
 *        alignment is applied)
 * @param plane_scanline: an array of VDEF_RAW_MAX_PLANE_COUNT plane scanlines
 *        in lines, can be NULL (for NULL or value of 0, default scanline
 *        value is calculated)
 * @param plane_scanline_align: an array of VDEF_RAW_MAX_PLANE_COUNT plane
 *        scanline alignment constraints, can be NULL (for NULL or value of 0,
 *        no alignment is applied)
 * @param plane_size: an array of VDEF_RAW_MAX_PLANE_COUNT for plane sizes in
 *        bytes, can be NULL
 * @param plane_size_align: an array of VDEF_RAW_MAX_PLANE_COUNT for plane size
 *        alignment constraints, can NULL (for NULL or value of 0, no alignment
 *        is applied)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API int vdef_calc_raw_frame_size(const struct vdef_raw_format *format,
				      const struct vdef_dim *resolution,
				      size_t *plane_stride,
				      const unsigned int *plane_stride_align,
				      size_t *plane_scanline,
				      const unsigned int *plane_scanline_align,
				      size_t *plane_size,
				      const unsigned int *plane_size_align);


/**
 * Calculate the total raw frame size for a given raw frame format.
 * This function will calculate the total raw frame size for a frame stored
 * contiguously in memory.
 * For more details, please see vdef_calc_raw_frame_size().
 * @param format: raw frame format
 * @param resolution: the resolution of the frame in pixels
 * @param plane_stride: an array of VDEF_RAW_MAX_PLANE_COUNT plane strides in
 *        bytes, can be NULL (for NULL or value of 0, default stride value is
 *        calculated)
 * @param plane_stride_align: an array of VDEF_RAW_MAX_PLANE_COUNT plane stride
 *        alignment constraints, can be NULL (for NULL or value of 0, no
 *        alignment is applied)
 * @param plane_scanline: an array of VDEF_RAW_MAX_PLANE_COUNT plane scanlines
 *        in lines, can be NULL (for NULL or value of 0, default scanline
 *        value is calculated)
 * @param plane_scanline_align: an array of VDEF_RAW_MAX_PLANE_COUNT plane
 *        scanline alignment constraints, can be NULL (for NULL or value of 0,
 *        no alignment is applied)
 * @param plane_size_align: an array of VDEF_RAW_MAX_PLANE_COUNT for plane size
 *        alignment constraints, can be NULL (for NULL or value of 0, no
 *        alignment is applied)
 * @return the raw frame size in bytes, or negative errno value in case of error
 */
VDEF_API ssize_t
vdef_calc_raw_contiguous_frame_size(const struct vdef_raw_format *format,
				    const struct vdef_dim *resolution,
				    size_t *plane_stride,
				    const unsigned int *plane_stride_align,
				    size_t *plane_scanline,
				    const unsigned int *plane_scanline_align,
				    const unsigned int *plane_size_align);


/**
 * Get an enum vdef_encoding value from a string.
 * Valid strings are only the suffix of the encoding name (eg. 'H264').
 * The case is ignored.
 * @param str: encoding name to convert
 * @return the enum vdef_encoding value or VDEF_ENCODING_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_encoding vdef_encoding_from_str(const char *str);


/**
 * Get a string from an enum vdef_encoding value.
 * @param encoding: encoding value to convert
 * @return a string description of the encoding
 */
VDEF_API const char *vdef_encoding_to_str(enum vdef_encoding encoding);


/**
 * Get the MIME type for a given encoding.
 * @param encoding: encoding value
 * @return the MIME type string for the encoding
 */
VDEF_API const char *vdef_get_encoding_mime_type(enum vdef_encoding encoding);


/**
 * Get an enum vdef_coded_data_format value from a string.
 * Valid strings are only the suffix of the coded data format name (eg.
 * 'BYTE_STREAM'). The case is ignored.
 * @param str: coded data format name to convert
 * @return the enum vdef_coded_data_format value or
 *         VDEF_CODED_DATA_FORMAT_UNKNOWN if unknown
 */
VDEF_API enum vdef_coded_data_format
vdef_coded_data_format_from_str(const char *str);


/**
 * Get a string from an enum vdef_coded_data_format value.
 * @param format: coded data format value to convert
 * @return a string description of the coded data format
 */
VDEF_API const char *
vdef_coded_data_format_to_str(enum vdef_coded_data_format format);


/* Helper macros for printing a coded format as a string from a struct
 * vdef_coded_format */
#define VDEF_CODED_FORMAT_TO_STR_FMT "%s/%s"
/* codecheck_ignore[COMPLEX_MACRO] */
#define VDEF_CODED_FORMAT_TO_STR_ARG(_coded_format)                            \
	vdef_encoding_to_str((_coded_format)->encoding),                       \
		vdef_coded_data_format_to_str((_coded_format)->data_format)


/**
 * Fill a struct vdef_coded_format from a string.
 * The case is ignored.
 * @param str: raw format name to convert
 * @param format: raw format to fill
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API int vdef_coded_format_from_str(const char *str,
					struct vdef_coded_format *format);


/**
 * Get a string from a struct enum vdef_coded_format.
 * @param format: raw format to convert
 * @return a string description of the coded format, the string should be freed
 *         after usage
 */
VDEF_API char *vdef_coded_format_to_str(const struct vdef_coded_format *format);


/**
 * Get an enum vdef_coded_frame_type value from a string.
 * Valid strings are only the suffix of the coded frame type name (eg. 'P').
 * The case is ignored.
 * @param str: coded frame type name to convert
 * @return the enum vdef_coded_frame_type value or VDEF_CODED_FRAME_TYPE_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_coded_frame_type
vdef_coded_frame_type_from_str(const char *str);


/**
 * Get a string from an enum vdef_coded_frame_type value.
 * @param type: coded frame type value to convert
 * @return a string description of the coded frame type
 */
VDEF_API const char *
vdef_coded_frame_type_to_str(enum vdef_coded_frame_type color);


/**
 * Get an enum vdef_color_primaries value from an H.264 VUI colour_primaries
 * value (Rec. ITU-T H.264 E.1.1, also defined in Rec. ITU-T H.273).
 * @param color_primaries: H.264 VUI colour_primaries value to convert
 * @return the enum vdef_color_primaries value or VDEF_COLOR_PRIMARIES_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_color_primaries
vdef_color_primaries_from_h264(uint32_t color_primaries);


/**
 * Get an H.264 VUI colour_primaries value from an enum vdef_color_primaries
 * value (Rec. ITU-T H.264 E.1.1, also defined in Rec. ITU-T H.273).
 * @param color: color primaries value
 * @return the H.264 VUI colour_primaries value
 */
VDEF_API uint32_t vdef_color_primaries_to_h264(enum vdef_color_primaries color);


/**
 * Get an enum vdef_color_primaries value from an H.265 VUI colour_primaries
 * value (Rec. ITU-T H.265 E.2.1, also defined in Rec. ITU-T H.273).
 * @param color_primaries: H.265 VUI colour_primaries value to convert
 * @return the enum vdef_color_primaries value or VDEF_COLOR_PRIMARIES_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_color_primaries
vdef_color_primaries_from_h265(uint32_t color_primaries);


/**
 * Get an H.265 VUI colour_primaries value from an enum vdef_color_primaries
 * value (Rec. ITU-T H.265 E.2.1, also defined in Rec. ITU-T H.273).
 * @param color: color primaries value
 * @return the H.265 VUI colour_primaries value
 */
VDEF_API uint32_t vdef_color_primaries_to_h265(enum vdef_color_primaries color);


/**
 * Get an enum vdef_color_primaries value from a string.
 * Valid strings are only the suffix of the color primaries name (eg. 'BT709').
 * The case is ignored.
 * @param str: color primaries name to convert
 * @return the enum vdef_color_primaries value or VDEF_COLOR_PRIMARIES_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_color_primaries
vdef_color_primaries_from_str(const char *str);


/**
 * Get a string from an enum vdef_color_primaries value.
 * @param color: color primaries value to convert
 * @return a string description of the color primaries
 */
VDEF_API const char *
vdef_color_primaries_to_str(enum vdef_color_primaries color);


/**
 * Get an enum vdef_color_primaries value from color primaries and white point
 * normalized chromacity coordinates.
 * @param val: color primaries and white point normalized
 *             chromacity coordinates
 * @return the enum vdef_color_primaries value or VDEF_COLOR_PRIMARIES_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_color_primaries
vdef_color_primaries_from_values(const struct vdef_color_primaries_value *val);


/**
 * Get an enum vdef_transfer_function value from an H.264 VUI
 * transfer_characteristics value (Rec. ITU-T H.264 E.1.1, also defined
 * in Rec. ITU-T H.273).
 * @param transfer_characteristics: H.264 VUI transfer_characteristics
 *        value to convert
 * @return the enum vdef_transfer_function value or
 *         VDEF_TRANSFER_FUNCTION_UNKNOWN if unknown
 */
VDEF_API enum vdef_transfer_function
vdef_transfer_function_from_h264(uint32_t transfer_characteristics);


/**
 * Get an H.264 VUI transfer_characteristics value from an enum
 * vdef_transfer_function value (Rec. ITU-T H.264 E.1.1, also defined
 * in Rec. ITU-T H.273).
 * @param transfer: transfer function value
 * @return the H.264 VUI transfer_characteristics value
 */
VDEF_API uint32_t
vdef_transfer_function_to_h264(enum vdef_transfer_function transfer);


/**
 * Get an enum vdef_transfer_function value from an H.265 VUI
 * transfer_characteristics value (Rec. ITU-T H.265 E.2.1, also defined
 * in Rec. ITU-T H.273).
 * @param transfer_characteristics: H.265 VUI transfer_characteristics
 *        value to convert
 * @return the enum vdef_transfer_function value or
 *         VDEF_TRANSFER_FUNCTION_UNKNOWN if unknown
 */
VDEF_API enum vdef_transfer_function
vdef_transfer_function_from_h265(uint32_t transfer_characteristics);


/**
 * Get an H.265 VUI transfer_characteristics value from an enum
 * vdef_transfer_function value (Rec. ITU-T H.265 E.2.1, also defined
 * in Rec. ITU-T H.273).
 * @param transfer: transfer function value
 * @return the H.265 VUI transfer_characteristics value
 */
VDEF_API uint32_t
vdef_transfer_function_to_h265(enum vdef_transfer_function transfer);


/**
 * Get an enum vdef_transfer_function value from a string.
 * Valid strings are only the suffix of the transfer function name (eg.
 * 'BT709'). The case is ignored.
 * @param str: transfer function name to convert
 * @return the enum vdef_transfer_function value or
 * VDEF_TRANSFER_FUNCTION_UNKNOWN if unknown
 */
VDEF_API enum vdef_transfer_function
vdef_transfer_function_from_str(const char *str);


/**
 * Get a string from an enum vdef_transfer_function value.
 * @param transfer: transfer function value to convert
 * @return a string description of the transfer function
 */
VDEF_API const char *
vdef_transfer_function_to_str(enum vdef_transfer_function transfer);


/**
 * Get an enum vdef_matrix_coefs value from an H.264 VUI matrix_coefficients
 * value (Rec. ITU-T H.264 E.1.1, also defined in Rec. ITU-T H.273).
 * @param matrix_coefficients: H.264 VUI matrix_coefficients
 *        value to convert
 * @return the enum vdef_matrix_coefs value or VDEF_MATRIX_COEFS_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_matrix_coefs
vdef_matrix_coefs_from_h264(uint32_t matrix_coefficients);


/**
 * Get an H.264 VUI matrix_coefficients value from an enum vdef_matrix_coefs
 * value (Rec. ITU-T H.264 E.1.1, also defined in Rec. ITU-T H.273).
 * @param matrix: matrix coefficients value
 * @return the H.264 VUI matrix_coefficients value
 */
VDEF_API uint32_t vdef_matrix_coefs_to_h264(enum vdef_matrix_coefs matrix);


/**
 * Get an enum vdef_matrix_coefs value from an H.265 VUI matrix_coeffs
 * value (Rec. ITU-T H.265 E.2.1, also defined in Rec. ITU-T H.273).
 * @param matrix_coefficients: H.265 VUI matrix_coeffs value to convert
 * @return the enum vdef_matrix_coefs value or VDEF_MATRIX_COEFS_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_matrix_coefs
vdef_matrix_coefs_from_h265(uint32_t matrix_coefficients);


/**
 * Get an H.265 VUI matrix_coefficients value from an enum vdef_matrix_coefs
 * value (Rec. ITU-T H.265 E.2.1, also defined in Rec. ITU-T H.273).
 * @param matrix: matrix coefficients value
 * @return the H.265 VUI matrix_coefficients value
 */
VDEF_API uint32_t vdef_matrix_coefs_to_h265(enum vdef_matrix_coefs matrix);


/**
 * Get an enum vdef_matrix_coefs value from a string.
 * Valid strings are only the suffix of the matrix coefficients name (eg.
 * 'BT709'). The case is ignored.
 * @param str: matrix coefficients name to convert
 * @return the enum vdef_matrix_coefs value or VDEF_MATRIX_COEFS_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_matrix_coefs vdef_matrix_coefs_from_str(const char *str);


/**
 * Get a string from an enum vdef_matrix_coefs value.
 * @param matrix: matrix coefficients value to convert
 * @return a string description of the matrix coefficients
 */
VDEF_API const char *vdef_matrix_coefs_to_str(enum vdef_matrix_coefs matrix);


/**
 * Get an enum vdef_dynamic_range value from a string.
 * Valid strings are only the suffix of the dynamic range name (eg. 'HDR10').
 * The case is ignored.
 * @param str: dynamic range name to convert
 * @return the enum vdef_dynamic_range value or VDEF_DYNAMIC_RANGE_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_dynamic_range vdef_dynamic_range_from_str(const char *str);


/**
 * Get a string from an enum vdef_dynamic_range value.
 * @param range: dynamic range value to convert
 * @return a string description of the dynamic range
 */
VDEF_API const char *vdef_dynamic_range_to_str(enum vdef_dynamic_range range);


/**
 * Get an enum vdef_tone_mapping value from a string.
 * Valid strings are only the suffix of the tone mapping name (eg. 'PLOG').
 * The case is ignored.
 * @param str: tone mapping name to convert
 * @return the enum vdef_tone_mapping value or VDEF_TONE_MAPPING_UNKNOWN
 *         if unknown
 */
VDEF_API enum vdef_tone_mapping vdef_tone_mapping_from_str(const char *str);


/**
 * Get a string from an enum vdef_tone_mapping value.
 * @param map: tone mapping value to convert
 * @return a string description of the tone mapping
 */
VDEF_API const char *vdef_tone_mapping_to_str(enum vdef_tone_mapping map);


/**
 * Compare two struct vdef_dim.
 * The two components of the both dimensions are compared and if one of them is
 * different, the function will return false.
 * @param d1: the first dimension to compare
 * @param d2: the second dimension to compare
 * @return true if the both dimensions are identical, false otherwise
 */
static inline bool vdef_dim_cmp(const struct vdef_dim *d1,
				const struct vdef_dim *d2)
{
	return d1 && d2 && d1->width == d2->width && d1->height == d2->height;
}


/**
 * Check if one of dimension components is equal to zero.
 * The two components of the dimension are compared to zero and if one them is,
 * true is returned.
 * @param dim: the dimension to check
 * @return true if one of dimension components is equal to zero, false otherwise
 */
static inline bool vdef_dim_is_null(const struct vdef_dim *dim)
{
	return !dim || !dim->width || !dim->height;
}


/**
 * Check if dimension components are aligned.
 * This function checks if all values of a dimension are aligned. It is very
 * useful to check hardware limitations.
 * @param dim: the dimension to check
 * @param align: the dimension with alignment restriction
 * @return true if the dimension is aligned, false otherwise
 */
VDEF_API bool vdef_dim_is_aligned(const struct vdef_dim *dim,
				  const struct vdef_dim *align);


/**
 * Compare two struct vdef_frac.
 * The two fraction values are compared and the function returns:
 *  - < 0: if f1 is lower than f2,
 *  - = 0: if f1 is equal to f2,
 *  - > 0: if f1 is greater than f2.
 * @param f1: the first fraction to compare
 * @param f2: the second fraction to compare
 * @return the difference between the two fractions
 */
static inline int vdef_frac_diff(const struct vdef_frac *f1,
				 const struct vdef_frac *f2)
{
	if (!f1 || !f2)
		return -1;
	return (int)(((int64_t)f1->num * f2->den) -
		     ((int64_t)f1->den * f2->num));
}


/**
 * Check if fraction is equal to zero.
 * The two components of the fraction are compared to zero and if one them is,
 * true is returned.
 * @param frac: the fraction to check
 * @return true if the fraction is equal to zero, false otherwise
 */
static inline bool vdef_frac_is_null(const struct vdef_frac *frac)
{
	return !frac || !frac->num || !frac->den;
}


/**
 * Compare two struct vdef_rect.
 * The four components of the both rectangle are compared and if one of them is
 * different, the function will return false.
 * @param r1: the first rectangle to compare
 * @param r2: the second rectangle to compare
 * @return true if the both rectangles are identical, false otherwise
 */
static inline bool vdef_rect_cmp(const struct vdef_rect *r1,
				 const struct vdef_rect *r2)
{
	return r1 && r2 && r1->left == r2->left && r1->top == r2->top &&
	       r1->width == r2->width && r1->height == r2->height;
}


/**
 * Check if a rectangle fits in another rectangle.
 * The function checks if rect fits inside the rectangle bounds.
 * @param rect: the rectangle to check (the left and top can be set to -1, which
 *        means the rectangle is centered)
 * @param bound: the rectangle in which rect should fit
 * @return true if the rectangle fits inside the boundaries, false otherwise
 */
VDEF_API bool vdef_rect_fit(const struct vdef_rect *rect,
			    const struct vdef_rect *bounds);


/**
 * Check if rectangle components are aligned.
 * This function checks if all values of a rectangle are aligned. It is very
 * useful to check hardware limitations.
 * @param rect: the rectangle to check
 * @param align: the rectangle with alignment restriction
 * @return true if the rectangle is aligned, false otherwise
 */
VDEF_API bool vdef_rect_is_aligned(const struct vdef_rect *rect,
				   const struct vdef_rect *align);


/**
 * Align components of a rectangle.
 * @param rect: the rectangle to align
 * @param align: the rectangle with restriction to apply
 * @param align_lower: true to align left and top on lower values, false to
 *        align on upper values
 * @param enlarge: true to enlarge the rectangle, false to shrink the rectangle
 */
VDEF_API void vdef_rect_align(struct vdef_rect *rect,
			      const struct vdef_rect *align,
			      bool align_lower,
			      bool enlarge);


/**
 * Map a format information structure to a frame information structure.
 * The fields in the frame information that are not present in the format
 * information are set to 0 or UNKNOWN values.
 * @param format: pointer to an input format information structure
 * @param frame: pointer to an output frame information structure
 */
VDEF_API void vdef_format_to_frame_info(const struct vdef_format_info *format,
					struct vdef_frame_info *frame);


/**
 * Map a frame information structure to a format information structure.
 * The fields in the format information that are not present in the frame
 * information are set to 0 or UNKNOWN values.
 * @param frame: pointer to an input frame information structure
 * @param format: pointer to an output format information structure
 */
VDEF_API void vdef_frame_to_format_info(const struct vdef_frame_info *frame,
					struct vdef_format_info *format);


/**
 * Write a format information structure to a JSON object.
 * The jobj JSON object must have been previously allocated.
 * The ownership of the JSON object stays with the caller.
 * @param info: pointer to an input format information structure
 * @param jobj: pointer to the JSON object to write to (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_format_info_to_json(const struct vdef_format_info *info,
			     struct json_object *jobj);


/**
 * Write a frame information structure to a JSON object.
 * If the min parameter is true, only information not common with struct
 * vdef_format_info is written.
 * The jobj JSON object must have been previously allocated.
 * The ownership of the JSON object stays with the caller.
 * @param info: pointer to an input format information structure
 * @param min: if true, write only information not common with struct
 *             vdef_format_info
 * @param jobj: pointer to the JSON object to write to (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_frame_info_to_json(const struct vdef_frame_info *info,
			    bool min,
			    struct json_object *jobj);


/**
 * Write a raw format structure to a JSON object.
 * The jobj JSON object must have been previously allocated.
 * The ownership of the JSON object stays with the caller.
 * @param format: pointer to an input raw format structure
 * @param jobj: pointer to the JSON object to write to (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_raw_format_to_json(const struct vdef_raw_format *format,
			    struct json_object *jobj);


/**
 * Write a coded format structure to a JSON object.
 * The jobj JSON object must have been previously allocated.
 * The ownership of the JSON object stays with the caller.
 * @param format: pointer to an input coded format structure
 * @param jobj: pointer to the JSON object to write to (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_coded_format_to_json(const struct vdef_coded_format *format,
			      struct json_object *jobj);


/**
 * Write a raw format structure to a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * The string is allocated and the ownership is transferred to the caller.
 * @param format: pointer to an input raw format structure
 * @param str: pointer to the allocated string (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_raw_format_to_csv(const struct vdef_raw_format *format, char **str);


/**
 * Read a raw format structure from a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * @param str: pointer to an input string
 * @param format: pointer to the raw format structure (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_raw_format_from_csv(const char *str, struct vdef_raw_format *format);


/**
 * Write a coded format structure to a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * The string is allocated and the ownership is transferred to the caller.
 * @param format: pointer to an input coded format structure
 * @param str: pointer to the allocated string (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_coded_format_to_csv(const struct vdef_coded_format *format,
			     char **str);


/**
 * Read a coded format structure from a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * @param str: pointer to an input string
 * @param format: pointer to the coded format structure (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_coded_format_from_csv(const char *str,
			       struct vdef_coded_format *format);


/**
 * Write a format information structure to a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * All information is written except the MDCV and CLL information.
 * The string is allocated and the ownership is transferred to the caller.
 * @param info: pointer to an input format information structure
 * @param str: pointer to the allocated string (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_format_info_to_csv(const struct vdef_format_info *info, char **str);


/**
 * Read a format information structure from a CSV string.
 * The CSV separator is ';' to use for example as MIME type parameters.
 * All information is filled except the MDCV and CLL information.
 * @param str: pointer to an input string
 * @param info: pointer to the format information structure (output)
 * @return 0 on success, negative errno value in case of error
 */
VDEF_API
int vdef_format_info_from_csv(const char *str, struct vdef_format_info *info);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDEFS_H_ */
