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

#include <Lum/Edit/Syntax.h>

#include <limits>

#include <Lum/Edit/Style.h>

namespace Lum {
  namespace Edit {

    static SyntaxRepository*                 singletonStore;
    ::Lum::Base::Singleton<SyntaxRepository> syntaxRepository(&singletonStore);

    StateInfo::StateInfo()
    {
      Reset();
    }

    void StateInfo::Reset()
    {
      current=0;
      stack[0].state=0;
      stack[0].count=0;
    }

    Token::Token(StateType nextState,
                 unsigned long style,
                 Syntax* syntax,
                 unsigned long flags,
                 size_t indentTag)
    : nextState(nextState),style(style),syntax(syntax),flags(flags),indentTag(indentTag)
    {
      // no code
    }

    Token::~Token()
    {
      // no code
    }

    void Token::SetIndentTag(size_t tag)
    {
      indentTag=tag;
    }

    unsigned long Token::GetStyle() const
    {
      return style;
    }

    size_t Token::GetIndentTag() const
    {
      return indentTag;
    }

    bool Token::Match(const std::wstring& /*text*/, size_t /*pos*/, size_t& /*length*/) const
    {
      return false;
    }


    Syntax::Syntax(const std::wstring& name)
    : name(name)
    {
      word.insert(L'\r');
      word.insert(L'\n');
      word.insert(L'\t');
      word.insert(L' ');
      word.insert(L'.');
      word.insert(L',');
      word.insert(L';');
      word.insert(L':');

      tabSize=8;
    }

    Syntax::~Syntax()
    {
     // no code
    }

    std::wstring Syntax::GetName() const
    {
      return name;
    }

    void Syntax::AddDelimiter(wchar_t delimiter)
    {
      word.insert(delimiter);
    }

    void Syntax::SetDelimiter(const std::wstring& delimiters)
    {
      word.clear();
      for (size_t x=0; x<delimiters.length(); x++) {
        AddDelimiter(delimiters[x]);
      }
    }

    bool Syntax::IsDelimiter(wchar_t delimiter) const
    {
      return word.find(delimiter)!=word.end();
    }

    void Syntax::SetTabSize(size_t size)
    {
      tabSize=size;
    }

    class DefaultSyntax : public Syntax
    {
    private:
      Token *a;

    public:
      DefaultSyntax()
      : Syntax(L"Standard"),
      a(new Token(0,ColorSheme::normalStyle,this))
      {
        // no code
      }

      ~DefaultSyntax()
      {
        delete a;
      }

      Token* ParseToken(const std::wstring& text, size_t& /*x*/, size_t& length, StateInfo& /*state*/)
      {
        length=text.length();

        return a;
      }

      unsigned long GetStyle(const StateInfo& /*state*/) const
      {
        return ColorSheme::normalStyle;
      }
    };

    SyntaxRepository::SyntaxRepository()
    {
      Syntax* defaultSyntax;

      defaultSyntax=new DefaultSyntax();
      map[defaultSyntax->GetName()]=defaultSyntax;
    }

    SyntaxRepository::~SyntaxRepository()
    {
      std::map<std::wstring,Syntax*>::iterator iter;

      iter=map.begin();
      while (iter!=map.end()) {
        delete iter->second;
        ++iter;
      }
      // no code
    }

    void SyntaxRepository::Register(Syntax* syntax)
    {
      syntaxRepository->map[syntax->GetName()]=syntax;
    }

    Syntax* SyntaxRepository::Get(const std::wstring& name)
    {
      std::map<std::wstring,Syntax*>::const_iterator iter;

      iter=syntaxRepository->map.find(name);

      if (iter!=syntaxRepository->map.end()) {
        return iter->second;
      }
      else {
        return NULL;
      }
    }

    Keyword::Keyword(StateType nextState,
                     unsigned long style,
                     Syntax* syntax,
                     unsigned long flags,
                     const std::wstring& name,
                     size_t indentTag)
    : Token(nextState,style,syntax,flags,indentTag),name(name),match(NULL)
    {
      // no code
    }

    bool Keyword::Match(const std::wstring& text, size_t pos, size_t& length) const
    {
      length=1;
      while (pos+length<text.length() && length<name.length() && name[length]==text[pos+length]) {
        length++;
      }

      return length==name.length();
    }

    Letter::Letter(StateType nextState,
                   unsigned long style,
                   Syntax* syntax,
                   unsigned long flags,
                   size_t indentTag)
    : Token(nextState,style,syntax,flags,indentTag)
    {
      // no code
    }

    bool Letter::Match(const std::wstring& /*text*/, size_t /*pos*/, size_t& length) const
    {
      length=1;

      return true;
    }

    Word::Word(StateType nextState,
               unsigned long style,
               Syntax* syntax,
               unsigned long flags,
               size_t indentTag)
    : Token(nextState,style,syntax,flags,indentTag)
    {
      // no code
    }

    bool Word::Match(const std::wstring& text, size_t pos, size_t& length) const
    {
      length=1;
      while (pos+length<text.length() && !syntax->IsDelimiter(text[pos+length])) {
        length++;
      }

      return true;
    }

    Delimiter::Delimiter(StateType nextState,
                         unsigned long style,
                         Syntax* syntax,
                         unsigned long flags,
                         size_t indentTag)
    : Token(nextState,style,syntax,flags,indentTag)
    {
      // no code
    }

