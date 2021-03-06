

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    outColor = vec4(outColor.xyz, 0.5);
    outColor = vec4(1.0, 0.0, 0.0, 0.2);
}

