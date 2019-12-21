

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    //outColor = vec4(vec3(1.0-pow(abs(dot(normalize(fragNorm), vec3(0.1,0.1,1))),2)), 1.0);
    outColor = vec4(normalize(fragNorm), 0.5);
    //outColor = vec4(normalize(fragNorm), 0.5*(1-dot(normalize(fragNorm), vec3(0,0,1))));//dot(normalize(fragNorm), vec3(0,0,1)
    //outColor = vec4(outColor.xyz, (1-dot(normalize(fragNorm), vec3(1,0,0)))+0.2);//dot(normalize(fragNorm), vec3(0,0,1)
}

