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

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <strings.h>

#define ULOG_TAG vdef
#include <ulog.h>
ULOG_DECLARE_TAG(ULOG_TAG);

#include <video-defs/vdefs.h>


#define VDEF_ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))


/* Macro for checking the validity of bitfield-compatible enum values:
 * - the enum value must be less than the max value (e.g. UINT32_MAX)
 * - only one bit must be set in the enum value (i.e. value is a power of 2) */
#define VDEF_ENUM_ASSERT(_enum, _val, _max)                                    \
	static_assert((VDEF_##_enum##_##_val < _max) &&                        \
			      ((VDEF_##_enum##_##_val &                        \
				(VDEF_##_enum##_##_val - 1)) == 0),            \
		      #_enum " value " #_val " exceeds " #_max)


/* Values of enum vdef_frame_flag must not exceed UINT64_MAX */
VDEF_ENUM_ASSERT(FRAME_FLAG, NOT_MAPPED, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, DATA_ERROR, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, NO_CACHE_INVALIDATE, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, NO_CACHE_CLEAN, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, VISUAL_ERROR, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, SILENT, UINT64_MAX);
VDEF_ENUM_ASSERT(FRAME_FLAG, FAKE, UINT64_MAX);


bool vdef_is_raw_format_valid(const struct vdef_raw_format *format)
{
	enum vdef_raw_pix_order pix_order;
	enum vdef_raw_pix_layout pix_layout;
	enum vdef_raw_data_layout data_layout;

	if (!format)
		return false;

	pix_order = format->pix_order;
	pix_layout = format->pix_layout;
	data_layout = format->data_layout;

	/* Check enumerator bounds */
	if (pix_order > VDEF_RAW_PIX_ORDER_DCBA ||
	    data_layout > VDEF_RAW_DATA_LAYOUT_OPAQUE ||
	    pix_layout > VDEF_RAW_PIX_LAYOUT_HISI_TILE_64x16_COMPRESSED)
		return false;

	/* Check data size */
	if (!format->pix_size || !format->data_size ||
	    format->pix_size > format->data_size)
		return false;

	/* Parse pixel format */
	switch (format->pix_format) {
	case VDEF_RAW_PIX_FORMAT_UNKNOWN:
		/* No restrictions */
		break;
	case VDEF_RAW_PIX_FORMAT_YUV420:
	case VDEF_RAW_PIX_FORMAT_YUV422:
	case VDEF_RAW_PIX_FORMAT_YUV444:
		/* Y pixel always first */
		if (pix_order != VDEF_RAW_PIX_ORDER_YUYV &&
		    pix_order != VDEF_RAW_PIX_ORDER_YVYU &&
		    pix_order != VDEF_RAW_PIX_ORDER_YUV &&
		    pix_order != VDEF_RAW_PIX_ORDER_YVU)
			return false;

		/* Allow interleaved, planar and semi-planar */
		if (data_layout != VDEF_RAW_DATA_LAYOUT_YUYV &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PLANAR_Y_U_V &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR_Y_UV)
			return false;

		break;
	case VDEF_RAW_PIX_FORMAT_GRAY:
		/* Only one packed pixel */
		if (pix_order != VDEF_RAW_PIX_ORDER_A &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PACKED)
			return false;

		/* Gray is up to 16-bits */
		if (format->pix_size > 16)
			return false;

		break;
	case VDEF_RAW_PIX_FORMAT_RGB24:
		/* Only reversed order allowed */
		if (pix_order != VDEF_RAW_PIX_ORDER_RGB &&
		    pix_order != VDEF_RAW_PIX_ORDER_BGR)
			return false;

		/* Allow packed and planar */
		if (data_layout != VDEF_RAW_DATA_LAYOUT_RGB24 &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PLANAR)
			return false;

		/* Pixel size is only 8 */
		if (format->pix_size != 8)
			return false;

		break;
	case VDEF_RAW_PIX_FORMAT_RGBA32:
		/* Only 3 orders allowed */
		if (pix_order != VDEF_RAW_PIX_ORDER_RGBA &&
		    pix_order != VDEF_RAW_PIX_ORDER_ABGR &&
		    pix_order != VDEF_RAW_PIX_ORDER_BGRA)
			return false;

		/* Allow packed and planar */
		if (data_layout != VDEF_RAW_DATA_LAYOUT_RGBA32 &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PLANAR)
			return false;

		/* Pixel size is only 8 */
		if (format->pix_size != 8)
			return false;

		break;
	case VDEF_RAW_PIX_FORMAT_BAYER:
		/* Only accept RGGB, GRBG, GBRG and BGGR */
		if (pix_order != VDEF_RAW_PIX_ORDER_RGGB &&
		    pix_order != VDEF_RAW_PIX_ORDER_GRBG &&
		    pix_order != VDEF_RAW_PIX_ORDER_GBRG &&
		    pix_order != VDEF_RAW_PIX_ORDER_BGGR)
			return false;

		/* Allow packed and planar */
		if (data_layout != VDEF_RAW_DATA_LAYOUT_PACKED &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PLANAR)
			return false;

		/* Bayer doesn't exceed 16-bits */
		if (format->pix_size > 16)
			return false;

		break;
	case VDEF_RAW_PIX_FORMAT_DEPTH:
	case VDEF_RAW_PIX_FORMAT_DEPTH_FLOAT:
		/* Only one packed pixel */
		if (pix_order != VDEF_RAW_PIX_ORDER_A &&
		    data_layout != VDEF_RAW_DATA_LAYOUT_PACKED)
			return false;

		/* Depth is 32-bits */
		if (format->pix_size != 32)
			return false;

		break;
	default:
		return false;
	}

	return true;
}


bool vdef_raw_format_cmp(const struct vdef_raw_format *f1,
			 const struct vdef_raw_format *f2)
{
	if (!f1 || !f2)
		return false;
	return f1->pix_format == f2->pix_format &&
	       f1->pix_order == f2->pix_order &&
	       f1->pix_layout == f2->pix_layout &&
	       f1->pix_size == f2->pix_size &&
	       f1->data_layout == f2->data_layout &&
	       f1->data_pad_low == f2->data_pad_low &&
	       f1->data_little_endian == f2->data_little_endian &&
	       f1->data_size == f2->data_size;
}

bool vdef_raw_format_intersect(const struct vdef_raw_format *format,
			       const struct vdef_raw_format *caps,
			       unsigned int count)
{
	if (!caps || !vdef_is_raw_format_valid(format))
		return false;

	while (count--) {
		if (vdef_raw_format_cmp(format, caps++))
			return true;
	}

	return false;
}


bool vdef_is_coded_format_valid(const struct vdef_coded_format *format)
{
	enum vdef_coded_data_format data_format;

	if (!format)
		return false;

	data_format = format->data_format;

	/* Encoding */
	switch (format->encoding) {
	case VDEF_ENCODING_JPEG:
		/* JPEG data formats */
		if (data_format != VDEF_CODED_DATA_FORMAT_JFIF)
			return false;
		break;

	case VDEF_ENCODING_PNG:
		/* PNG data formats */
		if (data_format != VDEF_CODED_DATA_FORMAT_UNKNOWN)
			return false;
		break;

	case VDEF_ENCODING_H264:
		/* H.264 data formats */
		if ((data_format != VDEF_CODED_DATA_FORMAT_RAW_NALU) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_BYTE_STREAM) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_AVCC))
			return false;
		break;

	case VDEF_ENCODING_H265:
		/* H.265 data formats */
		if ((data_format != VDEF_CODED_DATA_FORMAT_RAW_NALU) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_BYTE_STREAM) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_HVCC))
			return false;
		break;

	case VDEF_ENCODING_UNKNOWN:
		/* Unknown encoding is invalid */
		if ((data_format != VDEF_CODED_DATA_FORMAT_JFIF) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_RAW_NALU) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_BYTE_STREAM) &&
		    (data_format != VDEF_CODED_DATA_FORMAT_HVCC))
			return false;
		break;

	default:
		/* Bad encoding value */
		return false;
	}

	return true;
}


bool vdef_coded_format_cmp(const struct vdef_coded_format *f1,
			   const struct vdef_coded_format *f2)
{
	if (!f1 || !f2)
		return false;
	return f1->encoding == f2->encoding &&
	       f1->data_format == f2->data_format;
}


bool vdef_coded_format_intersect(const struct vdef_coded_format *format,
				 const struct vdef_coded_format *caps,
				 unsigned int count)
{
	if (!caps || !vdef_is_coded_format_valid(format))
		return false;

	while (count--) {
		if (vdef_coded_format_cmp(format, caps++))
			return true;
	}

	return false;
}


enum vdef_frame_type vdef_frame_type_from_str(const char *str)
{
	if (strcasecmp(str, "RAW") == 0) {
		return VDEF_FRAME_TYPE_RAW;
	} else if (strcasecmp(str, "CODED") == 0) {
		return VDEF_FRAME_TYPE_CODED;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_FRAME_TYPE_UNKNOWN;
	} else {
		ULOGW("%s: unknown frame type '%s'", __func__, str);
		return VDEF_FRAME_TYPE_UNKNOWN;
	}
}


const char *vdef_frame_type_to_str(enum vdef_frame_type type)
{
	switch (type) {
	case VDEF_FRAME_TYPE_RAW:
		return "RAW";
	case VDEF_FRAME_TYPE_CODED:
		return "CODED";
	default:
		return "UNKNOWN";
	}
}


