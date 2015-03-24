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

#ifndef babextended_jsontranslatorimpl_h
#define babextended_jsontranslatorimpl_h

template <class T>
JsonTranslator<T>::JsonTranslator() : JsonTranslator("")
{
}

template <class T>
JsonTranslator<T>::JsonTranslator(const char * name)
{
    mJsonName = name;
    mBooleans = new Vector<DataObject<BoolMember> *>;
    mInts = new Vector<DataObject<IntMember> *>;
    mFloats = new Vector<DataObject<FloatMember> *>;
    mStrings = new Vector<DataObject<StringMember> *>;
}

template <class T>
JsonTranslator<T>::~JsonTranslator()
{
    delete mBooleans;
    delete mInts;
    delete mFloats;
    delete mStrings;
}

template <class T>
void JsonTranslator<T>::add(const char* name, StringMember member)
{
    DataObject<StringMember> * b = new DataObject<StringMember>(name, member);
    mStrings->push(b);
}

template <class T>
void JsonTranslator<T>::add(const char* name, BoolMember member)
{
    DataObject<BoolMember> * b = new DataObject<BoolMember>(name, member);
    mBooleans->push(b);
}

template <class T>
void JsonTranslator<T>::add(const char* name, IntMember member)
{
    DataObject<IntMember> * b = new DataObject<IntMember>(name, member);
    mInts->push(b);
}

template <class T>
void JsonTranslator<T>::add(const char* name, FloatMember member)
{
    DataObject<FloatMember> * b = new DataObject<FloatMember>(name, member);
    mInts->push(b);
}


template <class T>
bool JsonTranslator<T>::loadFromBuffer(char * buffer, T & values)
{
    cJSON *root = cJSON_Parse(buffer);
    if (root==0)
    {
        Log::global()->error("failed to parse json buffer !\n");
        return false;
    }
    
    cJSON* jsonObject = root;
    if (mJsonName.empty()==false)
    {
        jsonObject= cJSON_GetObjectItem(root, mJsonName.c_str());
    }
    
    for(size_t i=0; i < mInts->size(); i++)
    {
        DataObject<IntMember> * data = mInts->get(i);
        
        int32_t value = cJSON_GetObjectItem(jsonObject, data->name.c_str())->valueint;
        
        IntMember addr = data->address;
        values.*addr = value;
    }
    
    for(size_t i=0; i < mBooleans->size(); i++)
    {
        DataObject<BoolMember> * data = mBooleans->get(i);
        
        int32_t value = cJSON_GetObjectItem(jsonObject, data->name.c_str())->valueint;
        
        BoolMember addr = data->address;
        values.*addr = value;
    }
    
    for(size_t i=0; i < mFloats->size(); i++)
    {
        DataObject<FloatMember> * data = mFloats->get(i);
        
        float value = cJSON_GetObjectItem(jsonObject, data->name.c_str())->valuedouble;
        
        FloatMember addr = data->address;
        values.*addr = value;
    }
    
    for(size_t i=0; i < mStrings->size(); i++)
    {
        DataObject<StringMember> * data = mStrings->get(i);
        
        std::string value = cJSON_GetObjectItem(jsonObject, data->name.c_str())->valuestring;
        
        StringMember addr = data->address;
        values.*addr = value;
    }
    
    cJSON_Delete(root);
    
    return true;
}

template <class T>
bool JsonTranslator<T>::loadFromFile(const char * filename, T & values)
{
    uint32_t inputLen;
    char * input;
    
    input = (char *)fileRead(filename, &inputLen);
    if (input==0)
    {
        Log::global()->error("failed to open %s\n", filename);
        return false;
    }
    
    bool result = loadFromBuffer(input, values);
    
    delete input;
    
    return result;
}

#endif
