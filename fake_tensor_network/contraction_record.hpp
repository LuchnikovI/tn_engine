#pragma once

#include <unordered_map>
#include <stdexcept>

class Node;
class Edge;

struct ContractionRecord {
  ContractionRecord () : released_node_ptr(nullptr), 
  aggregator_node_ptr(nullptr), contracted_edge_ptr(nullptr),
  log_dim(-1), complexity(-1), degree(-1) {}

  Node* released_node_ptr;
  Node* aggregator_node_ptr;
  Edge* contracted_edge_ptr;
  std::unordered_map<Node*, Edge*> aggregated_edges;
  double log_dim;
  double complexity;
  int degree;
};

class RecordBuilder {
public:
  RecordBuilder () : released_node_filled_(false), aggregator_node_filled_(false),
  contracted_edge_filled_(false), aggregated_edges_filled_(false), log_dim_filled_(false),
  complexity_filled_(false) {}
  RecordBuilder& set_released_node (Node* ptr);
  RecordBuilder& set_aggregator_node (Node* ptr);
  RecordBuilder& set_contracted_edge (Edge* edge);
  RecordBuilder& set_aggregated_edges (std::unordered_map<Node*, Edge*> node2edge);
  RecordBuilder& set_log_dim (double log_dim);
  RecordBuilder& set_complexity (double complexity);
  ContractionRecord build();
private:
  ContractionRecord record_;
  bool released_node_filled_;
  bool aggregator_node_filled_;
  bool contracted_edge_filled_;
  bool aggregated_edges_filled_;
  bool log_dim_filled_;
  bool complexity_filled_;
};
