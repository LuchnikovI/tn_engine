#define BOOST_TEST_MODULE Test TN Wrapper
#include <boost/test/included/unit_test.hpp>
#include <iostream>

#define private public

#include "primitives.hpp"

using namespace std;

stack<ContractionRecord> Edge::contractions_ = stack<ContractionRecord>();

BOOST_AUTO_TEST_CASE(primitives_utils)
{
  Node n1, n2, n3, n4, n5, n6;
  auto e13 = Edge(n1, n3, 5);
  auto e23 = Edge(n2, n3, 4);
  auto e34 = Edge(n3, n4, 3);
  auto e45 = Edge(n4, n5, 6);
  auto e64 = Edge(n6, n4, 7);

  double complexity34 = contraction_complexity(&e34);
  double log_dimension34 = new_log_dimension(&e34);
  BOOST_CHECK_CLOSE(complexity34, 7.83201418051, 1e-5);
  BOOST_CHECK_CLOSE(log_dimension34, 6.73340189184, 1e-5);

  Node n7;

  auto e47 = Edge(n4, n7, 8);
  auto [node_ptr1, node_ptr2] = non_descending_degree_ordering(&n4, &n3);
  auto [node_ptr3, node_ptr4] = non_descending_degree_ordering(&n3, &n4);
  BOOST_CHECK_EQUAL(node_ptr1, &n3);
  BOOST_CHECK_EQUAL(node_ptr2, &n4);
  BOOST_CHECK_EQUAL(node_ptr3, node_ptr1);
  BOOST_CHECK_EQUAL(node_ptr4, node_ptr2);

  disconnect_edge(&e34);
  auto it1 = n3.node2edge_.find(&n4);
  auto it2 = n4.node2edge_.find(&n3);
  BOOST_CHECK_EQUAL(e34.node1_ptr_, nullptr);
  BOOST_CHECK_EQUAL(e34.node2_ptr_, nullptr);
  BOOST_CHECK(it1 == n3.node2edge_.end());
  BOOST_CHECK(it2 == n4.node2edge_.end());

  move_edge(&n7, &n3, n7.node2edge_.begin());
  BOOST_CHECK((e47.node1_ptr_ == &n3) || (e47.node1_ptr_ == &n4));
  BOOST_CHECK((e47.node2_ptr_ == &n3) || (e47.node2_ptr_ == &n4));
  BOOST_CHECK(e47.node1_ptr_->node2edge_.at(e47.node2_ptr_) == &e47);
  BOOST_CHECK(e47.node2_ptr_->node2edge_.at(e47.node1_ptr_) == &e47);
}

