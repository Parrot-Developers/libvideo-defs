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
#include <futils/futils.h>


static void test_vdef_rect_align(void)
{
	struct vdef_rect rect1 = {
		.left = 5,
		.top = 0,
		.width = 100,
		.height = 50,
	};
	struct vdef_rect rect2 = {
		.left = 4,
		.top = 0,
		.width = 0,
		.height = 0,
	};
	struct vdef_rect tmp = rect1;

	vdef_rect_align(&tmp, NULL, false, false);
	vdef_rect_align(NULL, &rect2, false, false);

	CU_ASSERT_EQUAL(tmp.left, 5);
	CU_ASSERT_EQUAL(tmp.top, 0);
	CU_ASSERT_EQUAL(tmp.width, 100);
	CU_ASSERT_EQUAL(tmp.height, 50);

	vdef_rect_align(&tmp, &rect2, false, false);
	CU_ASSERT_EQUAL(tmp.left, 8);
	CU_ASSERT_EQUAL(tmp.width, 100);

	tmp = rect1;
	vdef_rect_align(&tmp, &rect2, true, false);
	CU_ASSERT_EQUAL(tmp.left, 4);
	CU_ASSERT_EQUAL(tmp.width, 100);

	tmp = rect1;
	vdef_rect_align(&tmp, &rect2, false, true);
	CU_ASSERT_EQUAL(tmp.left, 8);
	CU_ASSERT_EQUAL(tmp.width, 97);
}


static void test_vdef_coded_format_from_str(void)
{
	struct vdef_coded_format format = {};

	CU_ASSERT_EQUAL(vdef_coded_format_from_str(NULL, NULL), -EINVAL);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str(NULL, &format), -EINVAL);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str("h265_raw_nalu", NULL),
			-EINVAL);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str("", &format), -EINVAL);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str("azeryuiop", &format),
			-EINVAL);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str("h265_raw_nalu", &format),
			0);
	CU_ASSERT_EQUAL(format.encoding, VDEF_ENCODING_H265);
	CU_ASSERT_EQUAL(format.data_format, VDEF_CODED_DATA_FORMAT_RAW_NALU);
	CU_ASSERT_EQUAL(vdef_coded_format_from_str("H264/AVCC", &format), 0);
	CU_ASSERT_EQUAL(format.encoding, VDEF_ENCODING_H264);
	CU_ASSERT_EQUAL(format.data_format, VDEF_CODED_DATA_FORMAT_AVCC);
}


static void test_vdef_coded_format_to_str(void)
{
	char *str;
	CU_ASSERT_EQUAL(vdef_coded_format_to_str(NULL), NULL);
	str = vdef_coded_format_to_str(&vdef_jpeg_jfif);
	CU_ASSERT_STRING_EQUAL(str, "jpeg_jfif");
	free(str);
}


static void test_vdef_frame_to_format_info(void)
{
	struct vdef_format_info format = {};
	struct vdef_frame_info frame = {
		.bit_depth = 25,
		.full_range = true,
		.color_primaries = VDEF_COLOR_PRIMARIES_BT601_525,
		.transfer_function = VDEF_TRANSFER_FUNCTION_BT601,
		.matrix_coefs = VDEF_MATRIX_COEFS_SRGB,
		.dynamic_range = VDEF_DYNAMIC_RANGE_SDR,
		.tone_mapping = VDEF_TONE_MAPPING_P_LOG,
	};

	vdef_frame_to_format_info(NULL, NULL);
	vdef_frame_to_format_info(&frame, NULL);
	vdef_frame_to_format_info(NULL, &format);
	vdef_frame_to_format_info(&frame, &format);
	CU_ASSERT_EQUAL(frame.bit_depth, format.bit_depth);
	CU_ASSERT_EQUAL(frame.full_range, format.full_range);
	CU_ASSERT_EQUAL(frame.color_primaries, format.color_primaries);
	CU_ASSERT_EQUAL(frame.transfer_function, format.transfer_function);
	CU_ASSERT_EQUAL(frame.matrix_coefs, format.matrix_coefs);
	CU_ASSERT_EQUAL(frame.dynamic_range, format.dynamic_range);
	CU_ASSERT_EQUAL(frame.tone_mapping, format.tone_mapping);
}


