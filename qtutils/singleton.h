//
// Copyright (c) Vladimir Kraus. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

//
// A templated singleton base class which can extend any other class by using multiple inheritance.
// It uses CRTP pattern, i.e. the template parameter is the derived class itself.
// It provides static method instance() which returns a pointer to the underlying object.
// 
// This implementation of singleton expects the user to explicitly instantiate it.
// It can be destroyed and instantiated again.  But it is the user's responsibility 
// to have only one instance at the same time (it is asserted in debug mode).
// In other words, the Singleton class does not affect the lifetime of 
// the object in any way, it relies solely on the user of this class.
// All it does is to provide global access to the object with instance().
// 

//
// Usage:
//
// class CentralWidget : public Widget, public Singleton<CentralWidget>
// { 
// ... 
// };
// 

#pragma once

#include <cassert>

template <typename T>
class Singleton
{
public:
    Singleton()
    {
        assert(s_instance == nullptr);
        s_instance = static_cast<T*>(this);
    }

    virtual ~Singleton()
    {
        s_instance = nullptr;
    }

    static T *instance()
    {
        return s_instance;
    }

private:
    static T *s_instance;
};

template <typename T>
T *Singleton<T>::s_instance = nullptr;
