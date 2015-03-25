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


/** Json writer object
 *
 */
class JsonWriter
{
public:
    
    cJSON * root;
    
    JsonWriter()
    {
        root = 0;
    }
    
    // Begin the object serialization
    void begin()
    {
        root = cJSON_CreateObject();
    }
    
    // End the serialization and get the json buffer
    char * end()
    {
        char * jsonOutput = cJSON_Print(root);
        cJSON_Delete(root);
        
        return jsonOutput;
    }
};


/** Json translator object
 *
 * How to use it:
 *
 * JsonTranslator<YourClass> * jsonYourClass = new JsonTranslator<YourClass>();
 * jsonYourClass->add("slope", &YourClass::aMember);
 * jsonYourClass->add("release", &YourClass::anotherMember);
 *
 * Then, load your object from a JSON file :
 *
 * YourClass myObject;
 * jsonYourClass->loadFromFile(path-to-json-file, myObject);
 *
 * After that, your object is loaded from the json file.
 *
 * How to save:
 * JsonWrite writer;
 * writer.begin();
 * jsonYourClass->save(writer, myObject);
 * char * json = writer.end();
 *
 * Then you can write to disk or send the json buffer.
 *
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
    
    // Create a new object with JSON root
    JsonTranslator();
    
    // Create a new object with this JSON item name
    JsonTranslator(const char * jsonItemName);
    
    ~JsonTranslator();
    
    void add(const char* name, StringMember member);
    void add(const char* name, BoolMember member);
    void add(const char* name, IntMember member);
    void add(const char* name, FloatMember member);
    
    // Load a single value from JSON
    bool loadFromFile(const char * filename, T & values);
    bool loadFromBuffer(const char * buffer, T & values);

    // Load an array of values from JSON
    bool loadArrayFromBuffer(const char * buffer, Vector<T> & values);
    bool loadArrayFromFile(const char * filename, Vector<T> & values);
    
    // Save value to a json write object
    void save(JsonWriter & state, T & values);
    void saveArray(JsonWriter & state, Vector<T> & values);
    
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
    
    
    // de la merde si il faut charger des gros trucs...
    
    void loadIntMembers(cJSON* item, T & object);
    void loadBoolMembers(cJSON* item, T & object);
    void loadFloatMembers(cJSON* item, T & object);
    void loadStringMembers(cJSON* item, T & object);
    
    
    void saveIntMembers(cJSON* item, T & object);
    void saveBoolMembers(cJSON* item, T & object);
    void saveFloatMembers(cJSON* item, T & object);
    void saveStringMembers(cJSON* item, T & object);
    
};


#include "jsontranslatorimpl.h"


#endif
