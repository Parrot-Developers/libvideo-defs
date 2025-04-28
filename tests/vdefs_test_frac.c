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


static void test_frac_is_null(void)
{
	bool res;
	struct vdef_frac frac = {0, 0};

	res = vdef_frac_is_null(NULL);
	CU_ASSERT_TRUE(res);

	res = vdef_frac_is_null(&frac);
	CU_ASSERT_TRUE(res);

	frac.num = 0;
	frac.den = 1;
	res = vdef_frac_is_null(&frac);
	CU_ASSERT_TRUE(res);

	frac.num = 1;
	frac.den = 0;
	res = vdef_frac_is_null(&frac);
	CU_ASSERT_TRUE(res);

	frac.num = 1;
	frac.den = 1;
	res = vdef_frac_is_null(&frac);
	CU_ASSERT_FALSE(res);

	frac.num = 30000;
	frac.den = 1001;
	res = vdef_frac_is_null(&frac);
	CU_ASSERT_FALSE(res);
}


static void test_frac_diff(void)
{
	int res;
	struct vdef_frac empty_fracs[] = {{0, 0}, {0, 1}, {1, 0}};
	struct vdef_frac frac30 = {30000, 1001};
	struct vdef_frac frac30_round = {30, 1};
	struct vdef_frac frac60 = {60000, 1001};
	struct vdef_frac frac60_same = {60000000, 1001000};

	/* invalid arguments */
	res = vdef_frac_diff(NULL, NULL);
	CU_ASSERT_EQUAL(res, -1);

	res = vdef_frac_diff(&empty_fracs[0], NULL);
	CU_ASSERT_EQUAL(res, -1);

	res = vdef_frac_diff(NULL, &empty_fracs[0]);
	CU_ASSERT_EQUAL(res, -1);

	/* empty frac */
	for (size_t i = 0; i < ARRAY_SIZE(empty_fracs); i++) {
		for (size_t j = 0; j < ARRAY_SIZE(empty_fracs); j++) {
			res = vdef_frac_diff(&empty_fracs[i], &empty_fracs[j]);
			CU_ASSERT_EQUAL(res, 0);
		}
	}

	/* valid + empty */
	for (size_t i = 0; i < ARRAY_SIZE(empty_fracs); i++) {
		res = vdef_frac_diff(&frac30, &empty_fracs[i]);
		CU_ASSERT_EQUAL(res, 1);

		res = vdef_frac_diff(&empty_fracs[i], &frac30);
		CU_ASSERT_EQUAL(res, -1);
	}

	res = vdef_frac_diff(&frac30, &frac30);
	CU_ASSERT_EQUAL(res, 0);

	res = vdef_frac_diff(&frac60, &frac60);
	CU_ASSERT_EQUAL(res, 0);

	res = vdef_frac_diff(&frac60, &frac30);
	CU_ASSERT(res > 0);

	res = vdef_frac_diff(&frac30, &frac60);
	CU_ASSERT(res < 0);

	/* close frac */
	res = vdef_frac_diff(&frac30, &frac30_round);
	CU_ASSERT(res != 0);

	/* same frac */
	res = vdef_frac_diff(&frac60, &frac60_same);
	CU_ASSERT_EQUAL(res, 0);
}

CU_TestInfo g_vdef_test_frac[] = {
	{FN("frac-is-null"), &test_frac_is_null},
	{FN("frac-diff"), &test_frac_diff},

	CU_TEST_INFO_NULL,
};
