/******************************************************************************
 *
 * Project:  COG Driver
 * Purpose:  Cloud optimized GeoTIFF write support.
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef COGDRIVER_H_INCLUDED
#define COGDRIVER_H_INCLUDED

#include "gdal_priv.h"
#include "cpl_string.h"

bool COGHasWarpingOptions(CSLConstList papszOptions);

bool COGGetTargetSRS(const char *const *papszOptions, CPLString &osTargetSRS);

std::string COGGetResampling(GDALDataset *poSrcDS,
                             const char *const *papszOptions);

bool COGGetWarpingCharacteristics(GDALDataset *poSrcDS,
                                  const char *const *papszOptions,
                                  CPLString &osResampling,
                                  CPLString &osTargetSRS, int &nXSize,
                                  int &nYSize, double &dfMinX, double &dfMinY,
                                  double &dfMaxX, double &dfMaxY);
void COGRemoveWarpingOptions(CPLStringList &aosOptions);

#endif  // COGDRIVER_H_INCLUDED