static const struct {
	enum vdef_raw_pix_format format;
	const char *str;
} raw_pix_format_map[] = {
	{VDEF_RAW_PIX_FORMAT_RAW, "RAW"},
	{VDEF_RAW_PIX_FORMAT_YUV420, "YUV420"},
	{VDEF_RAW_PIX_FORMAT_YUV422, "YUV422"},
	{VDEF_RAW_PIX_FORMAT_YUV444, "YUV444"},
	{VDEF_RAW_PIX_FORMAT_GRAY, "GRAY"},
	{VDEF_RAW_PIX_FORMAT_RGB24, "RGB24"},
	{VDEF_RAW_PIX_FORMAT_RGBA32, "RGBA32"},
	{VDEF_RAW_PIX_FORMAT_BAYER, "BAYER"},
	{VDEF_RAW_PIX_FORMAT_DEPTH, "DEPTH"},
	{VDEF_RAW_PIX_FORMAT_DEPTH_FLOAT, "DEPTH_FLOAT"},
};


enum vdef_raw_pix_format vdef_raw_pix_format_from_str(const char *str)
{
	if (!str)
		return VDEF_RAW_PIX_FORMAT_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_format_map); i++) {
		if (!strcasecmp(raw_pix_format_map[i].str, str))
			return raw_pix_format_map[i].format;
	}

	return VDEF_RAW_PIX_FORMAT_UNKNOWN;
}


const char *vdef_raw_pix_format_to_str(enum vdef_raw_pix_format format)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_format_map); i++) {
		if (raw_pix_format_map[i].format == format)
			return raw_pix_format_map[i].str;
	}

	return NULL;
}


static const struct {
	enum vdef_raw_pix_order order;
	const char *str;
} raw_pix_order_map[] = {
	{VDEF_RAW_PIX_ORDER_ABCD, "ABCD"}, {VDEF_RAW_PIX_ORDER_ABDC, "ABDC"},
	{VDEF_RAW_PIX_ORDER_ACBD, "ACBD"}, {VDEF_RAW_PIX_ORDER_ACDB, "ACDB"},
	{VDEF_RAW_PIX_ORDER_ADBC, "ADBC"}, {VDEF_RAW_PIX_ORDER_ADCB, "ADCB"},
	{VDEF_RAW_PIX_ORDER_BACD, "BACD"}, {VDEF_RAW_PIX_ORDER_BADC, "BADC"},
	{VDEF_RAW_PIX_ORDER_BCAD, "BCAD"}, {VDEF_RAW_PIX_ORDER_BCDA, "BCDA"},
	{VDEF_RAW_PIX_ORDER_BDAC, "BDAC"}, {VDEF_RAW_PIX_ORDER_BDCA, "BDCA"},
	{VDEF_RAW_PIX_ORDER_CABD, "CABD"}, {VDEF_RAW_PIX_ORDER_CADB, "CADB"},
	{VDEF_RAW_PIX_ORDER_CBAD, "CBAD"}, {VDEF_RAW_PIX_ORDER_CBDA, "CBDA"},
	{VDEF_RAW_PIX_ORDER_CDAB, "CDAB"}, {VDEF_RAW_PIX_ORDER_CDBA, "CDBA"},
	{VDEF_RAW_PIX_ORDER_DABC, "DABC"}, {VDEF_RAW_PIX_ORDER_DACB, "DACB"},
	{VDEF_RAW_PIX_ORDER_DBAC, "DBAC"}, {VDEF_RAW_PIX_ORDER_DBCA, "DBCA"},
	{VDEF_RAW_PIX_ORDER_DCAB, "DCAB"}, {VDEF_RAW_PIX_ORDER_DCBA, "DCBA"},
};

enum vdef_raw_pix_order vdef_raw_pix_order_from_str(const char *str)
{
	if (!str)
		return VDEF_RAW_PIX_ORDER_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_order_map); i++) {
		if (!strcasecmp(raw_pix_order_map[i].str, str))
			return raw_pix_order_map[i].order;
	}

	return VDEF_RAW_PIX_ORDER_UNKNOWN;
}


const char *vdef_raw_pix_order_to_str(enum vdef_raw_pix_order order)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_order_map); i++) {
		if (raw_pix_order_map[i].order == order)
			return raw_pix_order_map[i].str;
	}

	return NULL;
}


static const struct {
	enum vdef_raw_pix_layout layout;
	const char *str;
} raw_pix_layout_map[] = {
	{VDEF_RAW_PIX_LAYOUT_LINEAR, "LINEAR"},
	{VDEF_RAW_PIX_LAYOUT_HISI_TILE_64x16, "HISI_TILE_64x16"},
	{VDEF_RAW_PIX_LAYOUT_HISI_TILE_64x16_COMPRESSED,
	 "HISI_TILE_64x16_COMPRESSED"},
};


enum vdef_raw_pix_layout vdef_raw_pix_layout_from_str(const char *str)
{
	if (!str)
		return VDEF_RAW_PIX_LAYOUT_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_layout_map); i++) {
		if (!strcasecmp(raw_pix_layout_map[i].str, str))
			return raw_pix_layout_map[i].layout;
	}

	return VDEF_RAW_PIX_LAYOUT_UNKNOWN;
}


const char *vdef_raw_pix_layout_to_str(enum vdef_raw_pix_layout layout)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_pix_layout_map); i++) {
		if (raw_pix_layout_map[i].layout == layout)
			return raw_pix_layout_map[i].str;
	}

	return NULL;
}


static const struct {
	enum vdef_raw_data_layout layout;
	const char *str;
} raw_data_layout_map[] = {
	{VDEF_RAW_DATA_LAYOUT_PACKED, "PACKED"},
	{VDEF_RAW_DATA_LAYOUT_PLANAR, "PLANAR"},
	{VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR, "SEMI_PLANAR"},
	{VDEF_RAW_DATA_LAYOUT_INTERLEAVED, "INTERLEAVED"},
	{VDEF_RAW_DATA_LAYOUT_OPAQUE, "OPAQUE"},
};


enum vdef_raw_data_layout vdef_raw_data_layout_from_str(const char *str)
{
	if (!str)
		return VDEF_RAW_DATA_LAYOUT_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_data_layout_map);
	     i++) {
		if (!strcasecmp(raw_data_layout_map[i].str, str))
			return raw_data_layout_map[i].layout;
	}

	return VDEF_RAW_DATA_LAYOUT_UNKNOWN;
}


const char *vdef_raw_data_layout_to_str(enum vdef_raw_data_layout layout)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_data_layout_map);
	     i++) {
		if (raw_data_layout_map[i].layout == layout)
			return raw_data_layout_map[i].str;
	}

	return NULL;
}


static const struct {
	const char *str;
	const struct vdef_raw_format *format;
} raw_format_map[] = {
	/* RAW formats */
	{"raw8", &vdef_raw8},
	{"raw10_packed", &vdef_raw10_packed},
	{"raw10", &vdef_raw10},
	{"raw12_packed", &vdef_raw12_packed},
	{"raw12", &vdef_raw12},
	{"raw14_packed", &vdef_raw14_packed},
	{"raw14", &vdef_raw14},
	{"raw16", &vdef_raw16},
	{"raw16_be", &vdef_raw16_be},
	{"raw32", &vdef_raw32},
	{"raw32_be", &vdef_raw32_be},
	/* GRAY formats */
	{"gray", &vdef_gray},
	{"gray16", &vdef_gray16},
	/* YUV420 planar formats */
	{"i420", &vdef_i420},
	{"i420_10_16le", &vdef_i420_10_16le},
	{"i420_10_16be", &vdef_i420_10_16be},
	{"i420_10_16le_high", &vdef_i420_10_16le_high},
	{"i420_10_16be_high", &vdef_i420_10_16be_high},
	{"yv12", &vdef_yv12},
	{"yv12_10_16le", &vdef_yv12_10_16le},
	{"yv12_10_16be", &vdef_yv12_10_16be},
	{"yv12_10_16le_high", &vdef_yv12_10_16le_high},
	{"yv12_10_16be_high", &vdef_yv12_10_16be_high},
	/* YUV420 semi-planar formats */
	{"nv12", &vdef_nv12},
	{"nv12_10_packed", &vdef_nv12_10_packed},
	{"nv12_10_16le", &vdef_nv12_10_16le},
	{"nv12_10_16be", &vdef_nv12_10_16be},
	{"nv12_10_16le_high", &vdef_nv12_10_16le_high},
	{"nv12_10_16be_high", &vdef_nv12_10_16be_high},
	{"nv21", &vdef_nv21},
	{"nv21_10_packed", &vdef_nv21_10_packed},
	{"nv21_10_16le", &vdef_nv21_10_16le},
	{"nv21_10_16be", &vdef_nv21_10_16be},
	{"nv21_10_16le_high", &vdef_nv21_10_16le_high},
	{"nv21_10_16be_high", &vdef_nv21_10_16be_high},
	/* YUV444 planar formats */
	{"i444", &vdef_i444},
	/* RGB24 formats */
	{"rgb", &vdef_rgb},
	{"bgr", &vdef_bgr},
	/* RGBA32 formats */
	{"rgba", &vdef_rgba},
	{"bgra", &vdef_bgra},
	{"abgr", &vdef_abgr},
	/* Bayer formats */
	{"bayer_rggb", &vdef_bayer_rggb},
	{"bayer_bggr", &vdef_bayer_bggr},
	{"bayer_grbg", &vdef_bayer_grbg},
	{"bayer_gbrg", &vdef_bayer_gbrg},
	{"bayer_rggb_10_packed", &vdef_bayer_rggb_10_packed},
	{"bayer_bggr_10_packed", &vdef_bayer_bggr_10_packed},
	{"bayer_grbg_10_packed", &vdef_bayer_grbg_10_packed},
	{"bayer_gbrg_10_packed", &vdef_bayer_gbrg_10_packed},
	{"bayer_rggb_10", &vdef_bayer_rggb_10},
	{"bayer_bggr_10", &vdef_bayer_bggr_10},
	{"bayer_grbg_10", &vdef_bayer_grbg_10},
	{"bayer_gbrg_10", &vdef_bayer_gbrg_10},
	{"bayer_rggb_12_packed", &vdef_bayer_rggb_12_packed},
	{"bayer_bggr_12_packed", &vdef_bayer_bggr_12_packed},
	{"bayer_grbg_12_packed", &vdef_bayer_grbg_12_packed},
	{"bayer_gbrg_12_packed", &vdef_bayer_gbrg_12_packed},
	{"bayer_rggb_12", &vdef_bayer_rggb_12},
	{"bayer_bggr_12", &vdef_bayer_bggr_12},
	{"bayer_grbg_12", &vdef_bayer_grbg_12},
	{"bayer_gbrg_12", &vdef_bayer_gbrg_12},
	{"bayer_rggb_14_packed", &vdef_bayer_rggb_14_packed},
	{"bayer_bggr_14_packed", &vdef_bayer_bggr_14_packed},
	{"bayer_grbg_14_packed", &vdef_bayer_grbg_14_packed},
	{"bayer_gbrg_14_packed", &vdef_bayer_gbrg_14_packed},
	{"bayer_rggb_14", &vdef_bayer_rggb_14},
	{"bayer_bggr_14", &vdef_bayer_bggr_14},
	{"bayer_grbg_14", &vdef_bayer_grbg_14},
	{"bayer_gbrg_14", &vdef_bayer_gbrg_14},
	{"nv21_hisi_tiled", &vdef_nv21_hisi_tile},
	{"nv21_hisi_tiled_compressed", &vdef_nv21_hisi_tile_compressed},
	{"nv21_hisi_tiled_10_packed", &vdef_nv21_hisi_tile_10_packed},
	{"nv21_hisi_tiled_compressed_10_packed",
	 &vdef_nv21_hisi_tile_compressed_10_packed},
	{"opaque", &vdef_opaque},
};


