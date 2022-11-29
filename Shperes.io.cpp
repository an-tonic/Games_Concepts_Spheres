// Shperes.io.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

using namespace tle;
using namespace std;

//Constants
float kSphereSpeed = 0.05f;
float kRotationSpeed = 0.05f;
float kCamereSpeed = 0.05f;

enum EGameState {Over, Running, Paused};
EGameState gameState = Running;


//Functions

/// <summary>
/// Calculates the vector length.
/// </summary>
/// <param name="a">The pointer to the "from" object.</param>
/// <param name="b">The pointer to the "to" object.</param>
/// <returns></returns>
float vectorLen(IModel* a, IModel* b) {
	float vectorX = b->GetX() - a->GetX();
	float vectorY = b->GetY() - a->GetY();
	float vectorZ = b->GetZ() - a->GetZ();

	return sqrt(vectorX * vectorX + vectorY * vectorY + vectorZ * vectorZ);
}


void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\aserdyukov\\source\\repos\\Games-Concepts-Spheres\\Assessment1Resources");

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	//TODO: Change camera to Manual
	myCamera->RotateLocalX(90);
	myCamera->SetPosition(0, 200, 0);

	IMesh* waterMesh = myEngine->LoadMesh("water.x");
	IModel* water = waterMesh->CreateModel(0, -5, 0);

	IMesh* islandMesh = myEngine->LoadMesh("island.x");
	IModel* island = islandMesh->CreateModel(0, -5, 0);

	IMesh* skyMesh = myEngine->LoadMesh("sky.x");
	IModel* sky = skyMesh->CreateModel(0, -960, 0);

	IMesh* sphereMesh = myEngine->LoadMesh("spheremesh.x");
	IModel* sphere = sphereMesh->CreateModel(0, 10, 0);

	IMesh* cubeMesh = myEngine->LoadMesh("minicube.x");
	
	IModel* cube1 = cubeMesh->CreateModel(-80, 5, 80);
	IModel* cube2 = cubeMesh->CreateModel(80, 5, 80);
	IModel* cube3 = cubeMesh->CreateModel(80, 5, -80);
	IModel* cube4 = cubeMesh->CreateModel(-80, 5, -80);

	IModel* cubes[] = {cube1, cube2, cube3 , cube4};


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning() && gameState != Over)
	{
		// Draw the scene
		
		myEngine->DrawScene();
		
		

		/**** Update your scene each frame here ****/

		if (gameState != Paused) {
			//The sphere movement
			if (myEngine->KeyHeld(Key_W)) {
				sphere->MoveLocalZ(kSphereSpeed);
			}
			if (myEngine->KeyHeld(Key_S)) {
				sphere->MoveLocalZ(-kSphereSpeed);
			}
			if (myEngine->KeyHeld(Key_A)) {
				sphere->RotateLocalY(-kRotationSpeed);
			}
			if (myEngine->KeyHeld(Key_D)) {
				sphere->RotateLocalY(kRotationSpeed);
			}
			//Camere movement
			if (myEngine->KeyHeld(Key_Right)) {
				myCamera->MoveX(kCamereSpeed);
			}
			if (myEngine->KeyHeld(Key_Left)) {
				myCamera->MoveX(-kCamereSpeed);
			}
			if (myEngine->KeyHeld(Key_Up)) {
				myCamera->MoveZ(kCamereSpeed);
			}
			if (myEngine->KeyHeld(Key_Down)) {
				myCamera->MoveZ(-kCamereSpeed);
			}

		}
		//Exit game
		if (myEngine->KeyHit(Key_Escape)) {
			gameState = Over;
		}

		//Game pause
		if (myEngine->KeyHit(Key_P)) {
			if (gameState == Running) {
				gameState = Paused;
				
			}else{
				gameState = Running;
			}
		}
	//TODO: remove model properly.
		//Cube proximity evaluator
		for (IModel* cube : cubes){
			if (vectorLen(sphere, cube) < 5 + 5) {
				cubeMesh->RemoveModel(cube);
				break;
			}
		}

		

		//Game over when outside of island
		if (abs(sphere->GetX()) > 100 || abs(sphere->GetZ()) > 100) {
			gameState = Over;
		}
		
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
