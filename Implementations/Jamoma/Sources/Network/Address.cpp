#include "Network/Address.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{
  
private:
  
  // Implementation specific
  mutable TTObject            mData;
  mutable AddressValue *      mValue;
  AddressValue::Type          mValueType;
  AccessMode                  mAccessMode;
  BoundingMode                mBoundingMode;
  bool                        mRepetitionFilter;
  
  shared_ptr<Device>          device;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAddress(TTObject aData = TTObject()) :
  mData(aData),
  mValueType(AddressValue::Type::NONE),
  mAccessMode(AccessMode::BI),
  mBoundingMode(BoundingMode::FREE),
  mRepetitionFilter(false)
  {
    // edit value type, access mode, bounding mode and repetition filter attribute
    if (mData.valid())
    {
      TTSymbol objectName = mData.name();
      
      if (objectName == kTTSym_Mirror)
        objectName = TTMirrorPtr(mData.instance())->getName();
      
      if (objectName == "Data")
      {
        TTSymbol type;
        mData.get("type", type);
        
        if (type == kTTSym_none)
        {
          mValue = new AddressValue();
          mValueType = AddressValue::Type::NONE;
        }
        else if (type == kTTSym_generic)
        {
          // todo : do i need to create an array of AddressValue ?
          mValueType = AddressValue::Type::TUPLE;
        }
        else if (type == kTTSym_boolean)
        {
          mValue = new OSSIA::Bool(false);
          mValueType = AddressValue::Type::BOOL;
        }
        else if (type == kTTSym_integer)
        {
          mValue = new OSSIA::Int(0);
          mValueType = AddressValue::Type::INT;
        }
        else if (type == kTTSym_decimal)
        {
          mValue = new OSSIA::Float(0.);
          mValueType = AddressValue::Type::FLOAT;
        }
        else if (type == kTTSym_array)
        {
          // todo : do i need to create an array of AddressValue ?
          mValueType = AddressValue::Type::TUPLE;
        }
        else if (type == kTTSym_string)
        {
          mValue = new OSSIA::String("");
          mValueType = AddressValue::Type::STRING;
        }
        
        TTSymbol service;
        mData.get("service", service);
        
        if (service == kTTSym_parameter)
          mAccessMode = AccessMode::BI;
        else if (service == kTTSym_message)
          mAccessMode = AccessMode::SET;
        else if (service == kTTSym_return)
          mAccessMode = AccessMode::GET;
        
        TTSymbol clipmode;
        mData.get("rangeClipmode", clipmode);
        
        if (clipmode == kTTSym_none)
          mBoundingMode = BoundingMode::FREE;
        else if (clipmode == kTTSym_low)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_high)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_both)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_wrap)
          mBoundingMode = BoundingMode::WRAP;
        else if (clipmode == kTTSym_fold)
          mBoundingMode = BoundingMode::FOLD;
        
        mData.get("repetitionFilter", mRepetitionFilter);
        
        // enable callback to be notified each time the value change
        TTObject    callback("callback");
        TTValue     args(TTPtr(this), mData);
        callback.set("baton", args);
        callback.set("function", TTPtr(&JamomaAddress::ValueCallback));
        
        TTAttributePtr attribute;
        mData.instance()->findAttribute("value", &attribute);
        attribute->registerObserverForNotifications(callback);
      }
    }
  }
  
  virtual ~JamomaAddress()
  {}
  
