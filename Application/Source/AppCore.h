#pragma once
//Defines

#define __STDC_WANT_LIB_EXT1__

//Includes
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <wchar.h>
#include "LogSystem.h"
#include <algorithm>
#include <filesystem>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include <unordered_map>
#include <thread>
#include <queue>
#include <limits>
#include <chrono>
#include <cmath>
#include <set>
#include <stack>

#include "RandomGenerator.h"
#include "AppTime.h"

//Debug stuff
#include <intrin.h>

#ifdef max
#undef max
#endif
//FORMAT means any format
enum class Format{NULLFORMAT,INT16,INT32,INT64,UINT16,UINT32,UINT64,FLOAT,DOUBLE,CHAR,STRING,FORMAT};
#define SEC(x) 1000*x
#define DLLEXPORT __declspec(dllexport)

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
    GUUID(const std::string& str) {
        ID = (uint64_t)std::hash<std::string>{}(str);
    }
 
    bool operator!=(const GUUID& other) {
        if (other.ID != ID)
            return true;
        return false;
    }
    bool operator==(const GUUID& other) {
        if (other.ID == ID)
            return true;
        return false;
    }
    bool operator==(const uint64_t& other) {
        if (other== ID)
            return true;
        return false;
    }
    bool operator==(const int& other) {
        if ((uint32_t)other == ID)
            return true;
        return false;
    }
    GUUID& operator=(uint32_t b) {
        ID = b;
        return *this;
    }
    GUUID& operator=(uint64_t b) {
        ID = b;
        return *this;
    } 
    GUUID& operator=(int b) {
        ID = (uint32_t)b;
        return *this;
    }


};
namespace Core {
    GUUID GetStringHash(const std::string& Path);
    std::string GetFileExtension(const std::string& File);

}
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
struct Double2 {
    Double2() = default;
    union {
        struct {
            double x, y;
        };
        struct {
            double r, g;
        };
    };
};
struct Float2 {
    Float2()=default;
    Float2& operator+=(const Float2& rh) {
        x += rh.x;
        y += rh.y;
        return *this;
    }
    Float2& operator*=(const Float2& rh) {
        x *= rh.x;
        y *= rh.y;
        return *this;
    }
    bool operator!=(const Float2& rh) {
        if (this->x != rh.x || this->y != rh.y)
            return true;
        return false;
    }
    bool operator==(const Float2& rh) {
        if (this->x == rh.x && this->y == rh.y)
            return true;
        return false;
    }
    union{
        struct{
        float r,g;

        };
    struct{
        float x, y;

        };
    };
};
inline bool operator==(const Float2& l, const Float2& r) {
    if (l.x == r.x&& l.y == r.y)
        return true;
    return false;

}
namespace std{
    template<>
    struct hash<Float2>
    {
        size_t operator()(const Float2& key)const {
            return hash<uint64_t>()(*(uint64_t*)&key);
        }
    };
}
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
namespace Core {
    //works if this is ractangle
    bool IsWithinRectRegion(const Float2& PointPos, const Float2& ObjPos, const Float2& ObjSize);
    //Index 0 - left bot 
    //1- left top
    //2- right top
    //3- right bot
    bool IsWithinRegionOrg(const Float2& PointPos, Float2 Region[4]);
}
