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

#include "vdefs_test.h"

static inline struct json_object *get_json_object(const struct json_object *obj,
						  const char *key)
{
	struct json_object *res = NULL;

#if defined(JSON_C_MAJOR_VERSION) && defined(JSON_C_MINOR_VERSION) &&          \
	((JSON_C_MAJOR_VERSION == 0 && JSON_C_MINOR_VERSION >= 10) ||          \
	 (JSON_C_MAJOR_VERSION > 0))
	if (!json_object_object_get_ex(obj, key, &res))
		res = NULL;
#else
	/* json_object_object_get is deprecated started version 0.10 */
	res = json_object_object_get(obj, key);
#endif
	return res;
}


static inline void check_json_key_str(const struct json_object *jobj,
				      const char *key,
				      const char *val)
{
	json_object *jobjval = NULL;
	const char *val_str = NULL;

	jobjval = get_json_object(jobj, key);
	CU_ASSERT_PTR_NOT_NULL_FATAL(jobjval);
	val_str = json_object_get_string(jobjval);
	CU_ASSERT_PTR_NOT_NULL_FATAL(val_str);
	CU_ASSERT_STRING_EQUAL(val_str, val);
}


static inline void
check_json_key_int(const struct json_object *jobj, const char *key, int val)
{
	json_object *jobjval = NULL;
	int val_int;

	jobjval = get_json_object(jobj, key);
	CU_ASSERT_PTR_NOT_NULL_FATAL(jobjval);
	val_int = json_object_get_int(jobjval);
	CU_ASSERT_EQUAL(val_int, val);
}


