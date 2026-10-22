// file      : Utility/ExH/Logic/DescriptiveException.ipp
// author    : Boris Kolpackov <boris@kolpackov.net>
// copyright : Copyright (c) 2002-2003 Boris Kolpackov
// license   : http://kolpackov.net/license.html

namespace Utility
{
  namespace ExH
  {
    namespace Logic
    {
      // c-tor's & d-tor

      inline DescriptiveException::
      DescriptiveException () noexcept
      {
      }

      inline DescriptiveException::
      DescriptiveException (char const* description) noexcept
      {
        init (description);
      }

      inline DescriptiveException::
      DescriptiveException (std::string const& description) noexcept
      {
        try
        {
          init (description.c_str ());
        }
        catch (...)
        {
        }
      }

      inline DescriptiveException::
      DescriptiveException (DescriptiveException const& src) noexcept
        : std::exception (),
          Exception ()
      {
        init (src.what ());
      }

      inline DescriptiveException::
      ~DescriptiveException () noexcept = default;

      inline DescriptiveException& DescriptiveException::
      operator= (DescriptiveException const& src) noexcept
      {
        init (src.what ());
        return *this;
      }


      // accessors / modifiers

      inline void
      DescriptiveException::init (char const* description) noexcept
      {
        try
        {
          if (description == 0 || description[0] == '\0')
          {
            description_.reset ();
          }
          else
          {
            if (description_)
            {
              *description_ = description;
            }
            else
            {
              description_.reset (new std::string (description));
            }
          }
        }
        catch (...)
        {
          description_.reset ();
        }
      }

      inline char const*
      DescriptiveException::what () const noexcept
      {
        try
        {
          if (description_)
          {
            return description_->c_str ();
          }
        }
        catch (...)
        {
        }

        return Exception::what ();
      }
    }
  }
}
