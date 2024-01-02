#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "LogSystem.h"
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include <unordered_map>
#include <thread>
#include <queue>

#include "RandomGenerator.h"
#include "AppTime.h"

enum class Format{INT16,INT32,INT64,UINT16,UINT32,UINT64,FLOAT,DOUBLE,CHAR,STRING};

#define SEC(x) 1000*x

struct GUUID {
    uint64_t ID{};
    GUUID() {
        ID = Core::RandomUInt64(1, std::numeric_limits<uint64_t>().max());
    }
    GUUID(int a) {
        ID = a;
    }
    GUUID(uint64_t a) {
        ID = a;
    }
    bool operator!=(const GUUID& other) {
        if (other.ID != ID)
            return true;
        return false;
    }
    GUUID& operator=(int b) {
        ID += (uint32_t)b;
        return *this;
    }


};
inline bool operator==(const GUUID& l, const GUUID& r) {
    if (l.ID == r.ID)
        return true;
    return false;

}
inline bool operator==(const GUUID& l, int r) {
    if (l.ID == r)
        return true;
    return false;

}
namespace std {
    template<>
    struct hash<GUUID>
    {
        size_t operator()(const GUUID& key)const {
            return hash<uint64_t>()(key.ID);
        }
    };
}
struct Float2 {
    Float2()=default;
    union{
        struct{
        float r,g;

        };
    struct{
        float x, y;

        };
    };
};
struct Float3{

    union{
        struct{
        float r,g,b;

        };
    struct{
        float x,y,z;

        };
    };
};
struct Float4 {
  union{
        struct{
        float r,g,b,a;

        };
    struct{
        float x,y,z,w;

        };
    };
};