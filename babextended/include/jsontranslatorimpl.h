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
bool JsonTranslator<T>::loadFromBuffer(const char * buffer, T & values)
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
    
    loadIntMembers(jsonObject, values);
    loadBoolMembers(jsonObject, values);
    loadFloatMembers(jsonObject, values);
    loadStringMembers(jsonObject, values);
    
    cJSON_Delete(root);
    
    return true;
}

template <class T>
bool JsonTranslator<T>::loadArrayFromBuffer(const char * buffer, Vector<T> & values)
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
    
    
    for (int i = 0 ; i < cJSON_GetArraySize(jsonObject) ; i++)
    {
        T object;
        cJSON * subitem = cJSON_GetArrayItem(jsonObject, i);
        
        loadIntMembers(subitem, object);
        loadBoolMembers(subitem, object);
        loadFloatMembers(subitem, object);
        loadStringMembers(subitem, object);
        
        values.push(object);
    }
    
    cJSON_Delete(root);
    
    return true;
}

template <class T>
bool JsonTranslator<T>::loadArrayFromFile(const char * filename, Vector<T> & values)
{
    uint32_t inputLen;
    char * input;
    
    input = (char *)fileRead(filename, &inputLen);
    if (input==0)
    {
        Log::global()->error("failed to open %s\n", filename);
        return false;
    }
    
    bool result = loadArrayFromBuffer(input, values);
    
    delete input;
    
    return result;
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

template <class T>
void JsonTranslator<T>::save(JsonWriter & state, T & values)
{
    cJSON* subItem = state.root;
    if (mJsonName.empty()==false)
    {
        subItem = cJSON_CreateObject();
        cJSON_AddItemToObject(state.root, mJsonName.c_str(), subItem );
    }
    
    saveIntMembers(subItem, values);
    saveBoolMembers(subItem, values);
    saveFloatMembers(subItem, values);
    saveStringMembers(subItem, values);
    
}

template <class T>
void JsonTranslator<T>::saveArray(JsonWriter & state, Vector<T> & values)
{
    cJSON* subItem = state.root;
    
    if (mJsonName.empty()==false)
    {
        subItem = cJSON_CreateArray();
        cJSON_AddItemToObject(state.root, mJsonName.c_str(), subItem );
    }
    
    
    for (int i = 0 ; i < values.size() ; i++)
    {
        T object = values[i];
        
        cJSON* arrayItem = cJSON_CreateObject();
        cJSON_AddItemToArray(subItem, arrayItem);
        
        saveIntMembers(arrayItem, object);
        saveBoolMembers(arrayItem, object);
        saveFloatMembers(arrayItem, object);
        saveStringMembers(arrayItem, object);
        
    }
    
    
}

template <class T>
void JsonTranslator<T>::loadIntMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mInts->size(); i++)
    {
        DataObject<IntMember> * data = mInts->get(i);
        
        cJSON * internalItem = cJSON_GetObjectItem(item, data->name.c_str());
        if (internalItem!=0)
        {
            int32_t value = internalItem->valueint;
        
            IntMember addr = data->address;
            object.*addr = value;
        }
    }
}

template <class T>
void JsonTranslator<T>::loadBoolMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mBooleans->size(); i++)
    {
        DataObject<BoolMember> * data = mBooleans->get(i);
        
        cJSON * internalItem = cJSON_GetObjectItem(item, data->name.c_str());
        if (internalItem!=0)
        {
            bool value = (bool)internalItem->valueint;
        
            BoolMember addr = data->address;
            object.*addr = value;
        }
    }
}

template <class T>
void JsonTranslator<T>::loadFloatMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mFloats->size(); i++)
    {
        DataObject<FloatMember> * data = mFloats->get(i);
        
        cJSON * internalItem = cJSON_GetObjectItem(item, data->name.c_str());
        if (internalItem!=0)
        {
            float value = (float)internalItem->valuedouble;
        
            FloatMember addr = data->address;
            object.*addr = value;
        }
    }
}

template <class T>
void JsonTranslator<T>::loadStringMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mStrings->size(); i++)
    {
        DataObject<StringMember> * data = mStrings->get(i);
        
        cJSON * internalItem = cJSON_GetObjectItem(item, data->name.c_str());
        if (internalItem!=0)
        {
            std::string value(internalItem->valuestring);
        
            StringMember addr = data->address;
            object.*addr = value;
        }
    }
}

template <class T>
void JsonTranslator<T>::saveIntMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mInts->size(); i++)
    {
        DataObject<IntMember> * data = mInts->get(i);
        
        IntMember addr = data->address;
        int32_t value = object.*addr;
        
        cJSON_AddItemToObject(item, data->name.c_str(), cJSON_CreateNumber(value));
    }
}

template <class T>
void JsonTranslator<T>::saveBoolMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mBooleans->size(); i++)
    {
        DataObject<BoolMember> * data = mBooleans->get(i);
        
        BoolMember addr = data->address;
        int32_t value = object.*addr;
        cJSON_AddItemToObject(item, data->name.c_str(), cJSON_CreateBool(value));
    }
}

template <class T>
void JsonTranslator<T>::saveFloatMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mFloats->size(); i++)
    {
        DataObject<FloatMember> * data = mFloats->get(i);
        
        FloatMember addr = data->address;
        float value = object.*addr;
        cJSON_AddItemToObject(item, data->name.c_str(), cJSON_CreateNumber(value));
    }
}

template <class T>
void JsonTranslator<T>::saveStringMembers(cJSON* item, T & object)
{
    for(size_t i=0; i < mStrings->size(); i++)
    {
        DataObject<StringMember> * data = mStrings->get(i);
        
        StringMember addr = data->address;
        std::string value = object.*addr;
        
        cJSON_AddItemToObject(item, data->name.c_str(), cJSON_CreateString(value.c_str()));
    }
}

#endif
