import torch as th
import torch.utils.dlpack
import kernel as gpk
import datetime

def gp_gspmm(g, X, dim0, dim1, inverse, norm):
    X_dl = th.utils.dlpack.to_dlpack(X)

    # declare the output tensor here
    res = th.zeros(dim0, dim1)
    res_dl = th.utils.dlpack.to_dlpack(res)

    gpk.gspmm(g, X_dl, res_dl, inverse, norm)  # do not specify the reduce operation

    return res

def gp_graphsage(g, X, dim0, dim1, inverse, norm, sample_num, subgraph):
    X_dl = th.utils.dlpack.to_dlpack(X)

    # declare the output tensor here
    res = th.zeros(dim0, dim1)
    res_dl = th.utils.dlpack.to_dlpack(res)

    gpk.gsage(g, X_dl, res_dl, inverse, norm, sample_num, subgraph)  # do not specify the reduce operation

    return res