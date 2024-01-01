#version 460

layout(location =0) in vec4 InColor;
layout(location =1) flat in uvec2 InPickID;

layout(location =0) out vec4 OutInColor;
layout(location =1) out uvec2 OutInColor2;

void main(){


    OutInColor = InColor;
    OutInColor2 = InPickID;
}