static void test_vdef_format_to_frame_info(void)
{
	struct vdef_format_info format = {
		.bit_depth = 25,
		.full_range = true,
		.color_primaries = VDEF_COLOR_PRIMARIES_BT601_525,
		.transfer_function = VDEF_TRANSFER_FUNCTION_BT601,
		.matrix_coefs = VDEF_MATRIX_COEFS_SRGB,
		.dynamic_range = VDEF_DYNAMIC_RANGE_SDR,
		.tone_mapping = VDEF_TONE_MAPPING_P_LOG,
	};
	struct vdef_frame_info frame = {};

	vdef_format_to_frame_info(NULL, NULL);
	vdef_format_to_frame_info(NULL, &frame);
	vdef_format_to_frame_info(&format, NULL);
	vdef_format_to_frame_info(&format, &frame);
	CU_ASSERT_EQUAL(frame.bit_depth, format.bit_depth);
	CU_ASSERT_EQUAL(frame.full_range, format.full_range);
	CU_ASSERT_EQUAL(frame.color_primaries, format.color_primaries);
	CU_ASSERT_EQUAL(frame.transfer_function, format.transfer_function);
	CU_ASSERT_EQUAL(frame.matrix_coefs, format.matrix_coefs);
	CU_ASSERT_EQUAL(frame.dynamic_range, format.dynamic_range);
	CU_ASSERT_EQUAL(frame.tone_mapping, format.tone_mapping);
}


static void test_vdef_rect_is_aligned(void)
{
	struct vdef_rect boundRect = {
		.left = 0,
		.top = 0,
		.width = 1000,
		.height = 1000,
	};
	struct vdef_rect aligned = {
		.left = 0,
		.top = 0,
		.width = 0,
		.height = 0,
	};
	struct vdef_rect n_aligned = {
		.left = 3,
		.top = 3,
		.width = 7,
		.height = 7,
	};
	CU_ASSERT_FALSE(vdef_rect_is_aligned(NULL, NULL));
	CU_ASSERT_FALSE(vdef_rect_is_aligned(&boundRect, NULL));
	CU_ASSERT_FALSE(vdef_rect_is_aligned(NULL, &boundRect));
	CU_ASSERT_FALSE(vdef_rect_is_aligned(&n_aligned, &boundRect));
	CU_ASSERT_TRUE(vdef_rect_is_aligned(&aligned, &boundRect));
}


static void test_vdef_dim_is_aligned(void)
{
	struct vdef_dim a = {0, 0};
	CU_ASSERT_FALSE(vdef_dim_is_aligned(NULL, NULL));
	CU_ASSERT_FALSE(vdef_dim_is_aligned(&a, NULL));
	CU_ASSERT_FALSE(vdef_dim_is_aligned(NULL, &a));
	CU_ASSERT_TRUE(vdef_dim_is_aligned(&a, &a));
}


static void test_vdef_tone_mapping_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_tone_mapping_to_str(VDEF_TONE_MAPPING_UNKNOWN), "UNKNOWN");
	CU_ASSERT_STRING_EQUAL(
		vdef_tone_mapping_to_str(VDEF_TONE_MAPPING_STANDARD),
		"STANDARD");
	CU_ASSERT_STRING_EQUAL(
		vdef_tone_mapping_to_str(VDEF_TONE_MAPPING_P_LOG), "P_LOG");
}


static void test_vdef_tone_mapping_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str(NULL),
			VDEF_TONE_MAPPING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str(""),
			VDEF_TONE_MAPPING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("azertyuiop"),
			VDEF_TONE_MAPPING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("UNKNOWN"),
			VDEF_TONE_MAPPING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("STANDARD"),
			VDEF_TONE_MAPPING_STANDARD);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("standard"),
			VDEF_TONE_MAPPING_STANDARD);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("P_LOG"),
			VDEF_TONE_MAPPING_P_LOG);
	CU_ASSERT_EQUAL(vdef_tone_mapping_from_str("p_log"),
			VDEF_TONE_MAPPING_P_LOG);
}


static void test_vdef_dynamic_range_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_dynamic_range_to_str(VDEF_DYNAMIC_RANGE_UNKNOWN),
		"UNKNOWN");
	CU_ASSERT_STRING_EQUAL(
		vdef_dynamic_range_to_str(VDEF_DYNAMIC_RANGE_SDR), "SDR");
	CU_ASSERT_STRING_EQUAL(
		vdef_dynamic_range_to_str(VDEF_DYNAMIC_RANGE_HDR8), "HDR8");
	CU_ASSERT_STRING_EQUAL(
		vdef_dynamic_range_to_str(VDEF_DYNAMIC_RANGE_HDR10), "HDR10");
}


static void test_vdef_dynamic_range_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str(NULL),
			VDEF_DYNAMIC_RANGE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str(""),
			VDEF_DYNAMIC_RANGE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("azertyuiop"),
			VDEF_DYNAMIC_RANGE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("UNKNOWN"),
			VDEF_DYNAMIC_RANGE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("SDR"),
			VDEF_DYNAMIC_RANGE_SDR);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("sdr"),
			VDEF_DYNAMIC_RANGE_SDR);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("HDR8"),
			VDEF_DYNAMIC_RANGE_HDR8);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("hdr8"),
			VDEF_DYNAMIC_RANGE_HDR8);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("HDR10"),
			VDEF_DYNAMIC_RANGE_HDR10);
	CU_ASSERT_EQUAL(vdef_dynamic_range_from_str("hdr10"),
			VDEF_DYNAMIC_RANGE_HDR10);
}


