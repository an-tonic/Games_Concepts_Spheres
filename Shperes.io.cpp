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
int enemyPoints = 0;
int sphereRadius = 10;
int minumumDistance = 10;
float minicubeBounary = 2.5 + sphereRadius;
double hyperTime = 5.0;

enum EGameState {GameOver, Playing, Paused, GameWon};
EGameState gameState = Playing;
enum ECamera {TopView, Isometric};
enum EPowerUp {Regular, Hyper};

EPowerUp spherePowerUp = Regular;

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

	for (int i = 0; i < NUMofCUBES; i++) {

		IModel* newCube = mesh->CreateModel(rand() % 160 - 80, 5, rand() % 160 - 80);
		respawnCube(array, playerMesh, newCube, i);
		array[i] = newCube;
	}	
}




void main()
{
	srand(time(NULL));

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
	
	IMesh* enemySphereMesh = myEngine->LoadMesh("spheremesh.x");
	
	IModel* enemySphere = enemySphereMesh->CreateModel((rand() % 160 - 80), 10, (rand() % 160 - 80));
	while (vectorLen(sphere, enemySphere) < 60)
	{
		enemySphere->SetPosition((rand() % 160 - 80), 10, (rand() % 160 - 80));
	}
	enemySphere->SetSkin("enemysphere.jpg");

	IMesh* cubeMesh = myEngine->LoadMesh("minicube.x");
	
	IModel* hypercube = cubeMesh->CreateModel(rand() % 160 - 80, 10, rand() % 160 - 80);
	hypercube->SetSkin("hypercube.jpg");
	
	
	//Holds the cubes
	IModel** cubes;
	
	cubes = new IModel * [NUMofCUBES];

	randomCubeGenerator(cubes, cubeMesh, sphere);

	cubes[NUMofCUBES] = hypercube;
	

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
		
		
		

		if (gameState == Playing) {
			myFont->Draw("My Points: " + to_string(playerPoints), 1270 - myFont->MeasureTextWidth("My Points: " + to_string(playerPoints)), 10);
			enemySphere->MoveLocalZ(kSphereSpeed * frameTime / 2);

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
		for (int i = 0; i < NUMofCUBES+1; i++) {
			
			if (sphere->GetX() > cubes[i]->GetX() - minicubeBounary && sphere->GetX() < cubes[i]->GetX() + minicubeBounary &&
				sphere->GetY() > cubes[i]->GetY() - minicubeBounary && sphere->GetY() < cubes[i]->GetY() + minicubeBounary &&
				sphere->GetZ() > cubes[i]->GetZ() - minicubeBounary && sphere->GetZ() < cubes[i]->GetZ() + minicubeBounary) {
				//cubeMesh->RemoveModel(cube);

				//Update score for picking a sphere
				playerPoints += 10;
				
				respawnCube(cubes, sphere, cubes[i]);

				//Scale the player every 40 points and increase score 
				if (playerPoints % 40 == 0) {
					sphere->Scale(1.2);
					sphereRadius *= 1.2;
					sphere->SetY(sphereRadius);
					minicubeBounary = sphereRadius + 2.5;
				}

				//index of 13 is only hypercube
				if (i == 12) {
					cubes[i]->MoveLocalY(-100);
					spherePowerUp = Hyper;
				}
			}
		}

		//Enemy movement
		float closestCube = 1000;
		for (int i = 0; i < NUMofCUBES; i++) {
			

			if (vectorLen(enemySphere, cubes[i]) < closestCube) {
				enemySphere->LookAt(cubes[i]);
				
				closestCube = vectorLen(enemySphere, cubes[i]);
			}
			if (vectorLen(enemySphere, cubes[i]) < 10) {
				respawnCube(cubes, sphere, cubes[i]);

				if (gameState != GameOver) {
					enemyPoints += 10;
				}
				
			}
		}
		

		//Hyper mode
		if (spherePowerUp == Hyper) {
			sphere->SetSkin("hypersphere.jpg");
			hyperTime -= frameTime;


			for (int i = 0; i < NUMofCUBES; i++) {
				if (vectorLen(sphere, cubes[i]) < 50) {
					cubes[i]->LookAt(sphere);
					cubes[i]->MoveLocalZ(kSphereSpeed * frameTime/40 * vectorLen(sphere, cubes[i]));
				}
			}

			if (hyperTime < 0) {
				spherePowerUp = Regular;
				hyperTime = 5.0;
				sphere->SetSkin("regularsphere.jpg");
			}
		}

		//Game won
		if (playerPoints >= 120) {
			myFont->Draw("Congratulations! You have won!", 400, 300);
			myFont->Draw("Enemy Points: " + to_string(enemyPoints), 1270 - myFont->MeasureTextWidth("Enemy Points: " + to_string(enemyPoints)), 50);
			myFont->Draw("Your Points: " + to_string(playerPoints), 1270 - myFont->MeasureTextWidth("Your Points: " + to_string(playerPoints)), 10);
			gameState = GameWon;
		}
		//Exit game
		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}

		//Game over when enemy has more points 
		if (enemyPoints >= 120) {
			myFont->Draw("Congratulations! You have lost the game to a BOT!", 400, 300);
			gameState = GameOver;
		}

		//Game over when outside of island
		if (abs(sphere->GetX()) > 100 || abs(sphere->GetZ()) > 100) {
			myFont->Draw("Congratulations! You have DIED!", 400, 300);
			gameState = GameOver;
		}
		if (gameState == GameOver) {
			myFont->Draw("Enemy Points: " + to_string(enemyPoints), 1270 - myFont->MeasureTextWidth("Enemy Points: " + to_string(enemyPoints)), 10);
			myFont->Draw("Your Points: " + to_string(playerPoints), 1270 - myFont->MeasureTextWidth("Your Points: " + to_string(playerPoints)), 50);
			enemySphere->MoveLocalZ(kSphereSpeed* frameTime / 2);
		}
	}
	
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
