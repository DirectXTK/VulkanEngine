#! /bin/bash
SCRIPT_PATH="$(realpath "$0")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
vulkanSDK=$VULKANSDK
cd $vulkanSDK
PATH=$(pwd)
echo $vulkanSDK
#./glslangValidator -V $PATH/Shaders/**.frag -o $PATH/Shaders/Fragment.spv
#./glslangValidator -V $PATH/Shaders/**.vert -o $PATH/Shaders/Vertex.spv

