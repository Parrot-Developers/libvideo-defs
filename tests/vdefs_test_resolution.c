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


static void test_resolution_from_str(void)
{
	enum vdef_resolution res;

	res = vdef_resolution_from_str(NULL);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	res = vdef_resolution_from_str("");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	res = vdef_resolution_from_str("invalid_value");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	res = vdef_resolution_from_str("1080P_");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	res = vdef_resolution_from_str("1080P");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_1080P);

	res = vdef_resolution_from_str("1920X1080");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_1080P);

	res = vdef_resolution_from_str("360p");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_360P);

	res = vdef_resolution_from_str("640x360");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_360P);

	res = vdef_resolution_from_str("QQVGA");
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_QQVGA);
}


static void test_resolution_to_str(void)
{
	const char *res;

	res = vdef_resolution_to_str(VDEF_RESOLUTION_UNKNOWN);
	CU_ASSERT_STRING_EQUAL(res, "UNKNOWN");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_MAX);
	CU_ASSERT_STRING_EQUAL(res, "UNKNOWN");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_480P);
	CU_ASSERT_STRING_EQUAL(res, "480p");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_1024X544);
	CU_ASSERT_STRING_EQUAL(res, "1024x544");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_12MPX);
	CU_ASSERT_STRING_EQUAL(res, "12Mpx");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_21MPX);
	CU_ASSERT_STRING_EQUAL(res, "21Mpx");

	res = vdef_resolution_to_str(VDEF_RESOLUTION_48MPX);
	CU_ASSERT_STRING_EQUAL(res, "48Mpx");
}


static void test_resolution_from_dim(void)
{
	enum vdef_resolution res;
	struct vdef_dim dim = {0};

	res = vdef_resolution_from_dim(NULL);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	dim.width = 1920;
	dim.height = 0;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	dim.width = 0;
	dim.height = 1080;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UNKNOWN);

	dim.width = 1920;
	dim.height = 1080;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_1080P);

	dim.width = 1600;
	dim.height = 1200;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_UXGA);

	dim.width = 176;
	dim.height = 128;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_176X128);

	dim.width = 4000;
	dim.height = 3000;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_12MPX);

	dim.width = 5344;
	dim.height = 4016;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_21MPX);

	dim.width = 8000;
	dim.height = 6000;
	res = vdef_resolution_from_dim(&dim);
	CU_ASSERT_EQUAL(res, VDEF_RESOLUTION_48MPX);
}


static void test_resolution_to_dim(void)
{
	int ret;
	struct vdef_dim dim;

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_UNKNOWN, NULL);
	CU_ASSERT_EQUAL(ret, -EINVAL);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_MAX, &dim);
	CU_ASSERT_EQUAL(ret, -ENOENT);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_288P, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 512 && dim.height == 288);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_120P, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 214 && dim.height == 120);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_VGA, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 640 && dim.height == 480);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_12MPX, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 4000 && dim.height == 3000);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_21MPX, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 5344 && dim.height == 4016);

	ret = vdef_resolution_to_dim(VDEF_RESOLUTION_48MPX, &dim);
	CU_ASSERT_EQUAL(ret, 0);
	CU_ASSERT_TRUE(dim.width == 8000 && dim.height == 6000);
}


CU_TestInfo g_vdef_test_resolution[] = {
	{FN("resolution-from-str"), &test_resolution_from_str},
	{FN("resolution-to-str"), &test_resolution_to_str},
	{FN("resolution-from-dim"), &test_resolution_from_dim},
	{FN("resolution-to-dim"), &test_resolution_to_dim},

	CU_TEST_INFO_NULL,
};
