#ifndef LUM_BASE_DRAGDROP_H
#define LUM_BASE_DRAGDROP_H

#include <string>
#include <vector>

#include <Lum/Base/Model.h>

namespace Lum {
  namespace Base {

    class LUMAPI Datatype
    {
    public:
      enum Type {
        none,

        application,
        audio,
        image,
        text,
        video
      };

      enum SubType {
        // none, application
        octectStream,

        // text
        plain
      };

    private:
      Type    type;
      SubType subType;

    public:
      Datatype(Type type, SubType subType);

      void SetType(Type type, SubType subType);

      Type GetType() const;
      SubType GetSubType() const;
      bool Matches(Type type) const;
      bool Matches(Type type, SubType subType) const;
    };

    typedef std::vector<Datatype> DatatypeList;

    class LUMAPI Data
    {
    private:
      std::wstring text;

    public:
      Data();
      Data(const std::wstring& text);

      // setter
      void SetText(const std::wstring& text);

      // getter
      std::wstring GetText() const;
    };

    /**
    DnDObject holds the data content that is part of a drag or drop
    transaction.
    */
    class LUMAPI DataExchangeObject : public MsgObject
    {
    public:
      enum ExchangeAction {
        actionMove,  // Data should be moved (cut)            Read
        actionCopy,  // Data should be copied (copy)          Read
        actionLink,  // Data should be linked or referenced   Write
        actionInsert // Data should be inserted (paste)       Write
      };

    public:
      virtual ~DataExchangeObject();
      virtual bool GetSupportedTypes(DatatypeList& types, ExchangeAction action) const;
      virtual bool PostprocessAction(ExchangeAction action);
      virtual bool CanExecuteAction(const Datatype& type, ExchangeAction action) const;
      virtual bool CanExecuteAction(const DatatypeList& types, ExchangeAction action) const;
      virtual bool ExecuteAction(const Datatype& type, Data& data, ExchangeAction action);
      virtual void Deselect();
    };
  }
}

#endif
