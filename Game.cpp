#include "Game.h"
#include "Vertex.h"
#include <time.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

	prevMousePos = { 0,0 };

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;

	// Delete the entities
	for (Entity* entity : entities) {
		delete entity;
	}

	// Delete the mesh objects
	delete triangle;
	delete hexagon;
	delete cube;
	delete cone;
	delete cylinder;

	// Delete the camera
	delete camera;

	// Delete any materials
	delete standardMaterial;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	CreateEntities();
	camera = new Camera();
	camera->UpdateProjectionMatrix((float)width / height);
	camera->SetPosition(XMFLOAT3(0, 0, -5));

	directionalLights[0] = { XMFLOAT4(0.1f,0.1f,0.1f,0.1f), XMFLOAT4(0,0,1,1), XMFLOAT3(1,-1,0) };
	directionalLights[1] = { XMFLOAT4(0.1f,0.1f,0.1f,0.1f), XMFLOAT4(1,0,0,1), XMFLOAT3(-1,-1,0) };
	


	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	standardMaterial = new Material(vertexShader, pixelShader);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{

	XMFLOAT3 toCamera(0, 0, -1);
	XMFLOAT2 zeroUV(0, 0);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex triVertices[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), toCamera, zeroUV },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), toCamera, zeroUV  },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), toCamera, zeroUV  },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triIndices[] = { 0, 1, 2 };

	triangle = new Mesh(triVertices, sizeof(triVertices) / sizeof(Vertex), triIndices, sizeof(triIndices) / sizeof(unsigned int), device);

	float radius = 1;
	float height = radius * sqrt(3.0f) * 0.5f;
	float halfR = radius / 2.0f;

	XMFLOAT3 offset(0, 0, 0);

	Vertex hexVertices[] =
	{
		{ XMFLOAT3(-halfR + offset.x, height + offset.y, 0), toCamera, zeroUV },
		{ XMFLOAT3(halfR + offset.x, height + offset.y, 0), toCamera, zeroUV },
		{ XMFLOAT3(radius + offset.x, offset.y, 0), toCamera, zeroUV },
		{ XMFLOAT3(halfR + offset.x, -height + offset.y, 0), toCamera, zeroUV },
		{ XMFLOAT3(-halfR + offset.x, -height + offset.y, 0), toCamera, zeroUV },
		{ XMFLOAT3(-radius + offset.x, offset.y, 0), toCamera, zeroUV }
	};

	unsigned int hexIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5
	};

	hexagon = new Mesh(hexVertices, sizeof(hexVertices) / sizeof(Vertex), hexIndices, sizeof(hexIndices) / sizeof(unsigned int), device);

	cone = new Mesh("Assets/Models/cone.obj", device);
	cube = new Mesh("Assets/Models/cube.obj", device);
	cylinder = new Mesh("Assets/Models/cylinder.obj", device);
}


void Game::CreateEntities()
{
	entities.push_back(new Entity(cone, standardMaterial));
	entities.back()->SetPosition(XMFLOAT3(-2, 0, 0));

	entities.push_back(new Entity(cube, standardMaterial));

	entities.push_back(new Entity(cylinder, standardMaterial));
	entities.back()->SetPosition(XMFLOAT3(2, 0, 0));
	// entities.push_back(new Entity(hexagon, standardMaterial));
	// entities.push_back(new Entity(triangle, standardMaterial));
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	camera->UpdateProjectionMatrix((float)width / height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (int i = 0; i < entities.size(); ++i) {

		XMVECTOR rotationVec;
		rotationVec = XMQuaternionRotationRollPitchYaw(deltaTime, deltaTime, 0.0f);

		XMFLOAT4 rotation;
		XMStoreFloat4(&rotation, rotationVec);
		entities[i]->Rotate(rotation);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	pixelShader->SetData("lights", directionalLights, sizeof(DirectionalLight) * NUM_DIRECTIONAL_LIGHTS);

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Draw each entity
	for (Entity* entity : entities) {
		entity->RecalculateWorldMatrix();
		entity->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetPosition());

		ID3D11Buffer* entityVB = entity->GetMesh()->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &entityVB, &stride, &offset);
		context->IASetIndexBuffer(entity->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(entity->GetMesh()->GetIndexCount(), 0, 0);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain effect,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	int dx = x - prevMousePos.x;
	int dy = y - prevMousePos.y;

	camera->Rotate(dy * 0.001f, dx * 0.001f);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion