

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
layout(location = 4) in vec3 info;
layout(location = 5) in vec4 armAngles[4];
layout(location = 9) in vec4 armLengths[4];
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
	float angle = atan(pos.y, pos.x)+3.14159;
	int numArms = int(info.x);

	int prevArm = 0;
	int curArm = 0;
	float curAngle = 0;
	float prevAngle = 0;
	for (int f = 0; f < 16; f++) {
		if (f < numArms) {
			float newAngle = armAngles[f/4][f - (f/4)*4];

			if (newAngle =< angle) {
				curArm = ((f+1)+numArms)-(((f+1)+numArms)/numArms)*numArms;
				curAngle = armAngles[curArm/4][curArm - (curArm/4)*4] + 2.0*3.1459*int(curArm/numArms);
				prevArm = f;
				prevAngle = newAngle;
			}

			if (f == 0 && newAngle > angle) {
				curArm = 0;
				curAngle = newAngle;
				prevArm = ((f-1)+numArms)-(((f-1)+numArms)/numArms)*numArms;
				prevAngle = armAngles[prevArm/4][prevArm - (prevArm/4)*4]-2.0*3.1459;
			}
		}
	}

	//float len = length(pos.xy)*angle/(2.0*3.14159);
	//float len = length(pos.xy)*info.x;
	int armNum = int(numArms*angle/(2.0*3.14159));
	float len = length(pos.xy)*float(armNum)/float(numArms);
	//float a = armLengths[0];

	//curArm = armNum;
	//prevArm = ((curArm-1)+numArms)-(((curArm-1)+numArms)/numArms)*numArms;

	float curLength = length(pos.xy)*armLengths[curArm/4][curArm - (curArm/4)*4];
	float prevLength = length(pos.xy)*armLengths[prevArm/4][prevArm - (prevArm/4)*4];
	len = length(pos.xy)*armLengths[armNum/4][armNum - (armNum/4)*4];

	float sectionAngle = 2.0*3.14156/numArms;
	sectionAngle = curAngle-prevAngle;
	float lerp = (angle-prevAngle)/sectionAngle;//(angle-sectionAngle*armNum)/sectionAngle;
	float interp = (1.0-sin(lerp*3.14159))/2.0 + 0.5;
	float stretchFactor = ((1.0-0.5) + 0.5)*sectionAngle/(3.14159);
	interp = pow((lerp-0.5)*2.0,2.0)*stretchFactor + 1.0-stretchFactor;

	len = length(pos.xy)*interp*((1.0-lerp)*prevLength+lerp*curLength);
	//len = length(pos.xy)*armAngles[armNum/4][armNum - (armNum/4)*4];
	//len = length(pos.xy)*curArm/6.0;

	pos.x = len*cos(angle);
	pos.y = len*sin(angle);
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

