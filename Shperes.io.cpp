// Shperes.io.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

#define NUMofCUBES 12

using namespace tle;
using namespace std;

//Constants
float kSphereSpeed = 90.0f;
float kRotationSpeed = 200.0f;
float kCamereSpeed = 90.0f;

int playerPoints = 0;
int sphereRadius = 10;
int minumumDistance = 10;
float minicubeBounary = 2.5 + sphereRadius;

enum EGameState {GameOver, Playing, Paused, GameWon};
EGameState gameState = Playing;
enum ECamera {TopView, Isometric};



//Functions

/// <summary>
/// Calculates the vector length.
/// </summary>
/// <param name="a">The pointer to the "from" object.</param>
/// <param name="b">The pointer to the "to" object.</param>
/// <returns>Length of a vector</returns>
float vectorLen(IModel* a, IModel* b) {
 	float vectorX = b->GetX() - a->GetX();
	float vectorY = b->GetY() - a->GetY();
	float vectorZ = b->GetZ() - a->GetZ();

	return sqrt(vectorX * vectorX + vectorY * vectorY + vectorZ * vectorZ);
}


/// <summary>
/// Returns the closest distance of an object to surrounding objects and player
/// </summary>
/// <param name="array">Collection of objects to take distance from.</param>
/// <param name="playerMesh">The player object.</param>
/// <param name="object">The object we are mesuring distance from.</param>
/// <param name="arraySize">Number of objects in the array.</param>
/// <returns>Shortest distance to given object (Float). </returns>
float closestObject(IModel** array, IModel* playerMesh, IModel* object, int arraySize) {
	float smallesDistance = vectorLen(object, playerMesh);
	

	for (int i = 0; i < arraySize; i++) {
		int distance = vectorLen(object, array[i]);
		if (distance < smallesDistance && distance > 0) {
			smallesDistance = distance;
		}
	}
	return smallesDistance;
}

void respawnCube(IModel** array, IModel* playerMesh, IModel* object, int arraySize = NUMofCUBES) {
	object->SetPosition(rand() % 160 - 80, 5, rand() % 160 - 80);
	while (closestObject(array, playerMesh, object, arraySize) < minumumDistance) {
		object->SetPosition(rand() % 160 - 80, 5, rand() % 160 - 80);
	}
}

/// <summary>
/// Randomizes the cube positions withing the -80/80 coordinates.
/// </summary>
/// <param name="array">Array pointer for the cube.</param>
/// <param name="mesh">Mesh of a cube.</param>
void randomCubeGenerator(IModel **array, IMesh* mesh, IModel* playerMesh) {
	
	//Random cube generation
	
	for (int i = 0; i < NUMofCUBES; i++) {

		IModel* newCube = mesh->CreateModel(rand() % 160 - 80, 5, rand() % 160 - 80);
		respawnCube(array, playerMesh, newCube, i);
		array[i] = newCube;
	}	
}




void main()
{
	


	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\aserdyukov\\source\\repos\\Games-Concepts-Spheres\\Assessment1Resources");

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kFPS);
	//TODO: change camera to manual
	myCamera->RotateLocalX(90);
	myCamera->SetPosition(0, 200, 0);
	ECamera myECamera = TopView;

	IFont* myFont = myEngine->LoadFont("Times New Roman", 36);

	IMesh* waterMesh = myEngine->LoadMesh("water.x");
	IModel* water = waterMesh->CreateModel(0, -5, 0);

	IMesh* islandMesh = myEngine->LoadMesh("island.x");
	IModel* island = islandMesh->CreateModel(0, -5, 0);

	IMesh* skyMesh = myEngine->LoadMesh("sky.x");
	IModel* sky = skyMesh->CreateModel(0, -960, 0);

	IMesh* sphereMesh = myEngine->LoadMesh("spheremesh.x");
	IModel* sphere = sphereMesh->CreateModel(0, 10, 0);
	
	IMesh* cubeMesh = myEngine->LoadMesh("minicube.x");
	
	
	
	
	//Holds the cubes
	IModel* cubes[NUMofCUBES];
	srand(time(NULL));
	randomCubeGenerator(cubes, cubeMesh, sphere);

	
	

	//Starting the timer for variable timer
	myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->StopMouseCapture();
		myEngine->DrawScene();

		//Storage for previous frame render time
		float frameTime = myEngine->Timer();

		/**** Update your scene each frame here ****/
		myFont->Draw("Points: " + to_string(playerPoints), 1100, 10);
		
		if (gameState != Paused && gameState != GameOver && gameState != GameWon) {
			//The sphere movement
			if (myEngine->KeyHeld(Key_W)) {
				sphere->MoveLocalZ(kSphereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_S)) {
				sphere->MoveLocalZ(-kSphereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_A)) {
				sphere->RotateLocalY(-kRotationSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_D)) {
				sphere->RotateLocalY(kRotationSpeed * frameTime);
			}
			//Camere movement
			if (myEngine->KeyHeld(Key_Right)) {
				myCamera->MoveX(kCamereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_Left)) {
				myCamera->MoveX(-kCamereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_Up)) {
				myCamera->MoveZ(kCamereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_Down)) {
				myCamera->MoveZ(-kCamereSpeed * frameTime);
			}
		}
		//Isometric camera
		if (myEngine->KeyHit(Key_2) && myECamera == TopView) {
			myCamera->ResetOrientation();
			myCamera->SetPosition(150, 150, -150);
			myCamera->RotateLocalY(-45);
			myCamera->RotateLocalX(45);
			myECamera = Isometric;
		}
				
		if (myEngine->KeyHit(Key_1) && myECamera == Isometric) {
			myCamera->ResetOrientation();
			myCamera->SetPosition(0, 200, 0);
			myCamera->RotateLocalX(90);
			myECamera = TopView;
		}

		//Game pause
		if (myEngine->KeyHit(Key_P)) {
			if (gameState == Playing) {
				gameState = Paused;				
			}else{
				gameState = Playing;
			}
		}
		if (gameState == Paused) {
			myFont->Draw("PAUSED", 0, 0);
		}

		//Cube proximity evaluator
		for (IModel* cube : cubes) {
			
			if (sphere->GetX() > cube->GetX() - minicubeBounary && sphere->GetX() < cube->GetX() + minicubeBounary &&
				sphere->GetY() > cube->GetY() - minicubeBounary && sphere->GetY() < cube->GetY() + minicubeBounary &&
				sphere->GetZ() > cube->GetZ() - minicubeBounary && sphere->GetZ() < cube->GetZ() + minicubeBounary) {
				//cubeMesh->RemoveModel(cube);

				//Update score for picking a sphere
				playerPoints += 10;
				
				//Hide the cube
				respawnCube(cubes, sphere, cube);

				//Scale the player every 40 points and increase score 
				if (playerPoints % 40 == 0) {
					sphere->Scale(1.2);
					sphereRadius *= 1.2;
					sphere->SetY(sphereRadius);
					minicubeBounary = sphereRadius + 2.5;
				}
			}
		}
		//Game finished
		if (playerPoints >= 120) {
			myFont->Draw("Congratulations! You have won!", 400, 300);
			gameState = GameWon;
		}
		//Exit game
		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}

		//Game over when outside of island
		if (abs(sphere->GetX()) > 100 || abs(sphere->GetZ()) > 100) {
			myFont->Draw("Congratulations! You have DIED!", 400, 300);
			gameState = GameOver;
		}
		
	}
	
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
