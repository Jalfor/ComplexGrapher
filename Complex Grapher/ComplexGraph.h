#pragma once
#ifndef COMPLEX_GRAPHER_H
#define COMPLEX_GRAPHER_H

#include <GL/glew.h>
#include <vector>

class ComplexGraph
{
private:
	std::vector<GLfloat> mVertices;  //In the form x,y,z,w...x,y etc.
	std::vector<GLuint>  mIndices;
	std::vector<GLfloat> mColors;

	GLuint mVAO;
	GLuint mVertVBO;
	GLuint mIndexVBO;

	float mProjectionConstant;
	float mViewDist;
	int   mPosHandle;     //Vertex attribute indices

	int mFaceCount;
	
	int mDensity;
	float mViewSize;

	void genVertexData();

public:

	ComplexGraph(int density, int posHandle);
	
	void genVBOs();
	void genVAO();
	void draw();
};

#endif