    bool Delimiter::Match(const std::wstring& text, size_t pos, size_t& length) const
    {
      length=1;
      while (pos+length<text.length() && syntax->IsDelimiter(text[pos+length])) {
        length++;
      }

      return true;
    }

    Number::Number(StateType nextState,
                   unsigned long style,
                   Syntax* syntax,
                   unsigned long flags)
    : Token(nextState,style,syntax,flags)
    {
      // no code
    }

    bool Number::Match(const std::wstring& text, size_t pos, size_t& length) const
    {
      length=1;

      while (pos+length<text.length() &&
             text[pos+length]>=L'0' && text[pos+length]<=L'9') {
        length++;
      }

      return true;
    }

    Default::Default(unsigned long style, unsigned long flags)
    : Token(0,style,NULL,flags)
    {
      // no code
    }

    TokenSyntax::State::State()
    : style(ColorSheme::normalStyle),
      wordToken(NULL),
      defaultToken(new Default(ColorSheme::normalStyle,0))
    {
      // no code
    }

    TokenSyntax::State::~State()
    {
      CharTokenMap::iterator iter;

      iter=starters.begin();
      while (iter!=starters.begin()) {
        std::list<Token*>::iterator token;

        token=iter->second.begin();
        while (token!=iter->second.end()) {
          delete *token;

          ++token;
        }

        ++iter;
      }

      delete wordToken;
      delete defaultToken;
    }

    TokenSyntax::TokenSyntax(const std::wstring& name)
    : Syntax(name)
    {
      // no code
    }

    void TokenSyntax::SetScheme(StateType state, unsigned long  style)
    {
      stateMachine[state].style=style;
      stateMachine[state].defaultToken->style=style;
    }

    void TokenSyntax::AssignToken(StateType state, wchar_t character, Token* token)
    {
      stateMachine[state].starters[character].push_front(token);
    }

    void TokenSyntax::AddKeyword(StateType state,
                                 const std::wstring& name,
                                 unsigned long style,
                                 unsigned long flags,
                                 StateType nextState,
                                 size_t indentTag)
    {
      AssignToken(state,name[0],new Keyword(nextState,style,this,flags,name,indentTag));
    }

    void TokenSyntax::AddLetter(StateType state,
                                wchar_t letter,
                                unsigned long style,
                                unsigned long flags,
                                StateType nextState,
                                size_t indentTag)
    {
      AssignToken(state,letter,new Letter(nextState,style,this,flags,indentTag));
    }

    void TokenSyntax::AddWord(StateType state,
                              unsigned long style,
                              unsigned long flags,
                              StateType nextState,
                              size_t indentTag)
    {
      delete stateMachine[state].wordToken;
      stateMachine[state].wordToken=new Word(nextState,style,this,flags,indentTag);
    }

    void TokenSyntax::AddDelimiter(StateType state,
                                   unsigned long style,
                                   unsigned long flags,
                                   StateType nextState,
                                   size_t indentTag)
    {
      std::set<wchar_t>::const_iterator iter;

      iter=word.begin();
      while (iter!=word.end()) {
        AssignToken(state,*iter,new Delimiter(nextState,style,this,flags,indentTag));

        ++iter;
      }
    }

    void TokenSyntax::AddNumber(StateType state,
                                unsigned long style,
                                unsigned long flags,
                                StateType nextState)
    {
      for (wchar_t x=L'0'; x<=L'9'; x++) {
        AssignToken(state,x,new Number(nextState,style,this,flags));
      }
    }

    void TokenSyntax::AddKeywordMatch(StateType /*state*/,
                                      const std::wstring& /*a*/,
                                      const std::wstring /*b*/)
    {
    }

    Token* TokenSyntax::ParseToken(const std::wstring& text,
                                     size_t& x, size_t& length,
                                     StateInfo& state)
    {
      StateType              st;
      CharTokenMap::iterator entry;
      Token                  *token=NULL;

      assert(x<text.length());

      st=state.GetState();

      // Check all tokens which have the current character as starter
      // if they match
      entry=stateMachine[st].starters.find(text[x]);
      if (entry!=stateMachine[st].starters.end()) {
        std::list<Token*>::iterator iter;

        iter=entry->second.begin();
        while (iter!=entry->second.end()) {
          // Start of word precondition
          if ((Token::word & (*iter)->flags) && x>0 && !IsDelimiter(text[x-1])) {
            ++iter;
            continue;
          }

          if ((*iter)->Match(text,x,length)) {
            if ((Token::word & (*iter)->flags) && x+length<text.length() && !IsDelimiter(text[x+length])) {
              ++iter;
              continue;
            }

            token=*iter;
            break;
          }

          ++iter;
        }
      }

      if (token==NULL && stateMachine[st].wordToken!=NULL && !IsDelimiter(text[x])) {
        token=stateMachine[st].wordToken;
        token->Match(text,x,length);
      }

      if (token!=NULL) {
        if (Token::recurseUp & token->flags) {
          state.Inc();
        }
        else if (Token::recurseDown & token->flags && state.GetCount()>0) {
          state.Dec();
        }
        else if (Token::push & token->flags) {
          state.Push(token->nextState);
        }
        else if (Token::pop & token->flags) {
          state.Pop();
        }

        if (Token::escape & token->flags) {
          x++;
        }

        return token;
      }
      else {
        length=1;
        return stateMachine[st].defaultToken;
      }
    }

    unsigned long TokenSyntax::GetStyle(const StateInfo& state) const
    {
      return stateMachine[state.GetState()].style;
    }
  }
}