static void test_vdef_matrix_coefs_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_UNKNOWN), "UNKNOWN");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_IDENTITY),
		"IDENTITY");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_BT601_525),
		"BT601_525");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_BT601_625),
		"BT601_625");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_BT709), "BT709");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_BT2020_NON_CST),
		"BT2020_NON_CST");
	CU_ASSERT_STRING_EQUAL(
		vdef_matrix_coefs_to_str(VDEF_MATRIX_COEFS_BT2020_CST),
		"BT2020_CST");
}


static void test_vdef_matrix_coefs_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str(NULL),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str(""),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("azeryuiop"),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("UNKNOWN"),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("IDENTITY"),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("identity"),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("SRGB"),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("srgb"),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT601_525"),
			VDEF_MATRIX_COEFS_BT601_525);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("bt601_525"),
			VDEF_MATRIX_COEFS_BT601_525);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT601_625"),
			VDEF_MATRIX_COEFS_BT601_625);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT709"),
			VDEF_MATRIX_COEFS_BT709);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("bt709"),
			VDEF_MATRIX_COEFS_BT709);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT2020_NON_CST"),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("bt2020_non_cst"),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT2100"),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("bt2100"),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("BT2020_CST"),
			VDEF_MATRIX_COEFS_BT2020_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_str("bt2020_CST"),
			VDEF_MATRIX_COEFS_BT2020_CST);
}


static void test_vdef_matrix_coefs_to_h265(void)
{
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_UNKNOWN),
			2);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_IDENTITY),
			0);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_BT601_525),
			6);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_BT601_625),
			5);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_BT709), 1);
	CU_ASSERT_EQUAL(
		vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_BT2020_NON_CST), 9);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h265(VDEF_MATRIX_COEFS_BT2020_CST),
			10);
}


static void test_vdef_matrix_coefs_from_h265(void)
{
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(0),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(1),
			VDEF_MATRIX_COEFS_BT709);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(2),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(3),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(4),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(5),
			VDEF_MATRIX_COEFS_BT601_625);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(6),
			VDEF_MATRIX_COEFS_BT601_525);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(7),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(8),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(9),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(10),
			VDEF_MATRIX_COEFS_BT2020_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(11),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(12),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(13),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(14),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(15),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(16),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h265(18),
			VDEF_MATRIX_COEFS_UNKNOWN);
}


static void test_vdef_matrix_coefs_to_h264(void)
{
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_UNKNOWN),
			2);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_IDENTITY),
			0);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_BT601_525),
			6);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_BT601_625),
			5);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_BT709), 1);
	CU_ASSERT_EQUAL(
		vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_BT2020_NON_CST), 9);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_to_h264(VDEF_MATRIX_COEFS_BT2020_CST),
			10);
}


static void test_vdef_matrix_coefs_from_h264(void)
{
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(0),
			VDEF_MATRIX_COEFS_IDENTITY);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(1),
			VDEF_MATRIX_COEFS_BT709);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(2),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(3),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(4),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(5),
			VDEF_MATRIX_COEFS_BT601_625);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(6),
			VDEF_MATRIX_COEFS_BT601_525);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(7),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(8),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(9),
			VDEF_MATRIX_COEFS_BT2020_NON_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(10),
			VDEF_MATRIX_COEFS_BT2020_CST);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(11),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(12),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(13),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(14),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(15),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(16),
			VDEF_MATRIX_COEFS_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_matrix_coefs_from_h264(18),
			VDEF_MATRIX_COEFS_UNKNOWN);
}


static void test_vdef_transfer_function_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_BT601),
		"BT601");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_BT709),
		"BT709");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_BT2020),
		"BT2020");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_PQ), "PQ");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_HLG),
		"HLG");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_SRGB),
		"SRGB");
	CU_ASSERT_STRING_EQUAL(
		vdef_transfer_function_to_str(VDEF_TRANSFER_FUNCTION_UNKNOWN),
		"UNKNOWN");
}


static void test_vdef_transfer_function_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str(NULL),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str(""),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("azertyuiop"),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("UNKNOWN"),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("BT601"),
			VDEF_TRANSFER_FUNCTION_BT601);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("bt601"),
			VDEF_TRANSFER_FUNCTION_BT601);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("BT2020"),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("bt2020"),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("PQ"),
			VDEF_TRANSFER_FUNCTION_PQ);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("pq"),
			VDEF_TRANSFER_FUNCTION_PQ);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("HLG"),
			VDEF_TRANSFER_FUNCTION_HLG);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("hlg"),
			VDEF_TRANSFER_FUNCTION_HLG);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("SRGB"),
			VDEF_TRANSFER_FUNCTION_SRGB);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_str("srgb"),
			VDEF_TRANSFER_FUNCTION_SRGB);
}


