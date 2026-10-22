// file      : CCF/CodeGenerationKit/IndentationBuffer.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CCF_CODE_GENERATION_KIT_INDENTATION_BUFFER_HPP
#define CCF_CODE_GENERATION_KIT_INDENTATION_BUFFER_HPP

#include "CCF/CompilerElements/ExH.hpp"

#include <string>

namespace Indentation
{
  template <typename C>
  class Buffer
  {
  public:

    using Exception = ExH::Logic::DescriptiveException;

    class EndOfStream_ {};
    using EndOfStream = ExH::Compound<EndOfStream_, Exception>;

  public:
    virtual
    ~Buffer () { }

  public:
    using traits_type = std::char_traits<C>;

    using char_type = typename traits_type::char_type;

    using int_type = typename traits_type::int_type;

  public:
    virtual int_type
    put (char_type c) = 0;

    // Unbuffer flushes internal formatting buffers (if any).
    // Note that unbuffer is not exactly flushing since it can
    // result in formatting errors and in general can not be
    // called at arbitrary points. Natural use case would be
    // to call unbuffer at the end of the stream when no more
    // data is expected.
    //
    virtual void
    unbuffer () = 0;
  };
}

#endif // CCF_CODE_GENERATION_KIT_INDENTATION_BUFFER_HPP
