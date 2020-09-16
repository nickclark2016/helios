#pragma once

#if defined(_MSC_VER) || defined(__clang__)

#define PROPERTY(TYPE, NAME, GET, SET) __declspec(property(get=GET, put=SET)) TYPE NAME
#define PROPERTY_READONLY(TYPE, NAME, GET) __declspec(property(get=GET)) TYPE NAME
#define PROPERTY_WRITEONLY(TYPE, NAME, SET) __declspec(property(put=SET)) TYPE NAME

#else

#error Compiler does not support properties

#endif