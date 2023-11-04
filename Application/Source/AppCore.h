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
enum class Format{INT16,INT32,INT64,UINT16,UINT32,UINT64,FLOAT,DOUBLE,CHAR,STRING};
struct GUUID {
    uint64_t ID{};
    
};
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