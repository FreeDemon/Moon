// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_UTIL_PROPERTIES_I_H
#define ICE_UTIL_PROPERTIES_I_H

#include <IceUtil/Mutex.h>
#include <IceUtil/StringConverter.h>

#include <map>

namespace IceUtil
{

class ICE_UTIL_API Properties :  virtual public IceUtil::Shared, public IceUtil::Mutex
{
public:
    
    Properties();
    std::string getProperty(const std::string&);
    std::string getPropertyWithDefault(const std::string&, const std::string&);
    int getPropertyAsInt(const std::string&);
    int getPropertyAsIntWithDefault(const std::string&, int);
    std::vector<std::string> getPropertyAsList(const std::string&);
    std::vector<std::string> getPropertyAsListWithDefault(const std::string&, const std::vector<std::string>&);

    void setProperty(const std::string&, const std::string&);
    void load(const std::string&);

private:

    void parseLine(const std::string&, const IceUtil::StringConverterPtr&);

    void loadConfig();

    struct PropertyValue
    {
        PropertyValue() :
            used(false)
        {
        }

        PropertyValue(const std::string& v, bool u) :
            value(v),
            used(u)
        {
        }

        std::string value;
        bool used;
    };
    std::map<std::string, PropertyValue> _properties;
    const IceUtil::StringConverterPtr _converter;
};


typedef IceUtil::Handle<Properties> PropertiesPtr;

}

#endif
