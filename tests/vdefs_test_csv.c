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


static void test_csv_raw_format(void)
{
	int ret;
	bool ret2;
	char *str = NULL;
	const char *str2 = "format=YUV444/ABCD/LINEAR/12/PLANAR/LOW/LE/16";
	const char *str_i420 = "format=i420";
	const char *str_nv12 = "format=nv12";
	const char *str_gray = "format=gray";
	const char *str_raw32 = "format=raw32";
	struct vdef_raw_format format2 = {0};

	struct vdef_raw_format format = {
		.pix_format = VDEF_RAW_PIX_FORMAT_YUV444,
		.pix_order = VDEF_RAW_PIX_ORDER_YUV,
		.pix_layout = VDEF_RAW_PIX_LAYOUT_LINEAR,
		.pix_size = 12,
		.data_layout = VDEF_RAW_DATA_LAYOUT_PLANAR,
		.data_pad_low = true,
		.data_little_endian = true,
		.data_size = 16,
	};

	/* Error cases */
	ret = vdef_raw_format_to_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_raw_format_to_csv(&format, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_raw_format_to_csv(NULL, &str);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_raw_format_from_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_raw_format_from_csv(str2, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_raw_format_from_csv(NULL, &format2);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	/* Randomly created raw format */
	ret = vdef_raw_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	ret = vdef_raw_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_raw_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str2);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* I420 */
	format = vdef_i420;

	ret = vdef_raw_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_raw_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_raw_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_i420);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* NV12 */
	format = vdef_nv12;

	ret = vdef_raw_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_raw_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_raw_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_nv12);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* Gray */
	format = vdef_gray;

	ret = vdef_raw_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_raw_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_raw_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_gray);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* Raw32 */
	format = vdef_raw32;

	ret = vdef_raw_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_raw_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_raw_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_raw32);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);
}


static void test_csv_coded_format(void)
{
	int ret;
	bool ret2;
	char *str = NULL;
	const char *str_jpeg_jfif = "format=jpeg_jfif";
	const char *str_h264_raw_nalu = "format=h264_raw_nalu";
	const char *str_h265_byte_stream = "format=h265_byte_stream";
	struct vdef_coded_format format2 = {0};

	struct vdef_coded_format format = {
		.encoding = VDEF_ENCODING_JPEG,
		.data_format = VDEF_CODED_DATA_FORMAT_JFIF,
	};

	/* Error cases */
	ret = vdef_coded_format_to_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_coded_format_to_csv(&format, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_coded_format_to_csv(NULL, &str);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_coded_format_from_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_coded_format_from_csv(str_jpeg_jfif, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_coded_format_from_csv(NULL, &format2);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	/* JPEG/JFIF */
	ret = vdef_coded_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	ret = vdef_coded_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_coded_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_jpeg_jfif);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* H.264 raw NALU */
	format = vdef_h264_raw_nalu;

	ret = vdef_coded_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_coded_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_coded_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_h264_raw_nalu);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);

	/* H.265 byte stream */
	format = vdef_h265_byte_stream;

	ret = vdef_coded_format_to_csv(&format, &str);
	CU_ASSERT_EQUAL(ret, 0);

	memset(&format2, 0, sizeof(format2));
	ret = vdef_coded_format_from_csv(str, &format2);
	CU_ASSERT_EQUAL(ret, 0);

	ret2 = vdef_coded_format_cmp(&format, &format2);
	CU_ASSERT_TRUE(ret2);

	ret = strcmp(str, str_h265_byte_stream);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);
}


static void test_csv_format_info(void)
{
	int ret;
	char *str = NULL;
	const char *str2 =
		"resolution=1920x1080;framerate=30000/1001;"
		"sar=1:1;bit_depth=8;full_range=1;color_primaries=BT709;"
		"transfer_function=BT709;matrix_coefs=BT709;"
		"dynamic_range=SDR;tone_mapping=STANDARD";
	struct vdef_format_info info2 = {0};

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
	};

	/* Error cases */
	ret = vdef_format_info_to_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_format_info_to_csv(&info, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_format_info_to_csv(NULL, &str);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_format_info_from_csv(NULL, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_format_info_from_csv(str2, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);
	ret = vdef_format_info_from_csv(NULL, &info2);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	/* Randomly created format info */
	ret = vdef_format_info_to_csv(&info, &str);
	CU_ASSERT_EQUAL(ret, 0);

	ret = vdef_format_info_from_csv(str, &info2);
	CU_ASSERT_EQUAL(ret, 0);

	ret = memcmp(&info, &info2, sizeof(struct vdef_format_info));
	CU_ASSERT_EQUAL(ret, 0);

	ret = strcmp(str, str2);
	CU_ASSERT_EQUAL(ret, 0);

	free(str);
}


CU_TestInfo g_vdef_test_csv[] = {
	{FN("csv-raw-format"), &test_csv_raw_format},
	{FN("csv-coded-format"), &test_csv_coded_format},
	{FN("csv-format-info"), &test_csv_format_info},

	CU_TEST_INFO_NULL,
};
