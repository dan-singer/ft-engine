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
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_indexBufferSize = 0;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void Initialize(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices, ID3D11Device* device);

public:

	// --------------------------------------------------------
	// Mesh constructor
	// --------------------------------------------------------
	Mesh(Vertex* vertices, int numVertices, unsigned int* indices, int numIndices, ID3D11Device* device);

	// --------------------------------------------------------
	// Construct a mesh from an .obj file
	// @param const char * file path to file
	// @param ID3D11Device * device 
	// --------------------------------------------------------
	Mesh(const char* file, ID3D11Device* device);


	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	int			  GetIndexCount() { return m_indexBufferSize; }
	 
	~Mesh();

};

