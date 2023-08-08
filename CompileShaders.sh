#! /bin/bash
cd /home/vytas/Downloads/install/bin/
./glslangValidator -V /home/Repos/Vulkan/Shaders/**.frag -o /home/Repos/Vulkan/Shaders/Fragment.spv
./glslangValidator -V /home/Repos/Vulkan/Shaders/**.vert -o /home/Repos/Vulkan/Shaders/Vertex.spv
