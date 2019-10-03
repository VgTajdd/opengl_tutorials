// Include GLEW before glfw.
#include <GL/glew.h>

// Include GLFW.
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
	unsigned int id = glCreateShader( type );
	const char* src = source.c_str();
	glShaderSource( id, 1, &src, nullptr );
	glCompileShader( id );

	// Error handling.
	int result;
	glGetShaderiv( id, GL_COMPILE_STATUS, &result );
	std::cout << ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) << " shader compile status: " << result << std::endl;
	if ( result == GL_FALSE )
	{
		int length;
		glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
		char* message = (char*) alloca( length * sizeof( char ) );
		glGetShaderInfoLog( id, length, &length, message );
		std::cout
			<< "Failed to compile "
			<< ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" )
			<< "shader"
			<< std::endl;
		std::cout << message << std::endl;
		glDeleteShader( id );
		return 0;
	}

	return id;
}

static unsigned int CreateShader( const std::string& vertexShader, const std::string& fragmentShader )
{
	// Create a shader program
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader( GL_VERTEX_SHADER, vertexShader );
	unsigned int fs = CompileShader( GL_FRAGMENT_SHADER, fragmentShader );

	glAttachShader( program, vs );
	glAttachShader( program, fs );

	glLinkProgram( program );

	GLint program_linked;

	glGetProgramiv( program, GL_LINK_STATUS, &program_linked );
	std::cout << "Program link status: " << program_linked << std::endl;
	if ( program_linked != GL_TRUE )
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog( program, 1024, &log_length, message );
		std::cout << "Failed to link program" << std::endl;
		std::cout << message << std::endl;
	}

	glValidateProgram( program );

	glDeleteShader( vs );
	glDeleteShader( fs );

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

	// Open a window and create its OpenGL context.
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL );
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

	/*// Ensure we can capture the escape key being pressed below.
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );*/

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

	// Create buffer and copy data.
	unsigned int buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, 4 * 2 * sizeof( float ), positions, GL_STATIC_DRAW );

	// Define vertex layout.
	glVertexAttribPointer( 0, // Index of attrib. layout(location = 0) in shaders.
						   2, // size of the current attribute values (1-4) not bytes.
						   GL_FLOAT, // Type of the attribute values.
						   GL_FALSE, // Normalize? (0-255 -> 0-1).
						   2 * sizeof( float ), // stride: distance in bytes between 2 vertex.
						   0 ); // offset: distance in bytes of the current attribute from the begining of a vertex.
	glEnableVertexAttribArray( 0 );

	// Create index buffer.
	unsigned int ibo; // Index buffer object.
	glGenBuffers( 1, &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof( unsigned int ), indices, GL_STATIC_DRAW );

	ShaderProgramSource source = ParseShader( "res/shaders/Basic.shader" );

	std::cout << "VERTEX" << std::endl << source.VertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl << source.FragmentSource << std::endl;

	unsigned int shader = CreateShader( source.VertexSource, source.FragmentSource );
	glUseProgram( shader );

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window )
			/*&& ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS )*/ )
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT ); // Clear the screen.

		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

		/* Swap front and back buffers */
		glfwSwapBuffers( window );

		/* Poll for and process events */
		glfwPollEvents();
	}

	// Cleanup VBO.
	glDeleteBuffers( 1, &buffer );
	glDeleteProgram( shader );

	// Close OpenGL window and terminate GLFW.
	glfwTerminate();

	return 0;
}