static void test_vdef_transfer_function_to_h265(void)
{
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_UNKNOWN),
		2);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_BT601),
		6);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_BT709),
		1);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_BT2020),
		14);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_PQ), 16);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_HLG), 18);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h265(VDEF_TRANSFER_FUNCTION_SRGB),
		13);
}


static void test_vdef_transfer_function_from_h265(void)
{
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(0),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(1),
			VDEF_TRANSFER_FUNCTION_BT709);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(2),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(3),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(4),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(5),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(6),
			VDEF_TRANSFER_FUNCTION_BT601);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(7),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(8),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(9),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(10),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(11),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(12),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(13),
			VDEF_TRANSFER_FUNCTION_SRGB);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(14),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(15),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(16),
			VDEF_TRANSFER_FUNCTION_PQ);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h265(18),
			VDEF_TRANSFER_FUNCTION_HLG);
}


static void test_vdef_transfer_function_to_h264(void)
{
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_UNKNOWN),
		2);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_BT601),
		6);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_BT709),
		1);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_BT2020),
		14);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_PQ), 16);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_HLG), 18);
	CU_ASSERT_EQUAL(
		vdef_transfer_function_to_h264(VDEF_TRANSFER_FUNCTION_SRGB),
		13);
}


static void test_vdef_transfer_function_from_h264(void)
{
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(0),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(1),
			VDEF_TRANSFER_FUNCTION_BT709);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(2),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(3),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(4),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(5),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(6),
			VDEF_TRANSFER_FUNCTION_BT601);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(7),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(8),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(9),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(10),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(11),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(12),
			VDEF_TRANSFER_FUNCTION_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(13),
			VDEF_TRANSFER_FUNCTION_SRGB);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(14),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(15),
			VDEF_TRANSFER_FUNCTION_BT2020);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(16),
			VDEF_TRANSFER_FUNCTION_PQ);
	CU_ASSERT_EQUAL(vdef_transfer_function_from_h264(18),
			VDEF_TRANSFER_FUNCTION_HLG);
}


/* clang-format off */
static const struct vdef_color_primaries_value
s_vdef_color_primaries_values[VDEF_COLOR_PRIMARIES_MAX] = {
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


static void test_vdef_color_primaries_from_values(void)
{
	CU_ASSERT_EQUAL(vdef_color_primaries_from_values(NULL),
			VDEF_COLOR_PRIMARIES_UNKNOWN);

	for (size_t i = 0; i < VDEF_COLOR_PRIMARIES_MAX; i++) {
		CU_ASSERT_EQUAL(vdef_color_primaries_from_values(
					&s_vdef_color_primaries_values[i]),
				(enum vdef_color_primaries)i);
	}
}


static void test_vdef_color_primaries_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_BT601_525),
		"BT601_525");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_BT601_625),
		"BT601_625");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_BT709),
		"BT709");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_SRGB),
		"BT709");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_BT2020),
		"BT2020");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_BT2100),
		"BT2020");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_DCI_P3),
		"DCI_P3");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_DISPLAY_P3),
		"DISPLAY_P3");
	CU_ASSERT_STRING_EQUAL(
		vdef_color_primaries_to_str(VDEF_COLOR_PRIMARIES_UNKNOWN),
		"UNKNOWN");
}


static void test_vdef_color_primaries_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str(NULL),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str(""),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("azertyuiop"),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("UNKNOWN"),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("BT601_525"),
			VDEF_COLOR_PRIMARIES_BT601_525);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("bt601_525"),
			VDEF_COLOR_PRIMARIES_BT601_525);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("BT601_625"),
			VDEF_COLOR_PRIMARIES_BT601_625);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("bt601_625"),
			VDEF_COLOR_PRIMARIES_BT601_625);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("BT709"),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("bt709"),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("SRGB"),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("srgb"),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("BT2020"),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("bt2020"),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("BT2100"),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("bt2100"),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("DCI_P3"),
			VDEF_COLOR_PRIMARIES_DCI_P3);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("dci_p3"),
			VDEF_COLOR_PRIMARIES_DCI_P3);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("DISPLAY_P3"),
			VDEF_COLOR_PRIMARIES_DISPLAY_P3);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_str("display_p3"),
			VDEF_COLOR_PRIMARIES_DISPLAY_P3);
}


static void test_vdef_color_primaries_to_h265(void)
{
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_BT601_525),
		6);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_BT601_625),
		5);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_BT709), 1);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_BT2020), 9);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_DCI_P3), 11);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_DISPLAY_P3),
		12);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h265(VDEF_COLOR_PRIMARIES_UNKNOWN), 2);
}


