// Shperes.io.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

#define NUMofCUBES 12

using namespace tle;
using namespace std;

//Constants
float kSphereSpeed = 0.05f;
float kRotationSpeed = 0.05f;
float kCamereSpeed = 0.05f;
int playerPoints = 0;
int sphereRadius = 10;


enum EGameState {GameOver, Playing, Paused};
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
/// Randomizes the cube positions withing the -80/80 coordinates.
/// </summary>
/// <param name="array">Array pointer for the cube.</param>
/// <param name="mesh">Mesh of a cube.</param>
void randomCubeGenerator(IModel **array, IMesh* mesh, IModel* playerMesh) {
	
	//Random cube generation
	srand(time(NULL));
	for (int i = 0; i < NUMofCUBES; i++) {

		IModel* newCube = mesh->CreateModel(rand() % 160 - 80, 5, rand() % 160 - 80);

		

		for (int j = 0; j <= i; j++) {
			
			while (vectorLen(newCube, playerMesh) < 10 && vectorLen(newCube, array[j]) < 10) {
				newCube->SetPosition(rand() % 160 - 80, 5, rand() % 160 - 80);
			}
			array[i] = newCube;

		}
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

	randomCubeGenerator(cubes, cubeMesh, sphere);


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning() && gameState != GameOver)
	{
		// Draw the scene
		
		myEngine->DrawScene();
		

		/**** Update your scene each frame here ****/
		myFont->Draw("Points: " + to_string(playerPoints), 1100, 10);


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

		}
		//Exit game
		if (myEngine->KeyHit(Key_Escape)) {
			gameState = GameOver;
		}

		//Game pause
		if (myEngine->KeyHit(Key_P)) {
			if (gameState == Playing) {
				gameState = Paused;
				
			}else{
				gameState = Playing;
			}
		}
		//Cube proximity evaluator
		for (IModel* cube : cubes) {
			
			if (vectorLen(sphere, cube) < 5 + 5) {
				//cubeMesh->RemoveModel(cube);

				//Update score for picking a sphere
				playerPoints += 10;
				
				//Hide the cube
				cube->MoveLocalY(-100);

				//Scale the player every 40 points and increase score 
				if (playerPoints % 40 == 0) {
					sphere->Scale(1.2);
					sphereRadius *= 1.2;
					sphere->SetY(sphereRadius);
					
				}

		
			}
		}

		

		//Game over when outside of island
		/*if (abs(sphere->GetX()) > 100 || abs(sphere->GetZ()) > 100) {
			gameState = GameOver;
		}*/
		
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
