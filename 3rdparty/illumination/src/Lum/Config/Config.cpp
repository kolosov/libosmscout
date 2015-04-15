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

#include <Lum/Config/Config.h>

#include <cassert>
#include <cstring>
#include <fstream>

#include <Lum/Private/Config.h>

#if defined(HAVE_LIB_XERCES)
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

XERCES_CPP_NAMESPACE_USE

#endif

#if defined(HAVE_LIB_XML2)
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>
#endif

#include <Lum/Base/String.h>

namespace Lum {
  namespace Config {

    Node::Node()
    : hasValue(false),
      multiline(false),
      parent(NULL)
    {
      // no code
    }

    Node::Node(const std::wstring& name)
    : name(name),
      hasValue(false),
      multiline(false),
      parent(NULL)
    {
      // no code
    }

    Node::~Node()
    {
      NodeList::iterator iter;

      iter=children.begin();
      while (iter!=children.end()) {
        delete *iter;

        ++iter;
      }
    }

    Node* Node::GetParent() const
    {
      return parent;
    }

    std::wstring Node::GetName() const
    {
      return name;
    }

    void Node::SetName(const std::wstring& name)
    {
      this->name=name;
    }

    std::wstring Node::GetValue() const
    {
      assert(hasValue);

      return value;
    }

    bool Node::HasValue() const
    {
      return hasValue;
    }

    bool Node::HasMultilineValue() const
    {
      return hasValue && multiline;
    }

    bool Node::IsValueEmpty() const
    {
      return !hasValue || value.empty();
    }

    bool Node::GetValue(bool& value) const
    {
      if (this->value==L"true") {
        value=true;
        return true;
      }
      else if (this->value==L"false") {
        value=false;
        return true;
      }
      else {
        return false;
      }
    }

    bool Node::GetValue(std::wstring& value) const
    {
      if (!HasValue()) {
        return false;
      }

      value=this->value;
      return true;
    }

    bool Node::GetValue(std::string& value) const
    {
      if (!HasValue()) {
        return false;
      }

      value=Lum::Base::WStringToString(this->value);
      return true;
    }

    bool Node::GetValue(int& value) const
    {
      if (!HasValue()) {
        return false;
      }

      return Base::WStringToNumber(this->value,value);
    }

    bool Node::GetValue(unsigned int& value) const
    {
      if (!HasValue()) {
        return false;
      }

      return Base::WStringToNumber(this->value,value);
    }

    bool Node::GetValue(long& value) const
    {
      if (!HasValue()) {
        return false;
      }

      return Base::WStringToNumber(this->value,value);
    }

    bool Node::GetValue(unsigned long& value) const
    {
      if (!HasValue()) {
        return false;
      }

      return Base::WStringToNumber(this->value,value);
    }

    bool Node::GetValue(float& value) const
    {
      if (!HasValue()) {
        return false;
      }

      std::string tmp=Base::WStringToString(this->value);

      return sscanf(tmp.c_str(),"%f",&value)==1;
    }

    bool Node::GetValue(double& value) const
    {
      if (!HasValue()) {
        return false;
      }

      std::string tmp=Base::WStringToString(this->value);

      return sscanf(tmp.c_str(),"%lf",&value)==1;
    }

    void Node::SetValue(const std::wstring& value, bool multiline)
    {
      hasValue=true;
      this->multiline=multiline;
      this->value=value;
    }

    void Node::SetValue(int value)
    {
      hasValue=true;
      this->value=Base::NumberToWString(value);
    }

    void Node::SetValue(unsigned int value)
    {
      hasValue=true;
      this->value=Base::NumberToWString(value);
    }

    void Node::SetValue(long value)
    {
      hasValue=true;
      this->value=Base::NumberToWString(value);
    }

    void Node::SetValue(unsigned long value)
    {
      hasValue=true;
      this->value=Base::NumberToWString(value);
    }

    void Node::SetValue(bool value)
    {
      hasValue=true;
      this->value=value ? L"true" : L"false";
    }

    void Node::AppendValue(const std::wstring& value, bool multiline)
    {
      hasValue=true;
      this->multiline=multiline;
      this->value.append(value);
    }

