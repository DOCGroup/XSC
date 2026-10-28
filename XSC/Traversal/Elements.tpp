// file      : XSC/Traversal/Elements.tpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef XSC_ELEMENTS_TPP
#define XSC_ELEMENTS_TPP
namespace XSC
{
  namespace Traversal
  {
    // Traverser

    template<typename T>
    Traverser<T>::
    ~Traverser () = default;

    // Dispatcher

    template <typename T>
    Dispatcher<T>::
    ~Dispatcher () = default;

    template <typename T>
    void Dispatcher<T>::
    dispatch (T& n)
    {
      LevelMap levels;

      unsigned long max = compute_levels (n.type_info (), 0, levels);

      for (unsigned long l = 0; l < max + 1; ++l)
      {
        TypeInfoSet dispatched;

        for (auto const& [type_info, level] : levels)
        {
          if (level == l)
          {
            typename TraversalMap::const_iterator v (
              traversal_map_.find (type_info.type_id ()));

            if (v != traversal_map_.end ())
            {
              Traversers const& traversers (v->second);
              for (Traverser<T>* traverser : traversers)
              {
                traverser->trampoline (n);
              }

              flatten_tree (type_info, dispatched);
            }
          }
        }

        // Remove traversed types from level map.
        for (Introspection::TypeInfo const& type_info : dispatched)
        {
          levels.erase (type_info);
        }
      }
    }

    template <typename T>
    unsigned long Dispatcher<T>::
    compute_levels (Introspection::TypeInfo const& ti,
                    unsigned long cur,
                    LevelMap& map)
    {
      unsigned long ret = cur;

      if (map.find (ti) == map.end () || map[ti] < cur) map[ti] = cur;

      for (Introspection::TypeInfo::BaseIterator i = ti.begin_base ();
           i != ti.end_base ();
           i++)
      {
        unsigned long t = compute_levels (i->type_info (), cur + 1, map);
        if (t > ret) ret = t;
      }

      return ret;
    }

    template <typename T>
    void Dispatcher<T>::
    flatten_tree (Introspection::TypeInfo const& ti, TypeInfoSet& set)
    {
      set.insert (ti);

      for (Introspection::TypeInfo::BaseIterator i = ti.begin_base ();
           i != ti.end_base ();
           i++)
      {
        flatten_tree (i->type_info (), set);
      }
    }
  }
}

#endif /* XSC_ELEMENTS_TPP */
