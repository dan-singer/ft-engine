#include "Game.h"
#include "Vertex.h"
#include <WICTextureLoader.h>
#include <time.h>
#include "Transform.h"
#include "MaterialComponent.h"
#include "DebugMovement.h"
#include "World.h"
#include "Rotator.h"
#include "RigidBodyComponent.h"
#include "CollisionTester.h"
#include <SpriteFont.h>
#include "UITextComponent.h"
#include "ButtonComponent.h"

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

	prevMousePos = { 0,0 };

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

Game::~Game()
{

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	LoadResources();
	CreateEntities();	
	World::GetInstance()->Start();
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
void Game::LoadResources()
{
	World* world = World::GetInstance();

	// Meshes
	world->CreateMesh("cube", "Assets/Models/cube.obj", device);

	// Shaders
	SimpleVertexShader* vs = world->CreateVertexShader("vs", device, context, L"VertexShader.cso");
	SimplePixelShader* ps  = world->CreatePixelShader("ps", device, context, L"PixelShader.cso");
	SimpleVertexShader* particleVs = world->CreateVertexShader("particle", device, context, L"ParticleVS.cso");
	SimplePixelShader* particlePs = world->CreatePixelShader("particle", device, context, L"ParticlePS.cso");

	// Textures
	world->CreateTexture("leather", device, context, L"Assets/Textures/Leather.jpg");
	world->CreateTexture("metal", device, context, L"Assets/Textures/BareMetal.png");
	world->CreateTexture("velvet_normal", device, context, L"Assets/Textures/Velvet_N.jpg");
	world->CreateTexture("particle", device, context, L"Assets/Textures/particle.jpg");

	// Create the sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	world->CreateSamplerState("main", &samplerDesc, device);

	// UI
	world->CreateSpriteBatch("main", context);
	world->CreateFont("Open Sans", device, L"Assets/Fonts/open-sans.spritefont");


	// Particles
	D3D11_DEPTH_STENCIL_DESC particleDsDesc = {};
	particleDsDesc.DepthEnable = true;
	particleDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable depth writing
	particleDsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	world->CreateDepthStencilState("particle", &particleDsDesc, device);

	D3D11_BLEND_DESC particleBlendDesc = {};
	particleBlendDesc.AlphaToCoverageEnable = false;
	particleBlendDesc.IndependentBlendEnable = false;
	particleBlendDesc.RenderTarget[0].BlendEnable = true;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Still respect pixel shader output alpha
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	world->CreateBlendState("particle", &particleBlendDesc, device);

	

	// Materials
	world->CreateMaterial("leather", vs, ps, world->GetTexture("leather"), world->GetTexture("velvet_normal"), world->GetSamplerState("main"));
	Material* metal = world->CreateMaterial("metal", vs, ps, world->GetTexture("metal"), world->GetTexture("velvet_normal"), world->GetSamplerState("main"));
	metal->m_specColor = DirectX::XMFLOAT3(0.662124f, 0.654864f, 0.633732f);
	metal->m_roughness = 1.0f;
	metal->m_metalness = 0;
	world->CreateMaterial(
		"particle",
		particleVs,
		particlePs,
		world->GetTexture("particle"),
		nullptr,
		world->GetSamplerState("main"),
		world->GetBlendState("particle"),
		world->GetDepthStencilState("particle")
	);

}


void Game::CreateEntities()
{
	World* world = World::GetInstance();

	Entity* cube1 = world->Instantiate("cube1");
	cube1->GetTransform()->SetPosition(XMFLOAT3(0, 0, 0));
	XMFLOAT4 rot;
	XMStoreFloat4(&rot, XMQuaternionRotationRollPitchYaw(10.0f, 10.0f, 10.0f));
	cube1->GetTransform()->SetRotation(rot);
	cube1->AddComponent<MeshComponent>()->m_mesh = world->GetMesh("cube");
	cube1->AddComponent<MaterialComponent>()->m_material = world->GetMaterial("metal");
	RigidBodyComponent* rb = cube1->AddComponent<RigidBodyComponent>();
	rb->SetBoxCollider(.5f, .5f, .5f);
	rb->m_mass = 1.0f; // This has mass so it will be affected by gravity

	Entity* ground = world->Instantiate("ground");
	ground->GetTransform()->SetPosition(XMFLOAT3(0, -3, 0));
	ground->AddComponent<MeshComponent>()->m_mesh = world->GetMesh("cube");
	ground->AddComponent<MaterialComponent>()->m_material = world->GetMaterial("metal");
	ground->AddComponent<RigidBodyComponent>()->SetBoxCollider(.5f, .5f, .5f);

	Entity* camera = world->Instantiate("Cam");
	CameraComponent* cc = camera->AddComponent<CameraComponent>();
	cc->UpdateProjectionMatrix((float)width / height);
	camera->GetTransform()->SetPosition(XMFLOAT3(0, 0, -5));
	camera->AddComponent<DebugMovement>();
	world->m_mainCamera = cc;

	Entity* dirLight = world->Instantiate("DirLight1");
	LightComponent* dirLightComp = dirLight->AddComponent<LightComponent>();
	dirLightComp->m_data.type = LightComponent::Directional;
	dirLightComp->m_data.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dirLightComp->m_data.intensity = 1.0f;
	
	/*Entity* pointLight = world->Instantiate("PointLight1");
	LightComponent* pointLightComp = pointLight->AddComponent<LightComponent>();
	pointLightComp->m_data.type = LightComponent::Point;
	pointLightComp->m_data.color = XMFLOAT3(1.0f, 0, 0);
	pointLightComp->m_data.intensity = 1.0f;
	pointLight->GetTransform()->SetPosition(XMFLOAT3(-1, 1, 0));

	Entity* spotLight = world->Instantiate("SpotLight1");
	LightComponent* spotLightComp = spotLight->AddComponent<LightComponent>();
	spotLightComp->m_data.type = LightComponent::Spot;
	spotLightComp->m_data.color = XMFLOAT3(0, 1.0f, 0);
	spotLightComp->m_data.intensity = 1.0f;
	spotLightComp->m_data.spotFalloff = 1.0f;
	spotLight->GetTransform()->SetPosition(XMFLOAT3(1, 1, 0));
	XMFLOAT4 spotLightRot;
	XMStoreFloat4(&spotLightRot, XMQuaternionRotationRollPitchYaw(0, 90.0f, 0));
	spotLight->GetTransform()->SetRotation(spotLightRot);*/

	Entity* sprite = world->Instantiate("sprite");
	sprite->AddComponent<UITransform>()->Init(Anchor::BOTTOM_RIGHT, 0, XMFLOAT2(1, 1), XMFLOAT2(.25f,.25f), XMFLOAT2(0, 0));
	sprite->AddComponent<MaterialComponent>()->m_material = world->GetMaterial("leather");
	ButtonComponent* spriteButton = sprite->AddComponent<ButtonComponent>();
	spriteButton->AddOnEnter([]() 
		{
			printf("Entered\n");
		}
	);
	spriteButton->AddOnExit([]()
		{
			printf("Exited\n");
		}
	);

	Entity* text = world->Instantiate("text");
	text->AddComponent<UITransform>()->Init(Anchor::CENTER_CENTER, 0, XMFLOAT2(.5f, .5f), XMFLOAT2(1, 1), XMFLOAT2(0, 0));
	text->AddComponent<UITextComponent>()->Init("Hello World", world->GetFont("Open Sans"), Colors::White);
	ButtonComponent* button = text->AddComponent<ButtonComponent>();
	button->AddOnClick([]() 
		{
			Entity* text = World::GetInstance()->Find("text");
			text->GetComponent<UITextComponent>()->m_color = Colors::Black;
		}
	);
	
	// Particle System Test
	Entity* particleSystem = world->Instantiate("particle-system");
	//particleSystem->AddComponent<EmitterComponent>()->Init(
	//	110,
	//	20,
	//	5,
	//	2.0f,
	//	0.1f,
	//	2.0f,
	//	XMFLOAT4(1, 0.1f, 0.1f, 0.7f),
	//	XMFLOAT4(1, 0.6f, 0.1f, 0),
	//	XMFLOAT3(-2, 2, 0),
	//	XMFLOAT3(0.2f, 0.2f, 0.2f),
	//	XMFLOAT3(2, 0, 0),
	//	XMFLOAT3(0.1f, 0.1f, 0.1f),
	//	XMFLOAT4(-2, 2, -2, 2),
	//	XMFLOAT3(0, 0, 0),				
	//	device
	//);
	// Reading particle data from a configuration json file
	particleSystem->AddComponent<EmitterComponent>()->Init("Assets/Particles/Explosion.json", device);
	particleSystem->AddComponent<MaterialComponent>()->m_material = world->GetMaterial("particle");
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	World::GetInstance()->OnResize(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	World::GetInstance()->Tick(deltaTime);
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

	
	// Draw each entity
	SpriteBatch* mainSpriteBatch = World::GetInstance()->GetSpriteBatch("main");
	World::GetInstance()->DrawEntities(context, mainSpriteBatch, width, height);

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
	World::GetInstance()->OnMouseDown(buttonState, x, y);
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Capture the mouse so we keep getting mouse move
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
	World::GetInstance()->OnMouseUp(buttonState, x, y);
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
	World::GetInstance()->OnMouseMove(buttonState, x, y);
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
	World::GetInstance()->OnMouseWheel(wheelDelta, x, y);
}
#pragma endregion