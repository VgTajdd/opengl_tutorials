// Include GLEW before glfw.
#include <GL/glew.h>

// Include GLFW.
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static struct ShaderProgramSource ParseShader( const std::string& filepath )
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream( filepath );
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while ( getline( stream, line ) )
	{
		if ( line.find( "#shader" ) != std::string::npos )
		{
			if ( line.find( "vertex" ) != std::string::npos )
				type = ShaderType::VERTEX;
			else if ( line.find( "fragment" ) != std::string::npos )
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int) type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader( unsigned int type, const std::string& source )
{
	GLCall( unsigned int id = glCreateShader( type ) );
	const char* src = source.c_str();
	GLCall( glShaderSource( id, 1, &src, nullptr ) );
	GLCall( glCompileShader( id ) );

	// Error handling.
	int result;
	GLCall( glGetShaderiv( id, GL_COMPILE_STATUS, &result ) );
	std::cout << ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) << " shader compile status: " << result << std::endl;
	if ( result == GL_FALSE )
	{
		int length;
		GLCall( glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length ) );
		char* message = (char*) alloca( length * sizeof( char ) );
		GLCall( glGetShaderInfoLog( id, length, &length, message ) );
		std::cout
			<< "Failed to compile "
			<< ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" )
			<< "shader"
			<< std::endl;
		std::cout << message << std::endl;
		GLCall( glDeleteShader( id ) );
		return 0;
	}

	return id;
}

static unsigned int CreateShader( const std::string& vertexShader, const std::string& fragmentShader )
{
	// Create a shader program
	GLCall( unsigned int program = glCreateProgram() );
	GLCall( unsigned int vs = CompileShader( GL_VERTEX_SHADER, vertexShader ) );
	GLCall( unsigned int fs = CompileShader( GL_FRAGMENT_SHADER, fragmentShader ) );

	GLCall( glAttachShader( program, vs ) );
	GLCall( glAttachShader( program, fs ) );

	GLCall( glLinkProgram( program ) );

	GLint program_linked;

	GLCall( glGetProgramiv( program, GL_LINK_STATUS, &program_linked ) );
	std::cout << "Program link status: " << program_linked << std::endl;
	if ( program_linked != GL_TRUE )
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		GLCall( glGetProgramInfoLog( program, 1024, &log_length, message ) );
		std::cout << "Failed to link program" << std::endl;
		std::cout << message << std::endl;
	}

	GLCall( glValidateProgram( program ) );

	GLCall( glDeleteShader( vs ) );
	GLCall( glDeleteShader( fs ) );

	return program;
}

int main( void )
{
	// Initialize GLFW.
	if ( !glfwInit() )
	{
		/*fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();*/
		return -1;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	// Open a window and create its OpenGL context.
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "OpenGL Tutorial - Vertex arrays", NULL, NULL );
	if ( window == NULL )
	{
		/*fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();*/
		return -1;
	}
	glfwMakeContextCurrent( window );

	// Initialize GLEW
	/*glewExperimental = true; // Needed for core profile.*/
	if ( glewInit() != GLEW_OK )
	{
		/*fprintf( stderr, "Failed to initialize GLEW\n" );
		getchar();*/
		glfwTerminate();
		return -1;
	}

	std::cout << "Using GL Version: " << glGetString( GL_VERSION ) << std::endl;

	// Ensure we can capture the escape key being pressed below.
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

	/*glClearColor( 0.0f, 0.0f, 0.4f, 0.0f ); // Set background color. */

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

	VertexArray va;											// Have to set VAO before binding attrbutes.
	VertexBuffer vb( positions, 4 * 2 * sizeof( float ) );	// Create buffer and copy data.
	IndexBuffer ib( indices, 6 );							// Create index buffer.

	VertexBufferLayout layout;
	layout.Push< float >( 2 );

	va.AddBuffer( vb, layout );

	ShaderProgramSource source = ParseShader( "res/shaders/Basic.shader" );

	std::cout << "VERTEX" << std::endl << source.VertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl << source.FragmentSource << std::endl;

	unsigned int shader = CreateShader( source.VertexSource, source.FragmentSource );
	GLCall( glUseProgram( shader ) );

	GLCall( unsigned int u_Color = glGetUniformLocation( shader, "u_Color" ) );
	ASSERT( u_Color != -1 );

	float red = 0.0f;
	float step = 0.05f;

	// Unbind shader, vbo and ibo.
	GLCall( glUseProgram( 0 ) );
	GLCall( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
	GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
	GLCall( glBindVertexArray( 0 ) );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window )
			&& ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS ) )
	{
		// Increment red.
		if ( red < 0.0f || red > 1.0f ) step *= -1.0;
		red += step;

		GLCall( glClear( GL_COLOR_BUFFER_BIT ) ); // Clear the screen.

		GLCall( glUseProgram( shader ) );
		GLCall( glUniform4f( u_Color, red, 0.3f, 0.8f, 1.0f ) );// Set uniform color, before drawing.

		// Instead of binding vertex buffer, attrib pointer, just bind Vertex Array Object.
		va.Bind();

		// Bind index buffer.
		ib.Bind();

		GLCall( glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr ) );

		glfwSwapBuffers( window ); // Swap front and back buffers.
		glfwPollEvents(); // Poll for and process events.
	}

	// Cleanup.
	GLCall( glDeleteProgram( shader ) );

	// Close OpenGL window and terminate GLFW.
	glfwTerminate();

	return 0;
}

