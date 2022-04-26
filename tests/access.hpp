#include "../primitives.hpp"

template<typename Tensor>
struct TestAccess {
  static std::vector<Edge<Tensor>*> get_edges(const Node<Tensor>& node)
  {
    std::vector<Edge<Tensor>*> edges_ptrs;
    edges_ptrs.reserve(node.indices_.size());
    for (size_t i = 0; i < node.indices_.size(); ++i)
    {
      edges_ptrs.push_back(node.indices_[i]->edge_);
    }
    return edges_ptrs;
  }

  static std::pair<const IndexView<Tensor>&, const IndexView<Tensor>&> get_indices (const Edge<Tensor>& edge)
  {
    return {edge.index1_, edge.index2_};
  }
  static std::vector<Edge<Tensor>*> get_parallel_edges(Edge<Tensor>& edge)
  {
    return edge.get_parallel();
  }
};