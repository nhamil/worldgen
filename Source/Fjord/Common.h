#pragma once 

#include <cstdint> 
#include <cstring> 
#include <vector> 
#include <utility> 
#include <unordered_map> 
#include <unordered_set> 

// #ifdef DEBUG
    #define FJ_EASSERT(x) if (!(x)) { FJ_EFLOG(Fatal, "Assertion failed!"); exit(1); }
    #define FJ_EASSERT_RET(x, ret) if (!(x)) { FJ_EFLOG(Fatal, "Assertion failed!"); exit(1); return ret; } 
    #define FJ_EASSERT_MSG(x, ...) if (!(x)) { FJ_EFLOG(Fatal, __VA_ARGS__); exit(1); } 
    #define FJ_EASSERT_RET_MSG(x, ret, ...) if (!(x)) { FJ_EFLOG(Fatal, __VA_ARGS__); exit(1); return ret; } 
    #define FJ_ASSERT(x) if (!(x)) { FJ_FLOG(Fatal, "Assertion failed!"); exit(1); } 
    #define FJ_ASSERT_RET(x, ret) if (!(x)) { FJ_FLOG(Fatal, "Assertion failed!"); exit(1); return ret; } 
    #define FJ_ASSERT_MSG(x, ...) if (!(x)) { FJ_FLOG(Fatal, __VA_ARGS__); exit(1); } 
    #define FJ_ASSERT_RET_MSG(x, ret, ...) if (!(x)) { FJ_FLOG(Fatal, __VA_ARGS__); exit(1);  return ret; } 
// #else 
//     #define FJ_EASSERT(x) 
//     #define FJ_EASSERT_RET(x, ret) 
//     #define FJ_EASSERT_MSG(x, ...) 
//     #define FJ_EASSERT_RET_MSG(x, ret, ...) 
//     #define FJ_ASSERT(x) 
//     #define FJ_ASSERT_RET(x, ret) 
//     #define FJ_ASSERT_MSG(x, ...) 
//     #define FJ_ASSERT_RET_MSG(x, ret, ...) 
// #endif 

#define FJ_NO_COPY(type) \
    type(const type& other) = delete; \
    type& operator=(const type& other) = delete; 

namespace Fjord 
{

    using int8 = std::int8_t; 
    using int16 = std::int16_t; 
    using int32 = std::int32_t; 
    using int64 = std::int64_t; 

    using uint8 = std::uint8_t; 
    using uint16 = std::uint16_t; 
    using uint32 = std::uint32_t; 
    using uint64 = std::uint64_t; 

    template <class T> 
    using Vector = std::vector<T>; 

    template <class T, class S, class Hash = std::hash<T>> 
    using HashMap = std::unordered_map<T, S, Hash>; 

    template <class T, class Hash = std::hash<T>> 
    using HashSet = std::unordered_set<T, Hash>; 

    using String = std::string; 
    
    template <class T> 
    String ToString(const T& value) 
    {
        return std::to_string(value); 
    }

    template <class A, class B> 
    using Pair = std::pair<A, B>; 

    template <class A, class B>  
    Pair<A, B> MakePair(const A& a, const B& b) 
    {
        return std::make_pair(a, b); 
    }

    template <class A, class B>  
    Pair<A, B> MakeSortedPair(const A& a, const B& b) 
    {
        return a <= b ? std::make_pair(a, b) : std::make_pair(b, a); 
    }

    struct PairHash 
    {
        template <class A, class B> 
        std::size_t operator() (const Pair<A, B>& pair) const 
        {
            return std::hash<A>()(pair.first) ^ (15485867 * std::hash<B>()(pair.second)); 
        }
    };

}

#include "Fjord/Core/Log.h" 

#include "Fjord/Core/Engine.h" 
#include "Fjord/Core/Object.h" 