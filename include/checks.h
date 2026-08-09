#ifndef CHECKS_H
#define CHECKS_H

#define GUARD(condition) \
    if (!(condition)) {  \
        return;          \
    }

#define REQUIRE(condition)                                           \
    if (!(condition)) {                                              \
        throw std::runtime_error("Requirement failed: " #condition); \
    }

#endif
