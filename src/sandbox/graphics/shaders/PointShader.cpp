#include "sandbox/graphics/shaders/PointShader.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox/graphics/RenderState.h"


namespace sandbox {

PointShader::PointShader() : xDim(0), yDim(1), colorDim(2), xRange(-1.0,1.0), yRange(-1.0,1.0), colorRange(-1.0,1.0), color(0.0,0.0,0.0,1.0), hasColorGradient(false) {
	addType<PointShader>();
}

PointShader::~PointShader() {}

void PointShader::create(const SceneContext& sceneContext, ShaderProgramState& state) {
	GLuint shader;

	std::string vertexShader =
		            "#version 330 \n"
		            "layout(location = 0) in vec4 data[16]; "
		            ""
		            "uniform float scale; "
		            "uniform int xDataIndex; "
		            "uniform int yDataIndex; "
		            "uniform int zDataIndex; "
		            "uniform int xDim; "
		            "uniform int yDim; "
		            "uniform int zDim; "
		            "uniform vec2 xRange; "
		            "uniform vec2 yRange; "
				    "uniform vec2 zRange; "
		            "uniform mat4 ModelMatrix; "
		            "out vec3 pos; "
		            "out vec3 normalizedValue; "
		            ""
		            "void main() { "
		            "   vec3 val = vec3(data[xDataIndex][xDim], data[yDataIndex][yDim], data[zDataIndex][zDim]); "
		            "   vec3 min = vec3(xRange[0], yRange[0], zRange[0]); "
		            "   vec3 max = vec3(xRange[1], yRange[1], zRange[1]); "
		            "   normalizedValue = (val - min)/(max - min); "
		            "   pos = 2.0*normalizedValue-1.0; "
		            "   gl_Position = ModelMatrix*vec4(pos, 1.0); "
		            "}";

	state.addShader(compileShader(vertexShader, GL_VERTEX_SHADER));

    std::string fragmentShader =
            "#version 330 \n"
		    ""
		    "uniform bool hasColorGradient; "
		    "uniform vec4 color; "
		    "in vec3 pos; "
		    "in vec3 normalizedValue; "
		    ""
            "layout (location = 0) out vec4 colorOut;  "
            ""
            ""
            "void main() { "
            //"	colorOut = vec4(pos,0,1); "
            "	if (hasColorGradient) { "
            "	   colorOut = vec4(vec3(1.0)*(1-normalizedValue[2]) + normalizedValue[2]*color.xyz,color.a); "
            "   }"
            "	else {  "
            //"      colorOut = vec4(0,0,pos.z,1.0); "
            //"      colorOut = vec4(vec3(gl_FragCoord.z), 1.0); "
            "      colorOut = color; "
            "   }"
            "}";
    state.addShader(compileShader(fragmentShader, GL_FRAGMENT_SHADER));
}

void PointShader::setShaderParameters(const SceneContext& sceneContext, ShaderProgramState& state) {
	RenderState& renderState = RenderState::get(sceneContext);
	GLint loc = glGetUniformLocation(state.shaderProgram, "ModelMatrix");
	//glm::mat4 projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,100.0f);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(renderState.getModelMatrix().get()));
	loc = glGetUniformLocation(state.shaderProgram, "xDataIndex");
    glUniform1i(loc, xDim / 4);
	loc = glGetUniformLocation(state.shaderProgram, "xDim");
    glUniform1i(loc, xDim % 4);
	loc = glGetUniformLocation(state.shaderProgram, "yDataIndex");
    glUniform1i(loc, yDim / 4);
	loc = glGetUniformLocation(state.shaderProgram, "yDim");
    glUniform1i(loc, yDim % 4);
	loc = glGetUniformLocation(state.shaderProgram, "zDataIndex");
    glUniform1i(loc, colorDim / 4);
	loc = glGetUniformLocation(state.shaderProgram, "zDim");
    glUniform1i(loc, colorDim % 4);
	loc = glGetUniformLocation(state.shaderProgram, "xRange");
    glUniform2f(loc, xRange.x, xRange.y);
	loc = glGetUniformLocation(state.shaderProgram, "yRange");
    glUniform2f(loc, yRange.x, yRange.y);
	loc = glGetUniformLocation(state.shaderProgram, "zRange");
    glUniform2f(loc, colorRange.x, colorRange.y);
	loc = glGetUniformLocation(state.shaderProgram, "color");
	glUniform4f(loc, color.r, color.g, color.b, color.a);
	loc = glGetUniformLocation(state.shaderProgram, "hasColorGradient");
	glUniform1i(loc, hasColorGradient);
}

}