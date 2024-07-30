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

#ifndef COMMON_TB_HPP_
#define COMMON_TB_HPP_

#include "common.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
/////////////////////////////// LAYERS ///////////////////////////////
//////////////////////////////////////////////////////////////////////

template<int SIZE, int STREAMS, typename T>
void load_data(
    std::string filepath,
    T data[SIZE][STREAMS]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    // save to array
    int index = 0;
    for(int i=0;i<SIZE;i++) {
        for(int j=0;j<STREAMS;j++) {
            float val;
            fscanf(fp,"%f\n", &val);
            data[i][j] = T( val );
        }
    }

    // close file
    fclose(fp);
}

template<int SIZE, int STREAMS, typename T>
void to_stream(
    T in[SIZE][STREAMS],
    stream_t(T) out[STREAMS]
) {
    for(int i=0;i<SIZE;i++) {
        for(int j=0;j<STREAMS;j++) {
 	    out[j].write(in[i][j]);
	}
    }
}

///////////////////////////////////////////////////////////////////////
/////////////////////////////// MODULES ///////////////////////////////
///////////////////////////////////////////////////////////////////////

/* (ACCUM) */
template<int SIZE, typename T>
void load_data(
    std::string filepath,
    T data[SIZE]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    // save to array
    for(int i=0;i<SIZE;i++) {
        float val;
        fscanf(fp,"%f\n", &val);
        data[i] = T( val );
    }

    // close file
    fclose(fp);
}

template<int SIZE, typename T>
void to_stream(
    T in[SIZE],
    stream_t(T) &out
)
{
    for(int i=0;i<SIZE;i++) {
 	out.write(in[i]);
    }
}

/* (CONV,POOL) */
template<int SIZE, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void load_data(
    std::string filepath,
    T data[SIZE][KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {

    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }
     // save to array
    for(int i=0;i<SIZE;i++) {
        for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
            for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
                float val;
                fscanf(fp,"%f\n", &val);
                data[i][k1][k2] = T(val);
	        }
	    }
    }

    // close file
    fclose(fp);
}

template<int SIZE, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void to_stream(
    T in[SIZE][KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(T) out[KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {
    for(int i=0;i<SIZE;i++) {
        for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
            for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
                out[k1][k2].write(in[i][k1][k2]);
            }
	    }
    }
}

/* (FORK) */
template<int SIZE, int COARSE, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void load_data(
    std::string filepath,
    T data[SIZE][COARSE][KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    // save to array
    for(int i=0;i<SIZE;i++) {
        for(int c=0;c<COARSE;c++) {
            for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
                for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
                    float val;
                    fscanf(fp,"%f\n", &val);
                    data[i][c][k1][k2] = T(val);
                }
            }
        }
    }

    // close file
    fclose(fp);
}

template<int SIZE, int COARSE, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void to_stream(
    T in[SIZE][COARSE][KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(T) out[COARSE][KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {
    for(int i=0;i<SIZE;i++) {
        for(int c=0;c<COARSE;c++) {
            for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
	            for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
	                out[c][k1][k2].write(in[i][c][k1][k2]);
                }
	        }
        }
    }
}

/* (WEIGHTS_INTR) */
template<int COARSE_IN, int COARSE_OUT, int CHANNELS, int FILTERS, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void load_data(
    std::string filepath,
    T data[COARSE_IN][COARSE_OUT][DIVIDE(CHANNELS,COARSE_IN)*DIVIDE(FILTERS,COARSE_OUT)][KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    const int channels_per_coarse_in = DIVIDE(CHANNELS,COARSE_IN);
    const int filters_per_coarse_out = DIVIDE(FILTERS,COARSE_OUT);

    // save to array
    for(int i=0;i<channels_per_coarse_in;i++) {
        for(int cin=0;cin<COARSE_IN;cin++) {
            for(int j=0;j<filters_per_coarse_out;j++) {
                for(int cout=0;cout<COARSE_OUT;cout++) {
                    for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
                        for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
                            float val;
                            fscanf(fp,"%f\n", &val);
                            //int index = (i*COARSE_IN+cin)*filters_per_coarse_out+j*COARSE_OUT+cout;
                            int index = i*filters_per_coarse_out+j;
                            data[cin][cout][index][k1][k2] = T(val);
                        }
                    }
                }
            }
        }
    }

    // close file
    fclose(fp);
}

