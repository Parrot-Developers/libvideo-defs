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

#include <json-c/json.h>
#include <video-defs/vdefs.h>

#define ULOG_TAG vdef
#include <ulog.h>


static void vdef_json_add_dim(struct json_object *jobj,
			      const char *name,
			      const struct vdef_dim *val)
{
	struct json_object *jobj_val = json_object_new_object();

	json_object_object_add(
		jobj_val, "width", json_object_new_int(val->width));
	json_object_object_add(
		jobj_val, "height", json_object_new_int(val->height));

	json_object_object_add(jobj, name, jobj_val);
}


static void vdef_json_add_rect(struct json_object *jobj,
			       const char *name,
			       const struct vdef_rect *val)
{
	struct json_object *jobj_val = json_object_new_object();

	json_object_object_add(
		jobj_val, "left", json_object_new_int(val->left));
	json_object_object_add(jobj_val, "top", json_object_new_int(val->top));
	json_object_object_add(
		jobj_val, "width", json_object_new_int(val->width));
	json_object_object_add(
		jobj_val, "height", json_object_new_int(val->height));

	json_object_object_add(jobj, name, jobj_val);
}


static void vdef_json_add_frac(struct json_object *jobj,
			       const char *name,
			       const struct vdef_frac *val)
{
	struct json_object *jobj_val = json_object_new_object();

	json_object_object_add(jobj_val, "num", json_object_new_int(val->num));
	json_object_object_add(jobj_val, "den", json_object_new_int(val->den));

	json_object_object_add(jobj, name, jobj_val);
}


int vdef_format_info_to_json(const struct vdef_format_info *info,
			     struct json_object *jobj)
{
	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(jobj == NULL, EINVAL);

	/* Video frame rate */
	vdef_json_add_frac(jobj, "framerate", &info->framerate);

	/* Bit depth */
	json_object_object_add(
		jobj, "bit_depth", json_object_new_int(info->bit_depth));

	/* Full color range */
	json_object_object_add(
		jobj, "full_range", json_object_new_boolean(info->full_range));

	/* Color primaries */
	json_object_object_add(
		jobj,
		"color_primaries",
		json_object_new_string(
			vdef_color_primaries_to_str(info->color_primaries)));

	/* Transfer function */
	json_object_object_add(
		jobj,
		"transfer_function",
		json_object_new_string(vdef_transfer_function_to_str(
			info->transfer_function)));

	/* Matrix coefficients */
	json_object_object_add(jobj,
			       "matrix_coefs",
			       json_object_new_string(vdef_matrix_coefs_to_str(
				       info->matrix_coefs)));

	/* Dynamic range */
	json_object_object_add(jobj,
			       "dynamic_range",
			       json_object_new_string(vdef_dynamic_range_to_str(
				       info->dynamic_range)));

	/* Tone mapping */
	json_object_object_add(jobj,
			       "tone_mapping",
			       json_object_new_string(vdef_tone_mapping_to_str(
				       info->tone_mapping)));

	/* Video frame resolution */
	vdef_json_add_dim(jobj, "resolution", &info->resolution);

	/* Sample aspect ratio */
	vdef_json_add_dim(jobj, "sar", &info->sar);

	/* Mastering display colour volume */
	if (((info->mdcv.display_primaries != VDEF_COLOR_PRIMARIES_UNKNOWN) ||
	     ((info->mdcv.display_primaries_val.color_primaries[0].x != 0.) &&
	      (info->mdcv.display_primaries_val.color_primaries[0].y != 0.) &&
	      (info->mdcv.display_primaries_val.color_primaries[1].x != 0.) &&
	      (info->mdcv.display_primaries_val.color_primaries[1].y != 0.) &&
	      (info->mdcv.display_primaries_val.color_primaries[2].x != 0.) &&
	      (info->mdcv.display_primaries_val.color_primaries[2].y != 0.) &&
	      (info->mdcv.display_primaries_val.white_point.x != 0.) &&
	      (info->mdcv.display_primaries_val.white_point.y != 0.))) &&
	    (info->mdcv.max_display_mastering_luminance != 0.) &&
	    (info->mdcv.min_display_mastering_luminance != 0.)) {
		struct json_object *jobj_mdcv = json_object_new_object();
		enum vdef_color_primaries display_primaries =
			info->mdcv.display_primaries;
		if (display_primaries == VDEF_COLOR_PRIMARIES_UNKNOWN)
			display_primaries = vdef_color_primaries_from_values(
				&info->mdcv.display_primaries_val);
		json_object_object_add(
			jobj_mdcv,
			"display_primaries",
			json_object_new_string(vdef_color_primaries_to_str(
				display_primaries)));
		if (display_primaries == VDEF_COLOR_PRIMARIES_UNKNOWN) {
			struct json_object *jobj_dp = json_object_new_array();
			for (unsigned int i = 0; i < 3; i++) {
				struct json_object *jobj_val =
					json_object_new_object();
				json_object_object_add(
					jobj_val,
					"x",
					json_object_new_double(
						info->mdcv.display_primaries_val
							.color_primaries[i]
							.x));
				json_object_object_add(
					jobj_val,
					"y",
					json_object_new_double(
						info->mdcv.display_primaries_val
							.color_primaries[i]
							.y));
				json_object_array_add(jobj_dp, jobj_val);
			}
			json_object_object_add(
				jobj_mdcv, "color_primaries", jobj_dp);
			struct json_object *jobj_wp = json_object_new_object();
			json_object_object_add(
				jobj_wp,
				"x",
				json_object_new_double(
					info->mdcv.display_primaries_val
						.white_point.x));
			json_object_object_add(
				jobj_wp,
				"y",
				json_object_new_double(
					info->mdcv.display_primaries_val
						.white_point.y));
			json_object_object_add(
				jobj_mdcv, "white_point", jobj_wp);
		}
		json_object_object_add(
			jobj_mdcv,
			"max_display_mastering_luminance",
			json_object_new_double(
				info->mdcv.max_display_mastering_luminance));
		json_object_object_add(
			jobj_mdcv,
			"min_display_mastering_luminance",
			json_object_new_double(
				info->mdcv.min_display_mastering_luminance));
		json_object_object_add(jobj, "mdcv", jobj_mdcv);
	}