    Node* Node::GetAttribute(const std::wstring& name) const
    {
      NodeMap::const_iterator iter;

      iter=sub.find(name);
      if (iter==sub.end()) {
        return NULL;
      }

      if (!iter->second->HasValue()) {
        return NULL;
      }

      return iter->second;
    }

    bool Node::GetAttribute(const std::wstring& name, std::wstring& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, std::string& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, bool& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, int& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, unsigned int& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, long& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    bool Node::GetAttribute(const std::wstring& name, unsigned long& value) const
    {
      Node* attr;

      attr=GetAttribute(name);

      if (attr==NULL) {
        return false;
      }

      return attr->GetValue(value);
    }

    void Node::SetAttribute(const std::wstring& name, const std::wstring& value, bool multiline)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value,multiline);

      Add(child);
    }

    void Node::SetAttribute(const std::wstring& name, int value)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value);

      Add(child);
    }

    void Node::SetAttribute(const std::wstring& name, unsigned int value)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value);

      Add(child);
    }

    void Node::SetAttribute(const std::wstring& name, long value)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value);

      Add(child);
    }

    void Node::SetAttribute(const std::wstring& name, unsigned long value)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value);

      Add(child);
    }

    void Node::SetAttribute(const std::wstring& name, bool value)
    {
      Node *child;

      child=new Node();
      child->SetName(name);
      child->SetValue(value);

      Add(child);
    }

    bool Node::HasChildren() const
    {
      return !children.empty();
    }

    const Node::NodeList& Node::GetChildren() const
    {
      return children;
    }

    /**
      Searches for an entry item. If this item exists and a direct
      subitem with name and value exists then this
      item is returned. Else NULL will is returned.
    */
    Node* Node::GetSubNodeWithAttrValue(const std::wstring& name,
                                        const std::wstring& attr,
                                        const std::wstring& value) const
    {
      NodeMap::const_iterator iter;

      for (NodeMap::const_iterator iter=sub.lower_bound(name); iter!=sub.upper_bound(name); ++iter) {
        NodeMap::const_iterator iter2;

        iter2=iter->second->sub.find(attr);
        if (iter2==sub.end()) {
          return NULL;
        }

        if (iter2->second->GetValue()==value) {
          return iter->second;
        }
      }

      return NULL;
    }

    Node* Node::GetChild(const std::wstring& name) const
    {
      NodeMap::const_iterator iter;

      iter=sub.find(name);
      if (iter==sub.end()) {
        return NULL;
      }

      return iter->second;
    }

    void Node::Add(Node *node)
    {
      assert(node!=NULL && !node->GetName().empty());

      node->parent=this;
      sub.insert(NodeMap::value_type(node->GetName(),node));

      children.push_back(node);
    }

    Error::Error(const std::wstring& error, size_t column, size_t row)
    : error(error), column(column),row(row)
    {
      // no code
    }

    std::wstring Error::GetError() const
    {
      return error;
    }

    size_t Error::GetColumn() const
    {
      return column;
    }

    size_t Error::GetRow() const
    {
      return row;
    }

    bool ConfigAccessible(const std::wstring& filename)
    {
      std::string xmlFile=Base::WStringToString(filename);
      std::ifstream stream;

      stream.open(xmlFile.c_str(),std::ios_base::in);
      if (stream) {
        stream.close();
        return true;
      }

      return false;
    }

