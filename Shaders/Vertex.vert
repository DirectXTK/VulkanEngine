#version 460
precision highp float; 

layout(location =0) in vec3 InPos;
layout(location =1) in vec4 InFragColor;
layout(location =2) in uvec2 InPickID;
layout(location =3) in vec2 InTexCoords;
layout(location =4) in uint InTexID;

layout(binding=0) uniform MVP{
    mat4 ProjView;
}mvp;

layout(location =0) out vec4 OutFragColor;
layout(location =1) out uvec2 OutPickID;
layout(location =2) out highp vec2 OutTexCoords;
layout(location =3) out uint OutTexID;




void main(){
   



    gl_Position =mvp.ProjView* vec4(InPos,1.0);
    gl_Position.y = -gl_Position.y;
    OutTexCoords = InTexCoords;
    OutTexID = InTexID;
    OutFragColor = InFragColor;
    OutPickID=InPickID;
}