static void test_vdef_color_primaries_from_h265(void)
{
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(0),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(1),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(2),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(3),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(4),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(5),
			VDEF_COLOR_PRIMARIES_BT601_625);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(6),
			VDEF_COLOR_PRIMARIES_BT601_525);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(7),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(8),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(9),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(10),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(11),
			VDEF_COLOR_PRIMARIES_DCI_P3);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h265(12),
			VDEF_COLOR_PRIMARIES_DISPLAY_P3);
}


static void test_vdef_color_primaries_to_h264(void)
{
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_BT601_525),
		6);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_BT601_625),
		5);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_BT709), 1);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_BT2020), 9);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_DCI_P3), 11);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_DISPLAY_P3),
		12);
	CU_ASSERT_EQUAL(
		vdef_color_primaries_to_h264(VDEF_COLOR_PRIMARIES_UNKNOWN), 2);
}


static void test_vdef_color_primaries_from_h264(void)
{
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(0),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(1),
			VDEF_COLOR_PRIMARIES_BT709);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(2),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(3),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(4),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(5),
			VDEF_COLOR_PRIMARIES_BT601_625);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(6),
			VDEF_COLOR_PRIMARIES_BT601_525);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(7),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(8),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(9),
			VDEF_COLOR_PRIMARIES_BT2020);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(10),
			VDEF_COLOR_PRIMARIES_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(11),
			VDEF_COLOR_PRIMARIES_DCI_P3);
	CU_ASSERT_EQUAL(vdef_color_primaries_from_h264(12),
			VDEF_COLOR_PRIMARIES_DISPLAY_P3);
}


static void test_vdef_coded_frame_type_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_UNKNOWN),
		"UNKNOWN");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_NOT_CODED),
		"NOT_CODED");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_IDR), "IDR");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_I), "I");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_P_IR_START),
		"P_IR_START");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_P), "P");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_frame_type_to_str(VDEF_CODED_FRAME_TYPE_P_NON_REF),
		"P_NON_REF");
}


static void test_vdef_coded_frame_type_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str(NULL),
			VDEF_CODED_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str(""),
			VDEF_CODED_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("azertyuiop"),
			VDEF_CODED_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("UNKNOWN"),
			VDEF_CODED_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("NOT_CODED"),
			VDEF_CODED_FRAME_TYPE_NOT_CODED);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("not_coded"),
			VDEF_CODED_FRAME_TYPE_NOT_CODED);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("IDR"),
			VDEF_CODED_FRAME_TYPE_IDR);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("idr"),
			VDEF_CODED_FRAME_TYPE_IDR);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("CODED"),
			VDEF_CODED_FRAME_TYPE_IDR);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("coded"),
			VDEF_CODED_FRAME_TYPE_IDR);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("I"),
			VDEF_CODED_FRAME_TYPE_I);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("i"),
			VDEF_CODED_FRAME_TYPE_I);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("P_IR_START"),
			VDEF_CODED_FRAME_TYPE_P_IR_START);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("p_ir_start"),
			VDEF_CODED_FRAME_TYPE_P_IR_START);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("P"),
			VDEF_CODED_FRAME_TYPE_P);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("p"),
			VDEF_CODED_FRAME_TYPE_P);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("P_NON_REF"),
			VDEF_CODED_FRAME_TYPE_P_NON_REF);
	CU_ASSERT_EQUAL(vdef_coded_frame_type_from_str("p_non_ref"),
			VDEF_CODED_FRAME_TYPE_P_NON_REF);
}


static void test_vdef_coded_data_format_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_data_format_to_str(VDEF_CODED_DATA_FORMAT_UNKNOWN),
		"UNKNOWN");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_data_format_to_str(VDEF_CODED_DATA_FORMAT_JFIF),
		"JFIF");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_data_format_to_str(VDEF_CODED_DATA_FORMAT_RAW_NALU),
		"RAW_NALU");
	CU_ASSERT_STRING_EQUAL(vdef_coded_data_format_to_str(
				       VDEF_CODED_DATA_FORMAT_BYTE_STREAM),
			       "BYTE_STREAM");
	CU_ASSERT_STRING_EQUAL(
		vdef_coded_data_format_to_str(VDEF_CODED_DATA_FORMAT_AVCC),
		"AVCC");
}


static void test_vdef_coded_data_format_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str(NULL),
			VDEF_CODED_DATA_FORMAT_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("azertyuiop"),
			VDEF_CODED_DATA_FORMAT_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str(""),
			VDEF_CODED_DATA_FORMAT_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("UNKNOWN"),
			VDEF_CODED_DATA_FORMAT_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("JFIF"),
			VDEF_CODED_DATA_FORMAT_JFIF);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("jfif"),
			VDEF_CODED_DATA_FORMAT_JFIF);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("RAW_NALU"),
			VDEF_CODED_DATA_FORMAT_RAW_NALU);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("raw_nalu"),
			VDEF_CODED_DATA_FORMAT_RAW_NALU);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("BYTE_STREAM"),
			VDEF_CODED_DATA_FORMAT_BYTE_STREAM);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("byte_stream"),
			VDEF_CODED_DATA_FORMAT_BYTE_STREAM);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("AVCC"),
			VDEF_CODED_DATA_FORMAT_AVCC);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("avcc"),
			VDEF_CODED_DATA_FORMAT_AVCC);
	CU_ASSERT_EQUAL(vdef_coded_data_format_from_str("hvcc"),
			VDEF_CODED_DATA_FORMAT_AVCC);
}


