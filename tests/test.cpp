#define BOOST_TEST_MODULE Test TN Wrapper
#include <vector>
#include <boost/test/included/unit_test.hpp>

#include "../primitives.hpp"
#include "../tensors.hpp"
#include "access.hpp"

using namespace std;
using Access = TestAccess<FakeTensor>;

// TODO: add tests to check correctness of exceptions

BOOST_AUTO_TEST_CASE(parallel_edges_correctenss)
{
  Node<FakeTensor> n1({{2, 3, 4, 5}});
  Node<FakeTensor> n2({{3, 4, 5, 6, 7}});
  Node<FakeTensor> n3({{6, 7, 8}});
  Node<FakeTensor> n4({{8, 9}});
  Edge<FakeTensor> e1 = n1[1] ^ n2[0];
  Edge<FakeTensor> e2 = n1[2] ^ n2[1];
  Edge<FakeTensor> e3 = n1[3] ^ n2[2];
  Edge<FakeTensor> e4 = n2[3] ^ n3[0];
  Edge<FakeTensor> e5 = n2[4] ^ n3[1];
  Edge<FakeTensor> e6 = n3[2] ^ n4[0];

  {
    std::vector<Edge<FakeTensor>*> parallel = Access::get_parallel_edges(e1);
    BOOST_CHECK(parallel.size() == 3);
    BOOST_CHECK((parallel[0] == &e1) || (parallel[1] == &e1) || (parallel[2] == &e1));
    BOOST_CHECK((parallel[0] == &e2) || (parallel[1] == &e2) || (parallel[2] == &e2));
    BOOST_CHECK((parallel[0] == &e3) || (parallel[1] == &e3) || (parallel[2] == &e3));
  }

  {
    auto parallel = Access::get_parallel_edges(e5);
    BOOST_CHECK(parallel.size() == 2);
    BOOST_CHECK((parallel[0] == &e4) || (parallel[1] == &e4));
    BOOST_CHECK((parallel[0] == &e5) || (parallel[1] == &e5));
  }

  {
    auto parallel = Access::get_parallel_edges(e6);
    BOOST_CHECK(parallel.size() == 1);
    BOOST_CHECK(parallel[0] == &e6);
  }
}

BOOST_AUTO_TEST_CASE(fake_tensor_correctness)
{
  FakeTensor t1 = {{2, 6, 5, 4, 3}};
  FakeTensor t2 = {{5, 8, 6, 7}};
  Axes axes = {{1, 2}, {2, 0}};
  FakeTensor t3 = tensordot(move(t1), move(t2), move(axes));
  BOOST_CHECK(t3.past_complexity == 40320);
  BOOST_CHECK(t3.shape == std::vector<std::uint32_t>({2, 4, 3, 8, 7}));
}

BOOST_AUTO_TEST_CASE(small_tn_contruction_correctness)
{
  Node<FakeTensor> n1({{2, 3, 4}});
  Node<FakeTensor> n2({{4, 5}});
  Node<FakeTensor> n3({{6}});
  Node<FakeTensor> n4({{5, 6, 7, 8}});
  Node<FakeTensor> n5({{3, 7, 9, 10}});
  Edge<FakeTensor> e1 = n1[2] ^ n2[0];
  Edge<FakeTensor> e2 = n2[1] ^ n4[0];
  Edge<FakeTensor> e3 = n4[1] ^ n3[0];
  Edge<FakeTensor> e4 = n1[1] ^ n5[0];
  Edge<FakeTensor> e5 = n5[1] ^ n4[2];

  auto [n6, parallel1] = e1.contract();
  BOOST_CHECK(parallel1.size() == 1);
  BOOST_CHECK(parallel1[0] == &e1);
  BOOST_CHECK(n6->get_shape() == Shape({2, 3, 5}));
  BOOST_CHECK(n6->get_tensor().past_complexity == 120);

  auto [n7, parallel2] = e2.contract();
  BOOST_CHECK(parallel2.size() == 1);
  BOOST_CHECK(parallel2[0] == &e2);
  BOOST_CHECK(n7->get_shape() == Shape({2, 3, 6, 7, 8}));
  BOOST_CHECK(n7->get_tensor().past_complexity == 10200);

  auto [n8, parallel3] = e3.contract();
  BOOST_CHECK(parallel3.size() == 1);
  BOOST_CHECK(parallel3[0] == &e3);
  BOOST_CHECK(n8->get_shape() == Shape({2, 3, 7, 8}));
  BOOST_CHECK(n8->get_tensor().past_complexity == 12216);

  auto [n9, parallel4] = e5.contract();
  BOOST_CHECK(parallel4.size() == 2);
  BOOST_CHECK((parallel4[0] == &e4) || (parallel4[1] == &e4));
  BOOST_CHECK((parallel4[0] == &e5) || (parallel4[1] == &e5));
  BOOST_CHECK(n9->get_shape() == Shape({9, 10, 2, 8}));
  BOOST_CHECK(n9->get_tensor().past_complexity == 42456);
}