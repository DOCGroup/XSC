// file      : CCF/CodeGenerationKit/IndentationCxx.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#ifndef CCF_CODE_GENERATION_KIT_INDENTATION_CXX_HPP
#define CCF_CODE_GENERATION_KIT_INDENTATION_CXX_HPP

#include <algorithm>
#include <cstddef>
#include <deque>
#include <stack>
#include <string>

#include "CCF/CodeGenerationKit/IndentationBuffer.hpp"

namespace Indentation
{
  /**
   * @class Cxx
   *
   * Indentation implementation for C++ files.
   */
  template <typename C>
  class Cxx : public Buffer <C>
  {
  public:
    /// Type definition of the traits type.
    using traits_type = typename Buffer<C>::traits_type;

    /// Type definition of the char type.
    using char_type = typename Buffer<C>::char_type;

    /// Type definition of the integer type
    using int_type = typename Buffer<C>::int_type;

    /// Type definition of the end of stream.
    using EndOfStream = typename Buffer<C>::EndOfStream;

  public:
    /**
     * Initializing constructor
     *
     * @param[in]           out       Target output buffer
     */
    Cxx (Buffer <C> & out)
    : out_ (out),
      position_ (0),
      paren_balance_ (0),
      spaces_ (2),
      construct_ (OTHER),
      last_non_whitespace_ (0),
      inline_initializer_ (false)
    {
      indentation_.push (0);
    }

    /// Destructor
    ~Cxx () override = default;

