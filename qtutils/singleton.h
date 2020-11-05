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