int vdef_raw_format_from_str(const char *str, struct vdef_raw_format *format)
{
	const char *delim = "/";
	char *s, *tok, *p;
	int ret = -EINVAL;

	if (!str || !format)
		return -EINVAL;

	/* First find in registered formats */
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_format_map); i++) {
		if (!strcasecmp(raw_format_map[i].str, str)) {
			memcpy(format,
			       raw_format_map[i].format,
			       sizeof(*format));
			return 0;
		}
	}

	/* Copy string for parsing */
	s = strdup(str);

	/* Get pixel format */
	tok = strtok_r(s, delim, &p);
	if (!tok)
		goto out;
	format->pix_format = vdef_raw_pix_format_from_str(tok);

	/* Get pixel order */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->pix_order = vdef_raw_pix_order_from_str(tok);

	/* Get pixel layout */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->pix_layout = vdef_raw_pix_layout_from_str(tok);

	/* Get pixel size */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->pix_size = strtoul(tok, NULL, 10);

	/* Get data layout */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->data_layout = vdef_raw_data_layout_from_str(tok);

	/* Get data padding */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->data_pad_low = !strcmp(tok, "LOW") ? true : false;

	/* Get data endianness */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->data_little_endian = !strcmp(tok, "LE") ? true : false;

	/* Get data size */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->data_size = strtoul(tok, NULL, 10);

	/* Parsing succeed */
	ret = 0;

out:
	/* Free string */
	free(s);

	return ret;
}


char *vdef_raw_format_to_str(const struct vdef_raw_format *format)
{
	char *str;

	if (!format)
		return NULL;

	/* First find in registered formats */
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(raw_format_map); i++) {
		if (vdef_raw_format_cmp(raw_format_map[i].format, format))
			return strdup(raw_format_map[i].str);
	}

	/* Generate generic format name */
	if (asprintf(&str,
		     VDEF_RAW_FORMAT_TO_STR_FMT,
		     VDEF_RAW_FORMAT_TO_STR_ARG(format)) == -1)
		return NULL;

	return str;
}


unsigned int
vdef_get_raw_frame_plane_count(const struct vdef_raw_format *format)
{
	if (!format)
		return -EINVAL;

	switch (format->data_layout) {
	case VDEF_RAW_DATA_LAYOUT_PLANAR:
		if (format->pix_format == VDEF_RAW_PIX_FORMAT_YUV420 ||
		    format->pix_format == VDEF_RAW_PIX_FORMAT_YUV422 ||
		    format->pix_format == VDEF_RAW_PIX_FORMAT_YUV444 ||
		    format->pix_format == VDEF_RAW_PIX_FORMAT_RGB24)
			return 3;
		else if (format->pix_format == VDEF_RAW_PIX_FORMAT_RGBA32 ||
			 format->pix_format == VDEF_RAW_PIX_FORMAT_BAYER)
			return 4;
		else
			return 1;
	case VDEF_RAW_DATA_LAYOUT_SEMI_PLANAR:
		if (format->pix_format == VDEF_RAW_PIX_FORMAT_YUV420 ||
		    format->pix_format == VDEF_RAW_PIX_FORMAT_YUV422 ||
		    format->pix_format == VDEF_RAW_PIX_FORMAT_YUV444)
			return 2;
		else
			return 0;
	case VDEF_RAW_DATA_LAYOUT_UNKNOWN:
	case VDEF_RAW_DATA_LAYOUT_PACKED:
	case VDEF_RAW_DATA_LAYOUT_INTERLEAVED:
	case VDEF_RAW_DATA_LAYOUT_OPAQUE:
		return 1;
	default:
		return 0;
	}
}

unsigned int
vdef_get_raw_frame_component_count(enum vdef_raw_pix_format pix_format)
{
	switch (pix_format) {
	case VDEF_RAW_PIX_FORMAT_BAYER:
	case VDEF_RAW_PIX_FORMAT_RAW:
	case VDEF_RAW_PIX_FORMAT_GRAY:
	case VDEF_RAW_PIX_FORMAT_DEPTH:
	case VDEF_RAW_PIX_FORMAT_DEPTH_FLOAT:
		return 1;
	case VDEF_RAW_PIX_FORMAT_YUV420:
	case VDEF_RAW_PIX_FORMAT_YUV422:
	case VDEF_RAW_PIX_FORMAT_YUV444:
	case VDEF_RAW_PIX_FORMAT_RGB24:
		return 3;
	case VDEF_RAW_PIX_FORMAT_RGBA32:
		return 4;
	default:
		ULOGW("Unknown pix format");
		return 0;
	}
}


int vdef_calc_raw_frame_size(const struct vdef_raw_format *format,
			     const struct vdef_dim *resolution,
			     size_t *plane_stride,
			     const unsigned int *plane_stride_align,
			     size_t *plane_scanline,
			     const unsigned int *plane_scanline_align,
			     size_t *plane_size,
			     const unsigned int *plane_size_align)
{
	unsigned char stride_mul[VDEF_RAW_MAX_PLANE_COUNT] = {1, 1, 1, 1};
	unsigned char stride_div[VDEF_RAW_MAX_PLANE_COUNT] = {1, 1, 1, 1};
	unsigned char height_mul[VDEF_RAW_MAX_PLANE_COUNT] = {1, 1, 1, 1};
	unsigned char height_div[VDEF_RAW_MAX_PLANE_COUNT] = {1, 1, 1, 1};
	unsigned int plane_count;
	unsigned int component_count;

	if (!format || !resolution)
		return -EINVAL;

	if (!plane_stride && !plane_size)
		return 0;

	/* Get plane count */
	plane_count = vdef_get_raw_frame_plane_count(format);
	component_count =
		vdef_get_raw_frame_component_count(format->pix_format);

	/* Set plane multiplier / divisor */
	if (format->pix_format == VDEF_RAW_PIX_FORMAT_YUV420) {
		height_div[1] = height_div[2] = 2;
		stride_div[1] = stride_div[2] = plane_count - 1;
	} else if (format->pix_format == VDEF_RAW_PIX_FORMAT_YUV422) {
		stride_div[1] = stride_div[2] = plane_count - 1;
	} else if (format->pix_format == VDEF_RAW_PIX_FORMAT_YUV444) {
		stride_mul[1] = 4 - plane_count;
	} else if (plane_count == 1) {
		stride_mul[0] = component_count;
	}

	/* Calculate size for each plane */
	for (unsigned int i = 0; i < plane_count; i++) {
		size_t stride;
		size_t scanline;

		/* Calculate stride */
		stride = resolution->width * format->data_size / 8 *
			 stride_mul[i] / stride_div[i];
		if (plane_stride && plane_stride[i] != 0) {
			if (plane_stride[i] < stride)
				return -EPROTO;
			stride = plane_stride[i];
		}

		/* Align stride */
		if (plane_stride_align && plane_stride_align[i])
			stride = VDEF_ALIGN(stride, plane_stride_align[i]);

		/* Update stride */
		if (plane_stride)
			plane_stride[i] = stride;

		/* Calculate scanline */
		scanline = resolution->height * height_mul[i] / height_div[i];
		if (plane_scanline && plane_scanline[i] != 0) {
			if (plane_scanline[i] < scanline)
				return -EPROTO;
			scanline = plane_scanline[i];
		}

		/* Align scanline */
		if (plane_scanline_align && plane_scanline_align[i])
			scanline =
				VDEF_ALIGN(scanline, plane_scanline_align[i]);

		/* Update scanline */
		if (plane_scanline)
			plane_scanline[i] = scanline;

		/* Calculate size */
		if (plane_size) {
			plane_size[i] = stride * scanline;

			/* Align size */
			if (plane_size_align && plane_size_align[i])
				plane_size[i] = VDEF_ALIGN(plane_size[i],
							   plane_size_align[i]);
		}
	}

	return 0;
}


