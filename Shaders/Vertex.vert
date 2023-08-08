#version 460

layout(location =0) in vec3 InPos;
layout(location =1) in vec4 InFragColor;
layout(location =2) in uvec2 InPickID;

layout(binding=0) uniform MVP{
    mat4 ProjView;
}mvp;

layout(location =0) out vec4 OutFragColor;
layout(location =1) out uvec2 OutPickID;




void main(){
   



    gl_Position =mvp.ProjView* vec4(InPos,1.0);
    gl_Position.y = -gl_Position.y;
    OutFragColor = InFragColor;
    OutPickID=InPickID;
}