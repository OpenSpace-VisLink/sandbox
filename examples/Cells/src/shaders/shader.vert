

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 0, set = 1) uniform UniformBufferObject2 {
    mat4 transform;
} ubo2;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 3) in vec3 location;
//layout(location = 4) in vec3 numArms
//layout(location = 5) in mat4 armAngles
//layout(location = 9) in mat4 armAngles2
//layout(location = 13) in mat4 armLengths
//layout(location = 17) in mat4 armLengths2
//layout(location = 21) in mat4 value
//layout(location = 25) in mat4 value2


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
	vec3 pos = inPosition;
	if (pos.z < 0) {
		pos.z /= 10.0;
	}
	else {
		pos.z /= 5.0;
	}
	pos.z += 1.0/10.0/2.0;
	pos += location;
    gl_Position = ubo.proj * ubo.view * ubo.model * ubo2.transform * vec4(pos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

