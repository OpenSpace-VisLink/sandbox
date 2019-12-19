

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

float calculateLength(float angle, vec3 pos, int prevArm, int curArm, float oldLen) {
	float curAngle = armAngles[curArm/4][curArm - (curArm/4)*4];;
	float prevAngle = armAngles[prevArm/4][prevArm - (prevArm/4)*4];
	if (prevArm > curArm) {
		if (angle < curAngle) {
			angle += 2.0*3.1459;
		}
		curAngle += 2.0*3.1459;
	}

	float curLength = length(pos.xy)*armLengths[curArm/4][curArm - (curArm/4)*4];
	float prevLength = length(pos.xy)*armLengths[prevArm/4][prevArm - (prevArm/4)*4];

	float sectionAngle = curAngle-prevAngle;
	if (sectionAngle >= 3.14159) {
		return oldLen;
	}

	float lerp = (angle-prevAngle)/sectionAngle;
	float interp = (1.0-sin(lerp*3.14159))/2.0 + 0.5;
	float stretchFactor = ((1.0-.1) + 0.5)*sectionAngle/(3.14159);
	interp = pow((lerp-0.5)*2.0,2.0)*stretchFactor + 1.0-stretchFactor;

	float len = interp*((1.0-lerp)*prevLength+lerp*curLength);
	if (oldLen > len) {
		len = oldLen;
	}
	return len;
}

void main() {
	vec3 pos = inPosition;
	float angle = atan(pos.y, pos.x)+3.14159;
	int numArms = int(info.x);

	int prevArm = 0;
	int curArm = 0;
	int step = 1;
	for (int f = 0; f < 16; f++) {
		if (f < numArms) {
			float newAngle = armAngles[f/4][f - (f/4)*4];

			if (newAngle < angle) {
				curArm = ((f+1)+numArms)-(((f+step)+numArms)/numArms)*numArms;
				prevArm = f;
			}

			if (f == 0 && newAngle > angle) {
				curArm = 0;
				prevArm = numArms-1;
			}
		}
	}

	
	float len = calculateLength(angle, pos, prevArm, curArm, 0.0);
	len = calculateLength(angle, pos, ((prevArm-1)+numArms)-(((prevArm-1)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength(angle, pos, prevArm, ((curArm+1)+numArms)-(((curArm+1)+numArms)/numArms)*numArms, len);
	len = calculateLength(angle, pos, ((prevArm-2)+numArms)-(((prevArm-2)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength(angle, pos, prevArm, ((curArm+2)+numArms)-(((curArm+2)+numArms)/numArms)*numArms, len);

	


	len = length(pos.xy)*len;

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

