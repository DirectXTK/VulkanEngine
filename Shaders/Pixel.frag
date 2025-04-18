#version 460

precision highp float;

layout(location =0) in vec4 InColor;
layout(location =1) flat in uvec2 InPickID;
layout(location =2) in vec2 TexCoords;
layout(location =3) flat in uint TexID;

layout(location =0) out vec4 OutInColor;
layout(location =1) out uvec2 OutInColor2;

layout(set =1 ,binding =0) uniform sampler2D TextureSampler[4];

void main(){
    OutInColor = InColor*texture(TextureSampler[TexID],TexCoords);
    OutInColor2 = InPickID;
}