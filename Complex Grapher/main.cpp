#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>
#include <iostream>
#include <string>
#include "ComplexGraph.h"
#include "GLProgramr.h"

//Both matrices must be square
std::vector<GLfloat> multMM(std::vector<GLfloat> a, std::vector<GLfloat> b, int size)
{
	std::vector<GLfloat> result(size * size, 0.f);

	for (int col = 0; col < size; col++)
	{
		for (int row = 0; row < size; row++)
		{
			for (int i = 0; i < size; i++) {
				result[row + col * size] += a[row + i * size] * b[i + col * size];
			}
		}
	}

	return result;
}

std::vector<GLfloat> gen4DRotMatrix(int axis1, int axis2, float radians) {
	
	std::vector<GLfloat> matrix(16, 0.f);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if ((x == axis1 && y == axis1) || (x == axis2 && y == axis2)) {
				matrix[x * 4 + y] = std::cosf(radians);
			}
			else if (x == axis1 && y == axis2) {
				matrix[x * 4 + y] = -std::sinf(radians);
			}
			else if (x == axis2 && y == axis1) {
				matrix[x * 4 + y] = std::sinf(radians);
			}
			else if (x == y) {
				matrix[x * 4 + y] = 1.f;
			}
		}
	}

	return matrix;
}

std::vector<GLfloat> genCameraToClipMatrix(float xScale, float yScale, float zNear, float zFar)
{
	std::vector<GLfloat> cameraToClipMatrix(16, 0.f);

	cameraToClipMatrix[0]  = xScale;
	cameraToClipMatrix[5]  = yScale;
	cameraToClipMatrix[10] = (zFar + zNear) / (zNear - zFar);
	cameraToClipMatrix[14] = (2 * zFar * zNear) / (zNear - zFar);
	cameraToClipMatrix[11] = -1.0f;

	return cameraToClipMatrix;
}

std::vector<GLfloat> genTranslationMatrix(float x, float y, float z) {
	std::vector<GLfloat> translationMatrix(16, 0.f);
	translationMatrix[0] = 1.f;
	translationMatrix[5] = 1.f;
	translationMatrix[10] = 1.f;
	translationMatrix[15] = 1.f;
	
	translationMatrix[12] = x;
	translationMatrix[13] = y;
	translationMatrix[14] = z;

	return translationMatrix;
}

std::vector<GLfloat> genProjMatrix(float xScale, float yScale, float zNear, float zFar)
{
	std::vector<GLfloat> modelToWorldMatrix = genTranslationMatrix(0.f, 0.f, -2.f);
	std::vector<GLfloat> cameraToClipMatrix = genCameraToClipMatrix(xScale, yScale, zNear, zFar);
	
	return multMM(cameraToClipMatrix, modelToWorldMatrix, 4);
}

std::vector<GLfloat> gen4dTransformMatrix(float angle) 
{
	std::vector<GLfloat> rotationMatrix = gen4DRotMatrix(0, 2, angle);
	rotationMatrix = multMM(gen4DRotMatrix(2, 3, angle), rotationMatrix, 4);
	rotationMatrix = multMM(gen4DRotMatrix(3, 1, angle), rotationMatrix, 4);

	return rotationMatrix;
}

int main()
{
	const int windowWidth = 750;
	const int windowHeight = 750;

	const int maxMandelbrotIters = 100;
	const int posHandle = 0;

	float mandelbrotPower = 2.0f;
	float transform4dAngle = 0.f;
	bool drawSet = true;

	sf::Clock clock;

	sf::ContextSettings windowSettings;
	windowSettings.antialiasingLevel = 4;
	windowSettings.majorVersion = 3;
	windowSettings.minorVersion = 3;

	sf::Window window(sf::VideoMode(windowWidth, windowHeight), "Fractals", sf::Style::Default, windowSettings);
	window.setVerticalSyncEnabled(true);

	std::vector<GLfloat> projMatrix = genProjMatrix(1.f, 1.f, 0.01f, 100.f);
	std::vector<GLfloat> transformMatrix4d = gen4dTransformMatrix(transform4dAngle);

	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);

	glClearColor(0.7f, 0.7f, 0.7f, 1.f);

	std::string equation;
	std::cout << "Enter function in terms of z without any spaces.\n\n";
	std::cout << "Available functions include:\n";
	std::cout << "\tpow(Complex,Real)\n";
	std::cout << "\tsin(Complex)\n";
	std::cout << "\tcos(Complex)\n";
	std::cout << "\texp(Complex)\n";
	std::cout << "\tmult(Complex,Complex)\n\n";
	std::cout << "Addition and subtraction are available using + and -\n\n";
	std::cout << "For example: pow(z,2)+2*sin(10*z)/200\n\n";
	std::cout << "f(z) = ";
                  
	std::cin >> equation;

	std::vector<GLuint> shaders(2);
	shaders[0] = genShader(GL_VERTEX_SHADER, "vert.glsl", equation);
	shaders[1] = genShader(GL_FRAGMENT_SHADER, "frag.glsl");

	GLuint program = genProgram(shaders);

	GLint maxIterLoc = glGetUniformLocation(program, "maxIter");
	glProgramUniform1i(program, maxIterLoc, maxMandelbrotIters);

	GLint mandelbrotPowerLoc = glGetUniformLocation(program, "mandelbrotPower");
	glProgramUniform1f(program, mandelbrotPowerLoc, mandelbrotPower);

	GLint drawSetLoc = glGetUniformLocation(program, "drawSet");
	glProgramUniform1i(program, drawSetLoc, drawSet);

	GLint projMatrixLoc = glGetUniformLocation(program, "projMatrix");
	glProgramUniformMatrix4fv(program, projMatrixLoc, 1, GL_FALSE, &projMatrix[0]);

	GLint transform4dMatrixLoc = glGetUniformLocation(program, "transform4d");
	glProgramUniformMatrix4fv(program, transform4dMatrixLoc, 1, GL_FALSE, &transformMatrix4d[0]);

	ComplexGraph complexGraph(500, 0);

	clock.restart();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			else if (event.type == sf::Event::Resized)
				glViewport(0, 0, event.size.width, event.size.height);

			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
				drawSet = !drawSet;
				glProgramUniform1i(program, drawSetLoc, drawSet);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		complexGraph.draw();
		glUseProgram(0);

		window.display();

		mandelbrotPower += float(clock.getElapsedTime().asMicroseconds()) / 10000000.f;
		transform4dAngle += float(clock.getElapsedTime().asMicroseconds()) / 10000000.f;
		std::vector<GLfloat> transformMatrix4d = gen4dTransformMatrix(transform4dAngle);
		
		glProgramUniform1f(program, mandelbrotPowerLoc, mandelbrotPower);
		glProgramUniformMatrix4fv(program, transform4dMatrixLoc, 1, GL_FALSE, &transformMatrix4d[0]);

		//std::cout << "fps: " << 1000000.f / clock.getElapsedTime().asMicroseconds() << "\n";
		clock.restart();
	}

	return 0;
}