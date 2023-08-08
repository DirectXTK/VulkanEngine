#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "LogSystem.h"
#include <algorithm>
#include <vector>
#include <chrono>
#include <thread>

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