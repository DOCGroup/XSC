// file      : XSC/SemanticGraph/Context.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_SEMANTIC_GRAPH_CONTEXT_HPP
#define XSC_SEMANTIC_GRAPH_CONTEXT_HPP

#include <any>
#include <map>
#include <string>

namespace XSC
{
  namespace SemanticGraph
  {
    class Context
    {
      using Map = std::map<std::string, std::any>;

    public:
      Context () = default;

    public:
      class NotFound {};
      class Typing {};

      Map::size_type
      count (char const* key) const noexcept
      {
        return map_.count (key);
      }

      template <typename T>
      T const&
      get (char const* key) const
      {
        Map::const_iterator i = map_.find (key);
        if (i == map_.end ()) throw NotFound ();

        try
        {
          return std::any_cast<T const&> (i->second);
        }
        catch (std::bad_any_cast const&)
        {
          throw Typing ();
        }

      }

      template <typename T>
      T const&
      get (char const* key, T const& def)
      {
        Map::const_iterator i = map_.find (key);
        if (i == map_.end ()) return def;

        try
        {
          return std::any_cast<T const&> (i->second);
        }
        catch (std::bad_any_cast const&)
        {
          throw Typing ();
        }
      }

      template <typename T>
      void
      set (char const* key, T const& value)
      {
        try
        {
          if (!map_.emplace (key, value).second)
          {
            Map::iterator i = map_.find (key);
            std::any_cast<T&> (i->second) = value;
          }
        }
        catch (std::bad_any_cast const&)
        {
          throw Typing ();
        }
      }

      void
      remove (char const* key)
      {
        Map::iterator i (map_.find (key));

        if (i == map_.end ()) throw NotFound ();

        map_.erase (i);
      }

    private:
      Map map_;

    private:
      // Copy and move semantics are not supported.
      Context (Context const&) = delete;
      Context& operator= (Context const&) = delete;
      Context (Context&&) = delete;
      Context& operator= (Context&&) = delete;
    };
  }
}

#endif  // XSC_SEMANTIC_GRAPH_CONTEXT_HPP