  public:
    /**
     * Add a new character to the buffer. Based on the character, different
     * actions are taken. The newline always causes the buffer to be flushed.
     */
    int_type put (char_type c) override
    {
      int_type result = traits_type::to_int_type (c);

      try
      {
        if (!this->access_label_.empty ())
        {
          if (c == ':' && this->is_access_label ())
          {
            std::size_t indentation (this->indentation_.top ());
            if (indentation >= this->spaces_)
              indentation -= this->spaces_;

            this->output_indentation (indentation);
            for (char_type character : this->access_label_)
            {
              result = this->write (character);
              ++ this->position_;
            }
            result = this->write (c);
            ++ this->position_;
            this->last_non_whitespace_ = c;
            this->access_label_.clear ();
            return result;
          }

          this->access_label_.push_back (c);
          if (this->is_access_label_prefix ())
            return result;

          this->access_label_.pop_back ();
          this->flush_access_label (result);
        }

        if (this->construct_ == OTHER && c == 'p' && this->position_ == 0 &&
            !this->hold_.empty () && this->hold_.back () == '\n')
        {
          this->access_label_.push_back (c);
          return result;
        }

        bool defaulting = false;

        if (!this->hold_.empty () && this->hold_.back () == '(')
        {
          // We do not need the buffered elements anymore.
          this->unbuffer ();

          if (c == '\n')
            // Add another level to the indentation.
            this->indentation_.push (this->indentation_.top () + this->spaces_);
          else
            // Save the current position.
            this->indentation_.push (this->position_);
        }

        switch (c)
        {
        case '\n':
          // Save the eol character and start a new line.
          this->hold_.push_back (c);
          this->position_ = 0;

          if (this->construct_ == CXX_COMMENT)
            this->construct_ = OTHER;

          break;

        case '{':
          if (this->construct_ != CXX_COMMENT &&
              this->construct_ != STRING_LITERAL)
          {
            if (this->last_non_whitespace_ == '=' ||
                this->last_non_whitespace_ == '_')
            {
              this->output_indentation ();
              result = this->write (c);
              ++ this->position_;
              this->inline_initializer_ = true;
              break;
            }

            // Make sure there is a new line in the buffer. We then need to
            // move the correct position on the newline.
            this->ensure_new_line ();
            this->output_indentation ();
            result = this->write (c);

            // Make sure there is a newline after the brace.
            this->ensure_new_line ();

            // Increase the indentation by one.
            this->indentation_.push (this->indentation_.top () + this->spaces_);
          }
          else
            defaulting = true;

          break;

        case '}':
          if (!(this->construct_ == CXX_COMMENT || this->construct_ == STRING_LITERAL))
          {
            if (this->inline_initializer_)
            {
              this->output_indentation ();
              result = this->write (c);
              ++ this->position_;
              this->inline_initializer_ = false;
              break;
            }

            // Remove the current indentation value, if it exists.
            if (this->indentation_.size () > 1)
              this->indentation_.pop ();

            // Reduce multiple newlines to one.
            while (this->hold_.size () > 1)
            {
              typename Hold::reverse_iterator i = this->hold_.rbegin ();

              if (*i == '\n' && *(i + 1) == '\n')
                this->hold_.pop_back ();
              else
                break;
            }

            // Make sure we are on a newline and that we are at the
            // right position.
            this->ensure_new_line ();
            this->output_indentation ();

            // Now, we can add the new character to the newline.
            this->hold_.push_back (c);


            // Add a newline after '}'.
            this->hold_.push_back ('\n');

            // Set position to the start of the line.
            this->position_ = 0;
          }
          else
            defaulting = true;

          break;

        case ';':
          if (this->paren_balance_ != 0)
          {
            // We are inside for (;;) statement. Nothing to do here.
            defaulting = true;
          }
          else
          {
            // Handling the '};' case, i.e., complex type definitions.
            bool brace = false;

            if (this->hold_.size () > 1 && this->hold_.back () == '\n')
            {
              bool pop_nl = false;
              using iterator = typename Hold::reverse_iterator;

              for (iterator i = this->hold_.rbegin (),
                   e = this->hold_.rend ();
                   i != e; ++ i)
              {
                if (*i != '\n')
                {
                  if (*i == '}')
                    brace = pop_nl = true;

                  break;
                }
              }

              if (pop_nl)
              {
                while (this->hold_.back () == '\n')
                  this->hold_.pop_back ();
              }
            }

            // Make sure we write the indentation.
            this->output_indentation ();
            result = this->write (c);

            // Update the position
            this->position_ ++;

            if (brace)
            {
              this->hold_.push_back ('\n');
              this->hold_.push_back ('\n');
            }

            if (this->construct_ != STRING_LITERAL && this->construct_ != CHAR_LITERAL)
              this->ensure_new_line ();
          }
          break;

        case '\\':
          if (this->construct_ != CXX_COMMENT)
          {
            this->output_indentation ();
            this->hold_.push_back (c);

            // Increment the position marker.
            ++ this->position_;
          }
          else
            defaulting = true;

          break;

        case '"':
          if (this->construct_ != CXX_COMMENT &&
             (this->hold_.empty () || this->hold_.back () != '\\'))
          {
            // not escape sequence
            if (this->construct_ == STRING_LITERAL)
              this->construct_ = OTHER;
            else
              this->construct_ = STRING_LITERAL;
          }

          defaulting = true;
          break;

        case '\'':
          if (this->construct_ != CXX_COMMENT &&
              this->construct_ != STRING_LITERAL &&
             (this->hold_.empty () || this->hold_.back () != '\\'))
          {
            // not escape sequence
            if (this->construct_ == CHAR_LITERAL)
              this->construct_ = OTHER;
            else
              this->construct_ = CHAR_LITERAL;
          }

          defaulting = true;
          break;

        case '(':
          if (this->construct_ == OTHER)
          {
            // Hold it so that we can see what's coming next.
            this->output_indentation ();
            this->hold_.push_back (c);

            // Update the position by one space to account for the
            // location of this parenthesis.
            this->position_ ++;

            // We must balance a new set of parenthesis.
            this->paren_balance_ ++;
          }
          else
            defaulting = true;

          break;

        case ')':
          if (this->construct_ == OTHER)
          {
            // We can remove the current indentation.
            if (this->indentation_.size () > 1)
              this->indentation_.pop ();

            // We have balanced one of the parenthesis.
            if (this->paren_balance_ > 0)
              this->paren_balance_ --;
          }

          defaulting = true;
          break;

        case '/':
          if (this->construct_ == OTHER)
          {
            if (!this->hold_.empty () && this->hold_.back () == '/')
            {
              // We are starting a C++ comment.
              this->construct_ = CXX_COMMENT;
              defaulting = true;
            }
            else
            {
              // This is just another character. We can just write it to
              // the holding buffer.
              this->output_indentation ();
              this->hold_.push_back (c);

              // Update the position.
              ++ this->position_;
            }
          }
          else
            defaulting = true;

          break;

        default:
          defaulting = true;
        }

        if (defaulting)
        {
          // Write the character to the holding buffer.
          this->output_indentation ();
          result = this->write (c);

          // Update the position.
          ++ this->position_;
        }

        if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
          this->last_non_whitespace_ = c;
      }
      catch (const Full &)
      {
        result = traits_type::eof ();
      }

      return result;
    }

