// Copyright (c) 2015, Baptiste Burles
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of Baptiste Burles nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Baptiste Burles AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef babextended_jsontranslator_h
#define babextended_jsontranslator_h


#include <iostream>

/** Json translator object
 *
 * How to use it:
 *
 * JsonTranslator<YourClass> * jsonLoader = new JsonTranslator<YourClass>();
 * jsonLoader->add("slope", &YourClass::aMember);
 * jsonLoader->add("release", &YourClass::anotherMember);
 *
 * YourClass myClass;
 * jsonLoader->loadFromFile(path-to-json-file, myClass);
 *
 * After that, your object is loaded from the json file.
 *
 */
template <class T>
class JsonTranslator
{
public:
    typedef bool T::*BoolMember;
    typedef int32_t T::*IntMember;
    typedef float T::*FloatMember;
    typedef std::string T::*StringMember;
    
    
    typedef Vector<bool> T::*BoolArrayMember;
    typedef Vector<int> T::*IntArrayMember;
    typedef Vector<float> T::*FloatArrayMember;
    typedef Vector<std::string> T::*StringArrayMember;
    
    JsonTranslator();
    JsonTranslator(const char * name);
    
    ~JsonTranslator();
    
    void add(const char* name, StringMember member);
    void add(const char* name, BoolMember member);
    void add(const char* name, IntMember member);
    void add(const char* name, FloatMember member);
    
    bool loadFromFile(const char * filename, T & values);
    bool loadFromBuffer(char * buffer, T & values);
    
private:
    template <class Type>
    struct DataObject
    {
        std::string name;
        Type        address;
        
        DataObject(std::string n, Type a)
        {
            name = n;
            address = a;
        }
    };
    
    std::string                         mJsonName;
    Vector<DataObject<BoolMember> *> *  mBooleans;
    Vector<DataObject<IntMember> *> *   mInts;
    Vector<DataObject<FloatMember> *> * mFloats;
    Vector<DataObject<StringMember> *> * mStrings;
    
};


#include "jsontranslatorimpl.h"


#endif