BOOST_AUTO_TEST_CASE(primitives)
{

  // network
  Node n1, n2, n3, n4, n5, n6, n7, n8, n9;
  auto e1 = Edge(n1, n2, 2);
  auto e2 = Edge(n1, n3, 3);
  auto e3 = Edge(n4, n2, 4);
  auto e4 = Edge(n2, n5, 5);
  auto e5 = Edge(n5, n3, 6);
  auto e6 = Edge(n3, n6, 7);
  auto e7 = Edge(n4, n7, 8);
  auto e8 = Edge(n7, n5, 9);
  auto e9 = Edge(n5, n8, 10);
  auto e10 = Edge(n8, n6, 11);
  auto e11 = Edge(n7, n9, 12);
  auto e12 = Edge(n9, n8, 13);

  // partial contraction
  e1.contract();
  e5.contract();
  e4.contract();
  e10.contract();

  BOOST_CHECK(n1.is_released);
  BOOST_CHECK(n2.is_released);
  BOOST_CHECK(n3.is_released);
  BOOST_CHECK(n6.is_released);

  BOOST_CHECK_EQUAL(n5.degree_, 3u);
  BOOST_CHECK_EQUAL(n4.degree_, 2u);
  BOOST_CHECK_EQUAL(n7.degree_, 3u);
  BOOST_CHECK_EQUAL(n9.degree_, 2u);
  BOOST_CHECK_EQUAL(n8.degree_, 2u);

  BOOST_CHECK_CLOSE(n5.complexity_, 11.4576241492, 1e-5);
  BOOST_CHECK_CLOSE(n8.complexity_, 9.21133987231, 1e-5);
  BOOST_CHECK_CLOSE(n4.complexity_, -DBL_MAX, 1e-5);
  BOOST_CHECK_CLOSE(n7.complexity_, -DBL_MAX, 1e-5);
  BOOST_CHECK_CLOSE(n9.complexity_, -DBL_MAX, 1e-5);

  BOOST_CHECK_CLOSE(n5.total_log_dim_, 7.83201418051, 1e-5);
  BOOST_CHECK_CLOSE(n8.total_log_dim_, 6.81344459951, 1e-5);
  BOOST_CHECK_CLOSE(n4.total_log_dim_, 3.4657359028, 1e-5);
  BOOST_CHECK_CLOSE(n7.total_log_dim_, 6.7615727688, 1e-5);
  BOOST_CHECK_CLOSE(n9.total_log_dim_, 5.04985600725, 1e-5);

  BOOST_CHECK_EQUAL(n5.node2edge_.size(), 3u);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n4), &e3);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n7), &e8);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n8), &e9);
  BOOST_CHECK_EQUAL(n7.node2edge_.size(), 3u);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n4), &e7);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n5), &e8);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n9), &e11);
  BOOST_CHECK_EQUAL(n8.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n8.node2edge_.at(&n5), &e9);
  BOOST_CHECK_EQUAL(n8.node2edge_.at(&n9), &e12);
  BOOST_CHECK_EQUAL(n9.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n9.node2edge_.at(&n8), &e12);
  BOOST_CHECK_EQUAL(n9.node2edge_.at(&n7), &e11);
  BOOST_CHECK_EQUAL(n4.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n4.node2edge_.at(&n5), &e3);
  BOOST_CHECK_EQUAL(n4.node2edge_.at(&n7), &e7);

  BOOST_CHECK(e4.is_released);
  BOOST_CHECK(e10.is_released);
  BOOST_CHECK(e5.is_released);
  BOOST_CHECK(e1.is_released);
  BOOST_CHECK(e2.is_released);
  BOOST_CHECK(e4.is_released);
  BOOST_CHECK(e6.is_released);

  BOOST_CHECK_CLOSE(e3.log_dim_, 1.38629436112, 1e-5);
  BOOST_CHECK_CLOSE(e7.log_dim_, 2.07944154168, 1e-5);
  BOOST_CHECK_CLOSE(e8.log_dim_, 2.19722457734, 1e-5);
  BOOST_CHECK_CLOSE(e9.log_dim_, 4.24849524205, 1e-5);
  BOOST_CHECK_CLOSE(e11.log_dim_, 2.48490664979, 1e-5);
  BOOST_CHECK_CLOSE(e12.log_dim_, 2.56494935746, 1e-5);

  BOOST_CHECK(e3.node1_ptr_ == &n4 || e3.node1_ptr_ == &n5);
  BOOST_CHECK(e3.node2_ptr_ == &n4 || e3.node2_ptr_ == &n5);
  BOOST_CHECK(e3.node1_ptr_ != e3.node2_ptr_);
  BOOST_CHECK(e7.node1_ptr_ == &n4 || e7.node1_ptr_ == &n7);
  BOOST_CHECK(e7.node2_ptr_ == &n4 || e7.node2_ptr_ == &n7);
  BOOST_CHECK(e7.node1_ptr_ != e7.node2_ptr_);
  BOOST_CHECK(e8.node1_ptr_ == &n5 || e8.node1_ptr_ == &n7);
  BOOST_CHECK(e8.node2_ptr_ == &n5 || e8.node2_ptr_ == &n7);
  BOOST_CHECK(e8.node1_ptr_ != e8.node2_ptr_);
  BOOST_CHECK(e9.node1_ptr_ == &n5 || e9.node1_ptr_ == &n8);
  BOOST_CHECK(e9.node2_ptr_ == &n5 || e9.node2_ptr_ == &n8);
  BOOST_CHECK(e9.node1_ptr_ != e9.node2_ptr_);
  BOOST_CHECK(e12.node1_ptr_ == &n9 || e12.node1_ptr_ == &n8);
  BOOST_CHECK(e12.node2_ptr_ == &n9 || e12.node2_ptr_ == &n8);
  BOOST_CHECK(e12.node1_ptr_ != e12.node2_ptr_);
  BOOST_CHECK(e11.node1_ptr_ == &n9 || e11.node1_ptr_ == &n7);
  BOOST_CHECK(e11.node2_ptr_ == &n9 || e11.node2_ptr_ == &n7);
  BOOST_CHECK(e11.node1_ptr_ != e11.node2_ptr_);

  // continue until full contraction
  e11.contract();
  e9.contract();
  e8.contract();
  e3.contract();
  BOOST_CHECK_EQUAL(n5.degree_, 0u);
  BOOST_CHECK_SMALL(abs(n5.total_log_dim_), 1e-5);
  BOOST_CHECK_CLOSE(n5.complexity_, 11.9342507988, 1e-5);

  // roll back to the previouse state
  Edge::roll_back(4);

  BOOST_CHECK(n1.is_released);
  BOOST_CHECK(n2.is_released);
  BOOST_CHECK(n3.is_released);
  BOOST_CHECK(n6.is_released);

  BOOST_CHECK_EQUAL(n5.degree_, 3u);
  BOOST_CHECK_EQUAL(n4.degree_, 2u);
  BOOST_CHECK_EQUAL(n7.degree_, 3u);
  BOOST_CHECK_EQUAL(n9.degree_, 2u);
  BOOST_CHECK_EQUAL(n8.degree_, 2u);

  BOOST_CHECK_CLOSE(n5.complexity_, 11.4576241492, 1e-5);
  BOOST_CHECK_CLOSE(n8.complexity_, 9.21133987231, 1e-5);
  BOOST_CHECK_CLOSE(n4.complexity_, -DBL_MAX, 1e-5);
  BOOST_CHECK_CLOSE(n7.complexity_, -DBL_MAX, 1e-5);
  BOOST_CHECK_CLOSE(n9.complexity_, -DBL_MAX, 1e-5);

  BOOST_CHECK_CLOSE(n5.total_log_dim_, 7.83201418051, 1e-5);
  BOOST_CHECK_CLOSE(n8.total_log_dim_, 6.81344459951, 1e-5);
  BOOST_CHECK_CLOSE(n4.total_log_dim_, 3.4657359028, 1e-5);
  BOOST_CHECK_CLOSE(n7.total_log_dim_, 6.7615727688, 1e-5);
  BOOST_CHECK_CLOSE(n9.total_log_dim_, 5.04985600725, 1e-5);

  BOOST_CHECK_EQUAL(n5.node2edge_.size(), 3u);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n4), &e3);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n7), &e8);
  BOOST_CHECK_EQUAL(n5.node2edge_.at(&n8), &e9);
  BOOST_CHECK_EQUAL(n7.node2edge_.size(), 3u);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n4), &e7);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n5), &e8);
  BOOST_CHECK_EQUAL(n7.node2edge_.at(&n9), &e11);
  BOOST_CHECK_EQUAL(n8.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n8.node2edge_.at(&n5), &e9);
  BOOST_CHECK_EQUAL(n8.node2edge_.at(&n9), &e12);
  BOOST_CHECK_EQUAL(n9.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n9.node2edge_.at(&n8), &e12);
  BOOST_CHECK_EQUAL(n9.node2edge_.at(&n7), &e11);
  BOOST_CHECK_EQUAL(n4.node2edge_.size(), 2u);
  BOOST_CHECK_EQUAL(n4.node2edge_.at(&n5), &e3);
  BOOST_CHECK_EQUAL(n4.node2edge_.at(&n7), &e7);

  BOOST_CHECK(e4.is_released);
  BOOST_CHECK(e10.is_released);
  BOOST_CHECK(e5.is_released);
  BOOST_CHECK(e1.is_released);
  BOOST_CHECK(e2.is_released);
  BOOST_CHECK(e4.is_released);
  BOOST_CHECK(e6.is_released);

  BOOST_CHECK_CLOSE(e3.log_dim_, 1.38629436112, 1e-5);
  BOOST_CHECK_CLOSE(e7.log_dim_, 2.07944154168, 1e-5);
  BOOST_CHECK_CLOSE(e8.log_dim_, 2.19722457734, 1e-5);
  BOOST_CHECK_CLOSE(e9.log_dim_, 4.24849524205, 1e-5);
  BOOST_CHECK_CLOSE(e11.log_dim_, 2.48490664979, 1e-5);
  BOOST_CHECK_CLOSE(e12.log_dim_, 2.56494935746, 1e-5);

  BOOST_CHECK(e3.node1_ptr_ == &n4 || e3.node1_ptr_ == &n5);
  BOOST_CHECK(e3.node2_ptr_ == &n4 || e3.node2_ptr_ == &n5);
  BOOST_CHECK(e3.node1_ptr_ != e3.node2_ptr_);
  BOOST_CHECK(e7.node1_ptr_ == &n4 || e7.node1_ptr_ == &n7);
  BOOST_CHECK(e7.node2_ptr_ == &n4 || e7.node2_ptr_ == &n7);
  BOOST_CHECK(e7.node1_ptr_ != e7.node2_ptr_);
  BOOST_CHECK(e8.node1_ptr_ == &n5 || e8.node1_ptr_ == &n7);
  BOOST_CHECK(e8.node2_ptr_ == &n5 || e8.node2_ptr_ == &n7);
  BOOST_CHECK(e8.node1_ptr_ != e8.node2_ptr_);
  BOOST_CHECK(e9.node1_ptr_ == &n5 || e9.node1_ptr_ == &n8);
  BOOST_CHECK(e9.node2_ptr_ == &n5 || e9.node2_ptr_ == &n8);
  BOOST_CHECK(e9.node1_ptr_ != e9.node2_ptr_);
  BOOST_CHECK(e12.node1_ptr_ == &n9 || e12.node1_ptr_ == &n8);
  BOOST_CHECK(e12.node2_ptr_ == &n9 || e12.node2_ptr_ == &n8);
  BOOST_CHECK(e12.node1_ptr_ != e12.node2_ptr_);
  BOOST_CHECK(e11.node1_ptr_ == &n9 || e11.node1_ptr_ == &n7);
  BOOST_CHECK(e11.node2_ptr_ == &n9 || e11.node2_ptr_ == &n7);
  BOOST_CHECK(e11.node1_ptr_ != e11.node2_ptr_);
}