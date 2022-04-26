#pragma once

#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>
#include <stack>
#include <cfloat>

#include "contraction_record.hpp"

class Node;
class Edge;
//using ContractionOutput = std::pair<Node*, std::vector<Edge*>>;
//using RollBackOutput = std::pair<Node*, std::vector<Edge*>>;

class Node {
friend class Edge;
friend std::pair<Node*, Node*> non_descending_degree_ordering (Node* lhs, Node* rhs);
friend void disconnect_edge (Edge* edge);
friend void move_edge (Node* from_node, Node* to_node, std::unordered_map<Node*, Edge*>::iterator it);
friend double contraction_complexity (const Edge* edge);
friend double new_log_dimension (const Edge* edge);
friend void connect_edge (Node* lhs, Node* ths, Edge* edge);
public:
  Node () : total_log_dim_(0), complexity_(-DBL_MAX), degree_(0), is_released(false) {}
  Node (Node&&) = delete;
  Node (const Node&) = delete;
  Node& operator= (Node&&) = delete;
  Node& operator= (const Node&) = delete;
  double get_complexity () const;
  double get_total_log_dim () const;
  std::uint32_t get_degree () const;
private:
  std::unordered_map<Node*, Edge*> node2edge_;
  double total_log_dim_;
  double complexity_;
  std::uint32_t degree_;
  bool is_released;
};

class Edge {
friend class Node;
friend void disconnect_edge (Edge* edge);
friend void move_edge (Node* from_node, Node* to_node, std::unordered_map<Node*, Edge*>::iterator it);
friend double contraction_complexity (const Edge* edge);
friend double new_log_dimension (const Edge* edge);
friend void connect_edge (Node* lhs, Node* ths, Edge* edge);
public:
  Edge(Node& lhs, Node& rhs, std::uint32_t dim);
  Edge (Edge&&) = delete;
  Edge (const Edge&) = delete;
  Edge& operator= (Edge&&) = delete;
  Edge& operator= (const Edge&) = delete;
  void contract ();
  double get_log_dimension() const;
  static void roll_back (size_t n);
private:
  static void one_step_roll_back ();
private:
  Node* node1_ptr_;
  Node* node2_ptr_;
  double log_dim_;
  static std::stack<ContractionRecord> contractions_;
  bool is_released;
};
