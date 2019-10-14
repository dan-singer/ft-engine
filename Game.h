#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "Lights.h"

#define NUM_DIRECTIONAL_LIGHTS 1

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateEntities();


	// Mesh pointers
	Mesh* cube = nullptr;

	// Entities
	std::vector<Entity*> entities;

	// Camera
	Entity* camera = nullptr;

	// Materials
	Material* leatherMat = nullptr;
	Material* metalMat = nullptr;


	// Lights
	DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS] = {};

	// Sampler State
	ID3D11SamplerState* samplerState = nullptr;


	// Textures 
	ID3D11ShaderResourceView* leatherSRV = nullptr;
	ID3D11ShaderResourceView* metalSRV = nullptr;


	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