ssize_t
vdef_calc_raw_contiguous_frame_size(const struct vdef_raw_format *format,
				    const struct vdef_dim *resolution,
				    size_t *plane_stride,
				    const unsigned int *plane_stride_align,
				    size_t *plane_scanline,
				    const unsigned int *plane_scanline_align,
				    const unsigned int *plane_size_align)
{
	int ret;
	size_t plane_size[VDEF_RAW_MAX_PLANE_COUNT] = {0};

	ret = vdef_calc_raw_frame_size(format,
				       resolution,
				       plane_stride,
				       plane_stride_align,
				       plane_scanline,
				       plane_scanline_align,
				       plane_size,
				       plane_size_align);
	if (ret < 0)
		return ret;

	return plane_size[0] + plane_size[1] + plane_size[2] + plane_size[3];
}


enum vdef_encoding vdef_encoding_from_str(const char *str)
{
	if ((strcasecmp(str, "JPEG") == 0) || (strcasecmp(str, "MJPEG") == 0)) {
		return VDEF_ENCODING_JPEG;
	} else if (strcasecmp(str, "PNG") == 0) {
		return VDEF_ENCODING_PNG;
	} else if ((strcasecmp(str, "H264") == 0) ||
		   (strcasecmp(str, "AVC") == 0)) {
		return VDEF_ENCODING_H264;
	} else if ((strcasecmp(str, "H265") == 0) ||
		   (strcasecmp(str, "HEVC") == 0)) {
		return VDEF_ENCODING_H265;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_ENCODING_UNKNOWN;
	} else {
		ULOGW("%s: unknown encoding '%s'", __func__, str);
		return VDEF_ENCODING_UNKNOWN;
	}
}


const char *vdef_encoding_to_str(enum vdef_encoding encoding)
{
	switch (encoding) {
	case VDEF_ENCODING_JPEG:
		return "JPEG";
	case VDEF_ENCODING_PNG:
		return "PNG";
	case VDEF_ENCODING_H264:
		return "H264";
	case VDEF_ENCODING_H265:
		return "H265";
	default:
		return "UNKNOWN";
	}
}


const char *vdef_get_encoding_mime_type(enum vdef_encoding encoding)
{
	switch (encoding) {
	case VDEF_ENCODING_JPEG:
		return "image/jpeg";
	case VDEF_ENCODING_PNG:
		return "image/png";
	case VDEF_ENCODING_H264:
		return "video/avc";
	case VDEF_ENCODING_H265:
		return "video/hevc";
	default:
		return "";
	}
}


enum vdef_coded_data_format vdef_coded_data_format_from_str(const char *str)
{
	if (strcasecmp(str, "JFIF") == 0) {
		return VDEF_CODED_DATA_FORMAT_JFIF;
	} else if (strcasecmp(str, "RAW_NALU") == 0) {
		return VDEF_CODED_DATA_FORMAT_RAW_NALU;
	} else if (strcasecmp(str, "BYTE_STREAM") == 0) {
		return VDEF_CODED_DATA_FORMAT_BYTE_STREAM;
	} else if ((strcasecmp(str, "AVCC") == 0) ||
		   (strcasecmp(str, "HVCC") == 0)) {
		return VDEF_CODED_DATA_FORMAT_AVCC;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_CODED_DATA_FORMAT_UNKNOWN;
	} else {
		ULOGW("%s: unknown coded format '%s'", __func__, str);
		return VDEF_CODED_DATA_FORMAT_UNKNOWN;
	}
}


const char *vdef_coded_data_format_to_str(enum vdef_coded_data_format format)
{
	switch (format) {
	case VDEF_CODED_DATA_FORMAT_JFIF:
		return "JFIF";
	case VDEF_CODED_DATA_FORMAT_RAW_NALU:
		return "RAW_NALU";
	case VDEF_CODED_DATA_FORMAT_BYTE_STREAM:
		return "BYTE_STREAM";
	case VDEF_CODED_DATA_FORMAT_AVCC:
		return "AVCC";
	default:
		return "UNKNOWN";
	}
}


static const struct {
	const char *str;
	const struct vdef_coded_format *format;
} coded_format_map[] = {
	{"h264_raw_nalu", &vdef_h264_raw_nalu},
	{"h264_byte_stream", &vdef_h264_byte_stream},
	{"h264_avcc", &vdef_h264_avcc},
	{"h265_raw_nalu", &vdef_h265_raw_nalu},
	{"h265_byte_stream", &vdef_h265_byte_stream},
	{"h265_hvcc", &vdef_h265_hvcc},
	{"jpeg_jfif", &vdef_jpeg_jfif},
	{"png", &vdef_png},
};


int vdef_coded_format_from_str(const char *str,
			       struct vdef_coded_format *format)
{
	const char *delim = "/";
	char *s, *tok, *p;
	int ret = -EINVAL;

	if (!str || !format)
		return -EINVAL;

	/* First find in registered formats */
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(coded_format_map); i++) {
		if (!strcasecmp(coded_format_map[i].str, str)) {
			memcpy(format,
			       coded_format_map[i].format,
			       sizeof(*format));
			return 0;
		}
	}

	/* Copy string for parsing */
	s = strdup(str);

	/* Get encoding */
	tok = strtok_r(s, delim, &p);
	if (!tok)
		goto out;
	format->encoding = vdef_encoding_from_str(tok);

	/* Get data format */
	tok = strtok_r(NULL, delim, &p);
	if (!tok)
		goto out;
	format->data_format = vdef_coded_data_format_from_str(tok);

	/* Parsing succeed */
	ret = 0;

out:
	/* Free string */
	free(s);

	return ret;
}


char *vdef_coded_format_to_str(const struct vdef_coded_format *format)
{
	char *str;

	if (!format)
		return NULL;

	/* First find in registered formats */
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(coded_format_map); i++) {
		if (vdef_coded_format_cmp(coded_format_map[i].format, format))
			return strdup(coded_format_map[i].str);
	}

	/* Generate generic format name */
	if (asprintf(&str,
		     VDEF_CODED_FORMAT_TO_STR_FMT,
		     VDEF_CODED_FORMAT_TO_STR_ARG(format)) == -1)
		return NULL;

	return str;
}


enum vdef_coded_frame_type vdef_coded_frame_type_from_str(const char *str)
{
	if (strcasecmp(str, "NOT_CODED") == 0) {
		return VDEF_CODED_FRAME_TYPE_NOT_CODED;
	} else if ((strcasecmp(str, "IDR") == 0) ||
		   (strcasecmp(str, "CODED") == 0)) {
		return VDEF_CODED_FRAME_TYPE_IDR;
	} else if (strcasecmp(str, "I") == 0) {
		return VDEF_CODED_FRAME_TYPE_I;
	} else if (strcasecmp(str, "P_IR_START") == 0) {
		return VDEF_CODED_FRAME_TYPE_P_IR_START;
	} else if (strcasecmp(str, "P") == 0) {
		return VDEF_CODED_FRAME_TYPE_P;
	} else if (strcasecmp(str, "P_NON_REF") == 0) {
		return VDEF_CODED_FRAME_TYPE_P_NON_REF;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_CODED_FRAME_TYPE_UNKNOWN;
	} else {
		ULOGW("%s: unknown coded frame type '%s'", __func__, str);
		return VDEF_CODED_FRAME_TYPE_UNKNOWN;
	}
}


const char *vdef_coded_frame_type_to_str(enum vdef_coded_frame_type color)
{
	switch (color) {
	case VDEF_CODED_FRAME_TYPE_NOT_CODED:
		return "NOT_CODED";
	case VDEF_CODED_FRAME_TYPE_IDR:
		return "IDR";
	case VDEF_CODED_FRAME_TYPE_I:
		return "I";
	case VDEF_CODED_FRAME_TYPE_P_IR_START:
		return "P_IR_START";
	case VDEF_CODED_FRAME_TYPE_P:
		return "P";
	case VDEF_CODED_FRAME_TYPE_P_NON_REF:
		return "P_NON_REF";
	default:
		return "UNKNOWN";
	}
}


enum vdef_color_primaries
vdef_color_primaries_from_h264(uint32_t color_primaries)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (color_primaries) {
	case 6:
		return VDEF_COLOR_PRIMARIES_BT601_525;
	case 5:
		return VDEF_COLOR_PRIMARIES_BT601_625;
	case 1:
		return VDEF_COLOR_PRIMARIES_BT709;
	case 9:
		return VDEF_COLOR_PRIMARIES_BT2020;
	case 11:
		return VDEF_COLOR_PRIMARIES_DCI_P3;
	case 12:
		return VDEF_COLOR_PRIMARIES_DISPLAY_P3;
	default:
		return VDEF_COLOR_PRIMARIES_UNKNOWN;
	}
}


