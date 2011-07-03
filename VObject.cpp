/*
 *  VObject.cpp
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/28.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "VObject.h"
#include "debug.h"
#include <sstream>


static unsigned int numTotalVObjects = 0;   // Number of VObjects.
static unsigned int numTotalObjects = 0;    // Number of Object typed VObjects.
static unsigned int numTotalArrays = 0;     // Number of Array typed VObjects.

VObject::VObject() 
{
    reserved = NULL;
    m_vobjectIdentifier = numTotalVObjects++;
    m_value.stringValue = NULL;
    m_type = NullType;      // Default is an null object.
    m_typeinfo = "Object";  // m_type is how to tell the type of VObject, but m_typeinfo may not be set by parser.
                            // primitive values has no m_typeinfo.
}

VObject::~VObject()
{
    switch (m_type) {
        case NullType:
        case IntType:
        case UIntType:
        case DoubleType:
            //fprintf(stderr, "(%d) Primitive value done\n", m_vobjectIdentifier);
            break;
        case StringType:
            //fprintf(stderr, "(%d) String(%s) done\n", m_vobjectIdentifier, m_value.stringValue->c_str());
            delete m_value.stringValue;
            break;
        case ObjectType:
            //fprintf(stderr, "(%d) deleting Object\n", m_vobjectIdentifier);
            // delete properties.
            for(std::vector<Property>::iterator it = m_propertyList.begin(); it != m_propertyList.end(); it++) {
                //fprintf(stderr, ".%s\n", it->name.c_str());
                delete it->value;
            }
            //fprintf(stderr, "(%d) Object deleted\n", m_vobjectIdentifier);
            break;
        case ArrayType:
            //fprintf(stderr, "(%d) deleting Array\n", m_vobjectIdentifier);
            // delete properties.
            for(std::vector<Property>::iterator it = m_propertyList.begin(); it != m_propertyList.end(); it++) {
                //fprintf(stderr, "[%s]\n", it->name.c_str());
                delete it->value;
            }
            //fprintf(stderr, "(%d) Object deleted\n", m_vobjectIdentifier);
            break;
        case StreamType:
            free(m_value.streamValue);
        default:
            break;
    }
    /*
    if (m_type == StringType) {
        fprintf(stderr, "String(%s)\n", m_value.stringValue->c_str());
        delete m_value.stringValue;
    }
    if ((m_type == ObjectType) || (m_type == ArrayType)) {        
        if (m_type == ObjectType) {
            fprintf(stderr, "Object\n");
        } else {
            fprintf(stderr, "Array\n");
        }
        // delete properties.
        for(std::vector<Property>::iterator it = m_propertyList.begin(); it != m_propertyList.end(); it++) {
            fprintf(stderr, "property %s\n", it->name.c_str());
            delete it->value;
        }
    }
    */
    
    numTotalVObjects--;
    
    /*
    if (numTotalVObjects == 0) {
        fprintf(stderr, "VObject memory cleaned\n");
    }
    */
}

unsigned int VObject::numVObjects()
{
    return numTotalVObjects;
}

unsigned int VObject::numObjects()
{
    return numTotalObjects;
}

unsigned int VObject::numArrays()
{
    return numTotalArrays;
}

void VObject::resetCounters()
{
    numTotalArrays   = 0;
    numTotalVObjects = 0;
    numTotalObjects  = 0;
}

void VObject::checkTypeChange(VObjectType newType) 
{
    // NullType -> newType is ok. just return.
    if (!m_type)
        return;
    
    if (m_type != newType) {
        fprintf(stderr, "Warnning: Type changed from (%d -> %d)... something goes wrong.\n", m_type, newType);
        abort();
    }
}

const char*  VObject::toString()
{
    char buff[128];
    bzero(buff, 128);
    switch (m_type) {
        case NullType:
            snprintf (buff, 127, "NULL");
            break;
        case StringType:
            return asString();
        case IntType:
        case UIntType:
            snprintf (buff, 127, "%d", asInt());
            break;
        case DoubleType:
            snprintf (buff, 127, "%g", asDouble());
            break;
        case StreamType:
            snprintf (buff, 127, "Binary (%d bytes)", m_value.streamValue->length());
            break;
        default:
            fprintf(stderr, "Error: Non-defined Type\n");
            abort();
            break;
    }
    m_toString = buff;
    //fprintf(stderr, "%d: %s : %s\n", m_type, m_typeinfo.c_str(), m_toString.c_str());
    return m_toString.c_str();
}

// Primitive values constructor
/*
VObject::VObject(signed int value)
{
    m_value.stringValue = 0;
    m_type = IntType;
    m_value.intValue = value;
}

VObject::VObject(unsigned int value)
{
    m_value.stringValue = 0;
    m_type = UIntType;
    m_value.uintValue = value;
}

VObject::VObject(char *value)
{
    m_type = StringType;
    m_value.stringValue = new std::string(value);   // don't forget to free stringValue once the type is changed
}

VObject::VObject(std::string &value)
{
    m_type = StringType;
    m_value.stringValue = new std::string(value);   // don't forget to free stringValue once the type is changed
}
*/


// operators
// TODO: All the "name" could be stored in a hash table to reduce the memory cost and also optimize the performance instead of string compare.
VObject &
VObject::operator[](const char *name)
{
    checkTypeChange(ObjectType);
    
    if (m_type == NullType) {
        m_type = ObjectType;
        numTotalObjects++;
        //fprintf(stderr, "type = %s\n", m_typeinfo.c_str());
    }
    
    // try to find the name exist or not.
    VObject *target = getProperty(name);
    if (target) {
        return *target;
    }
    
    // create the new property and return.
    Property *prop = new Property();
    prop->name  = name;
    prop->value = new VObject();
    
    // link into our property list
    m_propertyList.push_back(*prop);
    
    return *(prop->value);
}

