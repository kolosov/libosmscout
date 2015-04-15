#ifndef LUM_EDIT_SYNTAX_H
#define LUM_EDIT_SYNTAX_H

/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <bitset>
#include <map>
#include <set>
#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Object.h>
#include <Lum/Base/Singleton.h>

namespace Lum {
  namespace Edit {

    typedef unsigned char StateType;
    typedef unsigned char StateCount;

    class Syntax;

    class LUMAPI StateStackEntry
    {
    public:
      StateType  state;  //< the state the first character of row is in
      StateCount count;  //< For recursive states, the current cursion count
    };

    class LUMAPI StateInfo
    {
    public:
      StateStackEntry stack[/*stateRecursionDepth*/10];
      StateCount      current;

    public:
      StateInfo();
      void Reset();

      inline void Push(StateType state)
      {
        current++;

        stack[current].state=state;
        stack[current].count=1;
      }

      inline void Pop()
      {
        assert(current>0);

        current--;
      }

      inline void Inc()
      {
        stack[current].count++;
      }

      inline void Dec()
      {
        assert(stack[current].count>0);

        stack[current].count--;

        if (stack[current].count==0 && current>0) {
          current--;
        }
      }

      inline StateType GetState() const
      {
        return stack[current].state;
      }

      inline StateCount GetCount() const
      {
        return stack[current].count;
      }

      inline bool operator==(const StateInfo& other) const
      {
        return stack[current].state==other.stack[other.current].state &&
        stack[current].count==other.stack[other.current].count;
      }

      inline bool operator!=(const StateInfo& other) const
      {
        return stack[current].state!=other.stack[other.current].state ||
        stack[current].count!=other.stack[other.current].count;
      }
    };

    class LUMAPI Token
    {
    public:
      enum Flags
      {
        word        =  1 <<  0,
        escape      =  1 <<  1,
        recurseUp   =  1 <<  2,
        recurseDown =  1 <<  3,
        push        =  1 <<  4,
        pop         =  1 <<  5,
        blockStart  =  1 <<  6,
        blockEnd    =  1 <<  7
      };

    public:
      StateType     nextState;
      unsigned long style;

    protected:
      Syntax        *syntax;

    public:
      unsigned long flags;

    private:
      size_t        indentTag;

    public:
      Token(StateType nextState,
            unsigned long style,
            Syntax* syntax,
            unsigned long flags=0,
            size_t indentTag=0);
      virtual ~Token();

      void SetIndentTag(size_t tag);

      unsigned long GetStyle() const;
      size_t GetIndentTag() const;

      virtual bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Syntax
    {
    private:
      std::wstring      name;

    protected:
      std::set<wchar_t> word;

    private:
      size_t            tabSize;

    public:
      Syntax(const std::wstring& name);
      virtual ~Syntax();

      std::wstring GetName() const;

      void AddDelimiter(wchar_t delimiter);
      void SetDelimiter(const std::wstring& delimiters);
      bool IsDelimiter(wchar_t delimiter) const;
      void SetTabSize(size_t size);
      virtual Token* ParseToken(const std::wstring& text,
                                  size_t& x, size_t& length,
                                  StateInfo& state) = 0;
      virtual unsigned long GetStyle(const StateInfo& state) const = 0;
    };

    class LUMAPI SyntaxRepository
    {
    private:
      std::map<std::wstring,Syntax*> map;

    public:
      SyntaxRepository();
      virtual ~SyntaxRepository();

      void Register(Syntax* syntax);
      Syntax* Get(const std::wstring& name);
    };

    extern LUMAPI ::Lum::Base::Singleton<SyntaxRepository> syntaxRepository;

    class LUMAPI Keyword : public Token
    {
    private:
      std::wstring name;
      Token        *match;

    public:
      Keyword(StateType nextState,
              unsigned long style,
              Syntax* syntax,
              unsigned long flags,
              const std::wstring& name,
              size_t indentTag);

      bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Letter : public Token
    {
    public:
      Letter(StateType nextState,
             unsigned long style,
             Syntax* syntax,
             unsigned long flags=0,
             size_t indentTag=0);

      bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Delimiter : public Token
    {
    public:
      Delimiter(StateType nextState,
                unsigned long style,
                Syntax* syntax,
                unsigned long flags=0,
                size_t indentTag=0);

      bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Word : public Token
    {
    public:
      Word(StateType nextState,
             unsigned long style,
             Syntax* syntax,
             unsigned long flags=0,
             size_t indentTag=0);

      bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Number : public Token
    {
    public:
      Number(StateType nextState,
             unsigned long style,
             Syntax* syntax,
             unsigned long flags);

      bool Match(const std::wstring& text, size_t pos, size_t& length) const;
    };

    class LUMAPI Default : public Token
    {
    public:
      Default(unsigned long style, unsigned long flags);
    };

    /**
     Special configuable syntax parser based on tokens
    */
    class LUMAPI TokenSyntax : public Syntax
    {
    private:
      typedef std::map<wchar_t,std::list<Token*> > CharTokenMap;

      /**
        One instance of the state engine
      */
      class State
      {
      public:
        CharTokenMap  starters;     //< Maps start characters to potential list of Tokens
        unsigned long style;
        Token         *wordToken;    //< Special token for all characters belonging to a "word"
        Token         *defaultToken; //< general fall back token

      public:
        State();
        virtual ~State();
      };

    private:
      State stateMachine[100];

    public:
      TokenSyntax(const std::wstring& name);

      void SetScheme(StateType state, unsigned long  style);
      void AssignToken(StateType state, wchar_t character, Token* token);

      void AddKeyword(StateType state,
                      const std::wstring& name,
                      unsigned long style,
                      unsigned long flags=0,
                      StateType nextState=0,
                      size_t indentTag=0);
      void AddLetter(StateType state,
                     wchar_t letter,
                     unsigned long style,
                     unsigned long flags=0,
                     StateType nextState=0,
                     size_t indentTag=0);
      void AddWord(StateType state,
                   unsigned long style,
                   unsigned long flags=0,
                   StateType nextState=0,
                   size_t indentTag=0);
      void AddDelimiter(StateType state,
                        unsigned long style,
                        unsigned long flags=0,
                        StateType nextState=0,
                        size_t indentTag=0);
      void AddNumber(StateType state,
                     unsigned long style,
                     unsigned long flags=0,
                     StateType nextState=0);
      void AddKeywordMatch(StateType state, const std::wstring& a, const std::wstring b);

      Token* ParseToken(const std::wstring& text,
                          size_t& x, size_t& length,
                          StateInfo& state);
      unsigned long GetStyle(const StateInfo& state) const;
    };
  }
}

#endif
