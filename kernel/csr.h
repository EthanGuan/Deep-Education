#pragma once

#include "stdint.h"
using namespace std;

#ifdef B64
typedef uint64_t vid_t;
#elif B32
typedef uint32_t vid_t;
#endif

class csr_t {
 public:
    vid_t  v_count; //This is actual vcount in a graph 
    vid_t  e_count;
    vid_t  dst_size;
    vid_t* offset;
    //char* nebrs;
    vid_t* nebrs;
    int*  degrees;
    int64_t flag;

 public:
    csr_t() {};
    csr_t(const csr_t & csr_object) {
        v_count = csr_object.v_count;
        dst_size = csr_object.dst_size; //
        e_count = csr_object.v_count;
        flag = csr_object.flag;   

        for(int i=0;i<v_count+1;i++) {
            offset[i] = csr_object.offset[i];
        }

        for(int i=0;i<offset[v_count];i++) {
            nebrs[i] = csr_object.nebrs[i];
        }
    };

    void init(vid_t a_vcount, vid_t a_dstsize, void* a_offset, void* a_nebrs, int64_t a_flag, vid_t edge_count) {
        v_count = a_vcount;
        dst_size = a_dstsize;
        offset = (vid_t*)a_offset;
        nebrs = (vid_t*)a_nebrs;
        e_count = offset[edge_count];
        flag = a_flag;
    }
    vid_t get_vcount() {
        return v_count;
    }
    vid_t get_ecount() {
        return e_count;
    }
    vid_t get_degree(vid_t index) {
        return offset[index + 1] - offset[index];
    }
};

class edge_t {
 public:
     vid_t src;
     vid_t dst;
     //edge properties here if any.
};

class coo_t {
 public:
     edge_t* edges;
     vid_t dst_size;
     vid_t v_count;
     vid_t e_count;
     coo_t() {
         edges = 0;
         dst_size = 0;
         v_count = 0;
         e_count = 0;
     }
     void init(vid_t a_vcount, vid_t a_dstsize, vid_t a_ecount, edge_t* a_edges) {
     }
};

class graph_t {
 public:
    csr_t csr;
    csr_t csc;
    coo_t coo;
 public:
    graph_t() {}
    graph_t(const graph_t& graph) {
        csr = graph.csr;
        csc = graph.csc;
    }

    void init(vid_t a_vcount, vid_t a_dstsize, void* a_offset, void* a_nebrs, void* a_offset1, void* a_nebrs1, int64_t flag, int64_t num_vcount) {
        csr.init(a_vcount, a_dstsize, a_offset, a_nebrs, flag, num_vcount);
        csc.init(a_vcount, a_dstsize, a_offset1, a_nebrs1, flag, num_vcount);
    }

    vid_t get_vcount() {
        return csr.v_count;
    }
    vid_t get_edge_count() {
        return csr.e_count;
    }
};

