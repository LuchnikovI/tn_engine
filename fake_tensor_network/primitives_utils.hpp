#pragma once

#include <tuple>
#include <unordered_map>

class Node;
class Edge;

double log_sum_exp (double x, double y, double z);

std::pair<Node*, Node*> non_descending_degree_ordering (Node* lhs, Node* rhs);

void disconnect_edge (Edge* edge);

void move_edge (Node* from_node, Node* to_node, std::unordered_map<Node*, Edge*>::iterator it);

double contraction_complexity (const Edge* edge);

double new_log_dimension (const Edge* edge);

void connect_edge (Node* lhs, Node* rhs, Edge* edge);