    /**
     * Unbuffer the elements. This will remove the elements on
     * the holding queue to the target output stream.
     */
    void unbuffer () override
    {
      int_type result;

      while (!this->hold_.empty ())
      {
        result = this->out_.put (this->hold_.front ());

        if (result == traits_type::eof ())
          throw EndOfStream ("unable to flush buffer");

        this->hold_.pop_front ();
      }
    }

  private:
    class Full {};

    bool
    is_access_label_prefix () const
    {
      static char const* const labels[] = {"public", "private", "protected"};
      for (char const* label : labels)
      {
        std::size_t length (std::char_traits<char>::length (label));
        if (this->access_label_.size () <= length &&
            std::equal (this->access_label_.begin (), this->access_label_.end (), label))
          return true;
      }
      return false;
    }

    bool
    is_access_label () const
    {
      static char const* const labels[] = {"public", "private", "protected"};
      for (char const* label : labels)
      {
        std::size_t length (std::char_traits<char>::length (label));
        if (this->access_label_.size () == length &&
            std::equal (this->access_label_.begin (), this->access_label_.end (), label))
          return true;
      }
      return false;
    }

    void
    flush_access_label (int_type& result)
    {
      this->output_indentation (this->indentation_.top ());
      for (char_type character : this->access_label_)
      {
        result = this->write (character);
        ++ this->position_;
      }
      if (!this->access_label_.empty ())
        this->last_non_whitespace_ = this->access_label_.back ();
      this->access_label_.clear ();
    }

    /**
     * Helper method that makes sure a newline exists at the end
     * of the holding buffer.
     */
    void ensure_new_line ()
    {
      if (this->hold_.empty () || hold_.back () != '\n')
      {
        // Insert a new line in the holding buffer.
        this->hold_.push_back ('\n');

        // Update the position to the start of the line.
        this->position_ = 0;
      }
    }

    /**
     * Helper method that writes the current indentation to the
     * output buffer.
     */
    void output_indentation ()
    {
      this->output_indentation (this->indentation_.top ());
    }

    void output_indentation (std::size_t indentation)
    {
      if (!this->hold_.empty () && this->hold_.back () == '\n')
      {
        for (std::size_t i {}; i < indentation; ++i)
          this->write (' ');

        this->position_ += indentation;
      }
    }

    /**
     * Write a new character to the holding buffer.
     *
     * @param[in]       c       The new character.
     */
    int_type write (char_type c)
    {
      this->hold_.push_back (c);
      int_type result (traits_type::eof ());

      while (!this->hold_.empty ())
      {
        result = this->out_.put (this->hold_.front ());

        if (result == traits_type::eof ())
          throw Full ();

        this->hold_.pop_front ();
      }

      return result;
    }

  private:
    /// Target output buffer.
    Buffer <C> & out_;

    /// Current position on the line
    std::size_t position_;

    /// Balance the parenthesis
    std::size_t paren_balance_;

    /// Saved indentations based on state/scope.
    std::stack<std::size_t> indentation_;

    /// Number of space in a indentation.
    std::size_t spaces_;

    enum Construct
    {
      OTHER,
      CXX_COMMENT,
      STRING_LITERAL,
      CHAR_LITERAL
    };

    /// The type of construct.
    Construct construct_;

    /// Last non-whitespace input character.
    char_type last_non_whitespace_;

    /// Whether the current brace pair is an inline initializer.
    bool inline_initializer_;

    /// Potential access label buffered until it can be distinguished from code.
    std::basic_string<char_type> access_label_;

    /// Type definition of the holding queue.
    using Hold = std::deque<int_type>;

    /// Holding queue for elements.
    Hold hold_;
  };
}

#endif // CCF_CODE_GENERATION_KIT_INDENTATION_IDL_HPP
