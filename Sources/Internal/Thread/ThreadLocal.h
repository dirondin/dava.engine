/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#ifndef __DAVAENGINE_THREADLOCAL_H__
#define __DAVAENGINE_THREADLOCAL_H__

#include <type_traits>
#include <cassert>

#include "Base/BaseTypes.h"

namespace DAVA
{

/*
    template class ThreadLocal - implementation of cross-platform thread local storage (TLS) variables of specified type.
    Specified type can be pointer or fundamental type (int, char, etc). Size of type cannot exceed pointer size.

    C++ 11 supports thread_local keyword which does the same thing but not all compilers support it.
    Also, compiler specific stuff (__declspec(thread), __thread) does not work well between platforms.

    Restrictions:
        variables of type ThreadLocal can have only static storage duration (global or local static, and static data member)
        if you declare ThreadLocal as automatic object it's your own problems, so don't cry: Houston, we've got a problem
        
        initialization of 'ThreadLocal<T> tls = value' is not implemented as in such case thread variable will be initialized only
        in main thread, other threads will have default value
*/
template<typename T>
class ThreadLocal final
{
    static_assert(sizeof(T) <= sizeof(void*) && (std::is_fundamental<T>::value || std::is_pointer<T>::value), "ThreadLocal supports only fundamental and pointer types of no more than pointer size");

#if defined(__DAVAENGINE_WIN32__)
    using KeyType = DWORD;
#else
    using KeyType = pthread_key_t;
#endif

    // This union is used for conversion between void* and T without UB, I hope
    union ValueUnion
    {
        void* voidPtr;
        T typedValue;
    };

public:
    ThreadLocal() DAVA_NOEXCEPT;
    ~ThreadLocal() DAVA_NOEXCEPT;

    ThreadLocal(const ThreadLocal&) = delete;
    ThreadLocal& operator = (const ThreadLocal&) = delete;
    ThreadLocal(ThreadLocal&&) = delete;
    ThreadLocal& operator = (ThreadLocal&&) = delete;

    ThreadLocal& operator = (const T value) DAVA_NOEXCEPT;
    operator T () const DAVA_NOEXCEPT;

    // Method to test whether thread local storage has been successfully created by system
    bool IsCreated() const DAVA_NOEXCEPT;

private:
    void SetValue(T value) const DAVA_NOEXCEPT;
    T GetValue() const DAVA_NOEXCEPT;
    
    // Platform-specific methods
    void CreateTlsKey() DAVA_NOEXCEPT;
    void DeleteTlsKey() const DAVA_NOEXCEPT;
    void SetTlsValue(void* rawValue) const DAVA_NOEXCEPT;
    void* GetTlsValue() const DAVA_NOEXCEPT;

private:
    KeyType key;
    bool isCreated = false;
};

//////////////////////////////////////////////////////////////////////////
template<typename T>
inline ThreadLocal<T>::ThreadLocal() DAVA_NOEXCEPT
{
    CreateTlsKey();
}

template<typename T>
inline ThreadLocal<T>::~ThreadLocal() DAVA_NOEXCEPT
{
    DeleteTlsKey();
}

template<typename T>
inline ThreadLocal<T>& ThreadLocal<T>::operator = (const T value) DAVA_NOEXCEPT
{
    SetValue(value);
    return *this;
}

template<typename T>
inline ThreadLocal<T>::operator T () const DAVA_NOEXCEPT
{
    return GetValue();
}

template<typename T>
inline bool ThreadLocal<T>::IsCreated() const DAVA_NOEXCEPT
{
    return isCreated;
}

template<typename T>
inline void ThreadLocal<T>::SetValue(T value) const DAVA_NOEXCEPT
{
    ValueUnion x{};
    x.typedValue = value;
    SetTlsValue(x.voidPtr);
}

template<typename T>
inline T ThreadLocal<T>::GetValue() const DAVA_NOEXCEPT
{
    ValueUnion x{};
    x.voidPtr = GetTlsValue();
    return x.typedValue;
}

// Win32 specific implementation
#if defined(__DAVAENGINE_WIN32__)

template<typename T>
inline void ThreadLocal<T>::CreateTlsKey() DAVA_NOEXCEPT
{
    key = TlsAlloc();
    isCreated = key != TLS_OUT_OF_INDEXES;
    assert(isCreated);
}

template<typename T>
inline void ThreadLocal<T>::DeleteTlsKey() const DAVA_NOEXCEPT
{
    TlsFree(key);
}

template<typename T>
inline void ThreadLocal<T>::SetTlsValue(void* rawValue) const DAVA_NOEXCEPT
{
    TlsSetValue(key, rawValue);
}

template<typename T>
inline void* ThreadLocal<T>::GetTlsValue() const DAVA_NOEXCEPT
{
    return TlsGetValue(key);
}

#else   // POSIX specific implementation

template<typename T>
inline void ThreadLocal<T>::CreateTlsKey() DAVA_NOEXCEPT
{
    isCreated = (0 == pthread_key_create(&key, nullptr));
    assert(isCreated);
}

template<typename T>
inline void ThreadLocal<T>::DeleteTlsKey() const DAVA_NOEXCEPT
{
    pthread_key_delete(key);
}

template<typename T>
inline void ThreadLocal<T>::SetTlsValue(void* rawValue) const DAVA_NOEXCEPT
{
    pthread_setspecific(key, rawValue);
}

template<typename T>
inline void* ThreadLocal<T>::GetTlsValue() const DAVA_NOEXCEPT
{
    return pthread_getspecific(key);
}

#endif
}

#endif  // __DAVAENGINE_THREADLOCAL_H__
