// Include GLEW before glfw.
#include <GL/glew.h>

// Include GLFW.
#include <GLFW/glfw3.h>

//#include <iostream> // Included in Renderer.h.

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
//#include "Debug.h"

GLFWwindow* initWindow()
{
	// Initialize GLFW.
	if ( !glfwInit() )
	{
		return nullptr;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	// Open a window and create its OpenGL context.
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "OpenGL Tutorial - Shader abstraction", nullptr, nullptr );
	if ( window == nullptr )
	{
		return nullptr;
	}
	glfwMakeContextCurrent( window );

	// Initialize GLEW.
	if ( glewInit() != GLEW_OK )
	{
		glfwTerminate();
		return nullptr;
	}

	std::cout << "Using GL Version: " << glGetString( GL_VERSION ) << std::endl;

	// Ensure we can capture the escape key being pressed below.
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

	/*glClearColor( 0.0f, 0.0f, 0.4f, 0.0f ); // Set background color. */

	return window;
}

int main( void )
{
	GLFWwindow* window = initWindow();
	if ( window == nullptr )
	{
		return -1;
	}

	float positions[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f  // 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	{
		VertexArray va;											// Have to set VAO before binding attrbutes.
		VertexBuffer vb( positions, 4 * 2 * sizeof( float ) );	// Create buffer and copy data.
		IndexBuffer ib( indices, 6 );							// Create index buffer.

		VertexBufferLayout layout;
		layout.Push< float >( 2 );

		va.AddBuffer( vb, layout );

		Shader shader( "res/shaders/Basic.shader" );
		shader.Bind();

		float red = 0.0f;
		float step = 0.05f;

		Renderer renderer;

		// Unbind shader, vbo and ibo.
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		/* Loop until the user closes the window */
		while ( !glfwWindowShouldClose( window )
				&& ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS ) )
		{
			if ( red < 0.0f || red > 1.0f ) step *= -1.0;
			red += step; // Increment red.

			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f( "u_Color", red, 0.3f, 0.8f, 1.0f ); // Set uniform color, before drawing.

			renderer.Draw( va, ib, shader );

			glfwSwapBuffers( window ); // Swap front and back buffers.
			glfwPollEvents(); // Poll for and process events.
		}
	}

	// Close OpenGL window and terminate GLFW.
	glfwTerminate();

	return 0;
}

