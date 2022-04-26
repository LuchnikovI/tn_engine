#include "contraction_record.hpp"

using namespace std;

RecordBuilder& RecordBuilder::set_released_node (Node* ptr)
{
  record_.released_node_ptr = ptr;
  released_node_filled_ = true;
  return *this;
}

RecordBuilder& RecordBuilder::set_aggregator_node (Node* ptr)
{
  record_.aggregator_node_ptr = ptr;
  aggregator_node_filled_ = true;
  return *this;
}

RecordBuilder& RecordBuilder::set_contracted_edge (Edge* ptr)
{
  record_.contracted_edge_ptr = ptr;
  contracted_edge_filled_ = true;
  return *this;
}

RecordBuilder& RecordBuilder::set_aggregated_edges (unordered_map<Node*, Edge*> node2edge)
{
  record_.aggregated_edges = move(node2edge);
  aggregated_edges_filled_ = true;
  return *this;
}

RecordBuilder& RecordBuilder::set_log_dim (double log_dim)
{
  record_.log_dim = log_dim;
  log_dim_filled_ = true;
  return *this;
}

RecordBuilder& RecordBuilder::set_complexity (double complexity)
{
  record_.complexity = complexity;
  complexity_filled_ = true;
  return *this;
}

ContractionRecord RecordBuilder::build()
{
  if (released_node_filled_ && aggregator_node_filled_ && contracted_edge_filled_ &&
      aggregated_edges_filled_ && log_dim_filled_ && complexity_filled_)
  {
    return move(record_);
  }
  else
  {
    throw logic_error("Record is not filled properly");
  }
}
