/**
 * Copyright (C) 2022 Alexander Montgomerie-Corcoran
 *
 * This program is free software: you can redistribute it and/or modify
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GLUE_HPP_
#define GLUE_HPP_

#include "common.hpp"

/**
 *  GLUE FUNCTION
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int FILTERS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int COARSE_GROUP,
    typename glue_acc_t,
    typename glue_data_t
>
void glue(
    stream_t(glue_acc_t) in[COARSE_IN*COARSE_GROUP][COARSE_OUT],
    stream_t(glue_data_t) out[COARSE_OUT*COARSE_GROUP]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int filters      = FILTERS;
    const unsigned int coarse_in    = COARSE_IN;
    const unsigned int coarse_out   = COARSE_OUT;
    const unsigned int coarse_group = COARSE_GROUP;
    const unsigned int filters_per_coarse   = DIVIDE(filters,coarse_out*coarse_group);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    glue_acc_t acc[coarse_out*coarse_group];
    #pragma HLS ARRAY_PARTITION variable=acc complete dim=0

    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols*filters_per_coarse;pixel_index++) {
        #pragma HLS pipeline II=1 rewind
        coarse_group_loop: for(unsigned int group_index=0; group_index<coarse_group; group_index++) {
            coarse_out_loop: for(unsigned int out_index=0; out_index<coarse_out; out_index++) {
                coarse_in_loop: for(unsigned int in_index=0; in_index<coarse_in; in_index++) {

                    // update accumulation cache
                    glue_acc_t prev = ( in_index == 0 ) ? glue_acc_t(0) : acc[group_index*coarse_out+out_index] ;
                    acc[group_index*coarse_out+out_index] = prev + in[group_index*coarse_in+in_index][out_index].read() ;

                    // write to output stream
                    if( in_index == (coarse_in-1) ) {
                        out[group_index*coarse_out+out_index].write( glue_data_t(acc[group_index*coarse_out+out_index]) ) ;
                    }

                }
            }
        }
    }
}

#endif
