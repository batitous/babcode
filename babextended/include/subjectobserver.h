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

#ifndef babextended_subjectobserver_h
#define babextended_subjectobserver_h

/** A really simple subject / observer event system
 *
 * How to use it:
 *
 * Subject aVeryImportantSubject;
 *
 * YourClass object1;
 * AnotherClass object2;
 *
 * aVeryImportantSubject(createCallback(&object1, &YourClass::method));
 * aVeryImportantSubject(createCallback(&object2, &AnotherClass::aRandomMethod));
 *
 * Then, notify objects about an very important subject event :
 *
 * aVeryImportantSubject.notify();
 *
 *
 */
class Subject
{
public:
    Subject()
    {
    }
    
    void addObserver(Callback * c)
    {
        mObservers.push_back(c);
    }
    
    
    void notify()
    {
        for (int i=0; i < mObservers.size(); i++)
        {
            mObservers[i]->call();
        }
    }
    
    
private:
    std::vector<Callback *>  mObservers;
};


#endif