/* (WEIGHTS_INTR) */
template<int COARSE_IN, int COARSE_OUT, int CHANNELS, int FILTERS, typename T>
void load_data(
    std::string filepath,
    T data[COARSE_IN][COARSE_OUT][DIVIDE(CHANNELS,COARSE_IN)*DIVIDE(FILTERS,COARSE_OUT)]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    const int channels_per_coarse_in = DIVIDE(CHANNELS,COARSE_IN);
    const int filters_per_coarse_out = DIVIDE(FILTERS,COARSE_OUT);

    // save to array
    for(int i=0;i<channels_per_coarse_in;i++) {
        for(int cin=0;cin<COARSE_IN;cin++) {
            for(int j=0;j<filters_per_coarse_out;j++) {
                for(int cout=0;cout<COARSE_OUT;cout++) {
                    float val;
                    fscanf(fp,"%f\n", &val);
                    int index = i*filters_per_coarse_out+j;
                    data[cin][cout][index] = T(val);
                }
            }
        }
    }

    // close file
    fclose(fp);
}


template<int SIZE, int COARSE_IN, int COARSE_OUT, int KERNEL_SIZE_X, int KERNEL_SIZE_Y, typename T>
void to_stream(
    T in[COARSE_IN][COARSE_OUT][SIZE][KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(T) out[COARSE_IN][COARSE_OUT][KERNEL_SIZE_X][KERNEL_SIZE_Y]
) {
    for(int cin=0;cin<COARSE_IN;cin++) {
        for(int cout=0;cout<COARSE_OUT;cout++) {
            for(int i=0;i<SIZE;i++) {
                for(int k1=0;k1<KERNEL_SIZE_X;k1++) {
                    for(int k2=0;k2<KERNEL_SIZE_Y;k2++) {
                        out[cin][cout][k1][k2].write(in[cin][cout][i][k1][k2]);
                    }
                }
	    }
        }
    }
}

/* /1* (WEIGHTS_INTR) *1/ */
/* template<int SIZE, int COARSE_IN, int COARSE_OUT, typename T> */
/* void load_data( */
/*     std::string filepath, */
/*     T data[COARSE_IN][COARSE_OUT][SIZE] */
/* ) { */
/*     // read in file */
/*     const char *filepath_cstr = filepath.c_str(); */
/*     FILE * fp = fopen(filepath_cstr,"r"); */

/*     // check file opened */
/*     if (fp == NULL) { */
/*         perror("Failed: "); */
/*     } */

/*     // save to array */
/*     for(int cin=0;cin<COARSE_IN;cin++) { */
/*         for(int cout=0;cout<COARSE_OUT;cout++) { */
/*             for(int i=0;i<SIZE;i++) { */
/*                 float val; */
/*                 fscanf(fp,"%f\n", &val); */
/*                 data[cin][cout][i] = T(val); */
/*             } */
/*         } */
/*     } */

/*     // close file */
/*     fclose(fp); */
/* } */

template<int SIZE, int COARSE_IN, int COARSE_OUT, typename T>
void to_stream(
    T in[COARSE_IN][COARSE_OUT][SIZE],
    stream_t(T) out[COARSE_IN][COARSE_OUT]
) {
    for(int cin=0;cin<COARSE_IN;cin++) {
        for(int cout=0;cout<COARSE_OUT;cout++) {
            for(int i=0;i<SIZE;i++) {
                out[cin][cout].write(in[cin][cout][i]);
	    }
        }
    }
}

/* (WEIGHTS_INTR) */
template<int SIZE, int COARSE_IN, int COARSE_OUT, int FINE, typename T>
void load_data(
    std::string filepath,
    T data[COARSE_IN][COARSE_OUT][SIZE][FINE]
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    // save to array
    for(int cin=0;cin<COARSE_IN;cin++) {
        for(int cout=0;cout<COARSE_OUT;cout++) {
            for(int i=0;i<SIZE;i++) {
                for(int f=0;f<FINE;f++) {
                    float val;
                    fscanf(fp,"%f\n", &val);
                    data[cin][cout][i][f] = T(val);
                }
            }
        }
    }

    // close file
    fclose(fp);
}

template<int SIZE, int COARSE_IN, int COARSE_OUT, int FINE, typename T>
void to_stream(
    T in[COARSE_IN][COARSE_OUT][SIZE][FINE],
    stream_t(T) out[COARSE_IN][COARSE_OUT][FINE]
) {
    for(int cin=0;cin<COARSE_IN;cin++) {
        for(int cout=0;cout<COARSE_OUT;cout++) {
            for(int i=0;i<SIZE;i++) {
                for(int f=0;f<FINE;f++) {
                    out[cin][cout][f].write(in[cin][cout][i][f]);
                }
	    }
        }
    }
}


/* (GLUE) */
/* same as CONV/POOL */
/*template<int SIZE, int COARSE_IN, int COARSE_OUT, typename T>
void load_data(
    std::string filepath,
    T data[SIZE][COARSE_IN][COARSE_OUT]
) {

    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed: ");
    }

    // save to array
    for(int i=0;i<SIZE;i++) {
        for(int a=0;a<COARSE_IN;a++) {
            for(int b=0;b<COARSE_OUT;b++) {
                float val;
                fscanf(fp,"%f\n", &val);
                data[i][a][b] = T(val);
	    }
        }
    }

    // close file
    fclose(fp);
}

template<int SIZE, int COARSE_IN, int COARSE_OUT, typename T>
void to_stream(
    T in[SIZE][COARSE_IN][COARSE_OUT],
    stream_t(T) out[COARSE_IN][COARSE_OUT]
) {
    for(int i=0;i<SIZE;i++) {
        for(int a=0;a<COARSE_IN;a++) {
            for(int b=0;b<COARSE_OUT;b++) {
	        out[a][b].write(in[i][a][b]);
	    }
        }
    }
}*/


////////////////////////////////////////////////
////////// CHECK STREAM TO VALID OUTPUT ////////
////////////////////////////////////////////////


template<typename T>
int checkStreamEqual(
		stream_t(T) &test,
		stream_t(T) &valid,
		bool print_out=false
)
{
    int err = 0;
    int index = 0;
    while(!valid.empty()) {
		if(test.empty()) {
			printf("ERROR: empty early\n");
			return 1;
		}
		T tmp = test.read();
		T tmp_valid = valid.read();

		// if(print_out) printf("%x,%x\n",tmp.range(),tmp_valid.range());
		if(print_out) printf("%x,%x,%f,%f\n",tmp.range(),tmp_valid.range(),
                tmp.to_float(), tmp_valid.to_float());

		if(
				(tmp.to_float() > tmp_valid.to_float()+ERROR_TOLERANCE) ||
				(tmp.to_float() < tmp_valid.to_float()-ERROR_TOLERANCE)
		)
		{
			//printf("ERROR: wrong value\n");
			printf("(%d) ERROR: wrong value %f != %f (err=%f)\n", index, tmp.to_float(),
                    tmp_valid.to_float(), tmp.to_float()-tmp_valid.to_float());
			return 1;
			err++;
		}
        // increment index
        index++;
	}

	if(!test.empty())
	{
		printf("ERROR: still data in stream\n");
		return 1;
		err++;
	}
	return err;
}

template<
    unsigned int SIZE,
    unsigned int STREAMS,
    unsigned int DMA_WIDTH = 64,
    unsigned int DATA_WIDTH = 16
>
int check_array_equal(
    volatile mem_int test[SIZE],
    volatile mem_int valid[SIZE]
)
{

    const unsigned dma_width = DMA_WIDTH;
    const unsigned data_width = DATA_WIDTH;
    const unsigned bit_mask = (1 << data_width) - 1;

    int err = 0;
    for(int i=0;i<SIZE;i++) {
        for(int j=0;j<STREAMS;j++) {
        	mem_int temp = test[i];
        	mem_int temp_valid = valid[i];
            // std::cout<< "at i = " << i << ", j = " << j << ", 64 bit temp = " << temp << ", temp_valid = " << temp_valid << std::endl;
            data_t tmp, tmp_valid;
            tmp.range()         = (temp >> j*data_width) & bit_mask;
            tmp_valid.range()   = (temp_valid >> j*data_width) & bit_mask;
            // std::cout << "at i = " << i << ", j = " << j << ", tmp = " << tmp << ", tmp_valid = " << tmp_valid << std::endl;
            // std::cout << "at i = " << i << ", j = " << j << ", bitwise tmp = " << tmp.range() << ", bitwise tmp_valid = " << tmp_valid.range() << std::endl;
            if(
                (tmp.to_float() > tmp_valid.to_float()+ERROR_TOLERANCE) ||
                (tmp.to_float() < tmp_valid.to_float()-ERROR_TOLERANCE)
            ){
                err += 1;
                printf("ERROR (%d,%d): %f != %f\n",i,j,tmp.to_float(),tmp_valid.to_float());
            }
        }
    }
    return err;
}

////////////////////////////////////////////////////
////////// LOAD DATA FOR PARTITION TESTS ///////////
////////////////////////////////////////////////////

template<
    int INPUTS,
    int SIZE,
    int WR_FACTOR=1
>
// void load_net_weights(
//     std::string filepath,
//     volatile mem_int data[INPUTS][DIVIDE(SIZE,INPUTS)],
//     int wr_index = 0
// )
// {
//     // read in file
//     const char *filepath_cstr = filepath.c_str();
//     FILE * fp = fopen(filepath_cstr,"r");

//     // check file opened
//     if (fp == NULL) {
//         perror("Failed to load weights at");
//     }

//     for(int w=0;w<WR_FACTOR;w++) {
//         for(int i=0;i<INPUTS;i++) {
//             for(int j=0;j<DIVIDE(SIZE,INPUTS);j++) {
//                 // read in the value from the file
//                 mem_int val;
//                 fscanf(fp,"%l\n", &val);
//                 // add to the input if correct weights reloading index
//                 if ( w == wr_index ) {
//                     data[i][j] = val;
//                 }
//            }
//         }
//     }
//     // close file
//     fclose(fp);
// }
void load_net_weights(
    std::string filepath,
    volatile mem_int data[INPUTS][DIVIDE(SIZE, INPUTS)],
    int wr_index = 0
) {
    // read in file
    const char *filepath_cstr = filepath.c_str();
    std::ifstream fp(filepath_cstr);

    // check file opened
    if (!fp.is_open()) {
        perror("Failed to load weights at");
        return;
    }

    for (int w = 0; w < WR_FACTOR; w++) {
        for (int i = 0; i < INPUTS; i++) {
            for (int j = 0; j < DIVIDE(SIZE, INPUTS); j++) {
                // read in the value from the file
                std::string line;
                if (std::getline(fp, line)) {
                    mem_int val(line.c_str(), 10);
                    // add to the input if correct weights reloading index
                    if (w == wr_index) {
                        data[i][j] = val;
                        mem_int non_volatile_val = data[i][j];
                        // std::cout << "data[" << i << "][" << j << "] = " << non_volatile_val.to_string(10) << std::endl;
                    }
                } 
            }
        }
    }
    // close file
    fp.close();
}

template<
    unsigned int INPUTS,
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int STREAMS,
    unsigned int WR_FACTOR=1,
    unsigned int DMA_WIDTH = 64,
    unsigned int DATA_WIDTH = 16,
    typename mem_t = mem_int
>
// void load_net_data(
//     std::string filepath,
//     mem_t data[INPUTS][BATCH_SIZE*ROWS*COLS*DIVIDE(CHANNELS,STREAMS)*WR_FACTOR],
//     int wr_index = 0
// )
// {

//     // read in file
//     const char *filepath_cstr = filepath.c_str();
//     FILE * fp = fopen(filepath_cstr,"r");

//     // check file opened
//     if (fp == NULL) {
//         perror("Failed to load data at");
//     }

//     // get variables
//     int channels_per_stream = DIVIDE(CHANNELS,STREAMS);
//     int dma_channels = DIVIDE(DMA_WIDTH,DATA_WIDTH);

//     // save to array
//     for(int i=0;i<BATCH_SIZE*ROWS*COLS;i++) {
//         for(int j=0;j<WR_FACTOR;j++) {
//             for(int k=0;k<channels_per_stream;k++) {
//                 // read in the value from the file
//                 mem_int val;
//                 fscanf(fp,"%lu\n", &val);
//                 // specific weights reloading index
//                 if (j == wr_index) {
//                     int out_index = i*channels_per_stream*WR_FACTOR + j*channels_per_stream + k;
//                     data[0][out_index] = val;
//                     // std::cout << "data[0][" << out_index << "] = " << data[0][out_index] << std::endl;
//                 }
//             }
//         }
//     }
//     // close file
//     fclose(fp);
// }
void load_net_data(
    std::string filepath,
    mem_t data[INPUTS][BATCH_SIZE*ROWS*COLS*DIVIDE(CHANNELS,STREAMS)*WR_FACTOR],
    int wr_index = 0
)
{

    // read in file
    std::ifstream fp(filepath);

    // check file opened
    if (!fp.is_open()) {
        perror("Failed to load data at");
        return;
    }

    // get variables
    int channels_per_stream = DIVIDE(CHANNELS,STREAMS);
    int dma_channels = DIVIDE(DMA_WIDTH,DATA_WIDTH);

    // save to array
    for(int i=0;i<BATCH_SIZE*ROWS*COLS;i++) {
        for(int j=0;j<WR_FACTOR;j++) {
            for(int k=0;k<channels_per_stream;k++) {
                // read in the value from the file
                std::string line;
                if (std::getline(fp, line)) {
                    mem_int val(line.c_str(), 10);
                    // specific weights reloading index
                    if (j == wr_index) {
                        int out_index = i*channels_per_stream*WR_FACTOR + j*channels_per_stream + k;
                        data[0][out_index] = val;
                        // std::cout << "data[0][" << out_index << "] = " << data[0][out_index].to_string(10) << std::endl;
                    }
                }
            }
        }
    }
    // close file
    fp.close();
}

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int STREAMS,
    unsigned int WR_FACTOR,
    typename T
>
void load_wr_data(
    std::string filepath,
    T data[BATCH_SIZE*ROWS*COLS*DIVIDE(CHANNELS,STREAMS)][STREAMS],
    int wr_index = 0
)
{

    // read in file
    const char *filepath_cstr = filepath.c_str();
    FILE * fp = fopen(filepath_cstr,"r");

    // check file opened
    if (fp == NULL) {
        perror("Failed to load data at");
    }

    // get variables
    int channels_per_stream = DIVIDE(CHANNELS,STREAMS);

    // save to array
    for(int i=0;i<BATCH_SIZE*ROWS*COLS;i++) {
        for(int j=0;j<WR_FACTOR;j++) {
            for(int k=0;k<channels_per_stream;k++) {
                for(int l=0;l<STREAMS;l++) {
                    // read in the value from the file
                    float val;
                    fscanf(fp,"%f\n", &val);
                    // specific weights reloading index
                    if (j == wr_index) {
                        data[i*channels_per_stream+k][l] = T( val );
                    }
                }
            }
        }
    }

    // close file
    fclose(fp);
}

#endif