uint32_t vdef_color_primaries_to_h264(enum vdef_color_primaries color)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (color) {
	case VDEF_COLOR_PRIMARIES_BT601_525:
		return 6;
	case VDEF_COLOR_PRIMARIES_BT601_625:
		return 5;
	case VDEF_COLOR_PRIMARIES_BT709:
		return 1;
	case VDEF_COLOR_PRIMARIES_BT2020:
		return 9;
	case VDEF_COLOR_PRIMARIES_DCI_P3:
		return 11;
	case VDEF_COLOR_PRIMARIES_DISPLAY_P3:
		return 12;
	default:
		return 2;
	}
}


enum vdef_color_primaries
vdef_color_primaries_from_h265(uint32_t color_primaries)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (color_primaries) {
	case 6:
		return VDEF_COLOR_PRIMARIES_BT601_525;
	case 5:
		return VDEF_COLOR_PRIMARIES_BT601_625;
	case 1:
		return VDEF_COLOR_PRIMARIES_BT709;
	case 9:
		return VDEF_COLOR_PRIMARIES_BT2020;
	case 11:
		return VDEF_COLOR_PRIMARIES_DCI_P3;
	case 12:
		return VDEF_COLOR_PRIMARIES_DISPLAY_P3;
	default:
		return VDEF_COLOR_PRIMARIES_UNKNOWN;
	}
}


uint32_t vdef_color_primaries_to_h265(enum vdef_color_primaries color)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (color) {
	case VDEF_COLOR_PRIMARIES_BT601_525:
		return 6;
	case VDEF_COLOR_PRIMARIES_BT601_625:
		return 5;
	case VDEF_COLOR_PRIMARIES_BT709:
		return 1;
	case VDEF_COLOR_PRIMARIES_BT2020:
		return 9;
	case VDEF_COLOR_PRIMARIES_DCI_P3:
		return 11;
	case VDEF_COLOR_PRIMARIES_DISPLAY_P3:
		return 12;
	default:
		return 2;
	}
}


enum vdef_color_primaries vdef_color_primaries_from_str(const char *str)
{
	if (strcasecmp(str, "BT601_525") == 0) {
		return VDEF_COLOR_PRIMARIES_BT601_525;
	} else if (strcasecmp(str, "BT601_625") == 0) {
		return VDEF_COLOR_PRIMARIES_BT601_625;
	} else if ((strcasecmp(str, "BT709") == 0) ||
		   (strcasecmp(str, "SRGB") == 0)) {
		return VDEF_COLOR_PRIMARIES_BT709;
	} else if ((strcasecmp(str, "BT2020") == 0) ||
		   (strcasecmp(str, "BT2100") == 0)) {
		return VDEF_COLOR_PRIMARIES_BT2020;
	} else if (strcasecmp(str, "DCI_P3") == 0) {
		return VDEF_COLOR_PRIMARIES_DCI_P3;
	} else if (strcasecmp(str, "DISPLAY_P3") == 0) {
		return VDEF_COLOR_PRIMARIES_DISPLAY_P3;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_COLOR_PRIMARIES_UNKNOWN;
	} else {
		ULOGW("%s: unknown color primaries '%s'", __func__, str);
		return VDEF_COLOR_PRIMARIES_UNKNOWN;
	}
}


const char *vdef_color_primaries_to_str(enum vdef_color_primaries color)
{
	switch (color) {
	case VDEF_COLOR_PRIMARIES_BT601_525:
		return "BT601_525";
	case VDEF_COLOR_PRIMARIES_BT601_625:
		return "BT601_625";
	case VDEF_COLOR_PRIMARIES_BT709:
		return "BT709";
	case VDEF_COLOR_PRIMARIES_BT2020:
		return "BT2020";
	case VDEF_COLOR_PRIMARIES_DCI_P3:
		return "DCI_P3";
	case VDEF_COLOR_PRIMARIES_DISPLAY_P3:
		return "DISPLAY_P3";
	default:
		return "UNKNOWN";
	}
}


/**
 * Color primaries values; see the corresponding specification documents
 */
/* clang-format off */
const struct vdef_color_primaries_value
vdef_color_primaries_values[VDEF_COLOR_PRIMARIES_MAX] = {
	[VDEF_COLOR_PRIMARIES_UNKNOWN] = {
		.color_primaries = {
			{0.f, 0.f},
			{0.f, 0.f},
			{0.f, 0.f},
		},
		.white_point = {
			0.f, 0.f,
		},
	},
	[VDEF_COLOR_PRIMARIES_BT601_525] = {
		.color_primaries = {
			{0.310f, 0.595f},
			{0.155f, 0.070f},
			{0.630f, 0.340f},
		},
		.white_point = {
			0.3127f, 0.3290f, /* D65 / 6500K */
		},
	},
	[VDEF_COLOR_PRIMARIES_BT601_625] = {
		.color_primaries = {
			{0.290f, 0.600f},
			{0.150f, 0.060f},
			{0.640f, 0.330f},
		},
		.white_point = {
			0.3127f, 0.3290f, /* D65 / 6500K */
		},
	},
	[VDEF_COLOR_PRIMARIES_BT709] = {
		.color_primaries = {
			{0.300f, 0.600f},
			{0.150f, 0.060f},
			{0.640f, 0.330f},
		},
		.white_point = {
			0.3127f, 0.3290f, /* D65 / 6500K */
		},
	},
	[VDEF_COLOR_PRIMARIES_BT2020] = {
		.color_primaries = {
			{0.170f, 0.797f},
			{0.131f, 0.046f},
			{0.708f, 0.292f},
		},
		.white_point = {
			0.3127f, 0.3290f, /* D65 / 6500K */
		},
	},
	[VDEF_COLOR_PRIMARIES_DCI_P3] = {
		.color_primaries = {
			{0.265f, 0.690f},
			{0.150f, 0.060f},
			{0.680f, 0.320f},
		},
		.white_point = {
			0.3140f, 0.3510f, /* 6300K */
		},
	},
	[VDEF_COLOR_PRIMARIES_DISPLAY_P3] = {
		.color_primaries = {
			{0.265f, 0.690f},
			{0.150f, 0.060f},
			{0.680f, 0.320f},
		},
		.white_point = {
			0.3127f, 0.3290f, /* D65 / 6500K */
		},
	},
};
/* clang-format on */


enum vdef_color_primaries
vdef_color_primaries_from_values(const struct vdef_color_primaries_value *val)
{
	/* Round to a precision of 10^-3 for color primaries
	 * and 10^-4 for white point */
	for (size_t i = 0; i < VDEF_COLOR_PRIMARIES_MAX; i++) {
		if ((roundf(val->color_primaries[0].x * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[0].x) &&
		    (roundf(val->color_primaries[0].y * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[0].y) &&
		    (roundf(val->color_primaries[1].x * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[1].x) &&
		    (roundf(val->color_primaries[1].y * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[1].y) &&
		    (roundf(val->color_primaries[2].x * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[2].x) &&
		    (roundf(val->color_primaries[2].y * 1000.f) / 1000.f ==
		     vdef_color_primaries_values[i].color_primaries[2].y) &&
		    (roundf(val->white_point.x * 10000.f) / 10000.f ==
		     vdef_color_primaries_values[i].white_point.x) &&
		    (roundf(val->white_point.y * 10000.f) / 10000.f ==
		     vdef_color_primaries_values[i].white_point.y))
			return (enum vdef_color_primaries)i;
	}
	return VDEF_COLOR_PRIMARIES_UNKNOWN;
}


enum vdef_transfer_function
vdef_transfer_function_from_h264(uint32_t transfer_characteristics)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (transfer_characteristics) {
	case 6:
		return VDEF_TRANSFER_FUNCTION_BT601;
	case 1:
		return VDEF_TRANSFER_FUNCTION_BT709;
	case 14:
	case 15:
		return VDEF_TRANSFER_FUNCTION_BT2020;
	case 16:
		return VDEF_TRANSFER_FUNCTION_PQ;
	case 18:
		return VDEF_TRANSFER_FUNCTION_HLG;
	case 13:
		return VDEF_TRANSFER_FUNCTION_SRGB;
	default:
		return VDEF_TRANSFER_FUNCTION_UNKNOWN;
	}
}


uint32_t vdef_transfer_function_to_h264(enum vdef_transfer_function transfer)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (transfer) {
	case VDEF_TRANSFER_FUNCTION_BT601:
		return 6;
	case VDEF_TRANSFER_FUNCTION_BT709:
		return 1;
	case VDEF_TRANSFER_FUNCTION_BT2020:
		return 14;
	case VDEF_TRANSFER_FUNCTION_PQ:
		return 16;
	case VDEF_TRANSFER_FUNCTION_HLG:
		return 18;
	case VDEF_TRANSFER_FUNCTION_SRGB:
		return 13;
	default:
		return 2;
	}
}


enum vdef_transfer_function
vdef_transfer_function_from_h265(uint32_t transfer_characteristics)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (transfer_characteristics) {
	case 6:
		return VDEF_TRANSFER_FUNCTION_BT601;
	case 1:
		return VDEF_TRANSFER_FUNCTION_BT709;
	case 14:
	case 15:
		return VDEF_TRANSFER_FUNCTION_BT2020;
	case 16:
		return VDEF_TRANSFER_FUNCTION_PQ;
	case 18:
		return VDEF_TRANSFER_FUNCTION_HLG;
	case 13:
		return VDEF_TRANSFER_FUNCTION_SRGB;
	default:
		return VDEF_TRANSFER_FUNCTION_UNKNOWN;
	}
}


uint32_t vdef_transfer_function_to_h265(enum vdef_transfer_function transfer)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (transfer) {
	case VDEF_TRANSFER_FUNCTION_BT601:
		return 6;
	case VDEF_TRANSFER_FUNCTION_BT709:
		return 1;
	case VDEF_TRANSFER_FUNCTION_BT2020:
		return 14;
	case VDEF_TRANSFER_FUNCTION_PQ:
		return 16;
	case VDEF_TRANSFER_FUNCTION_HLG:
		return 18;
	case VDEF_TRANSFER_FUNCTION_SRGB:
		return 13;
	default:
		return 2;
	}
}


enum vdef_transfer_function vdef_transfer_function_from_str(const char *str)
{
	if (strcasecmp(str, "BT601") == 0) {
		return VDEF_TRANSFER_FUNCTION_BT601;
	} else if (strcasecmp(str, "BT709") == 0) {
		return VDEF_TRANSFER_FUNCTION_BT709;
	} else if (strcasecmp(str, "BT2020") == 0) {
		return VDEF_TRANSFER_FUNCTION_BT2020;
	} else if (strcasecmp(str, "PQ") == 0) {
		return VDEF_TRANSFER_FUNCTION_PQ;
	} else if (strcasecmp(str, "HLG") == 0) {
		return VDEF_TRANSFER_FUNCTION_HLG;
	} else if (strcasecmp(str, "SRGB") == 0) {
		return VDEF_TRANSFER_FUNCTION_SRGB;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_TRANSFER_FUNCTION_UNKNOWN;
	} else {
		ULOGW("%s: unknown transfer function '%s'", __func__, str);
		return VDEF_TRANSFER_FUNCTION_UNKNOWN;
	}
}


const char *vdef_transfer_function_to_str(enum vdef_transfer_function transfer)
{
	switch (transfer) {
	case VDEF_TRANSFER_FUNCTION_BT601:
		return "BT601";
	case VDEF_TRANSFER_FUNCTION_BT709:
		return "BT709";
	case VDEF_TRANSFER_FUNCTION_BT2020:
		return "BT2020";
	case VDEF_TRANSFER_FUNCTION_PQ:
		return "PQ";
	case VDEF_TRANSFER_FUNCTION_HLG:
		return "HLG";
	case VDEF_TRANSFER_FUNCTION_SRGB:
		return "SRGB";
	default:
		return "UNKNOWN";
	}
}


enum vdef_matrix_coefs vdef_matrix_coefs_from_h264(uint32_t matrix_coefficients)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (matrix_coefficients) {
	case 0:
		return VDEF_MATRIX_COEFS_IDENTITY;
	case 6:
		return VDEF_MATRIX_COEFS_BT601_525;
	case 5:
		return VDEF_MATRIX_COEFS_BT601_625;
	case 1:
		return VDEF_MATRIX_COEFS_BT709;
	case 9:
		return VDEF_MATRIX_COEFS_BT2020_NON_CST;
	case 10:
		return VDEF_MATRIX_COEFS_BT2020_CST;
	default:
		return VDEF_MATRIX_COEFS_UNKNOWN;
	}
}


