#include "primitives.hpp"
#include "primitives_utils.hpp"

using namespace std;

double Node::get_complexity () const
{
  return complexity_;
}

double Node::get_total_log_dim () const
{
  return total_log_dim_;
}

uint32_t Node::get_degree () const
{
  return degree_;
}

void Edge::roll_back (size_t n)
{
  for (size_t i = 0; i < n; ++i)
  {
    one_step_roll_back();
  }
}

Edge::Edge (Node& lhs, Node& rhs, uint32_t dim) 
: log_dim_(log(dim)), is_released(false)
{
  auto [it, flag] = lhs.node2edge_.try_emplace(&rhs, this);
  if (!flag)
  {
    throw logic_error("Nodes are already connected");
  }
  rhs.node2edge_[&lhs] = this;
  node1_ptr_ = &lhs;
  node2_ptr_ = &rhs;
  node1_ptr_->total_log_dim_ += log_dim_;
  node2_ptr_->total_log_dim_ += log_dim_;
  ++(node1_ptr_->degree_);
  ++(node2_ptr_->degree_);
}

double Edge::get_log_dimension() const
{
  return log_dim_;
}

void Edge::contract ()
{
  if (!(this->node1_ptr_) || !(this->node2_ptr_))
  {
    throw logic_error("Edge is not connected to nodes");
  }

  auto [released_node_ptr, aggregator_node_ptr] = non_descending_degree_ordering(node1_ptr_, node2_ptr_);

  released_node_ptr->is_released = true;

  // make a record
  auto aggregated_edges = released_node_ptr->node2edge_;
  aggregated_edges.erase(aggregator_node_ptr);
  contractions_.emplace(
    RecordBuilder().set_released_node(released_node_ptr)
                   .set_aggregator_node(aggregator_node_ptr)
                   .set_contracted_edge(this)
                   .set_aggregated_edges(move(aggregated_edges))
                   .set_log_dim(aggregator_node_ptr->total_log_dim_)
                   .set_complexity(aggregator_node_ptr->complexity_)
                   .build()
  );

  // update complexity and dimension
  aggregator_node_ptr->complexity_ = log_sum_exp(contraction_complexity(this),
                                                 aggregator_node_ptr->complexity_,
                                                 released_node_ptr->complexity_);
  aggregator_node_ptr->total_log_dim_ = new_log_dimension(this);

  // release this edge
  disconnect_edge(this);

  // move edges to the aggregator node
  auto it = released_node_ptr->node2edge_.begin();
  while (it != released_node_ptr->node2edge_.end())
  {
    auto found_elem = aggregator_node_ptr->node2edge_.find(it->first);
    if (found_elem ==  aggregator_node_ptr->node2edge_.end())
    {
      auto next_it = next(it);
      move_edge(released_node_ptr, aggregator_node_ptr, it); // this invalidates iterator
      it = next_it;
    }
    else
    {
      auto next_it = next(it);
      found_elem->second->log_dim_ += it->second->log_dim_;
      disconnect_edge(it->second); // this invalidates iterator
      it = next_it;
    }
  }
}

void Edge::one_step_roll_back ()
{
  if (Edge::contractions_.empty())
  {
    throw logic_error("Network is in its initial state");
  }
  const ContractionRecord& record = contractions_.top();

  record.released_node_ptr->is_released = false;

  // reconstruct back a contracted edge
  connect_edge(record.released_node_ptr, record.aggregator_node_ptr, record.contracted_edge_ptr);

  // reconstruct back parameters of the aggregator node
  //record.aggregator_node_ptr->degree_ = record.degree;
  record.aggregator_node_ptr->total_log_dim_ = record.log_dim;
  record.aggregator_node_ptr->complexity_ = record.complexity;

  // move edges back to the released node
  for (auto it = record.aggregated_edges.begin(); it != record.aggregated_edges.end(); ++it)
  {
    auto found_one = record.aggregator_node_ptr->node2edge_.find(it->first);
    if (found_one->second != it->second)
    {
      connect_edge(record.released_node_ptr, it->first, it->second);
      Edge* parallel_edge = record.aggregator_node_ptr->node2edge_.at(it->first);
      parallel_edge->log_dim_ -= it->second->log_dim_;
    }
    else
    {
      move_edge(record.aggregator_node_ptr, record.released_node_ptr, found_one);
    }
  }

  // roll back stack with contractions
  contractions_.pop();
}
