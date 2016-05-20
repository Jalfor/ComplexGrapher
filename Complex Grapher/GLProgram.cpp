#include "GLProgramr.h"
#include <sstream>
#include <fstream>
#include <iostream>

std::string readTextFile(std::string filePath)
{
	std::ifstream ifs(filePath);
	std::stringstream ss;
	ss << ifs.rdbuf();

	return ss.str();
}

GLuint genShader(GLenum type, std::string filePath) {

	std::string code = readTextFile(filePath);

	const GLchar *cCode = code.c_str();
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &cCode, NULL);
	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	//If the compilation failed, delete the shader.
	if (compileStatus == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		std::cout << "Error compiling shader: " << strInfoLog;
		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}

GLuint genShader(GLenum type, std::string filePath, std::string equation) {

	std::string code = readTextFile(filePath);

	std::string placeholder = "$EQUATION$";
	size_t eqPos = code.find(placeholder);
	code.replace(eqPos, placeholder.length(), equation);

	const GLchar *cCode = code.c_str();
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &cCode, NULL);
	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	//If the compilation failed, delete the shader.
	if (compileStatus == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		std::cout << "Error compiling shader: " << strInfoLog;
		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}

int genProgram(std::vector<GLuint> shaders) {
	GLuint program = glCreateProgram();

	for (int i = 0; i < shaders.size(); i++) {
		glAttachShader(program, shaders[i]);
	}

	glLinkProgram(program);

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == 0)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);

		std::cout << "Error linking program: " << strInfoLog;
		glDeleteProgram(program);
		program = 0;
	}

	for (int i = 0; i < shaders.size(); i++) {
		glDetachShader(program, shaders[i]);
	}

	return program;
}