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

#ifndef babextended_fsm_h
#define babextended_fsm_h



/** Finite State Machine
 */
class Fsm
{
public:
    
    typedef void (Fsm::*FsmCallback)(void);
    
    Fsm();
    
    /** Initialize the finite state machine
     *
     * Use this functions when you are outside this fsm
     *
     * @param state         New state
     * @param call          State function
     * @param first         State initialization function
     */
    void init(int state, FsmCallback call, FsmCallback init);
    
    /** Run the finite state machine
     */
    void update();
    
    /** Is the current state egual to a specific state ?
     *
     * @param state         State to test
     * @return True if we are in the state, else false
     */
    bool isInState(int state);
    
    
    /** Is the current state initialized ?
     *
     * @return True if the current state is initialized, else false
     */
    bool isStateInitialized(void);
    
protected:
    /** Set the FSM to a new state
     *
     * Use this function when you are inside a state of THIS fsm
     *
     * @param newstate      New state
     * @param call          State function
     * @param first         State initialization function
     */
    void set(int newstate, FsmCallback call, FsmCallback init);
    
    
    int mCurrent;           /**< Current state of the automate */
    int mOld;               /**< Previous state */
    bool mInit;             /**< Init the state */
    FsmCallback mInitCall;  /**< Initializer's code */
    FsmCallback mCall;      /**< Updater's code */
};

#endif
