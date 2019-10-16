// Include GLEW before glfw.
#include <GL/glew.h>

// Include GLFW.
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTriangle.h"
#include "tests/TestUniform.h"
#include "tests/TestMultipleObjects.h"

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
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "OpenGL Tutorial - Test framework", nullptr, nullptr );
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

	GLCall( glEnable( GL_BLEND ) );
	GLCall( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init( window, true );
	ImGui::StyleColorsDark();

	int currentSelection = -1;
	int radioSelection = 0;
	std::unique_ptr< test::Test > test;

	/* Loop until the user closes the window */
	while ( !glfwWindowShouldClose( window )
			&& ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS ) )
	{
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
			ImGui::RadioButton( "ClearColor", &radioSelection, 0 ); ImGui::SameLine();
			ImGui::RadioButton( "Triangle", &radioSelection, 1 ); ImGui::SameLine();
			ImGui::RadioButton( "Uniform", &radioSelection, 2 ); ImGui::SameLine();
			ImGui::RadioButton( "MultipleObjects", &radioSelection, 3 );
		}

		if ( currentSelection != radioSelection )
		{
			switch ( radioSelection )
			{
				case 0: test = std::make_unique< test::TestClearColor >(); break;
				case 1: test = std::make_unique< test::TestTriangle >(); break;
				case 2: test = std::make_unique< test::TestUniform >(); break;
				case 3: test = std::make_unique< test::TestMultipleObjects >();break;
			}
			currentSelection = radioSelection;
		}

		test->OnUpdate( 0.0f );
		test->OnRender();
		test->OnImGuiRender();

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );

		glfwSwapBuffers( window ); // Swap front and back buffers.
		glfwPollEvents(); // Poll for and process events.
	}

	test = nullptr;

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	// Close OpenGL window and terminate GLFW.
	glfwTerminate();

	return 0;
}

