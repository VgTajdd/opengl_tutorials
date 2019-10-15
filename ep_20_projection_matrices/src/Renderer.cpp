#include "Renderer.h"

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Debug.h"

void Renderer::Clear() const
{
	GLCall( glClear( GL_COLOR_BUFFER_BIT ) ); // Clear the screen.
}

void Renderer::Draw( const VertexArray& va, const IndexBuffer& ib, const Shader& shader ) const
{
	shader.Bind();
	va.Bind(); // Instead of binding vertex buffer, attrib pointer, just bind Vertex Array Object.
	ib.Bind(); // Bind index buffer.
	GLCall( glDrawElements( GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr ) );
}