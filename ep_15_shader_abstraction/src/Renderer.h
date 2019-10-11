#pragma once

#include <GL/glew.h>
#include <assert.h>

#define ASSERT(x) if (!(x)) __debugbreak()

#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
const char* GLCheckError( const GLenum error );
bool GLLogCall( const char* function, const char* file, int line );