# pragma mark -
# pragma mark Value
  
  virtual bool updateValue() const override
  {
    TTValue v;
    mData.get("value", v);
    
    // clear former value
    delete mValue;
    
    // create new value and fill it
    switch (mValueType)
    {
      case AddressValue::Type::NONE :
      {
        mValue = new OSSIA::None();
        return v.size() == 0;
      }
      case AddressValue::Type::BOOL :
      {
        if (v.size() == 1)
        {
          mValue = new OSSIA::Bool(v[0]);
          return true;
        }
        break;
      }
      case AddressValue::Type::INT :
      {
        if (v.size() == 1)
        {
          mValue = new OSSIA::Int(v[0]);
          return true;
        }
        break;
      }
      case AddressValue::Type::FLOAT :
      {
        if (v.size() == 1)
        {
          mValue = new OSSIA::Float(v[0]);
          return true;
        }
        break;
      }
      case AddressValue::Type::CHAR :
      {
        if (v.size() == 1)
        {
          if (v[0].type() == kTypeString)
          {
            char* c_value = TTString(v[0]).data();
            mValue = new OSSIA::Char(c_value[0]);
            return true;
          }
        }
        break;
      }
      case AddressValue::Type::STRING :
      {
        if (v.size() == 1)
        {
          if (v[0].type() == kTypeSymbol)
          {
            TTSymbol s_value = v[0];
            mValue = new OSSIA::String(s_value.c_str());
            return true;
          }
        }
        break;
      }
      case AddressValue::Type::TUPLE :
      {
        ; // todo
      }
      case AddressValue::Type::GENERIC :
      {
        ; // todo
      }
    }

    return false;
  }
  
  virtual AddressValue * getValue() const override
  {
    updateValue();
    return mValue;
  }
  
  virtual bool sendValue(const AddressValue * value) const override
  {
    TTValue v;
    
    // convert AddressValue into TTValue
    if (value->getType() == AddressValue::Type::NONE)
    {
      ;
    }
    else if (value->getType() == AddressValue::Type::BOOL)
    {
      Bool * b = (Bool*)value;
      v = TTBoolean(b->value);
    }
    else if (value->getType() == AddressValue::Type::INT)
    {
      Int * i = (Int*)value;
      v = TTInt32(i->value);
    }
    else if (value->getType() == AddressValue::Type::FLOAT)
    {
      Float * f = (Float*)value;
      v = TTFloat64(f->value);
    }
    else if (value->getType() == AddressValue::Type::CHAR)
    {
      ; // todo
    }
    else if (value->getType() == AddressValue::Type::STRING)
    {
      String * s = (String*)value;
      v = TTSymbol(s->value);
    }
    else if (value->getType() == AddressValue::Type::TUPLE)
    {
      ; // todo
    }
    else if (value->getType() == AddressValue::Type::GENERIC)
    {
      ; // todo
    }
    
    if (mData.name() == "Data")
      return !mData.send("Command", v);
    else
      return !mData.set("value", v);
  }

# pragma mark -
# pragma mark Network
  
  virtual const std::shared_ptr<Device> & getDevice() const override
  {
    return device;
  }

# pragma mark -
# pragma mark Accessors
  
  virtual AddressValue::Type getValueType() const override
  {
    return mValueType;
  }
  
  virtual AccessMode getAccessMode() const override
  {
    return mAccessMode;
  }
  
  virtual Address & setAccessMode(AccessMode) override
  {
    // note : it is not possible to change the service attribute of Data after its creation
    return *this;
  }
  
  virtual BoundingMode getBoundingMode() const override
  {
    return mBoundingMode;
  }
  
  virtual Address & setBoundingMode(BoundingMode) override
  {
    if (mBoundingMode == BoundingMode::FREE)
      mData.set("rangeClipmode", kTTSym_none);
      else if (mBoundingMode == BoundingMode::CLIP)
        mData.set("rangeClipmode", kTTSym_both);
        else if (mBoundingMode == BoundingMode::WRAP)
          mData.set("rangeClipmode", kTTSym_wrap);
          else if (mBoundingMode == BoundingMode::FOLD)
            mData.set("rangeClipmode", kTTSym_fold);
            
            return *this;
  }
  
  virtual bool getRepetitionFilter() const override
  {
    return mRepetitionFilter;
  }
  
  virtual Address & setRepetitionFilter(bool repetitionFilter) override
  {
    mData.set("repetitionFilter", repetitionFilter);
    
    return *this;
  }
  
# pragma mark -
# pragma mark Expression
  
  virtual bool evaluate() const override
  {
    return false;
  }
  
# pragma mark -
# pragma mark Implementation specific
  
private:
  
  static TTErr ValueCallback(const TTValue& baton, const TTValue& value)
  {
    JamomaAddress * self = static_cast<JamomaAddress*>(TTPtr(baton[0]));
    TTObject aData = baton[1];
    
    // check data object
    if (aData.instance() == self->mData.instance())
    {
      // print the returned value
      TTLogMessage("address has been updated to %s \n", value.toString().data());
      return kTTErrNone;
    }
    
    return kTTErrGeneric;
  }
};
