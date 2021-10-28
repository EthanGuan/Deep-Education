#include <cassert>
#include <iostream>
#include <limits>

#include "kernel.h"

using std::cout;
using std::endl;

int THD_COUNT = 1;

using std::string;

void normalize(array2d_t<float> & matrix, csr_t* snaph) {
    for (int i = 0 ; i < matrix.row_count ; i++) {
        matrix.row_normalize(i, snaph->get_degree(i));
    }
}

void apply(array2d_t<float> & input, array2d_t<float> & output, csr_t* snaph){
    vid_t* offset = snaph->offset;
    vid_t* nebrs = snaph->nebrs;

    for (int i = 0; i < output.row_count; i++) {
        for (int j = offset[i] ; j < offset[i+1] ; j++) {
            output.row_add(input.data_ptr + nebrs[j]*output.col_count, nebrs[j]);
        }
    }
}

void _gspmm(csr_t* snaph, array2d_t<float> & input, array2d_t<float> & output, 
                     op_t op, bool reverse, bool norm /*= true*/)
{
    //cout << "spmm " << op << "reverse = " << reverse << endl;

    //If in backward, normalize it first, else normalize it after computation
    
    //The core logic goes here.    
    if (reverse) {
        normalize(input, snaph);
        apply(input, output, snaph);
    } else {
        apply(input, output, snaph);
        normalize(output, snaph);
    }
}

void invoke_gspmm(graph_t& graph, array2d_t<float> & input_array, array2d_t<float> & output_array,
                 bool reverse, bool norm /*= true*/)
{
    if (reverse) {
         return _gspmm(&graph.csr, input_array, output_array, eSUM, reverse, norm);
    } else {
         return _gspmm(&graph.csc, input_array, output_array, eSUM, reverse, norm);
    }
}
