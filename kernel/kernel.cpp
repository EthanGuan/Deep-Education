#include <cassert>
#include <iostream>
#include <random>
#include <algorithm>
#include <limits>
#include <omp.h>
#include "kernel.h"

using std::cout;
using std::endl;

int THD_COUNT = 1;

using std::string;

void normalize(array2d_t<float> & matrix, csr_t* snaph) {

    #pragma omp parallel for num_threads(8)
    for (int i = 0 ; i < matrix.row_count ; i++) {
        matrix.row_normalize(i, snaph->get_degree(i));
    }
}

void apply(array2d_t<float> & input, array2d_t<float> & output, csr_t* snaph){
    vid_t* offset = snaph->offset;
    vid_t* nebrs = snaph->nebrs;

    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < output.row_count; i++) {  
        output.row_add(input.data_ptr + i*output.col_count, i);
        for (int j = offset[i] ; j < offset[i+1] ; j++) {
            output.row_add(input.data_ptr + nebrs[j]*output.col_count, i);
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

void sample(csr_t* sub_snaph) {

    std::random_device rd;
    std::mt19937 g(rd());

    vid_t* offset = sub_snaph->offset;
    vid_t* nebrs = sub_snaph->nebrs;

    for (int i=0; i<sub_snaph->get_vcount();i++) {
        std::shuffle(nebrs+offset[i], nebrs+offset[i+1], g);
    }
}

        // float* row_ptr = matrix.data_ptr + i * matrix.col_count;
        // float _sum_square = 0;
        // for (int64_t i = 0; i < matrix.col_count; ++i) {
        //     _sum_square += row_ptr[i]*row_ptr[i];
        // }
        // printf("%f ", _sum_square);
        // for (int64_t i = 0; i < matrix.col_count; ++i) {
        //     row_ptr[i] /= _sum_square;
        // }

void normalize_sage(array2d_t<float> & matrix, csr_t* snaph, int sample_num) {

    #pragma omp parallel for num_threads(8)
    for (int i = 0 ; i < matrix.row_count ; i++) {
        matrix.row_normalize(i, std::min((int)(snaph->get_degree(i)), sample_num));

    }
}

void apply_sage(array2d_t<float> & input, array2d_t<float> & output, csr_t* snaph, int sample_num){
    vid_t* offset = snaph->offset;
    vid_t* nebrs = snaph->nebrs;

    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < output.row_count; i++) {
        output.row_add(input.data_ptr + i*output.col_count, i);
        for (int j = offset[i] ; j < offset[i] + std::min((int)(offset[i+1] - offset[i]), sample_num) ; j++) {
            output.row_add(input.data_ptr + nebrs[j]*output.col_count, i);
        }
    }

    // float* row_ptr = output.data_ptr + i * output.col_count;
    // float* neib_ptr = input.data_ptr + nebrs[j]*output.col_count;
    // for (int k=0; k<output.col_count; k++) {
    //     row_ptr[k] = std::max(row_ptr[k], neib_ptr[k]);
    // }

}


void _graphsage(csr_t* snaph, array2d_t<float> & input, 
                array2d_t<float> & output, 
                op_t op, bool reverse, 
                bool norm /*= true*/, 
                int num_sample, 
                csr_t* sub_snaph)
{  
    sample(sub_snaph);
    if (reverse) {
        normalize_sage(input, sub_snaph, num_sample);
        apply_sage(input, output, sub_snaph, num_sample);
    } else {
        apply_sage(input, output, sub_snaph, num_sample);
        normalize_sage(output, sub_snaph, num_sample);
    }
}

void invoke_graphsage(graph_t& graph, array2d_t<float> & input_array, array2d_t<float> & output_array,
                 bool reverse, bool norm, int num_sample, graph_t& subgraph)
{
    if (reverse) {
         return _graphsage(&graph.csr, input_array, output_array, eSUM, reverse, norm, num_sample, &subgraph.csr);
    } else {
         return _graphsage(&graph.csc, input_array, output_array, eSUM, reverse, norm, num_sample, &subgraph.csc);
    }
}

// void gat(array2d_t<float> & input_array, array2d_t<float> & edge_weight, array2d_t<float> & output_array, bool reverse) {
//     for(int i=0; i<input_array.row_count; i++) {
//         for(int j=0; j<input_array.col_count; j++) {
//             int idx = input_array.col_count*i+j;
//             output_array[idx] = input_array[idx]*edge_weight[idx];
//         }
//     }

// }
// void invoke_gat(array2d_t<float> & input_array, array2d_t<float> & edge_weight, array2d_t<float> & output_array, bool reverse)
// {
//     return gat(input_array, edge_weight, output_array, reverse);
// }