VObject &
VObject::operator[](int arrayIndex)
{
    return operator[]((unsigned int)arrayIndex);
}    

VObject &
VObject::operator[](unsigned int arrayIndex)
{
    checkTypeChange(ArrayType);
    
    if (m_type == NullType) {
        m_type = ArrayType;
        numTotalArrays++;
    }
    
    std::stringstream indexName;
    indexName << arrayIndex;
    
    std::string name;
    name += indexName.str();
    
    // try to find the name exist or not.
    VObject *target = getProperty(name.c_str());
    
    if (target)
        return *target;
    
    // create the new property and return.
    Property *prop = new Property();
    prop->name  = name;
    prop->value = new VObject();
    
    // link into our property list
    m_propertyList.push_back(*prop);
    
    return *(prop->value);
}

VObject*
VObject::getProperty(const char *name)
{
    for(std::vector<Property>::iterator it = m_propertyList.begin(); it != m_propertyList.end(); it++) {
        if (it->name == name)
            return it->value;
    }
    return NULL;
}

VObject &
VObject::operator=(char *value)
{
    checkTypeChange(StringType);
    
    // check string reassignment... this should not happened so far.
    if (m_type == StringType) {
        fprintf(stderr, "Assert: String are reassigned.. should not happened.\n");
        abort();
    }
    
    m_value.stringValue = new std::string(value);
    m_type = StringType;
    return (*this);
}

VObject &
VObject::operator=(unsigned int value)
{
    checkTypeChange(UIntType);
    
    m_value.uintValue = value;
    m_type = UIntType;
    return (*this);
}

VObject &
VObject::operator=(signed int value)
{
    checkTypeChange(IntType);
    
    m_value.intValue = value;
    m_type = IntType;
    return (*this);
}

VObject &
VObject::operator=(float value)
{
    (*this) = (double) value;   // use operator=(double) overloading here instead of reimplement the same code.
    return (*this);
}

VObject &
VObject::operator=(double value)
{
    checkTypeChange(DoubleType);
    
    m_value.doubleValue = value;
    m_type = DoubleType;
    return (*this);
}

/*
VObject &
VObject::operator=(void *value)
{
    
    checkTypeChange(StreamType);
    
    m_value.streamValue = value;
    m_type = StreamType;
    return (*this);
}
*/

VObject &
VObject::operator=(MemoryStream *value)
{
    
    checkTypeChange(StreamType);
    
    m_value.streamValue = value;
    m_type = StreamType;
    return (*this);
}

std::string& VObject::toJSON(std::string &str)
{
    std::string json_str = "";
    char value_str[30];
    
    if(isValue()) {
        switch (m_type) {
            case NullType:
                json_str += "null";
                break;
            case StringType:
                json_str += '"';
                
                /* Encode to standard JSON string format, but don't care control characters */
                for (const char *c = (*m_value.stringValue).c_str(); *c != 0; c++) {
                    switch (*c) {
                        case '\"':
                            json_str += "\\\"";
                            break;
                        case '\\':
                            json_str += "\\\\";
                            break;
                        case '/':
                            json_str += "\\/";
                            break;
                        case '\b':
                            json_str += "\\b";
                            break;
                        case '\f':
                            json_str += "\\f";
                            break;
                        case '\n':
                            json_str += "\\n";
                            break;
                        case '\r':
                            json_str += "\\r";
                            break;
                        case '\t':
                            json_str += "\\t";
                            break;
                        default:
                            if (((unsigned char)(*c) <= 0x1f) || ((unsigned char)(*c) > 127)) {
                                sprintf(value_str, "\\u00%02x", (unsigned char)(*c));
                                json_str += value_str;
                            } else {
                                json_str += (*c);
                            }
                            break;
                    }
                }
                
                json_str += '"';
                break;
            case IntType:
                sprintf(value_str, "%d", m_value.intValue);
                json_str += value_str;
                break;
            case UIntType:
                sprintf(value_str, "%d", m_value.intValue);
                json_str += value_str;
                break;
            case DoubleType:
                sprintf(value_str, "%e", m_value.doubleValue);
                json_str += value_str;
                break;
            default:
                ASSERT(1);
                break;
        }
    } else {
        if (isObject())
            json_str += "{";
        else
            json_str += "[";
        
#ifdef DEBUG
        if (m_typeinfo.length()) {
            json_str += " /* " + m_typeinfo + " */";
        }
#else
        if (m_typeinfo.length()) {
            json_str += "\n\"__TYPE_INFO__\" : \"" + m_typeinfo + "\",";
        }
#endif

        json_str += "\n";

        // get properties.toJSON
        for(std::vector<Property>::iterator it = m_propertyList.begin(); it != m_propertyList.end(); it++) {
            if (isObject()) {
                json_str += '"' + it->name + '"';
                json_str += " : ";
            }
            
            VObject *value = it->value;
            value->toJSON(json_str);
            if (it != (m_propertyList.end() - 1)) {
                json_str += ",";
            }

#ifdef DEBUG
            if (value->isValue()) { // append comment on this value
                if (value->hasTypeInfo()) {
                    json_str += " /* ";
                    json_str += value->getTypeInfo();
                    json_str += " */";
                }
            }
#endif            
            json_str += "\n";
            
        }
        
        if (isObject())
            json_str += "}";
        else
            json_str += "]";
    }
    str += json_str;
    
    return str;
}


