#version 450

layout(location =0) out vec3 FragColor;


vec3 Positions[3] = vec3[](vec3(0.0,-0.4,0.0),vec3(0.4,0.4,0.0),vec3(-0.4,0.4,0.0));

vec3 Color[3] = vec3[](vec3(1.0f,0.0f,0.0f),vec3(1.0f,0.0f,0.0f),vec3(1.0f,0.0f,0.0f));

void main(){
    gl_Position =vec4(Positions[gl_VertexIndex],1.0f);
    FragColor = Color;
}