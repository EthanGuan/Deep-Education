import torch as th
import gp_apis


class GSpmm(th.autograd.Function):
    @staticmethod
    def forward(ctx, graph, X, norm, num_vcount, dim):
        res = gp_apis.gp_gspmm(graph, X, num_vcount, dim, 0, norm)  # do not specify the reduce operation
        ctx.backward_cache = graph, norm, num_vcount, dim
        return res

    @staticmethod
    def backward(ctx, dZ):
        graph, norm, num_vcount, dim = ctx.backward_cache
        res = gp_apis.gp_gspmm(graph, dZ, num_vcount, dim, 1, norm)  # do not specify the reduce operation
        #print(th.sum(dZ-res))
        return None, res, None, None, None

# the gspmv has only 1 input, and then apply different operations such as sum, max on it
def run_gspmm(graph, X, norm, num_vcount, dim):
    return GSpmm.apply(graph, X, norm, num_vcount, dim)

class GSAGE(th.autograd.Function):
    @staticmethod
    def forward(ctx, graph, X, norm, num_vcount, dim, sample_num, subgraph):
        res = gp_apis.gp_graphsage(graph, X, num_vcount, dim, 0, norm, sample_num, subgraph)  # do not specify the reduce operation
        ctx.backward_cache = graph, norm, num_vcount, dim, sample_num, subgraph
        return res

    @staticmethod
    def backward(ctx, dZ):
        graph, norm, num_vcount, dim, sample_num, subgraph = ctx.backward_cache
        res = gp_apis.gp_graphsage(graph, dZ, num_vcount, dim, 1, norm, sample_num, subgraph)  # do not specify the reduce operation
        
        return None, res, None, None, None, None, None

def run_graphsage(graph, X, norm, num_vcount, dim, sample_num, subgraph):
    # from torch.autograd import gradcheck
    # func = GSAGE.apply
    # test = gradcheck(func, (graph, X, norm, num_vcount, dim, sample_num, subgraph), eps=1e-6, atol=1e-4)
    # print(test)
    return GSAGE.apply(graph, X, norm, num_vcount, dim, sample_num, subgraph)


