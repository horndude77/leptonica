/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -  This software is distributed in the hope that it will be
 -  useful, but with NO WARRANTY OF ANY KIND.
 -  No author or distributor accepts responsibility to anyone for the
 -  consequences of using this software, or for whether it serves any
 -  particular purpose or works at all, unless he or she says so in
 -  writing.  Everyone is granted permission to copy, modify and
 -  redistribute this source code, for commercial or non-commercial
 -  purposes, with the following restrictions: (1) the origin of this
 -  source code must not be misrepresented; (2) modified versions must
 -  be plainly marked as such; and (3) this notice may not be removed
 -  or altered from any source or modified source distribution.
 *====================================================================*/

/*
 * skew_reg.c
 *
 *     Regression test for skew detection.
 */

#include <stdio.h>
#include <stdlib.h>
#include "allheaders.h"

    /* deskew */
#define   DESKEW_REDUCTION      4      /* 1, 2 or 4 */

    /* sweep only */
#define   SWEEP_RANGE           5.     /* degrees */
#define   SWEEP_DELTA           0.2    /* degrees */
#define   SWEEP_REDUCTION       2      /* 1, 2, 4 or 8 */

    /* sweep and search */
#define   SWEEP_RANGE2          5.     /* degrees */
#define   SWEEP_DELTA2          1.     /* degrees */
#define   SWEEP_REDUCTION2      2      /* 1, 2, 4 or 8 */
#define   SEARCH_REDUCTION      2      /* 1, 2, 4 or 8 */
#define   SEARCH_MIN_DELTA      0.01   /* degrees */

static const l_int32  BORDER = 150;


main(int    argc,
     char **argv)
{
l_int32      w, h, wd, hd;
l_float32    deg2rad, angle, conf, score;
PIX         *pixs, *pixb1, *pixb2, *pixr, *pixf, *pixd, *pixc;
PIXA        *pixa;
static char  mainName[] = "skew_reg";

    if (argc != 1)
	exit(ERROR_INT(" Syntax: skew_reg", mainName, 1));

    deg2rad = 3.1415926535 / 180.;

    pixa = pixaCreate(0);
    pixs = pixRead("feyn.tif");
    pixSetOrClearBorder(pixs, 100, 250, 100, 0, PIX_CLR);
    pixb1 = pixReduceRankBinaryCascade(pixs, 2, 2, 0, 0);
    pixDisplay(pixb1, 0, 100);

        /* Add a border and locate and deskew a 40 degree rotation */
    pixb2 = pixAddBorder(pixb1, BORDER, 0);
    pixGetDimensions(pixb2, &w, &h, NULL);
    pixSaveTiled(pixb2, pixa, 2, 1, 20, 8);
    pixr = pixRotateBySampling(pixb2, w / 2, h / 2,
                                    deg2rad * 40., L_BRING_IN_WHITE);
    pixSaveTiled(pixr, pixa, 2, 0, 20, 0);
    pixFindSkewSweepAndSearchScorePivot(pixr, &angle, &conf, NULL, 1, 1,
                                        0.0, 45.0, 2.0, 0.03,
                                        L_SHEAR_ABOUT_CENTER);
    fprintf(stderr, "Should be 40 degrees: angle = %7.3f, conf = %7.3f\n",
            angle, conf);
    pixf = pixRotateBySampling(pixr, w / 2, h / 2,
                                    deg2rad * angle, L_BRING_IN_WHITE);
    pixd = pixRemoveBorder(pixf, BORDER);
    pixSaveTiled(pixd, pixa, 2, 0, 20, 0);
    pixDestroy(&pixr);
    pixDestroy(&pixf);
    pixDestroy(&pixd);

        /* Do a rotation larger than 90 degrees using embedding;
         * Use 2 sets of measurements at 90 degrees to scan the
         * full range of possible rotation angles. */
    pixGetDimensions(pixb1, &w, &h, NULL);
    pixr = pixRotate(pixb1, deg2rad * 37., L_ROTATE_SAMPLING,
                     L_BRING_IN_WHITE, w, h);
    pixSaveTiled(pixr, pixa, 2, 1, 20, 0);
    startTimer();
    pixFindSkewOrthogonalRange(pixr, &angle, &conf, 2, 1,
                               47.0, 1.0, 0.03, 0.0);
    fprintf(stderr, "Orth search time: %7.3f sec\n", stopTimer());
    fprintf(stderr, "Should be about -128 degrees: angle = %7.3f\n", angle);
    pixd = pixRotate(pixr, deg2rad * angle, L_ROTATE_SAMPLING,
                     L_BRING_IN_WHITE, w, h);
    pixGetDimensions(pixd, &wd, &hd, NULL);
    pixc = pixCreate(w, h, 1);
    pixRasterop(pixc, 0, 0, w, h, PIX_SRC, pixd, (wd - w) / 2, (hd - h) / 2);
    pixWrite("junkpixc", pixc, IFF_PNG);
    pixSaveTiled(pixc, pixa, 2, 0, 20, 0);
    pixDestroy(&pixr);
    pixDestroy(&pixf);
    pixDestroy(&pixd);
    pixDestroy(&pixc);

    pixd = pixaDisplay(pixa, 0, 0);
    pixDisplay(pixd, 100, 100);
    pixWrite("junkskew.png", pixd, IFF_PNG);
    pixDestroy(&pixd);
    pixaDestroy(&pixa);
    return 0;
}

#if 0
    pixFindSkewSweepAndSearchScore(pixs, &angle, &conf, &endscore,
                                   4, 2, 0.0, 5.0, 1.0, 0.01);
    fprintf(stderr, "angle = %8.4f, conf = %8.4f, endscore = %f\n",
            angle, conf, endscore);
    startTimer();
    pixd = pixDeskew(pixs, DESKEW_REDUCTION);
    fprintf(stderr, "Time to deskew = %7.3f sec\n", stopTimer());
    pixWrite(fileout, pixd, IFF_BMP);
    pixDestroy(&pixd);
#endif


#if 0
    if (pixFindSkew(pixs, &angle)) {
	L_WARNING("skew angle not valid", mainName);
	exit(1);
    }
#endif

#if 0
    if (pixFindSkewSweep(pixs, &angle, SWEEP_REDUCTION,
                         SWEEP_RANGE, SWEEP_DELTA)) {
	L_WARNING("skew angle not valid", mainName);
	exit(1);
    }
#endif

#if 0
    if (pixFindSkewSweepAndSearch(pixs, &angle, SWEEP_REDUCTION2,
                         SEARCH_REDUCTION, SWEEP_RANGE2, SWEEP_DELTA2,
			 SEARCH_MIN_DELTA)) {
	L_WARNING("skew angle not valid", mainName);
	exit(1);
    }
#endif



