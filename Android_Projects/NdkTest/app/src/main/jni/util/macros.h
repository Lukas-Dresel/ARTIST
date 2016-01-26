//
// Created by Lukas on 8/17/2015.
//

#ifndef NDKTEST_MACROS_H
#define NDKTEST_MACROS_H


#define QUOTE(x) #x
#define STRINGIFY(x) QUOTE(x)

#define __LINE_STRING__ STRINGIFY(__LINE__)

#define LIKELY(x)       __builtin_expect((x), true)
#define UNLIKELY(x)     __builtin_expect((x), false)

#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))

// DISALLOW_COPY_AND_ASSIGN disallows the copy and operator= functions.
// It goes in the private: declarations in a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif //NDKTEST_MACROS_H