uint32_t vdef_matrix_coefs_to_h264(enum vdef_matrix_coefs matrix)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (matrix) {
	case VDEF_MATRIX_COEFS_IDENTITY:
		return 0;
	case VDEF_MATRIX_COEFS_BT601_525:
		return 6;
	case VDEF_MATRIX_COEFS_BT601_625:
		return 5;
	case VDEF_MATRIX_COEFS_BT709:
		return 1;
	case VDEF_MATRIX_COEFS_BT2020_NON_CST:
		return 9;
	case VDEF_MATRIX_COEFS_BT2020_CST:
		return 10;
	default:
		return 2;
	}
}


enum vdef_matrix_coefs vdef_matrix_coefs_from_h265(uint32_t matrix_coefficients)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (matrix_coefficients) {
	case 0:
		return VDEF_MATRIX_COEFS_IDENTITY;
	case 6:
		return VDEF_MATRIX_COEFS_BT601_525;
	case 5:
		return VDEF_MATRIX_COEFS_BT601_625;
	case 1:
		return VDEF_MATRIX_COEFS_BT709;
	case 9:
		return VDEF_MATRIX_COEFS_BT2020_NON_CST;
	case 10:
		return VDEF_MATRIX_COEFS_BT2020_CST;
	default:
		return VDEF_MATRIX_COEFS_UNKNOWN;
	}
}


uint32_t vdef_matrix_coefs_to_h265(enum vdef_matrix_coefs matrix)
{
	/* These values also defined in Rec. ITU-T H.273 */
	switch (matrix) {
	case VDEF_MATRIX_COEFS_IDENTITY:
		return 0;
	case VDEF_MATRIX_COEFS_BT601_525:
		return 6;
	case VDEF_MATRIX_COEFS_BT601_625:
		return 5;
	case VDEF_MATRIX_COEFS_BT709:
		return 1;
	case VDEF_MATRIX_COEFS_BT2020_NON_CST:
		return 9;
	case VDEF_MATRIX_COEFS_BT2020_CST:
		return 10;
	default:
		return 2;
	}
}


enum vdef_matrix_coefs vdef_matrix_coefs_from_str(const char *str)
{
	if ((strcasecmp(str, "IDENTITY") == 0) ||
	    (strcasecmp(str, "SRGB") == 0)) {
		return VDEF_MATRIX_COEFS_IDENTITY;
	} else if (strcasecmp(str, "BT601_525") == 0) {
		return VDEF_MATRIX_COEFS_BT601_525;
	} else if (strcasecmp(str, "BT601_625") == 0) {
		return VDEF_MATRIX_COEFS_BT601_625;
	} else if (strcasecmp(str, "BT709") == 0) {
		return VDEF_MATRIX_COEFS_BT709;
	} else if ((strcasecmp(str, "BT2020_NON_CST") == 0) ||
		   (strcasecmp(str, "BT2100") == 0)) {
		return VDEF_MATRIX_COEFS_BT2020_NON_CST;
	} else if (strcasecmp(str, "BT2020_CST") == 0) {
		return VDEF_MATRIX_COEFS_BT2020_CST;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_MATRIX_COEFS_UNKNOWN;
	} else {
		ULOGW("%s: unknown matrix coefs '%s'", __func__, str);
		return VDEF_MATRIX_COEFS_UNKNOWN;
	}
}


const char *vdef_matrix_coefs_to_str(enum vdef_matrix_coefs matrix)
{
	switch (matrix) {
	case VDEF_MATRIX_COEFS_IDENTITY:
		return "IDENTITY";
	case VDEF_MATRIX_COEFS_BT601_525:
		return "BT601_525";
	case VDEF_MATRIX_COEFS_BT601_625:
		return "BT601_625";
	case VDEF_MATRIX_COEFS_BT709:
		return "BT709";
	case VDEF_MATRIX_COEFS_BT2020_NON_CST:
		return "BT2020_NON_CST";
	case VDEF_MATRIX_COEFS_BT2020_CST:
		return "BT2020_CST";
	default:
		return "UNKNOWN";
	}
}


enum vdef_dynamic_range vdef_dynamic_range_from_str(const char *str)
{
	if (strcasecmp(str, "SDR") == 0) {
		return VDEF_DYNAMIC_RANGE_SDR;
	} else if (strcasecmp(str, "HDR8") == 0) {
		return VDEF_DYNAMIC_RANGE_HDR8;
	} else if (strcasecmp(str, "HDR10") == 0) {
		return VDEF_DYNAMIC_RANGE_HDR10;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_DYNAMIC_RANGE_UNKNOWN;
	} else {
		ULOGW("%s: unknown dynamic range '%s'", __func__, str);
		return VDEF_DYNAMIC_RANGE_UNKNOWN;
	}
}


const char *vdef_dynamic_range_to_str(enum vdef_dynamic_range range)
{
	switch (range) {
	case VDEF_DYNAMIC_RANGE_SDR:
		return "SDR";
	case VDEF_DYNAMIC_RANGE_HDR8:
		return "HDR8";
	case VDEF_DYNAMIC_RANGE_HDR10:
		return "HDR10";
	default:
		return "UNKNOWN";
	}
}


enum vdef_tone_mapping vdef_tone_mapping_from_str(const char *str)
{
	if (strcasecmp(str, "STANDARD") == 0) {
		return VDEF_TONE_MAPPING_STANDARD;
	} else if (strcasecmp(str, "P_LOG") == 0) {
		return VDEF_TONE_MAPPING_P_LOG;
	} else if (strcasecmp(str, "UNKNOWN") == 0) {
		return VDEF_TONE_MAPPING_UNKNOWN;
	} else {
		ULOGW("%s: unknown tone mapping '%s'", __func__, str);
		return VDEF_TONE_MAPPING_UNKNOWN;
	}
}


const char *vdef_tone_mapping_to_str(enum vdef_tone_mapping map)
{
	switch (map) {
	case VDEF_TONE_MAPPING_STANDARD:
		return "STANDARD";
	case VDEF_TONE_MAPPING_P_LOG:
		return "P_LOG";
	default:
		return "UNKNOWN";
	}
}


bool vdef_dim_is_aligned(const struct vdef_dim *dim,
			 const struct vdef_dim *align)
{
	if (!dim || !align ||
	    (align->width && !VDEF_IS_ALIGNED(dim->width, align->width)) ||
	    (align->height && !VDEF_IS_ALIGNED(dim->height, align->height)))
		return false;
	return true;
}