static void test_vdef_coded_format_to_json(void)
{
	int ret = 0;
	struct vdef_coded_format info = {
		.encoding = VDEF_ENCODING_JPEG,
		.data_format = VDEF_CODED_DATA_FORMAT_JFIF,
	};
	struct json_object *jobj = json_object_new_object();

	ret = vdef_coded_format_to_json(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_coded_format_to_json(NULL, jobj);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_coded_format_to_json(&info, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_coded_format_to_json(&info, jobj);
	CU_ASSERT_EQUAL(ret, 0);
	check_json_key_str(jobj, "encoding", "JPEG");
	check_json_key_str(jobj, "data_format", "JFIF");

	json_object_put(jobj);
}


static void test_vdef_raw_format_to_json(void)
{
	int ret = 0;
	struct vdef_raw_format info = {
		.pix_format = VDEF_RAW_PIX_FORMAT_YUV444,
		.pix_order = VDEF_RAW_PIX_ORDER_YUV,
		.pix_layout = VDEF_RAW_PIX_LAYOUT_LINEAR,
		.pix_size = 12,
		.data_layout = VDEF_RAW_DATA_LAYOUT_PLANAR,
		.data_pad_low = true,
		.data_little_endian = true,
		.data_size = 16,
	};
	struct json_object *jobj = json_object_new_object();

	ret = vdef_raw_format_to_json(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_raw_format_to_json(NULL, jobj);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_raw_format_to_json(&info, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_raw_format_to_json(&info, jobj);
	CU_ASSERT_EQUAL(ret, 0);
	check_json_key_str(jobj, "pix_format", "YUV444");
	check_json_key_str(jobj, "pix_order", "ABCD");
	check_json_key_str(jobj, "pix_layout", "LINEAR");
	check_json_key_int(jobj, "pix_size", 12);
	check_json_key_str(jobj, "data_layout", "PLANAR");
	check_json_key_int(jobj, "data_pad_low", 1);
	check_json_key_int(jobj, "data_little_endian", 1);
	check_json_key_int(jobj, "data_size", 16);

	json_object_put(jobj);
}


static void test_vdef_frame_info_to_json(void)
{
	int ret = 0;
	struct vdef_frame_info info = {
		.bit_depth = 8,
		.full_range = true,
		.color_primaries = VDEF_COLOR_PRIMARIES_BT709,
		.transfer_function = VDEF_TRANSFER_FUNCTION_BT709,
		.matrix_coefs = VDEF_MATRIX_COEFS_BT709,
		.dynamic_range = VDEF_DYNAMIC_RANGE_SDR,
		.tone_mapping = VDEF_TONE_MAPPING_STANDARD,
		.resolution.width = 1920,
		.resolution.height = 1080,
		.sar.width = 1,
		.sar.height = 1,
	};
	struct json_object *jobj = json_object_new_object();

	ret = vdef_frame_info_to_json(NULL, false, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_frame_info_to_json(NULL, false, jobj);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_frame_info_to_json(&info, false, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_frame_info_to_json(&info, false, jobj);
	CU_ASSERT_EQUAL(ret, 0);
	check_json_key_int(jobj, "bit_depth", 8);
	check_json_key_int(jobj, "full_range", 1);
	check_json_key_str(jobj, "color_primaries", "BT709");
	check_json_key_str(jobj, "transfer_function", "BT709");
	check_json_key_str(jobj, "matrix_coefs", "BT709");
	check_json_key_str(jobj, "dynamic_range", "SDR");
	check_json_key_str(jobj, "tone_mapping", "STANDARD");

	json_object_put(jobj);
}


static void test_vdef_format_info_to_json(void)
{
	int ret = 0;
	struct vdef_format_info info = {
		.framerate.num = 30000,
		.framerate.den = 1001,
		.bit_depth = 8,
		.full_range = true,
		.color_primaries = VDEF_COLOR_PRIMARIES_BT709,
		.transfer_function = VDEF_TRANSFER_FUNCTION_BT709,
		.matrix_coefs = VDEF_MATRIX_COEFS_BT709,
		.dynamic_range = VDEF_DYNAMIC_RANGE_SDR,
		.tone_mapping = VDEF_TONE_MAPPING_STANDARD,
		.resolution.width = 1920,
		.resolution.height = 1080,
		.sar.width = 1,
		.sar.height = 1,
		.mdcv.display_primaries = VDEF_COLOR_PRIMARIES_UNKNOWN,
		.mdcv.display_primaries_val =
			{
				.color_primaries =
					{
						{1.f, 2.f},
						{3.f, 4.f},
						{5.f, 6.f},
					},
				.white_point =
					{
						1.5f,
						1.5f,
					},
			},
		.mdcv.max_display_mastering_luminance = 1.f,
		.mdcv.min_display_mastering_luminance = 0.1f,
		.cll.max_cll = 1,
		.cll.max_fall = 1,

	};
	struct json_object *jobj = json_object_new_object();

	ret = vdef_format_info_to_json(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_format_info_to_json(NULL, jobj);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_format_info_to_json(&info, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_format_info_to_json(&info, jobj);
	CU_ASSERT_EQUAL(ret, 0);
	check_json_key_int(jobj, "bit_depth", 8);
	check_json_key_int(jobj, "full_range", 1);
	check_json_key_str(jobj, "color_primaries", "BT709");
	check_json_key_str(jobj, "transfer_function", "BT709");
	check_json_key_str(jobj, "matrix_coefs", "BT709");
	check_json_key_str(jobj, "dynamic_range", "SDR");
	check_json_key_str(jobj, "tone_mapping", "STANDARD");

	json_object_put(jobj);
}


CU_TestInfo g_vdef_test_json[] = {
	{FN("vdef-vdef-format-info-to-json"), &test_vdef_format_info_to_json},
	{FN("vdef-vdef-frame-info-to-json"), &test_vdef_frame_info_to_json},
	{FN("vdef-vdef-raw-format-to-json"), &test_vdef_raw_format_to_json},
	{FN("vdef-vdef-coded-format-to-json"), &test_vdef_coded_format_to_json},

	CU_TEST_INFO_NULL,
};
