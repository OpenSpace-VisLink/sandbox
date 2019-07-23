#version 330 
layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 normal; 
layout(location = 2) in vec2 coord; 

uniform mat4 ProjectionMatrix; 
uniform mat4 ViewMatrix; 
uniform mat4 ModelMatrix; 
uniform mat3 NormalMatrix; 

out vec3 pos; 
out vec3 norm;
out vec2 uv; 

void main() { 
   pos = (ModelMatrix*vec4(position,1.0)).xyz; 
   norm = NormalMatrix*normal.xyz; 
   uv = coord; 
   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(position, 1.0); 
}