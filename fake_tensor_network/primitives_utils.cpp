#include "primitives_utils.hpp"
#include "primitives.hpp"

using namespace std;

double log_sum_exp (double x, double y, double z)
{
  double m = max(max(x, y), z);
  return log(exp(x - m) + exp(y - m) + exp(z - m)) + m;
}

pair<Node*, Node*> non_descending_degree_ordering (Node* lhs, Node* rhs)
{
  if (lhs->degree_ > rhs->degree_)
  {
    return {rhs, lhs};
  }
  else
  {
    return {lhs, rhs};
  }
}

void disconnect_edge (Edge* edge)
{
  edge->node1_ptr_->node2edge_.erase(edge->node2_ptr_);
  edge->node2_ptr_->node2edge_.erase(edge->node1_ptr_);
  --(edge->node1_ptr_->degree_);
  --(edge->node2_ptr_->degree_);
  edge->node1_ptr_ = nullptr;
  edge->node2_ptr_ = nullptr;
  edge->is_released = true;
}

void move_edge (Node* from_node, Node* to_node, unordered_map<Node*, Edge*>::iterator it)
{
  auto moved_edge = from_node->node2edge_.extract(it);
  Node* node1_ptr = moved_edge.mapped()->node1_ptr_;
  Node* node2_ptr = moved_edge.mapped()->node2_ptr_;
  if (node1_ptr != from_node)
  {
    auto updated_one = node1_ptr->node2edge_.extract(from_node);
    updated_one.key() = to_node;
    node1_ptr->node2edge_.insert(move(updated_one));
    moved_edge.mapped()->node2_ptr_ = to_node;
  }
  else
  {
    auto updated_one = node2_ptr->node2edge_.extract(from_node);
    updated_one.key() = to_node;
    node2_ptr->node2edge_.insert(move(updated_one));
    moved_edge.mapped()->node1_ptr_ = to_node;
  }
  to_node->node2edge_.insert(move(moved_edge));
  --(from_node->degree_);
  ++(to_node->degree_);
}

double contraction_complexity (const Edge* edge)
{
  return edge->node1_ptr_->total_log_dim_ + edge->node2_ptr_->total_log_dim_ - edge->log_dim_;
}

double new_log_dimension (const Edge* edge)
{
  return edge->node1_ptr_->total_log_dim_ + edge->node2_ptr_->total_log_dim_ - 2 * edge->log_dim_;
}

void connect_edge (Node* lhs, Node* rhs, Edge* edge)
{
  ++lhs->degree_;
  ++rhs->degree_;
  edge->node1_ptr_ = lhs;
  edge->node2_ptr_ = rhs;
  lhs->node2edge_[rhs] = edge;
  rhs->node2edge_[lhs] = edge;
  edge->is_released = false;
}