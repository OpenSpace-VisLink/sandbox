#version 330 

in vec3 pos; 
in vec3 norm; 
in vec2 uv; 

layout (location = 0) out vec4 colorOut;

void main() {
  vec3 n = normalize(norm);
  //colorOut = vec4(n,1);
  colorOut = vec4(1,0,0,1);
}