	/* Content light level */
	if ((info->cll.max_cll != 0) && (info->cll.max_fall != 0)) {
		struct json_object *jobj_cll = json_object_new_object();
		json_object_object_add(jobj_cll,
				       "max_cll",
				       json_object_new_int(info->cll.max_cll));
		json_object_object_add(jobj_cll,
				       "max_fall",
				       json_object_new_int(info->cll.max_fall));
		json_object_object_add(jobj, "cll", jobj_cll);
	}

	return 0;
}


int vdef_frame_info_to_json(const struct vdef_frame_info *info,
			    bool min,
			    struct json_object *jobj)
{
	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(jobj == NULL, EINVAL);

	/* Frame timestamp in units of time scale */
	json_object_object_add(
		jobj, "timestamp", json_object_new_int64(info->timestamp));

	/* Time scale */
	json_object_object_add(
		jobj, "timescale", json_object_new_int(info->timescale));

	/* Frame original capture timestamp in microseconds */
	json_object_object_add(jobj,
			       "capture_timestamp",
			       json_object_new_int64(info->capture_timestamp));

	/* Frame index */
	json_object_object_add(jobj, "index", json_object_new_int(info->index));

	/* Frame flags */
	json_object_object_add(
		jobj, "flags", json_object_new_int64(info->flags));

	if (min)
		return 0;

	/* Bit depth */
	json_object_object_add(
		jobj, "bit_depth", json_object_new_int(info->bit_depth));

	/* Full color range */
	json_object_object_add(
		jobj, "full_range", json_object_new_boolean(info->full_range));

	/* Color primaries */
	json_object_object_add(
		jobj,
		"color_primaries",
		json_object_new_string(
			vdef_color_primaries_to_str(info->color_primaries)));

	/* Transfer function */
	json_object_object_add(
		jobj,
		"transfer_function",
		json_object_new_string(vdef_transfer_function_to_str(
			info->transfer_function)));

	/* Matrix coefficients */
	json_object_object_add(jobj,
			       "matrix_coefs",
			       json_object_new_string(vdef_matrix_coefs_to_str(
				       info->matrix_coefs)));

	/* Dynamic range */
	json_object_object_add(jobj,
			       "dynamic_range",
			       json_object_new_string(vdef_dynamic_range_to_str(
				       info->dynamic_range)));

	/* Tone mapping */
	json_object_object_add(jobj,
			       "tone_mapping",
			       json_object_new_string(vdef_tone_mapping_to_str(
				       info->tone_mapping)));

	/* Video frame resolution */
	vdef_json_add_dim(jobj, "resolution", &info->resolution);

	/* Sample aspect ratio */
	vdef_json_add_dim(jobj, "sar", &info->sar);

	return 0;
}


int vdef_raw_format_to_json(const struct vdef_raw_format *format,
			    struct json_object *jobj)
{
	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(jobj == NULL, EINVAL);

	/* Pixel format */
	json_object_object_add(
		jobj,
		"pix_format",
		json_object_new_string(
			vdef_raw_pix_format_to_str(format->pix_format)));

	/* Pixel order */
	json_object_object_add(jobj,
			       "pix_order",
			       json_object_new_string(vdef_raw_pix_order_to_str(
				       format->pix_order)));

	/* Pixel layout */
	json_object_object_add(
		jobj,
		"pix_layout",
		json_object_new_string(
			vdef_raw_pix_layout_to_str(format->pix_layout)));

	/* Pixel value size in bits (excluding padding) */
	json_object_object_add(
		jobj, "pix_size", json_object_new_int(format->pix_size));

	/* Data layout */
	json_object_object_add(
		jobj,
		"data_layout",
		json_object_new_string(
			vdef_raw_data_layout_to_str(format->data_layout)));

	/* Data padding: true is padding in lower bits,
	 * false is padding in higher bits */
	json_object_object_add(jobj,
			       "data_pad_low",
			       json_object_new_boolean(format->data_pad_low));

	/* Data endianness: true is little-endian, false is big-endian */
	json_object_object_add(
		jobj,
		"data_little_endian",
		json_object_new_boolean(format->data_little_endian));

	/* Data size in bits including padding */
	json_object_object_add(
		jobj, "data_size", json_object_new_int(format->data_size));

	return 0;
}


VDEF_API
int vdef_coded_format_to_json(const struct vdef_coded_format *format,
			      struct json_object *jobj)
{
	ULOG_ERRNO_RETURN_ERR_IF(format == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(jobj == NULL, EINVAL);

	/* Video encoding */
	json_object_object_add(
		jobj,
		"encoding",
		json_object_new_string(vdef_encoding_to_str(format->encoding)));

	/* Data format */
	json_object_object_add(
		jobj,
		"data_format",
		json_object_new_string(
			vdef_coded_data_format_to_str(format->data_format)));

	return 0;
}