#if defined(HAVE_LIB_XML2)

    class XML2SAXHandler
    {
    public:
      Node           *top;
      Node           *current;
      Node::NodeList stack;
      ErrorList&     errorList;


    public:
      XML2SAXHandler(ErrorList& errorList)
      : top(NULL),current(NULL),stack(),errorList(errorList)
      {
      }
    };


    xmlEntityPtr GetEntity(void* /*data*/, const xmlChar *name)
    {
      return xmlGetPredefinedEntity(name);
    }

    void StartElement(void *data, const xmlChar *name, const xmlChar **atts)
    {
      XML2SAXHandler *parser=(XML2SAXHandler*)data;

      Node *node=new Node();

      if (parser->current!=NULL) {
        //parser->current->Add(node);
        parser->stack.push_front(parser->current);
      }

      parser->current=node;

      if (parser->top==NULL) {
        parser->top=node;
      }

      parser->current->SetName(Base::UTF8ToWString(std::string((const char*)name)));

      if (atts!=NULL) {
        size_t x;

        x=0;
        while (atts[x]!=NULL && atts[x+1]!=NULL) {
          node=new Node();
          node->SetName(Base::UTF8ToWString(std::string((const char*)atts[x])));
          node->SetValue(Base::UTF8ToWString(std::string((const char*)atts[x+1])));

          parser->current->Add(node);

          x+=2;
        }
      }

    }

    void EndElement(void *data, const xmlChar *name)
    {
      XML2SAXHandler *parser=(XML2SAXHandler*)data;

      if (parser->stack.size()>0) {
        Node* node;

        node=parser->current;
        parser->current=parser->stack.front();
        parser->stack.pop_front();
        parser->current->Add(node);
      }
      else {
        parser->current=NULL;
      }
    }

    void Characters(void *data, const xmlChar *ch, int len)
    {
      XML2SAXHandler *parser=(XML2SAXHandler*)data;

      parser->current->AppendValue(Base::UTF8ToWString(std::string((const char*)ch,0,len)),true);
    }

    void StructuredErrorHandler(void *data, xmlErrorPtr error)
    {
      XML2SAXHandler *parser=(XML2SAXHandler*)data;

      parser->errorList.push_back(Error(Base::UTF8ToWString(error->message),
                                        error->int2,error->line));
    }

    Node* LoadConfigFromXMLFile(const std::wstring& filename,
                                ErrorList& errorList)
    {
      xmlSAXHandler parser;

      memset(&parser,0,sizeof(xmlSAXHandler));
      parser.initialized=XML_SAX2_MAGIC;
      parser.getEntity=GetEntity;
      parser.startElement=StartElement;
      parser.endElement=EndElement;
      parser.characters=Characters;
      parser.serror=StructuredErrorHandler;

      XML2SAXHandler handler(errorList);
      std::string    xmlFile=Base::WStringToString(filename);

      if (xmlSAXUserParseFile(&parser,&handler,xmlFile.c_str())<0) {
        delete handler.top;
        return NULL;
      }
      else if (handler.errorList.size()!=0) {
        delete handler.top;
        return NULL;
      }
      else {
        return handler.top;
      }
    }

