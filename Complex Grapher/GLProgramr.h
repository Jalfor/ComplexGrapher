#pragma once
#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

#include <string>
#include <GL/glew.h>
#include <vector>

std::string readTextFile(std::string filePath);

GLuint genShader(GLenum type, std::string filePath);
GLuint genShader(GLenum type, std::string filePath, std::string equation);

int genProgram(std::vector<GLuint> shaders);

#endif