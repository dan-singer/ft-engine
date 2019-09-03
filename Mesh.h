#pragma once
#include <d3d11.h>
#include "Vertex.h"

// --------------------------------------------------------
// This is a container class which holds and sets up 
// vertex and index buffers. It will not draw them.
// --------------------------------------------------------
class Mesh
{
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_indexBufferSize;
public:

	// --------------------------------------------------------
	// Mesh constructor
	// --------------------------------------------------------
	Mesh(Vertex* vertices, int numVertices, int* indices, int numIndices, ID3D11Device* device);

	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	int			  GetIndexCount() { return m_indexBufferSize; }
	 
	~Mesh();

};

