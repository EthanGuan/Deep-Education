#pragma once

#include "csr.h"
#include "op.h"

extern int THD_COUNT;
    
void invoke_gspmm(graph_t& graph, array2d_t<float> & input, array2d_t<float> & output, 
                 bool reverse, bool norm);

void invoke_graphsage(graph_t& graph, array2d_t<float> & input, array2d_t<float> & output,
                 bool reverse, bool norm, int num_sample, graph_t& subgraph);