/*
 *  VObject.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/28.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

/*
 Design Concept:
 1. Everything is an object. (include Object and values, except the name of a property)
 2. The name of a property is a key to look for the corresponding object (which may be a value, an array, or an object)
 
 
 Thinkings:
 VObject provides a tree view of the stream. It's better not to allocated it's own memory.
 Just point to the address of exactly where the data in the stream since stream is alread allocated memory there.
 
 In the future that user may modify VObject as editing in visual way. VObjects can be marked as dirty and also allocate the memory it has to.
 */ 
 
 // TODO:
 // Optimization:
 // String data is also interesting.
 // Hash table to store the string to use identifier instead of string compare function. (much faster)


#include <vector>
#include <string>

#include "Stream.h"

#ifndef _VOBJECT_H
#define _VOBJECT_H

enum VObjectType {
    NullType,       // undefined
    IntType,
    UIntType,
    DoubleType,
    StringType,
    ObjectType,
    ArrayType,
    StreamType
    // TODO: 
    // 1. StreamType (Such as video, audio and image streams) with (void *start and uint length)
    // or Devided to different types?
    // How user assign the stream pointer and the length to VObject?
    // - VObject a;
    // a.asStream((void *)start, uint length); // seems possible
    //
    // 2. FunctionType?
    // Just like ECMAScript which defined Function object.
    // How to define an function object?
    // - (void *) as a function pointer.
    // If this is not designed for SWF, may be possible to be used to indicate a native compiled function.
    // Maybe interesting as a way to visual programming. (Since function is also an object that everything may be convertable in a higher level abstraction programming.)
    // All data in the memory can be visualized.
};

class VObject;

// name strings are actually stored in Property class.
class Property {
public:
    std::string name;
    VObject    *value;
};

typedef std::vector<Property>           PropertyList;

// Design Concept : Everything is an object. (objects, properties and values are all objects)
// Extensibility for Visualization. "V" stands for visualization.

// 1. as Object or Array
//    - properties (since this must be also somebody's property)
//    - className
//    - 
//    -
// 2. as value (name, value)
//    - comment

/*
 example:
    (global).swf.version = "abc";
    ^^^^^^ object that has a property ("swf", value : Object), but no without a name (global is a variable name in the program, not a visible script name)
             ^^^ object has a property ("version", value : Object), value : Object.type is String.
                 ^^^^^^^ an object that is an string value actually. (This is seen as a value instead of a object. but it's ok and easy to change it to object type)
 */

// Considerations:
// 1. Memory management : Allocation/Free
// 2. Reference count
// 3. operator overloading on extensible values
// 4. primitive values exchange
class   VObject {
public:
    VObject();
    // TODO:
    ~VObject(); // destructor to clean the allocated memory.
    /* not used so far
    VObject::VObject(signed int value);
    VObject::VObject(unsigned int value);
    VObject::VObject(char *value);
    VObject::VObject(std::string &value);
     */
    
    void        checkTypeChange(VObjectType newType); // check the type change is allowed?? so far, it should only happened from NullType to other Types.
    
    // It's possible to set comment on both object and property
    // comment on object => { /* comment */
    //                      }
    // comment on array  => name : [ /* comment */
    //                      ]
    // comment on property => name : value (this is a primitive value), /* comment */
    //
    
    /*
    void        setComment(char *comment) { m_comment = comment; };
    int         hasComment() { return m_comment.length(); };
    const char* getComment() { return m_comment.c_str(); };
     */
    
    void        setTypeInfo(const char *typeinfo) { m_typeinfo = typeinfo; };
    int         hasTypeInfo() { return m_typeinfo.length(); };
    const char* getTypeInfo() { return m_typeinfo.c_str(); };
    
    
    void        setClassName(char *className) { m_className = className; }; 
    const char* getClassName() { return m_className.c_str(); };
    
    
    
    int         isValue() { return (m_type != ArrayType) && (m_type != ObjectType); }; // true if this is a primitive value
    int         isObject() { return m_type == ObjectType; }; // true if this is an object
    int         isArray() { return m_type == ArrayType; }; // true if this is an array
    
    void        setType(VObjectType newType) { m_type = newType; }; // FIXME, typeChange() should be called. to free not used memory.
    int         getType() { return m_type; };
    
    //void        setStreamLength(unsigned int length) { m_stream_length = length; };
    //unsigned int getStreamLength() { return m_stream_length; };
    
    
    signed int   asInt() { return m_value.intValue; };
    unsigned int asUInt() { return m_value.uintValue; };
    double       asDouble() { return m_value.doubleValue; };
    const char*  asString() { return m_value.stringValue->c_str(); };
    MemoryStream *asStream() { return m_value.streamValue; };
    
    const char*  toString();
    
    
    // TODO: These should be static functions?
    unsigned int    numVObjects();
    unsigned int    numObjects();
    unsigned int    numArrays();
    void            resetCounters();
    
    PropertyList*   getPropertyList() { return &m_propertyList; };
    unsigned int    length() { return m_propertyList.size(); };     // used as the length of an Array or the number of properties in an Object.
    
    std::string&    toJSON(std::string &str);
    
    /* Reserved for user to use */
    void        *reserved;
    
    /*
     operator=()    // setter
     operator++()   // setter
     operator--()   // setter
     operator[]()   // setter or getter (only for object type)
     */
    VObject&        operator[](const char *name);
    VObject&        operator[](unsigned int arrayIndex);
    VObject&        operator[](int arrayIndex);

    VObject&        operator=(char *value);
    VObject&        operator=(unsigned int value);
    VObject&        operator=(signed int value);
    VObject&        operator=(float value);
    VObject&        operator=(double value);
    VObject&        operator=(MemoryStream *value); // for StreamType


    
private:

    VObject*        getProperty(const char *name);

    union           VValue {
        unsigned    int     uintValue;
        signed      int     intValue;
        double              doubleValue;
        std::string        *stringValue;
        VObject            *objectValue;
        MemoryStream       *streamValue;
    };
    
    VValue          m_value;            // used if this is a property
    
    std::string     m_toString;         // used by toString(), callers don't need to allocate the string memory by themself.
    
    VObjectType     m_type;
    
    std::string     m_comment;
    std::string     m_typeinfo;
    
    std::string     m_className;      // used if this is an object or array (better to consider as an pointer to the static string hash table)
    PropertyList    m_propertyList;   // used if this is an object or array
    
    unsigned int    m_vobjectIdentifier;    // TODO: used as VObject identifier can be used as internal referenece.
};

#endif // _VOBJECT_H
