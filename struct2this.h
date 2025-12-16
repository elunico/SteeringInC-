//
// Created by Thomas Povinelli on 12/14/25.
//

#ifndef STRUCT2THIS_H
#define STRUCT2THIS_H

template <typename X>
struct This2Param {
    X meth;

    constexpr This2Param(X meth) : meth(meth)
    {
    }

    template <typename T, typename... Args>
    constexpr auto operator()(T self, Args... args) const
        -> decltype((self.*meth)(args...))
    {
        return (self.*meth)(args...);
    }
};

#endif  // STRUCT2THIS_H
