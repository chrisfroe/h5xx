/*
 * Copyright © 2010  Peter Colberg
 *
 * This file is part of h5xx.
 *
 * h5xx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H5XX_COMPAT_HPP
#define H5XX_COMPAT_HPP

// HDF5 ≥ 1.8.15 does not compile properly with GCC ≥ 4.8 in C++11 mode due to
// issues with SSE3 optimisation. For example, it emits:
//
//     /usr/lib64/gcc/x86_64-unknown-linux-gnu/4.8.2/include/mmintrin.h:61:54: error: can’t convert between vector values of different size
//         return (__m64) __builtin_ia32_vec_init_v2si (__i, 0);
//
// A related issue is described in http://stackoverflow.com/questions/19043109
//
// The following include serves as a work around, it must precede any HDF5
// headers. Thus, we can not even check for the HDF5 version used.
#if __cplusplus >= 201103L
#  include <random>
#endif

//
// h5xx wrapper supports the following HDF5 library versions:
//
//  - HDF5 1.8.x compiled using --disable-deprecate-symbols
//  - HDF5 1.8.x compiled using --with-default-api-version=v18
//  - HDF5 1.8.x compiled using --with-default-api-version=v16
//
// In this compatibility header file, we define a common HDF5 C API
// for all of the above versions, with the intent to minimize use
// of versioned #ifdefs in h5xx wrapper functions.
//

//
// Note for developers: If you make a change to h5xx wrapper, compile
// and run the test suite for *all* supported HDF5 library versions.
//

/**
 * if using HDF5 1.8.x, force HDF 1.8 API as needed
 */
#define H5Dcreate_vers   2
#define H5Dopen_vers     2
#define H5E_auto_t_vers  2
#define H5Eget_auto_vers 2
#define H5Eprint_vers    2
#define H5Eset_auto_vers 2
#define H5Ewalk_vers     2
#define H5Gcreate_vers   2
#define H5Gopen_vers     2

#include <hdf5.h>

#endif /* ! H5XX_COMPAT_HPP */
