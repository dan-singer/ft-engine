# FT Engine
The FT Engine is a Component-based DirectX 11 Game Engine.

## The Architecture
Everything about the engine is based on Components. To add functionality to `Entities`, create a new class that extends the base `Component` class. 

## The World
All resources and Entities are managed through a `World` singleton. Do not attempt to create entities or resources manually. Use the `World` for this. This way, memory leaks can be avoided.

## Transform
Each Entity comes with a `Transform` component out of the box, which can be used to manipulate the postion, rotation, and scale of entities.

## Rendering
Everything gets rendered through the World's `DrawEntities` method. This loops through all Entities and draws them in the manner appropriate for the Entity. An Entity needs a Material and Mesh to be rendered. These can be attached with `MaterialComponent` and `MeshComponent`. 

### CameraComponent
A `CameraComponent` can be attached to any `Entity`. This is where the scene will be rendered from. Because it is attached to an `Entity` and all `Entities` have `Transforms`, moving the `Entity` will move the camera's viewpoint. 

### Materials
Materials are implemented as a collection of resources and shaders, and are built to be customizable. Direct PBR is used as the lighting model. 

### Static Meshes
Meshes can be specified as either a collection of verticies or loaded from an .obj file. Animations are currently not supported.

### Lighting
FT Engine supports Point, Spot, and Directional Lights using direct PBR, as well as cubemap reflections.

### Particle Systems
Basic CPU-driven particle systems are implemented. Take a look at `Explosion.json` to see how to customize them.

### UI
A basic UI system has been implemented and is based upon Anchors and origins. 

## Integrations
Two external libraries have been implemented into the engine: Bullet and FMOD.

### Bullet
Bullet is used for 3D physics, and can be utilized with the `RigidBodyComponent`. Collision callbacks are available in Component-derived classes.

### FMOD
FMOD is used for audio, and can be utilized with the `SoundComponent`

## Game
The `Game` class is where everything is setup. Make entities and add components to them here.

## What comes out of the box?
The engine comes with a demo scene featuring Bullet physics, cubes, and a particle system. If you'd like to see a more fully-featured example, take a look at [Shape Shooter](https://github.com/dan-singer/shape-shooter)

## Contributions
This project was developed at RIT by Michael Capra, Michelle Petilli, Dan Singer, and Julian Washington. Starter code was provided by [Chris Cascioli](https://www.rit.edu/directory/cdccis-chris-cascioli). 

