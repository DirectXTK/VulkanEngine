#! /bin/bash
PATH=$(pwd)
echo $PATH
cd C:\\VulkanSDK\\1.3.246.1\\Bin\\
./glslangValidator -V $PATH/Shaders/**.frag -o $PATH/Shaders/Fragment.spv
./glslangValidator -V $PATH/Shaders/**.vert -o $PATH/Shaders/Vertex.spv