static void test_vdef_get_encoding_mime_type(void)
{
	CU_ASSERT_STRING_EQUAL(
		vdef_get_encoding_mime_type(VDEF_ENCODING_UNKNOWN), "");
	CU_ASSERT_STRING_EQUAL(vdef_get_encoding_mime_type(VDEF_ENCODING_JPEG),
			       "image/jpeg");
	CU_ASSERT_STRING_EQUAL(vdef_get_encoding_mime_type(VDEF_ENCODING_PNG),
			       "image/png");
	CU_ASSERT_STRING_EQUAL(vdef_get_encoding_mime_type(VDEF_ENCODING_H264),
			       "video/avc");
	CU_ASSERT_STRING_EQUAL(vdef_get_encoding_mime_type(VDEF_ENCODING_H265),
			       "video/hevc");
}


static void test_vdef_encoding_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(vdef_encoding_to_str(VDEF_ENCODING_UNKNOWN),
			       "UNKNOWN");
	CU_ASSERT_STRING_EQUAL(vdef_encoding_to_str(VDEF_ENCODING_JPEG),
			       "JPEG");
	CU_ASSERT_STRING_EQUAL(vdef_encoding_to_str(VDEF_ENCODING_PNG), "PNG");
	CU_ASSERT_STRING_EQUAL(vdef_encoding_to_str(VDEF_ENCODING_H264),
			       "H264");
	CU_ASSERT_STRING_EQUAL(vdef_encoding_to_str(VDEF_ENCODING_H265),
			       "H265");
}


static void test_vdef_encoding_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_encoding_from_str(NULL), VDEF_ENCODING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_encoding_from_str(""), VDEF_ENCODING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("UNKNOWN"),
			VDEF_ENCODING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("azertyuiop"),
			VDEF_ENCODING_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("JPEG"), VDEF_ENCODING_JPEG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("jpeg"), VDEF_ENCODING_JPEG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("MJPEG"), VDEF_ENCODING_JPEG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("mjpeg"), VDEF_ENCODING_JPEG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("PNG"), VDEF_ENCODING_PNG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("png"), VDEF_ENCODING_PNG);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("H264"), VDEF_ENCODING_H264);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("h264"), VDEF_ENCODING_H264);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("avc"), VDEF_ENCODING_H264);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("AVC"), VDEF_ENCODING_H264);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("H265"), VDEF_ENCODING_H265);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("h265"), VDEF_ENCODING_H265);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("hevc"), VDEF_ENCODING_H265);
	CU_ASSERT_EQUAL(vdef_encoding_from_str("HEVC"), VDEF_ENCODING_H265);
}


static void test_vdef_frame_type_to_str(void)
{
	CU_ASSERT_STRING_EQUAL(vdef_frame_type_to_str(VDEF_FRAME_TYPE_UNKNOWN),
			       "UNKNOWN");
	CU_ASSERT_STRING_EQUAL(vdef_frame_type_to_str(VDEF_FRAME_TYPE_CODED),
			       "CODED");
	CU_ASSERT_STRING_EQUAL(vdef_frame_type_to_str(VDEF_FRAME_TYPE_RAW),
			       "RAW");
}


static void test_vdef_frame_type_from_str(void)
{
	CU_ASSERT_EQUAL(vdef_frame_type_from_str(NULL),
			VDEF_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str(""), VDEF_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("azertyuiop"),
			VDEF_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("UNKNOWN"),
			VDEF_FRAME_TYPE_UNKNOWN);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("coded"),
			VDEF_FRAME_TYPE_CODED);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("CODED"),
			VDEF_FRAME_TYPE_CODED);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("raw"), VDEF_FRAME_TYPE_RAW);
	CU_ASSERT_EQUAL(vdef_frame_type_from_str("RAW"), VDEF_FRAME_TYPE_RAW);
}


