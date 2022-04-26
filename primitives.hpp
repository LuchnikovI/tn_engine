#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "tensors_utils.hpp"

template<typename Tensor>
class Node;
template<typename Tensor>
class Edge;

using Shape = std::vector<std::uint32_t>;

template<typename Tensor>
class IndexView {
template<typename T>
friend struct TestAccess;  // for testing purposes
friend class Node<Tensor>;
friend class Edge<Tensor>;
public:
  IndexView (const IndexView&) = delete;
  IndexView (IndexView&&) = delete;
  IndexView& operator= (const IndexView&) = delete;
  IndexView& operator= (IndexView&&) = delete;
  Edge<Tensor> operator^ (IndexView<Tensor>& other);
  IndexView (Node<Tensor>& node, std::uint8_t index, std::uint32_t dim);
private:
  Node<Tensor>* node_;
  std::uint8_t index_;
  const std::uint32_t dim_;
  Edge<Tensor>* edge_;
};

template<typename Tensor>
using Indices = std::vector<std::unique_ptr<IndexView<Tensor>>>;

template<typename Tensor>
class Node {
template<typename T>
friend struct TestAccess;  // for testing purposes
friend class Edge<Tensor>;
friend class IndexView<Tensor>;
public:
  Node (Tensor tensor);
  Node (const Node&) = delete;
  Node (Node&&) = delete;
  Node& operator= (const Node&) = delete;
  Node& operator= (Node&&) = delete;
  IndexView<Tensor>& operator[] (std::uint8_t key);
  const Shape& get_shape() const;
  const Tensor& get_tensor () const;
private:
  Tensor tensor_;
  Indices<Tensor> indices_;
  Shape shape_;
};

template<typename Tensor>
using ContractionOutput = std::pair<std::unique_ptr<Node<Tensor>>, std::vector<Edge<Tensor>*>>;

template<typename Tensor>
class Edge {
template<typename T>
friend struct TestAccess;  // for testing purposes
friend class Node<Tensor>;
friend class IndexView<Tensor>;
public:
  Edge (IndexView<Tensor>& index1, IndexView<Tensor>& index2);
  Edge (const Edge&) = delete;
  Edge (Edge&&) = delete;
  Edge& operator= (const Edge&) = delete;
  Edge& operator= (Edge&&) = delete;
  ContractionOutput<Tensor> contract ();
private:
  std::vector<Edge<Tensor>*> get_parallel ();
private:
  const IndexView<Tensor>& index1_;
  const IndexView<Tensor>& index2_;
};

// ----------------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------------

template<typename Tensor>
IndexView<Tensor>::IndexView (Node<Tensor>& node, std::uint8_t index, std::uint32_t dim)
: node_(&node), index_(index), dim_(dim), edge_(nullptr)
{
}

template<typename Tensor>
Edge<Tensor> IndexView<Tensor>::operator^ (IndexView<Tensor>& other)
{
  return {*this, other};
}

template<typename Tensor>
Node<Tensor>::Node (Tensor tensor) 
: tensor_(std::move(tensor)), shape_(tensor_.shape)
{
  indices_.reserve(tensor_.shape.size());
  for (uint32_t i = 0; i < tensor_.shape.size(); ++i)
  {
    indices_.push_back(std::make_unique<IndexView<Tensor>>(*this, i, tensor_.shape[i]));
  }
}

template<typename Tensor>
IndexView<Tensor>& Node<Tensor>::operator[] (std::uint8_t key)
{
  return *indices_[key];
}

template<typename Tensor>
Edge<Tensor>::Edge (IndexView<Tensor>& index1, IndexView<Tensor>& index2)
: index1_(index1), index2_(index2)
{
  if (index1.edge_)
    throw std::logic_error("First index is already connected to some edge");
  if (index2.edge_)
    throw std::logic_error("Second index is already connected to some edge");
  if (index1.dim_ != index2.dim_)
    throw std::logic_error("Indices have different dimensions");
  index1.edge_ = this;
  index2.edge_ = this;
}

template<typename Tensor>
std::vector<Edge<Tensor>*> Edge<Tensor>::get_parallel ()
{
  const auto& start_indices = index1_.node_->indices_;
  const auto destination_node_ptr = index2_.node_;
  std::vector<Edge<Tensor>*> parallel_edges;
  parallel_edges.reserve(start_indices.size());
  for (auto it = start_indices.begin(); it < start_indices.end(); ++it)
  {
    if ((*it)->edge_)
    {
      if (   (*it)->edge_->index1_.node_ == destination_node_ptr 
          || (*it)->edge_->index2_.node_ == destination_node_ptr)
      {
        parallel_edges.push_back((*it)->edge_);
      }
    }
  }
  return parallel_edges;
}

template <typename Tensor>
ContractionOutput<Tensor> Edge<Tensor>::contract ()
{
  std::vector<Edge<Tensor>*> parallel = get_parallel();
  std::vector<std::uint32_t> axes_lhs;
  std::vector<std::uint32_t> axes_rhs;
  axes_lhs.reserve(parallel.size());
  axes_rhs.reserve(parallel.size());
  for (auto edge_ptr : parallel)
  {
    if (index1_.node_ == edge_ptr->index1_.node_)
    {
      axes_lhs.push_back(edge_ptr->index1_.index_);
      axes_rhs.push_back(edge_ptr->index2_.index_);
    }
    else
    {
      axes_lhs.push_back(edge_ptr->index2_.index_);
      axes_rhs.push_back(edge_ptr->index1_.index_);
    }
  }
  Axes axes = {axes_lhs, axes_rhs};
  Tensor new_tensor = tensordot(std::move(index1_.node_->tensor_), std::move(index2_.node_->tensor_), axes);
  Indices<Tensor> new_indices = attrs_dot(index1_.node_->indices_, index2_.node_->indices_, std::move(axes));
  auto new_node = std::make_unique<Node<Tensor>>(std::move(new_tensor));
  new_node->indices_ = std::move(new_indices);
  for (size_t i = 0; i < new_node->indices_.size(); ++i)
  {
    new_node->indices_[i]->node_ = new_node.get();
    new_node->indices_[i]->index_ = i;
  }
  return {std::move(new_node), std::move(parallel)};
}

template<typename Tensor>
const Shape& Node<Tensor>::get_shape() const
{
  return shape_;
}

template<typename Tensor>
const Tensor& Node<Tensor>::get_tensor () const
{
  return tensor_;
}
