// file      : XSC/SemanticGraph/Graph.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef ELEMENT_GRAPH_HPP
#define ELEMENT_GRAPH_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>

#include <iostream>

namespace XSC
{
  namespace SemanticGraph
  {
    template <typename N, typename E>
    class Graph
    {
    public:
      Graph () = default;

      ~Graph ();

    public:
      template <typename T>
      T&
      new_node ();

      template <typename T, typename A0>
      T&
      new_node (A0 const& a0);

      template <typename T, typename A0, typename A1>
      T&
      new_node (A0 const& a0, A1 const& a1);

      template <typename T, typename A0, typename A1, typename A2>
      T&
      new_node (A0 const& a0, A1 const& a1, A2 const& a2);

      void
      delete_node (N const& n);

    public:
      template <typename T, typename X, typename Y>
      T&
      new_edge (X& left, Y& right);

      template <typename T, typename X, typename Y,
                typename A0>
      T&
      new_edge (X& left, Y& right, A0 const& a0);

      template <typename T, typename X, typename Y,
                typename A0,
                typename A1>
      T&
      new_edge (X& left, Y& right, A0 const& a0, A1 const& a1);

      void
      delete_edge (E const& e);

    protected:
      using NodePtr = N*;
      using EdgePtr = E*;

      using Nodes_ = std::set<NodePtr>;
      using Edges_ = std::set<EdgePtr>;

      Nodes_ nodes_;
      Edges_ edges_;

    private:
      Graph (Graph const&) = delete;
      Graph& operator= (Graph const&) = delete;
      Graph (Graph&&) = delete;
      Graph& operator= (Graph&&) = delete;
    };
  }
}

#include "Graph.tpp"

#endif // ELEMENT_GRAPH_HPP