/**
 * Build a resolution from:
 * - width and height
 */
#define MAKE_RESOLUTION(w, h)                                                  \
	{                                                                      \
		VDEF_RESOLUTION_##w##X##h, NULL, #w "x" #h,                    \
		{                                                              \
			w, h                                                   \
		}                                                              \
	}


/**
 * Build a resolution from:
 * - preset name (eg. 'UXGA'),
 * - width and height
 */
#define MAKE_RESOLUTION_PRESET(preset, w, h)                                   \
	{                                                                      \
		VDEF_RESOLUTION_##preset, #preset, #w "x" #h,                  \
		{                                                              \
			w, h                                                   \
		}                                                              \
	}


/**
 * Build a resolution from:
 * - width and height
 * Note: preset is of form 'height'p, enum 'height'P
 *     e.g.: for 1280, 720: 720p / 720P
 */
#define MAKE_RESOLUTION_PRESET_P(w, h)                                         \
	{                                                                      \
		VDEF_RESOLUTION_##h##P, #h "p", #w "x" #h,                     \
		{                                                              \
			w, h                                                   \
		}                                                              \
	}


/**
 * Build a resolution from:
 * - Mpx count, width and height
 */
#define MAKE_RESOLUTION_PRESET_MPX(mpx, w, h)                                  \
	{                                                                      \
		VDEF_RESOLUTION_##mpx##MPX, #mpx "Mpx", #w "x" #h,             \
		{                                                              \
			w, h                                                   \
		}                                                              \
	}


struct {
	enum vdef_resolution res;
	const char *preset_str;
	const char *str;
	struct vdef_dim dim;
} resolution_map[] = {
	/**
	 * 16:9 resolutions
	 */
	MAKE_RESOLUTION_PRESET_P(214, 120),
	MAKE_RESOLUTION_PRESET_P(256, 144),
	MAKE_RESOLUTION_PRESET_P(320, 180),
	MAKE_RESOLUTION_PRESET_P(426, 240),
	MAKE_RESOLUTION_PRESET_P(512, 288),
	MAKE_RESOLUTION_PRESET_P(640, 360),
	MAKE_RESOLUTION_PRESET_P(854, 480),
	MAKE_RESOLUTION_PRESET_P(1280, 720),
	MAKE_RESOLUTION_PRESET_P(1920, 1080),
	MAKE_RESOLUTION_PRESET_P(3840, 2160),
	MAKE_RESOLUTION_PRESET(DCI4K, 4096, 2160),

	/**
	 * 4:3 resolutions
	 */
	MAKE_RESOLUTION_PRESET(QQVGA, 160, 120),
	MAKE_RESOLUTION(192, 144),
	MAKE_RESOLUTION(240, 180),
	MAKE_RESOLUTION_PRESET(QVGA, 320, 240),
	MAKE_RESOLUTION(384, 288),
	MAKE_RESOLUTION(480, 360),
	MAKE_RESOLUTION_PRESET(VGA, 640, 480),
	MAKE_RESOLUTION_PRESET(D1_NTSC, 720, 480),
	MAKE_RESOLUTION_PRESET(D1_PAL, 720, 576),
	MAKE_RESOLUTION_PRESET(SVGA, 800, 600),
	MAKE_RESOLUTION_PRESET(XGA, 1024, 768),
	MAKE_RESOLUTION_PRESET(SXGA, 1280, 1024),
	MAKE_RESOLUTION_PRESET(UXGA, 1600, 1200),
	MAKE_RESOLUTION_PRESET_MPX(12, 4000, 3000),
	MAKE_RESOLUTION_PRESET_MPX(21, 5344, 4016),
	MAKE_RESOLUTION_PRESET_MPX(48, 8000, 6000),

	/**
	 * Special resolutions
	 */
	MAKE_RESOLUTION(176, 90),
	MAKE_RESOLUTION(176, 128),
	MAKE_RESOLUTION(1408, 720),
	MAKE_RESOLUTION(2048, 544),
	MAKE_RESOLUTION(1024, 544),
	MAKE_RESOLUTION(1024, 272),
	MAKE_RESOLUTION(1024, 272),
	MAKE_RESOLUTION(512, 136),
	MAKE_RESOLUTION(256, 136),
	MAKE_RESOLUTION(1280, 800),
	MAKE_RESOLUTION(2560, 832),
	MAKE_RESOLUTION(1280, 832),
	MAKE_RESOLUTION(864, 480),
	MAKE_RESOLUTION(432, 240),
	MAKE_RESOLUTION(640, 512),
};


enum vdef_resolution vdef_resolution_from_str(const char *str)
{
	if (!str)
		return VDEF_RESOLUTION_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(resolution_map); i++) {
		if (!strcasecmp(resolution_map[i].str, str))
			return resolution_map[i].res;
		if (resolution_map[i].preset_str != NULL &&
		    !strcasecmp(resolution_map[i].preset_str, str))
			return resolution_map[i].res;
	}

	return VDEF_RESOLUTION_UNKNOWN;
}


const char *vdef_resolution_to_str(enum vdef_resolution res)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(resolution_map); i++) {
		if (resolution_map[i].res == res)
			return resolution_map[i].preset_str != NULL
				       ? resolution_map[i].preset_str
				       : resolution_map[i].str;
	}

	return "UNKNOWN";
}


enum vdef_resolution vdef_resolution_from_dim(const struct vdef_dim *dim)
{
	if (!dim)
		return VDEF_RESOLUTION_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(resolution_map); i++) {
		if (vdef_dim_cmp(&resolution_map[i].dim, dim))
			return resolution_map[i].res;
	}

	return VDEF_RESOLUTION_UNKNOWN;
}


int vdef_resolution_to_dim(enum vdef_resolution res, struct vdef_dim *dim)
{
	if (!dim)
		return -EINVAL;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(resolution_map); i++) {
		if (resolution_map[i].res == res) {
			*dim = resolution_map[i].dim;
			return 0;
		}
	}

	return -ENOENT;
}


/**
 * Build a framerate from:
 * - numerator and denominator
 */
#define MAKE_FRAMERATE(n, d)                                                   \
	{                                                                      \
		VDEF_FRAMERATE_##n##_##d, NULL, #n "/" #d,                     \
		{                                                              \
			n, d                                                   \
		}                                                              \
	}


/**
 * Build a framerate from:
 * - preset name (eg. '30'),
 * - numerator and denominator
 */
#define MAKE_FRAMERATE_PRESET(preset, n, d)                                    \
	{                                                                      \
		VDEF_FRAMERATE_##preset, #preset, #n "/" #d,                   \
		{                                                              \
			n, d                                                   \
		}                                                              \
	}


struct {
	enum vdef_framerate rate;
	const char *preset_str;
	const char *str;
	struct vdef_frac frac;
} framerate_map[] = {
	/**
	 * Common framerates
	 */
	MAKE_FRAMERATE_PRESET(24, 24000, 1001),
	MAKE_FRAMERATE_PRESET(25, 25, 1),
	MAKE_FRAMERATE_PRESET(30, 30000, 1001),

	MAKE_FRAMERATE_PRESET(48, 48000, 1001),
	MAKE_FRAMERATE_PRESET(50, 50, 1),
	MAKE_FRAMERATE_PRESET(60, 60000, 1001),

	MAKE_FRAMERATE_PRESET(96, 96000, 1001),
	MAKE_FRAMERATE_PRESET(100, 100, 1),
	MAKE_FRAMERATE_PRESET(120, 120000, 1001),

	MAKE_FRAMERATE_PRESET(192, 192000, 1001),
	MAKE_FRAMERATE_PRESET(200, 200, 1),
	MAKE_FRAMERATE_PRESET(240, 240000, 1001),

	/**
	 * Special framerates
	 */
	MAKE_FRAMERATE(60, 7),
	MAKE_FRAMERATE(60, 8),
	MAKE_FRAMERATE(30, 1),
};


enum vdef_framerate vdef_framerate_from_str(const char *str)
{
	if (!str)
		return VDEF_FRAMERATE_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(framerate_map); i++) {
		if (!strcasecmp(framerate_map[i].str, str))
			return framerate_map[i].rate;
		if (framerate_map[i].preset_str != NULL &&
		    !strcasecmp(framerate_map[i].preset_str, str))
			return framerate_map[i].rate;
	}

	return VDEF_FRAMERATE_UNKNOWN;
}


const char *vdef_framerate_to_str(enum vdef_framerate rate)
{
	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(framerate_map); i++) {
		if (framerate_map[i].rate == rate)
			return framerate_map[i].preset_str != NULL
				       ? framerate_map[i].preset_str
				       : framerate_map[i].str;
	}

	return "UNKNOWN";
}


enum vdef_framerate vdef_framerate_from_frac(const struct vdef_frac *frac)
{
	if (!frac)
		return VDEF_FRAMERATE_UNKNOWN;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(framerate_map); i++) {
		if (vdef_frac_diff(&framerate_map[i].frac, frac) == 0)
			return framerate_map[i].rate;
	}

	return VDEF_FRAMERATE_UNKNOWN;
}


int vdef_framerate_to_frac(enum vdef_framerate rate, struct vdef_frac *frac)
{
	if (!frac)
		return -EINVAL;

	for (unsigned int i = 0; i < VDEF_ARRAY_SIZE(framerate_map); i++) {
		if (framerate_map[i].rate == rate) {
			*frac = framerate_map[i].frac;
			return 0;
		}
	}

	return -ENOENT;
}