static void test_vdef_coded_format_intersect(void)
{
	bool ret;
	struct vdef_coded_format supported_formats[] = {
		vdef_h264_raw_nalu,
		vdef_h265_byte_stream,
	};

	ret = vdef_coded_format_intersect(NULL, NULL, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_intersect(NULL, supported_formats, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_intersect(&vdef_h264_raw_nalu, NULL, 2);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_intersect(
		&vdef_h264_raw_nalu, supported_formats, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_intersect(
		&vdef_png,
		supported_formats,
		FUTILS_SIZEOF_ARRAY(supported_formats));
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_intersect(
		&vdef_h264_raw_nalu, supported_formats, 2);
	CU_ASSERT_TRUE(ret);
}


static void test_vdef_coded_format_cmp(void)
{
	bool ret;

	ret = vdef_coded_format_cmp(NULL, NULL);
	CU_ASSERT_TRUE(ret);

	ret = vdef_coded_format_cmp(&vdef_h264_raw_nalu, NULL);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_cmp(NULL, &vdef_h264_raw_nalu);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_cmp(&vdef_h265_byte_stream,
				    &vdef_h264_raw_nalu);
	CU_ASSERT_FALSE(ret);

	ret = vdef_coded_format_cmp(&vdef_h264_raw_nalu, &vdef_h264_raw_nalu);
	CU_ASSERT_TRUE(ret);
}


static void test_vdef_is_coded_format_valid(void)
{
	bool ret;
	struct vdef_coded_format format = {};
	struct vdef_coded_format invalid_jpeg = vdef_jpeg_jfif;
	invalid_jpeg.data_format = VDEF_CODED_DATA_FORMAT_RAW_NALU;
	struct vdef_coded_format invalid_png = vdef_png;
	invalid_png.data_format = VDEF_CODED_DATA_FORMAT_BYTE_STREAM;
	struct vdef_coded_format invalid_h264 = vdef_h264_raw_nalu;
	invalid_h264.data_format = VDEF_CODED_DATA_FORMAT_JFIF;
	struct vdef_coded_format invalid_h265 = vdef_h265_raw_nalu;
	invalid_h265.data_format = VDEF_CODED_DATA_FORMAT_JFIF;
	struct vdef_coded_format valid_unknown = {
		.encoding = VDEF_ENCODING_UNKNOWN,
		.data_format = VDEF_CODED_DATA_FORMAT_JFIF,
	};

	ret = vdef_is_coded_format_valid(NULL);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&format);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&invalid_jpeg);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&invalid_png);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&invalid_h264);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&invalid_h265);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_coded_format_valid(&valid_unknown);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h264_raw_nalu);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h264_byte_stream);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h264_avcc);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h265_raw_nalu);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h265_byte_stream);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_h265_hvcc);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_jpeg_jfif);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_coded_format_valid(&vdef_png);
	CU_ASSERT_TRUE(ret);
}


static void test_vdef_raw_format_intersect(void)
{
	bool ret;
	struct vdef_raw_format supported_formats[] = {
		vdef_i420,
		vdef_nv12,
	};

	ret = vdef_raw_format_intersect(NULL, NULL, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_intersect(NULL, supported_formats, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_intersect(&vdef_i420, NULL, 2);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_intersect(&vdef_i420, supported_formats, 0);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_intersect(&vdef_bayer_gbrg_14,
					supported_formats,
					FUTILS_SIZEOF_ARRAY(supported_formats));
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_intersect(&vdef_i420,
					supported_formats,
					FUTILS_SIZEOF_ARRAY(supported_formats));
	CU_ASSERT_TRUE(ret);
}


static void test_vdef_raw_format_cmp(void)
{
	bool ret;

	ret = vdef_raw_format_cmp(NULL, NULL);
	CU_ASSERT_TRUE(ret);

	ret = vdef_raw_format_cmp(&vdef_raw8, NULL);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_cmp(NULL, &vdef_raw8);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_cmp(&vdef_i420, &vdef_raw8);
	CU_ASSERT_FALSE(ret);

	ret = vdef_raw_format_cmp(&vdef_raw8, &vdef_raw8);
	CU_ASSERT_TRUE(ret);
}


static void test_vdef_is_raw_format_valid(void)
{
	bool ret;
	struct vdef_raw_format format = {};
	struct vdef_raw_format invalid_i420 = vdef_i420;
	invalid_i420.pix_order = VDEF_RAW_PIX_ORDER_DCBA + 1;
	struct vdef_raw_format invalidi420_2 = vdef_i420;
	invalidi420_2.pix_order = VDEF_RAW_PIX_ORDER_BDAC;
	struct vdef_raw_format invalid_gray = vdef_gray;
	invalid_gray.pix_order = VDEF_RAW_PIX_ORDER_BA;
	invalid_gray.data_layout = VDEF_RAW_DATA_LAYOUT_PLANAR_Y_U_V;

	ret = vdef_is_raw_format_valid(NULL);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_raw_format_valid(&format);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_raw_format_valid(&invalid_i420);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_raw_format_valid(&invalidi420_2);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_raw_format_valid(&invalid_gray);
	CU_ASSERT_FALSE(ret);

	ret = vdef_is_raw_format_valid(&vdef_raw8);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_i420);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_yv12);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_nv12);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_i444);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_rgba);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_bayer_bggr);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_bayer_rggb_10_packed);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(
		&vdef_nv21_hisi_tile_compressed_10_packed);
	CU_ASSERT_TRUE(ret);

	ret = vdef_is_raw_format_valid(&vdef_opaque);
	CU_ASSERT_TRUE(ret);
}


