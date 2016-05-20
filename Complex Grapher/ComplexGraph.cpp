#include "ComplexGraph.h"

ComplexGraph::ComplexGraph(int density, int posHandle) :
	mVertices(density * density * 2),
	mIndices((density - 1) * (density - 1) * 6)
{
	mDensity = density;
	mPosHandle = posHandle;

	genVertexData();
	genVBOs();
	genVAO();
}

void ComplexGraph::genVertexData()
{
	for (int realIter = 0; realIter < mDensity; realIter++) {
		for (int imagIter = 0; imagIter < mDensity; imagIter++) {

			float real = ((float(realIter) - float(mDensity - 1.f) / 2.f) / float(mDensity - 1.f));
			float imag = ((float(imagIter) - float(mDensity - 1.f) / 2.f) / float(mDensity - 1.f));

			int vertexIndex = realIter * mDensity + imagIter;
			int coordsPerVertex = 2;

			mVertices[vertexIndex * coordsPerVertex + 0] = real;
			mVertices[vertexIndex * coordsPerVertex + 1] = imag;
		}
	}

	for (int realIter = 0; realIter < mDensity - 1; realIter++) {
		for (int imagIter = 0; imagIter < mDensity - 1; imagIter++) {

			//First triangle
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 0] = (realIter + 0) * mDensity + imagIter + 0;
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 1] = (realIter + 1) * mDensity + imagIter + 0;
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 2] = (realIter + 1) * mDensity + imagIter + 1;

			//Second triangle
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 3] = (realIter + 0) * mDensity + imagIter + 0;
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 4] = (realIter + 0) * mDensity + imagIter + 1;
			mIndices[(realIter * (mDensity - 1) + imagIter) * 6 + 5] = (realIter + 1) * mDensity + imagIter + 1;
		}
	}
}

void ComplexGraph::genVBOs()
{
	glGenBuffers(1, &mVertVBO);
	glGenBuffers(1, &mIndexVBO);

	glBindBuffer(GL_ARRAY_BUFFER, mVertVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		mVertices.size() * sizeof(float),
		&mVertices[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mIndexVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		mIndices.size() * sizeof(int),
		&mIndices[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComplexGraph::genVAO()
{
	glGenVertexArrays(1, &mVAO);

	glBindVertexArray(mVAO);

	glEnableVertexAttribArray(mPosHandle);

	glBindBuffer(GL_ARRAY_BUFFER, mVertVBO);

	glVertexAttribPointer(
		mPosHandle,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);

	glBindVertexArray(0);
}

void ComplexGraph::draw()
{
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, GLsizei(mIndices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}