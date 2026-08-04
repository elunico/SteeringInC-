//
// Created by Thomas Povinelli on 12/14/25.
//

#ifndef STRUCT2THIS_H
#define STRUCT2THIS_H

/**
 * This struct is used to convert a member function pointer into a callable
 * object that can be used with std::bind or other functional programming
 * techniques. It takes a member function pointer as a template parameter and
 * defines an operator() that takes an instance of the class and the arguments
 * for the member function, and calls the member function on the instance with
 * the provided arguments. This allows you to use member functions in a more
 * flexible way, such as passing them as callbacks or using them in algorithms
 * that expect callable objects. Example usage: struct MyClass { void
 * myMethod(int x) {
 *         // ...
 *     }
 * };
 * auto callable = This2Param(&MyClass::myMethod);
 * MyClass obj;
 * callable(obj, 42); // Calls obj.myMethod(42);
 */
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
