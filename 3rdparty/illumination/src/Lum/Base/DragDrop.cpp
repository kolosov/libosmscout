#include <Lum/Base/DragDrop.h>

namespace Lum {
  namespace Base {

    Datatype::Datatype(Type type, SubType subType)
    : type(type),subType(subType)
    {
      // no code
    }

    void Datatype::SetType(Type type, SubType subType)
    {
      this->type=type;
      this->subType=subType;
    }

    Datatype::Type Datatype::GetType() const
    {
      return type;
    }

    Datatype::SubType Datatype::GetSubType() const
    {
      return subType;
    }

    bool Datatype::Matches(Type type) const
    {
      return this->type==type;
    }

    bool Datatype::Matches(Type type, SubType subType) const
    {
      return this->type==type && this->subType==subType;
    }

    Data::Data()
    {
      // no code
    }

    Data::Data(const std::wstring& text)
     : text(text)
    {
      // no code
    }

    void Data::SetText(const std::wstring& text)
    {
      this->text=text;
    }

    std::wstring Data::GetText() const
    {
      return text;
    }

    DataExchangeObject::~DataExchangeObject()
    {
      // no code
    }

    bool DataExchangeObject::GetSupportedTypes(DatatypeList& types, ExchangeAction action) const
    {
      return false;
    }

    bool DataExchangeObject::PostprocessAction(ExchangeAction action)
    {
      return true;
    }

    bool DataExchangeObject::CanExecuteAction(const DatatypeList& types,
                                              ExchangeAction action) const
    {
      for (Base::DatatypeList::const_iterator iter=types.begin(); iter!=types.end(); ++iter) {
        if (CanExecuteAction(*iter,action)) {
          return true;
        }
      }

      return false;
    }

    bool DataExchangeObject::CanExecuteAction(const Datatype& type, ExchangeAction action) const
    {
      return false;
    }

    bool DataExchangeObject::ExecuteAction(const Datatype& type, Data& data, ExchangeAction action)
    {
      return false;
    }

    void DataExchangeObject::Deselect()
    {
      // no code
    }
  }
}