#elif defined(HAVE_LIB_XERCES)

  /**
    The following string conversion code is a modified version of code copied
    from the source of the ConvertUTF tool, as can be found for example at
    http://www.unicode.org/Public/PROGRAMS/CVTUTF/

    It is free to copy and use.
  */

  static const int halfShift=10; /* used for shifting by 10 bits */

  static const unsigned long halfMask=0x3FFUL;

  #define UNI_SUR_HIGH_START  0xD800
  #define UNI_SUR_LOW_END     0xDFFF
  #define UNI_MAX_BMP         0x0000FFFF
  #define UNI_MAX_LEGAL_UTF32 0x0010FFFF

  std::wstring Char16ToWString(const XMLCh* text,size_t length)
  {
    std::wstring result;

    result.reserve(length+1);

    unsigned long ch,ch2;
    const char*   source=text;

    while (source!=text+length) {
      ch = *source++;
      /* If we have a surrogate pair, convert to UTF32 first. */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
        /* If the 16 bits following the high surrogate are in the source buffer... */

        if (source>=text+length) {
          return result;
        }

       ch2 = *source;
       /* If it's a low surrogate, convert to UTF32. */
        if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
          ch=((ch-UNI_SUR_HIGH_START) << halfShift) + (ch2-UNI_SUR_LOW_START) + halfBase;
          ++source;
        }
        else {
          return result;
        }
      }
      else {
        return result;
      }
      result.append(1,ch);
    }

    return result;
  }

  XMLCh* WStringToChar16(const std::wstring& text, size_t& length)
  {
    const wchar_t* source=text.c_str();

    XMLCh *result=new XMLCh[(wstring.length()+1)*2];
    XMLCh *target=result;

    while (source!=text.c_str()+text.length()) {
      unsigned long ch;

      ch = *source++;

      if (ch<=UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
        /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
        if (ch>=UNI_SUR_HIGH_START && ch<=UNI_SUR_LOW_END) {
          return result;
        }
        else {
          *target++=ch; /* normal case */
        }
      }
      else if (ch>UNI_MAX_LEGAL_UTF32) {
        return result;
      }
      else {
        /* target is a character in range 0xFFFF - 0x10FFFF. */
        ch-=halfBase;
        *target++=((ch >> halfShift) + UNI_SUR_HIGH_START);
        *target++=((ch & halfMask) + UNI_SUR_LOW_START);
      }
    }

    class MySAX2Handler : public DefaultHandler
    {
    public:
      Node       *top;
      Node       *current;
      NodeList   stack;
      ErrorList& errorList;


    public:
      MySAX2Handler(ErrorList& errorList);
      ~MySAX2Handler();

      void startElement(const XMLCh* const uri,
                        const XMLCh* const localname,
                        const XMLCh* const name,
                        const Attributes&  attrs);

      void characters(const XMLCh *const chars,
                      const unsigned int length);

      void endElement(const XMLCh* const uri,
                      const XMLCh* const localname,
                      const XMLCh* const name);

      void warning(const SAXParseException&);
      void fatalError(const SAXParseException&);
    };

    MySAX2Handler::MySAX2Handler(ErrorList& errorList)
    : top(NULL),current(NULL),stack(),errorList(errorList)
    {
    }

    MySAX2Handler::~MySAX2Handler()
    {
    }

    void MySAX2Handler::startElement(const   XMLCh* const    /*uri*/,
                                     const   XMLCh* const    /*localname*/,
                                     const   XMLCh* const    name,
                                     const   Attributes&     attrs)
    {
      Node *node;

      node=new Node();

      if (current!=NULL) {
        //current->Add(node);
        stack.push_front(current);
      }

      current=node;

      if (top==NULL) {
        top=node;
      }

      current->SetName(Char16ToWString(name,XMLString::stringLen(name)));

      for (size_t x=0; x<attrs.getLength(); x++) {
        const XMLCh *attrName,*attrValue;

        node=new Node();

        attrName=attrs.getLocalName(x);
        attrValue=attrs.getValue(x);

        node->SetName(Char16ToWString(attrName,XMLString::stringLen(attrName)));
        node->SetValue(Char16ToWString(attrValue,XMLString::stringLen(attrValue)));

        current->Add(node);
      }
    }

    void MySAX2Handler::characters(const XMLCh *const chars,
                                   const unsigned int length)
    {
      std::wstring value=Char16ToWString(chars,length);
      size_t       x;

      // String only whitespace?
      x=0;
      while (x<value.length() && value[x]==L' ') {
        x++;
      }

      if (!current->IsValueEmpty() || x<value.length()) {
        current->AppendValue(value,true);
      }
    }

    void MySAX2Handler::endElement(const XMLCh* const /*uri*/,
                                   const XMLCh* const /*localname*/,
                                   const XMLCh* const /*name*/)
    {
      if (stack.size()>0) {
        Node *node=current;

        current=stack.front();
        current->Add(node);
        stack.pop_front();
      }
      else {
        current=NULL;
      }
    }

    void MySAX2Handler::warning(const SAXParseException& exception)
    {
      errorList.push_back(Error(Char16ToWString(exception.getMessage(),
                                                XMLString::stringLen(exception.getMessage())),
                                exception.getColumnNumber(),
                                exception.getLineNumber()));
    }

    void MySAX2Handler::fatalError(const SAXParseException& exception)
    {
      errorList.push_back(Error((exception.getMessage(),
                                 XMLString::stringLen(exception.getMessage())),
                                exception.getColumnNumber(),
                                exception.getLineNumber()));
    }

    Node* LoadConfigFromXMLFile(const std::wstring& filename,
                                ErrorList& errorList)
    {
      Node *node;

      try {
        XMLPlatformUtils::Initialize();
      }
      catch (const XMLException& toCatch) {
        errorList.push_back(Error(Char16ToWString(toCatch.getMessage(),
                                                  XMLString::stringLen(toCatch.getMessage()))));
        return NULL;
      }

      std::string xmlFile=Base::WStringToString(filename);

      SAX2XMLReader* parser=XMLReaderFactory::createXMLReader();
      parser->setFeature(XMLUni::fgSAX2CoreValidation,true);   // optional
      parser->setFeature(XMLUni::fgSAX2CoreNameSpaces,true);   // optional

      MySAX2Handler* defaultHandler=new MySAX2Handler(errorList);
      parser->setContentHandler(defaultHandler);
      parser->setErrorHandler(defaultHandler);

      try {
        parser->parse(xmlFile.c_str());
      }
      catch (const XMLException& toCatch) {
        errorList.push_back(Error(Char16ToWString(toCatch.getMessage(),
                                                  XMLString::stringLen(toCatch.getMessage()))));

        XMLPlatformUtils::Terminate();
        return NULL;
      }
      catch (const SAXException& toCatch) {
        errorList.push_back(Error(Char16ToWString(toCatch.getMessage(),
                                  XMLString::stringLen(toCatch.getMessage()))));
        XMLPlatformUtils::Terminate();
        return NULL;
      }
      catch (...) {
        errorList.push_back(Error(L"Unexpected exception"));
        XMLPlatformUtils::Terminate();
        return NULL;
      }

      node=defaultHandler->top;

      delete parser;
      delete defaultHandler;

      if (errorList.size()!=0) {
        delete node;
        XMLPlatformUtils::Terminate();
        return NULL;
      }

      XMLPlatformUtils::Terminate();

      return node;
    }

#else
    Node* LoadConfigFromXMLFile(const std::wstring& filename,
                                ErrorList& errorList)
    {
      errorList.push_back(Error(L"XML parsing not supported"));

      return NULL;
    }
#endif

#if defined(HAVE_LIB_XML2)

    bool WriteNode(Node* node, xmlTextWriterPtr writer)
    {
      if (xmlTextWriterStartElement(writer,(const xmlChar*)Base::WStringToUTF8(node->GetName()).c_str())<0) {
        return false;
      }

      for (Node::NodeList::const_iterator iter=node->GetChildren().begin(); iter!=node->GetChildren().end(); ++iter) {
        Node* sub=*iter;

        if (sub->HasValue() && !sub->HasMultilineValue()) {
          if (xmlTextWriterWriteAttribute(writer,
                                          (const xmlChar*)Base::WStringToUTF8(sub->GetName()).c_str(),
                                          (const xmlChar*)Base::WStringToUTF8(sub->GetValue()).c_str())<0) {
            return false;
          }
        }
      }

      if (node->HasMultilineValue()) {
        if (xmlTextWriterWriteRaw(writer,(const xmlChar*)Base::WStringToUTF8(node->GetValue()).c_str())<0) {
          return false;
        }
      }

      for (Node::NodeList::const_iterator iter=node->GetChildren().begin(); iter!=node->GetChildren().end(); ++iter) {
        Node* sub=*iter;

        if (!sub->HasValue() || sub->HasMultilineValue()) {
          if (!WriteNode(sub,writer)) {
            return false;
          }
        }
      }

      if (xmlTextWriterEndElement(writer)<0) {
        return false;
      }

      return true;
    }

    bool SaveConfigToXMLFile(const std::wstring& filename,Node* node)
    {
      xmlTextWriterPtr writer;

      /* Create a new XmlWriter for uri, with no compression. */
      writer=xmlNewTextWriterFilename(Lum::Base::WStringToString(filename).c_str(),0);
      if (writer==NULL) {
        xmlFreeTextWriter(writer);
        return false;
      }

      if (xmlTextWriterSetIndent(writer,2)<0) {
        xmlFreeTextWriter(writer);
        return false;
      }

      if (xmlTextWriterStartDocument(writer,NULL,"UTF-8",NULL)<0) {
        xmlFreeTextWriter(writer);
        return false;
      }

      if (!WriteNode(node,writer)) {
        xmlFreeTextWriter(writer);
        return false;
      }

      if (xmlTextWriterEndDocument(writer)<0) {
        xmlFreeTextWriter(writer);
        return false;
      }

      xmlFreeTextWriter(writer);

      return true;
    }


#elif defined(HAVE_LIB_XERCES)

    static const XMLCh  gXMLDecl1[] =
    {
      chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
      ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
      ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chDigit_1, chPeriod
      ,   chDigit_0, chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
      ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
      ,   chDoubleQuote, chNull
    };

    static const XMLCh  gXMLDecl2[] =
    {
      chDoubleQuote, chQuestion, chCloseAngle, chLF, chNull
    };

    void WriteNode(Node* node, size_t depth, XMLFormatter& formatter)
    {
      size_t size;

      for (size_t x=0; x<depth; x++) {
        formatter << XMLFormatter::NoEscapes << chSpace;
      }

      formatter << XMLFormatter::NoEscapes << chOpenAngle;
      formatter << WStringToChar16(node->GetName(),size);

      for (Node::NodeList::const_iterator iter=node->GetChildren().begin(); iter!=node->GetChildren().end(); ++iter) {
        Node* sub=*iter;

        if (sub->HasValue() && !sub->HasMultilineValue()) {
          formatter << XMLFormatter::NoEscapes << chSpace;
          formatter << WStringToChar16(sub->GetName(),size);
          formatter << chEqual << chDoubleQuote;
          formatter << XMLFormatter::AttrEscapes << WStringToChar16(sub->GetString(),size);
          formatter << XMLFormatter::NoEscapes << chDoubleQuote;
        }
      }

      size_t subCount=0;
      for (Node::NodeList::const_iterator iter=node->GetChildren().begin(); iter!=node->GetChildren().end(); ++iter) {
        Node* sub=*iter;

        if (!sub->HasValue() || sub->HasMultilineValue()) {
          subCount++;
        }
      }

      if (subCount==0 && !node->HasMultilineValue()) {
        formatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle << chLF;
      }
      else {
        formatter << chCloseAngle;

        if (!node->HasMultilineValue()) {
          formatter << chLF;
        }

        for (Node::NodeList::const_iterator iter=node->GetChildren().begin(); iter!=node->GetChildren().end(); ++iter) {
        Node* sub=*iter;

          if (!sub->HasValue() || sub->HasMultilineValue()) {
            WriteNode(sub,depth+2,formatter);
          }
        }

        if (node->HasMultilineValue()) {
          formatter << XMLFormatter::AttrEscapes << WStringToChar16(node->GetString(),size);
        }
        else {
          for (size_t x=0; x<depth; x++) {
            formatter << XMLFormatter::NoEscapes << chSpace;
          }
        }

        formatter << XMLFormatter::NoEscapes << chOpenAngle << chForwardSlash;
        formatter << WStringToChar16(node->GetName(),size);
        formatter << chCloseAngle << chLF;
      }
    }

    bool SaveConfigToXMLFile(const std::wstring& filename,Node* node)
    {
      XMLFormatTarget* target=NULL;
      XMLFormatter*    formatter=NULL;

      try {
        XMLPlatformUtils::Initialize();

        std::string xmlFile=Base::WStringToString(filename);

        target=new LocalFileFormatTarget(xmlFile.c_str());
        formatter=new XMLFormatter("UTF-8",NULL,target);

        (*formatter) << gXMLDecl1 << formatter->getEncodingName() << gXMLDecl2;

        WriteNode(node,0,*formatter);

        target->flush();

        delete formatter;

        XMLPlatformUtils::Terminate();
      }
      catch (const XMLException& toCatch) {
        //errorList.push_back(Error(Base::Char16ToWString((const char*)toCatch.getMessage(),
        //                                                XMLString::stringLen(toCatch.getMessage()))));
        delete formatter;
        return false;
      }

      return true;
    }

#else

    bool SaveConfigToXMLFile(const std::wstring& filename,Node* node)
    {
      return false;

    }
#endif

  }
}

