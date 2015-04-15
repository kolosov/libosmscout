#ifndef LUM_CONFIG_CONFIG_H
#define LUM_CONFIG_CONFIG_H

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

#include <list>
#include <map>
#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Config {

    /**
      This is one node of a tree-like configuration.

      One node can have zero or more sub nodes.
      Each node has a name and one (optional) value.

      The configuration is not validated against any implict or
      explicit ruleset. You have to check for completeness and
      validity.
    */
    class LUMAPI Node
    {
    private:
      typedef std::multimap<std::wstring,Node*> NodeMap;

    public:
      typedef std::list<Node*>                  NodeList;

    private:
      std::wstring name;      //! Name of the node
      NodeMap      sub;       //! Map of sub nodes
      NodeList     children;  //! List of sub nodes
      std::wstring value;     //! Optional value
      bool         hasValue;  //! This is a value node
      bool         multiline; //! The value is a multiline (entity body) value
      Node*        parent;    //! Parent node

    public:
      Node();
      Node(const std::wstring& name);
      ~Node();

      // Tree

      Node* GetParent() const;

      // Name

      std::wstring GetName() const;
      void SetName(const std::wstring& name);

      // Values

      bool HasValue() const;
      bool HasMultilineValue() const;
      bool IsValueEmpty() const;
      std::wstring GetValue() const;
      bool GetValue(std::wstring& value) const;
      bool GetValue(std::string& value) const;
      bool GetValue(bool& value) const;
      bool GetValue(int& value) const;
      bool GetValue(unsigned int& value) const;
      bool GetValue(long& value) const;
      bool GetValue(unsigned long& value) const;
      bool GetValue(float& value) const;
      bool GetValue(double& value) const;

      void SetValue(const std::wstring& value, bool multiline=false);
      void SetValue(bool value);
      void SetValue(int value);
      void SetValue(unsigned int value);
      void SetValue(long value);
      void SetValue(unsigned long value);
      void AppendValue(const std::wstring& value, bool multiline=false);

      // Attributes

      void SetAttribute(const std::wstring& name, const std::wstring& value, bool multiline=false);
      void SetAttribute(const std::wstring& name, int value);
      void SetAttribute(const std::wstring& name, unsigned int value);
      void SetAttribute(const std::wstring& name, long value);
      void SetAttribute(const std::wstring& name, unsigned long value);
      void SetAttribute(const std::wstring& name, bool value);

      Node* GetAttribute(const std::wstring& name) const;
      bool GetAttribute(const std::wstring& name, std::wstring& value) const;
      bool GetAttribute(const std::wstring& name, std::string& value) const;
      bool GetAttribute(const std::wstring& name, bool& value) const;
      bool GetAttribute(const std::wstring& name, int& value) const;
      bool GetAttribute(const std::wstring& name, unsigned int& value) const;
      bool GetAttribute(const std::wstring& name, long& value) const;
      bool GetAttribute(const std::wstring& name, unsigned long& value) const;

      void Add(Node *node);

      bool HasChildren() const;
      const NodeList& GetChildren() const;

      Node*  GetSubNodeWithAttrValue(const std::wstring& name,
                                     const std::wstring& attr,
                                     const std::wstring& value) const;
      Node*  GetChild(const std::wstring& name) const;
    };

    class LUMAPI Error
    {
    private:
      std::wstring error;
      size_t       column,row;

    public:
      Error(const std::wstring& error, size_t column=0, size_t row=0);

      std::wstring GetError() const;
      size_t       GetColumn() const;
      size_t       GetRow() const;
    };

    typedef std::list<Error> ErrorList;

    extern LUMAPI bool ConfigAccessible(const std::wstring& filename);
    extern LUMAPI Node* LoadConfigFromXMLFile(const std::wstring& filename,
                                              ErrorList& errorList);
    extern LUMAPI bool SaveConfigToXMLFile(const std::wstring& filename,
                                           Node* node);
  }
}

#endif