bool vdef_rect_fit(const struct vdef_rect *rect, const struct vdef_rect *bounds)
{
	if (!rect || !bounds || bounds->left < 0 || bounds->top < 0 ||
	    (rect->left >= 0 && rect->left < bounds->left &&
	     rect->left + rect->width > bounds->left + bounds->width) ||
	    (rect->left < 0 && rect->width > bounds->width) ||
	    (rect->top >= 0 && rect->top < bounds->top &&
	     rect->top + rect->height > bounds->top + bounds->height) ||
	    (rect->top < 0 && rect->height > bounds->height))
		return false;
	return true;
}


bool vdef_rect_is_aligned(const struct vdef_rect *rect,
			  const struct vdef_rect *align)
{
	if (!rect || !align ||
	    (align->left && !VDEF_IS_ALIGNED(rect->left, align->left)) ||
	    (align->top && !VDEF_IS_ALIGNED(rect->top, align->top)) ||
	    (align->width && !VDEF_IS_ALIGNED(rect->width, align->width)) ||
	    (align->height && !VDEF_IS_ALIGNED(rect->height, align->height)))
		return false;
	return true;
}


void vdef_rect_align(struct vdef_rect *rect,
		     const struct vdef_rect *align,
		     bool align_lower,
		     bool enlarge)
{
	int diff;

	if (!rect || !align)
		return;

	if (align->left) {
		diff = VDEF_ALIGN(rect->left, align->left) - rect->left;

		if (align_lower && diff)
			diff -= align->left;
		rect->left += diff;

		if (enlarge)
			rect->width -= diff;
	}
	if (align->top) {
		diff = VDEF_ALIGN(rect->top, align->top) - rect->top;

		if (align_lower && diff)
			diff -= align->top;
		rect->top += diff;

		if (enlarge)
			rect->height -= diff;
	}
	if (align->width)
		rect->width = VDEF_ALIGN(rect->width, align->width);
	if (align->height)
		rect->height = VDEF_ALIGN(rect->height, align->height);
}


void vdef_format_to_frame_info(const struct vdef_format_info *format,
			       struct vdef_frame_info *frame)
{
	ULOG_ERRNO_RETURN_IF(format == NULL, EINVAL);
	ULOG_ERRNO_RETURN_IF(frame == NULL, EINVAL);

	*frame = (struct vdef_frame_info){
		.bit_depth = format->bit_depth,
		.full_range = format->full_range,
		.color_primaries = format->color_primaries,
		.transfer_function = format->transfer_function,
		.matrix_coefs = format->matrix_coefs,
		.dynamic_range = format->dynamic_range,
		.tone_mapping = format->tone_mapping,
		.resolution = format->resolution,
		.sar = format->sar,
	};
}


void vdef_frame_to_format_info(const struct vdef_frame_info *frame,
			       struct vdef_format_info *format)
{
	ULOG_ERRNO_RETURN_IF(frame == NULL, EINVAL);
	ULOG_ERRNO_RETURN_IF(format == NULL, EINVAL);

	*format = (struct vdef_format_info){
		.bit_depth = frame->bit_depth,
		.full_range = frame->full_range,
		.color_primaries = frame->color_primaries,
		.transfer_function = frame->transfer_function,
		.matrix_coefs = frame->matrix_coefs,
		.dynamic_range = frame->dynamic_range,
		.tone_mapping = frame->tone_mapping,
		.resolution = frame->resolution,
		.sar = frame->sar,
	};
}


int vdef_raw_format_to_csv(const struct vdef_raw_format *format, char **str)
{
	int ret = 0;

	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);

	char *fmt = vdef_raw_format_to_str(format);
	if (fmt == NULL) {
		ret = -EPROTO;
		ULOG_ERRNO("vdef_raw_format_to_str", -ret);
		goto out;
	}

	ret = asprintf(str, "format=%s", fmt);
	if (ret < 0) {
		ret = -ENOMEM;
		ULOG_ERRNO("asprintf", -ret);
		goto out;
	}
	ret = 0;

out:
	free(fmt);
	return ret;
}


int vdef_raw_format_from_csv(const char *str, struct vdef_raw_format *format)
{
	char *str2, *param, *key, *val, *temp, *p;

	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);

	str2 = strdup(str);
	if (str2 == NULL) {
		ULOG_ERRNO("strdup", ENOMEM);
		return -ENOMEM;
	}

	param = strtok_r(str2, ";", &temp);
	while (param) {
		while (*param == ' ')
			param++;
		key = param;
		val = NULL;
		p = strchr(param, '=');
		if (p) {
			val = p + 1;
			*p = '\0';
		}

		if (strcmp(key, "format") == 0)
			vdef_raw_format_from_str(val, format);

		param = strtok_r(NULL, ";", &temp);
	}

	free(str2);
	return 0;
}


int vdef_coded_format_to_csv(const struct vdef_coded_format *format, char **str)
{
	int ret = 0;

	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);

	char *fmt = vdef_coded_format_to_str(format);
	if (fmt == NULL) {
		ret = -EPROTO;
		ULOG_ERRNO("vdef_coded_format_to_str", -ret);
		goto out;
	}

	ret = asprintf(str, "format=%s", fmt);
	if (ret < 0) {
		ret = -ENOMEM;
		ULOG_ERRNO("asprintf", -ret);
		goto out;
	}
	ret = 0;

out:
	free(fmt);
	return ret;
}


int vdef_coded_format_from_csv(const char *str,
			       struct vdef_coded_format *format)
{
	char *str2, *param, *key, *val, *temp, *p;

	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);

	str2 = strdup(str);
	if (str2 == NULL) {
		ULOG_ERRNO("strdup", ENOMEM);
		return -ENOMEM;
	}

	param = strtok_r(str2, ";", &temp);
	while (param) {
		while (*param == ' ')
			param++;
		key = param;
		val = NULL;
		p = strchr(param, '=');
		if (p) {
			val = p + 1;
			*p = '\0';
		}

		if (strcmp(key, "format") == 0)
			vdef_coded_format_from_str(val, format);

		param = strtok_r(NULL, ";", &temp);
	}

	free(str2);
	return 0;
}


int vdef_format_info_to_csv(const struct vdef_format_info *info, char **str)
{
	int ret;

	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);

	ret = asprintf(str,
		       "resolution=%ux%u"
		       ";framerate=%u/%u"
		       ";sar=%u:%u"
		       ";bit_depth=%u"
		       ";full_range=%u"
		       ";color_primaries=%s"
		       ";transfer_function=%s"
		       ";matrix_coefs=%s"
		       ";dynamic_range=%s"
		       ";tone_mapping=%s",
		       info->resolution.width,
		       info->resolution.height,
		       info->framerate.num,
		       info->framerate.den,
		       info->sar.width,
		       info->sar.height,
		       info->bit_depth,
		       info->full_range,
		       vdef_color_primaries_to_str(info->color_primaries),
		       vdef_transfer_function_to_str(info->transfer_function),
		       vdef_matrix_coefs_to_str(info->matrix_coefs),
		       vdef_dynamic_range_to_str(info->dynamic_range),
		       vdef_tone_mapping_to_str(info->tone_mapping));
	if (ret < 0) {
		ret = -ENOMEM;
		ULOG_ERRNO("asprintf", -ret);
		return ret;
	}

	return 0;
}


int vdef_format_info_from_csv(const char *str, struct vdef_format_info *info)
{
	char *str2, *param, *key, *val, *temp, *p;

	ULOG_ERRNO_RETURN_ERR_IF(str == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);

	str2 = strdup(str);
	if (str2 == NULL) {
		ULOG_ERRNO("strdup", ENOMEM);
		return -ENOMEM;
	}

	param = strtok_r(str2, ";", &temp);
	while (param) {
		while (*param == ' ')
			param++;
		key = param;
		val = NULL;
		p = strchr(param, '=');
		if (p) {
			val = p + 1;
			*p = '\0';
		}
		if (val == NULL) {
			param = strtok_r(NULL, ";", &temp);
			continue;
		}

		if (strcmp(key, "resolution") == 0) {
			sscanf(val,
			       "%ux%u",
			       &info->resolution.width,
			       &info->resolution.height);
		} else if (strcmp(key, "framerate") == 0) {
			sscanf(val,
			       "%u/%u",
			       &info->framerate.num,
			       &info->framerate.den);
		} else if (strcmp(key, "sar") == 0) {
			sscanf(val,
			       "%u:%u",
			       &info->sar.width,
			       &info->sar.height);
		} else if (strcmp(key, "bit_depth") == 0) {
			sscanf(val, "%u", &info->bit_depth);
		} else if (strcmp(key, "full_range") == 0) {
			unsigned int n;
			sscanf(val, "%u", &n);
			info->full_range = (n != 0);
		} else if (strcmp(key, "color_primaries") == 0) {
			info->color_primaries =
				vdef_color_primaries_from_str(val);
		} else if (strcmp(key, "transfer_function") == 0) {
			info->transfer_function =
				vdef_transfer_function_from_str(val);
		} else if (strcmp(key, "matrix_coefs") == 0) {
			info->matrix_coefs = vdef_matrix_coefs_from_str(val);
		} else if (strcmp(key, "dynamic_range") == 0) {
			info->dynamic_range = vdef_dynamic_range_from_str(val);
		} else if (strcmp(key, "tone_mapping") == 0) {
			info->tone_mapping = vdef_tone_mapping_from_str(val);
		}

		param = strtok_r(NULL, ";", &temp);
	}

	free(str2);
	return 0;
}
