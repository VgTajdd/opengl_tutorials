#pragma once

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak()
#ifdef DEBUG
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

void inline GLClearError()
{
	while ( glGetError() != GL_NO_ERROR );
}

const inline char* GLCheckError( const GLenum error )
{
	switch ( error )
	{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM : An unacceptable value is specified for an enumerated argument.";
			break;
		case GL_INVALID_VALUE:
			return "GL_INVALID_OPERATION : A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION : The specified operation is not allowed in the current state.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not complete.";
			break;
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY : There is not enough memory left to execute the command.";
			break;
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW : An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW : An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		default:
			return "Unrecognized error.";
	}
}

bool inline GLLogCall( const char* function, const char* file, int line )
{
	while ( GLenum error = glGetError() )
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function
			<< " " << file << "(line " << line << "): " << GLCheckError( error ) << std::endl;
		return false;
	}
	return true;
}

