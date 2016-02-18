/**
 * Test program to demonstrate the use of h5xx in order to save a
 * matrix distributed over MPI processes.  The global matrix is
 * a vertically stacket set of local 2D Boost multi_arrays.
 * The local matrices are filled with the local processes' rank.
 * The read-back and check is performed by rank 0 only.
 *
 * Copyright © 2016 Klaus Reuter
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


#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include "boost/multi_array.hpp"
#include <h5xx/h5xx.hpp>
#include <mpi.h>

typedef boost::multi_array<int, 2> array_2d_t;

int main(int argc, char ** argv) {
    const std::string filename = "test_h5xx_dataset_big_mpi.h5";
    const std::string matrix_name = "distributed integer matrix";

    // WARNING -- size of the process-local matrix is set to 1 GB -- WARNING
    const size_t NI = 256;  // local matrix blocks are stacked along this dimension globally
    const size_t NJ = 1048576;

    const size_t NE = NI * NJ;
    const double gigabytes_per_process = double(sizeof(int)*NI*NJ)/double(1073741824);

    int rank;
    int size;
    MPI_Init(&argc, &argv);
    const MPI_Comm comm = MPI_COMM_WORLD;
    const MPI_Info info = MPI_INFO_NULL;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    const double gigabytes_total = double(size) * gigabytes_per_process;

    if (rank == 0) {
        std::cout << "Running 1 test case..." << std::endl;
        std::cout << "write-out: " << size << " MPI processes, " << gigabytes_per_process << " GB per process" << std::endl;
    }

    // write distributed matrix to HDF5 file
    {
        // allocate and initialize local array block
        array_2d_t matrix(boost::extents[NI][NJ]);
        for (size_t i=0; i<NE; ++i)
            matrix.data()[i] = rank;

        // --- time the write operation
        double t0 = MPI_Wtime();
        {
            h5xx::file hdf5_file(filename, comm, info, h5xx::file::trunc);

            // --- create dataset
            h5xx::datatype matrix_datatype(matrix);
            //
            std::vector<size_t> matrix_global_dim;
            matrix_global_dim.push_back(NI * size);
            matrix_global_dim.push_back(NJ);
            h5xx::dataspace matrix_dataspace(matrix_global_dim);
            //
            std::vector<size_t> chunk_dims;
            chunk_dims.push_back(256);
            chunk_dims.push_back(256);
            //
            h5xx::create_dataset(hdf5_file,
                                 matrix_name,
                                 matrix_datatype,
                                 matrix_dataspace,
                                 h5xx::policy::storage::chunked(chunk_dims));

            // --- define slice and write dataset
            std::vector<size_t> slice_offset;
            slice_offset.push_back(NI * rank);  // stack matrix blocks
            slice_offset.push_back(0);
            std::vector<size_t> slice_count;
            slice_count.push_back(NI);
            slice_count.push_back(NJ);
            h5xx::slice matrix_slice(slice_offset, slice_count);
            //
            h5xx::write_dataset(hdf5_file,
                                matrix_name,
                                matrix,
                                matrix_slice);
        } // file is closed at the end of its scope
        double t1 = MPI_Wtime();
        if (rank == 0)
            std::cout << "write-out: total data rate = " << gigabytes_total/(t1 - t0) << " GB/s" << std::endl;
    }


    // read complete matrix back from the file and check its entries
    if (rank == 0) {
        std::cout << "Reading and checking " << gigabytes_total << " GB on MPI rank 0 ..." << std::endl;
        try {
            array_2d_t matrix;
            h5xx::file hdf5_file(filename);
            h5xx::read_dataset(hdf5_file,
                               matrix_name,
                               matrix);
            // test block by block if the matrix element is equal to the generating processors rank
            for (size_t j=0; j<size; ++j) {
                size_t idx = j * NE;
                for (size_t i=0; i<NE; ++i) {
                    if (matrix.data()[idx] != j)
                        throw std::string("matrix element is wrong");
                    ++idx;
                }
            }

            std::cout << "*** No errors detected" << std::endl;
        } catch (std::string & s) {
            std::cout << "*** Error: " + s << std::endl;
        } catch (...) {
            std::cout << "*** Error in matrix read-back from HDF5 file." << std::endl;
        }
        //remove(filename.c_str());  // comment to inspect HDF5 file
    }

    MPI_Barrier(comm);

    MPI_Finalize();
    return 0;
}