CU_TestInfo g_vdef_test_utils[] = {
	{FN("vdef-is-raw-format-valid"), &test_vdef_is_raw_format_valid},
	{FN("vdef-raw-format-cmp"), &test_vdef_raw_format_cmp},
	{FN("vdef-raw-format-intersect"), &test_vdef_raw_format_intersect},
	{FN("vdef-is-coded-format-valid"), &test_vdef_is_coded_format_valid},
	{FN("vdef-coded-format-cmp"), &test_vdef_coded_format_cmp},
	{FN("vdef-coded-format-intersect"), &test_vdef_coded_format_intersect},
	{FN("vdef-frame-type-from-str"), &test_vdef_frame_type_from_str},
	{FN("vdef-frame-type-to-str"), &test_vdef_frame_type_to_str},
	{FN("vdef-encoding-from-str"), &test_vdef_encoding_from_str},
	{FN("vdef-encoding-to-str"), &test_vdef_encoding_to_str},
	{FN("vdef-get-encoding-mime-type"), &test_vdef_get_encoding_mime_type},
	{FN("vdef-coded-data-from-str"), &test_vdef_coded_data_format_from_str},
	{FN("vdef-coded-data-to-str"), &test_vdef_coded_data_format_to_str},
	{FN("vdef-coded-frame-type-from-str"),
	 &test_vdef_coded_frame_type_from_str},
	{FN("vdef-coded-frame-type-to-str"),
	 &test_vdef_coded_frame_type_to_str},
	{FN("vdef-color-primaries-from-h264"),
	 &test_vdef_color_primaries_from_h264},
	{FN("vdef-color-primaries-to-h264"),
	 &test_vdef_color_primaries_to_h264},
	{FN("vdef-color-primaries-from-h265"),
	 &test_vdef_color_primaries_from_h265},
	{FN("vdef-color-primaries-to-h265"),
	 &test_vdef_color_primaries_to_h265},
	{FN("vdef-color-primaries-from-str"),
	 &test_vdef_color_primaries_from_str},
	{FN("vdef-color-primaries-to-str"), &test_vdef_color_primaries_to_str},
	{FN("vdef-color-primaries-from-values"),
	 &test_vdef_color_primaries_from_values},
	{FN("vdef-transfer-function-from-h264"),
	 &test_vdef_transfer_function_from_h264},
	{FN("vdef-transfer-function-to-h264"),
	 &test_vdef_transfer_function_to_h264},
	{FN("vdef-transfer-function-from-h265"),
	 &test_vdef_transfer_function_from_h265},
	{FN("vdef-transfer-function-to-h265"),
	 &test_vdef_transfer_function_to_h265},
	{FN("vdef-transfer-function-from-str"),
	 &test_vdef_transfer_function_from_str},
	{FN("vdef-transfer-function-to-str"),
	 &test_vdef_transfer_function_to_str},
	{FN("vdef-matrix-coefs-from-h264"), &test_vdef_matrix_coefs_from_h264},
	{FN("vdef-matrix-coefs-to-h264"), &test_vdef_matrix_coefs_to_h264},
	{FN("vdef-matrix-coefs-from-h265"), &test_vdef_matrix_coefs_from_h265},
	{FN("vdef-matrix-coefs-to-h265"), &test_vdef_matrix_coefs_to_h265},
	{FN("vdef-matrix-coefs-from-str"), &test_vdef_matrix_coefs_from_str},
	{FN("vdef-matrix-coefs-to-str"), &test_vdef_matrix_coefs_to_str},
	{FN("vdef-dynamic-range-from-str"), &test_vdef_dynamic_range_from_str},
	{FN("vdef-dynamic-range-to-str"), &test_vdef_dynamic_range_to_str},
	{FN("vdef-tone-mapping-from-str"), &test_vdef_tone_mapping_from_str},
	{FN("vdef-tone-mapping-to-str"), &test_vdef_tone_mapping_to_str},
	{FN("vdef-dim-is-aligned"), &test_vdef_dim_is_aligned},
	{FN("vdef-rect-is-aligned"), &test_vdef_rect_is_aligned},
	{FN("vdef-format-to-frame-info"), &test_vdef_format_to_frame_info},
	{FN("vdef-frame-to-format-info"), &test_vdef_frame_to_format_info},
	{FN("vdef-coded-format-from-str"), &test_vdef_coded_format_from_str},
	{FN("vdef-coded-format-to-str"), &test_vdef_coded_format_to_str},
	{FN("vdef-rect-align"), &test_vdef_rect_align},

	CU_TEST_INFO_NULL,
};
