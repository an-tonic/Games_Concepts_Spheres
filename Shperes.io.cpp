//Serdyukov Anoniy
// Shperes.io.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>

#define NUMofCUBES 12

using namespace tle;
using namespace std;

//Constants
float kSphereSpeed = 90.0f;
float kRotationSpeed = 200.0f;
float kCamereSpeed = 70.0f;

int playerPoints = 0;
int enemyPoints = 0;
float sphereRadius = 10;
int minumumDistanceToObject = 10;
float minicubeBounary = 2.5 + sphereRadius;
double hyperTime = 5.0;
float push = 0.02;
bool pushSpheres = false;
float componentX = 0.0;
float componentZ = 0.0;

enum EGameState {GameOver, Playing, Paused, GameWon};
EGameState gameState = Playing;

enum EPowerUp {Regular, Hyper};
EPowerUp spherePowerUp = Regular;

enum EEnemySphere {Active, Dead};
EEnemySphere enemyState = Active;

//Functions

float* returnVector(IModel* a, IModel* b) {
	float vectorX = b->GetX() - a->GetX();
	float vectorY = b->GetY() - a->GetY();
	float vectorZ = b->GetZ() - a->GetZ();

	float fullVector[] = { vectorX, vectorY, vectorZ };

	return fullVector;
}

/// <summary>
/// Calculates the vector length.
/// </summary>
/// <param name="a">The pointer to the "from" object.</param>
/// <param name="b">The pointer to the "to" object.</param>
/// <returns>Length of a vector</returns>
float vectorLen(IModel* a, IModel* b) {
 	
	float* someVector =  returnVector(a, b);

	return sqrt(someVector[0]* someVector[0] + someVector[1] * someVector[1] + someVector[2] * someVector[2]);
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
		float distance = vectorLen(object, array[i]);
		if (distance < smallesDistance && distance > 0) {
			smallesDistance = distance;
		}
	}
	return smallesDistance;
}


/// <summary>
/// Iteratively randomly places the cube far from othercubes and players
/// </summary>
/// <param name="array">Collection of objects to take distance from.</param>
/// <param name="playerMesh">The player object.</param>
/// <param name="object">The object we are mesuring distance from.</param>
/// <param name="arraySize">Number of objects in the array.</param>
void respawnCube(IModel** array, IModel* playerMesh, IModel* object, int arraySize = NUMofCUBES) {
	object->SetPosition(rand() % 160 - 80, 5, rand() % 160 - 80);
	while (closestObject(array, playerMesh, object, arraySize) < minumumDistanceToObject) {
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
	myEngine->AddMediaFolder( "./Assessment1Resources");



	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kFPS);
	//TODO: change camera to manual
	myCamera->RotateLocalX(90);
	myCamera->SetPosition(0, 200, 0);

	IFont* myFont = myEngine->LoadFont("Times New Roman", 36);

	IMesh* waterMesh = myEngine->LoadMesh("water.x");
	IModel* water = waterMesh->CreateModel(0, -5, 0);

	IMesh* islandMesh = myEngine->LoadMesh("island.x");
	IModel* island = islandMesh->CreateModel(0, -5, 0);

	IMesh* skyMesh = myEngine->LoadMesh("sky.x");
	IModel* sky = skyMesh->CreateModel(0, -960, 0);

	IMesh* sphereMesh = myEngine->LoadMesh("spheremesh.x");
	IModel* playerSphere = sphereMesh->CreateModel(0, 10, 0);
	
	IMesh* enemySphereMesh = myEngine->LoadMesh("spheremesh.x");
	
	IModel* enemySphere = enemySphereMesh->CreateModel((rand() % 160 - 80), sphereRadius, (rand() % 160 - 80));
	while (vectorLen(playerSphere, enemySphere) < 60)
	{
		enemySphere->SetPosition((rand() % 160 - 80), sphereRadius, (rand() % 160 - 80));
	}
	enemySphere->SetSkin("enemysphere.jpg");


	IMesh* cubeMesh = myEngine->LoadMesh("minicube.x");
	
	

	//Holds the cubes
	IModel** cubes;
	
	cubes = new IModel * [NUMofCUBES];

	randomCubeGenerator(cubes, cubeMesh, playerSphere);

	IModel* hypercube = cubeMesh->CreateModel();
	respawnCube(cubes, playerSphere, hypercube);

	hypercube->SetSkin("hypercube.jpg");

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
				playerSphere->MoveLocalZ(kSphereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_S)) {
				playerSphere->MoveLocalZ(-kSphereSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_A)) {
				playerSphere->RotateLocalY(-kRotationSpeed * frameTime);
			}
			if (myEngine->KeyHeld(Key_D)) {
				playerSphere->RotateLocalY(kRotationSpeed * frameTime);
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

			//Cube proximity evaluator
			for (int i = 0; i < NUMofCUBES + 1; i++) {

				if (playerSphere->GetX() > cubes[i]->GetX() - minicubeBounary && playerSphere->GetX() < cubes[i]->GetX() + minicubeBounary &&
					playerSphere->GetY() > cubes[i]->GetY() - minicubeBounary && playerSphere->GetY() < cubes[i]->GetY() + minicubeBounary &&
					playerSphere->GetZ() > cubes[i]->GetZ() - minicubeBounary && playerSphere->GetZ() < cubes[i]->GetZ() + minicubeBounary) {


					//Update score for picking a sphere
					playerPoints += 10;

					respawnCube(cubes, playerSphere, cubes[i]);

					//Scale the player every 40 points and increase score 
					if (playerPoints % 40 == 0) {
						playerSphere->Scale(1.2);
						sphereRadius *= 1.2;
						playerSphere->SetY(sphereRadius);
						minicubeBounary = sphereRadius + 2.5;
					}

					//index of 13 is only hypercube
					if (i == 12) {
						cubes[i]->MoveLocalY(-100);
						spherePowerUp = Hyper;
					}
				}
			}
			//Hyper mode
			if (spherePowerUp == Hyper) {
				playerSphere->SetSkin("hypersphere.jpg");
				hyperTime -= frameTime;

				for (int i = 0; i < NUMofCUBES; i++) {
					if (vectorLen(playerSphere, cubes[i]) < 50) {
						cubes[i]->LookAt(playerSphere);
						cubes[i]->MoveLocalZ(kSphereSpeed * frameTime / 40 * vectorLen(playerSphere, cubes[i]));
					}
				}

				if (hyperTime < 0) {
					spherePowerUp = Regular;
					hyperTime = 5.0;
					playerSphere->SetSkin("regularsphere.jpg");
				}
			}
		}
		//Isometric camera
		if (myEngine->KeyHit(Key_2)) {
			myCamera->ResetOrientation();
			myCamera->SetPosition(150, 150, -150);
			myCamera->RotateLocalY(-45);
			myCamera->RotateLocalX(45);
		}

		if (myEngine->KeyHit(Key_1)) {
			myCamera->ResetOrientation();
			myCamera->SetPosition(0, 200, 0);
			myCamera->RotateLocalX(90);	
		}

		//Game pause
		if (myEngine->KeyHit(Key_P)) {
			if (gameState == Playing) {
				gameState = Paused;
			}
			else {
				gameState = Playing;
			}
		}
		if (gameState == Paused) {
			myFont->Draw("PAUSED", 0, 0);
		}

		//Spheres collision and logic
		if (enemyState == Active){
			
			//"10" is the radius of the enemy - it does not change
			if (vectorLen(playerSphere, enemySphere) < sphereRadius + 10) {
								
				//Points difference not greater than 40
				if (abs(playerPoints - enemyPoints) <= 40 && gameState != GameOver) {
					pushSpheres = true;
					float* collisonVector = returnVector(enemySphere, playerSphere);
					componentX = collisonVector[0];
					componentZ = collisonVector[2];
				}
				//Player has more points
				else if (playerPoints > enemyPoints) {
					enemySphere->MoveY(-100);
					enemyState = Dead;
					playerPoints += 40;
					sphereRadius *= 1.2;
					playerSphere->SetY(sphereRadius);
					minicubeBounary = sphereRadius + 2.5;
				}
				//Enemy has more points
				else if (playerPoints < enemyPoints && gameState != GameOver) {
					enemyPoints += 40;
					enemySphere->Scale(1.2);
					enemySphere->SetY(12);
					playerSphere->MoveY(-100);
					gameState = GameOver;				
				}		
			}
			
			//Enemy movement
			float closestCube = 1000;
			for (int i = 0; i < NUMofCUBES; i++) {

				if (vectorLen(enemySphere, cubes[i]) < closestCube) {
					enemySphere->LookAt(cubes[i]->GetX(), 10, cubes[i]->GetZ());
					closestCube = vectorLen(enemySphere, cubes[i]);
				}
				if (vectorLen(enemySphere, cubes[i]) < 10) {
					respawnCube(cubes, playerSphere, cubes[i]);

					if (gameState != GameOver) {
						enemyPoints += 10;
					}
				}
			}
		}

		//Spheres are pushed from each other
		if (pushSpheres) {
			playerSphere->Move(componentX* push, 0, componentZ* push);
			enemySphere->Move(-componentX * push, 0, -componentZ * push);
			push -= 0.0003;
			if (push <= 0) {
				push = 0.02;
				pushSpheres = false;
			}
		}

		//Game won
		if (playerPoints >= 120) {
			myFont->Draw("Congratulations! You have won!", 635 - myFont->MeasureTextWidth("Congratulations! You have won!")/2, 300);
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
			gameState = GameOver;
		}

		//Game over when outside of island
		if (abs(playerSphere->GetX()) > 110 || abs(playerSphere->GetZ()) > 110) {
			myFont->Draw("Congratulations! You have DIED!", 400, 300);
			gameState = GameOver;
			playerSphere->SetY(-5);
		}

		//Enemy dead when in water
		if (abs(enemySphere->GetX()) > 110 || abs(enemySphere->GetZ()) > 110) {
			enemyState = Dead;
			enemySphere->SetY(-5);
			
		}

		if (gameState == GameOver) {
			myFont->Draw("Game over", 635 - myFont->MeasureTextWidth("Game over")/2, 250);
			myFont->Draw("Enemy Points: " + to_string(enemyPoints), 1270 - myFont->MeasureTextWidth("Enemy Points: " + to_string(enemyPoints)), 10);
			myFont->Draw("Your Points: " + to_string(playerPoints), 1270 - myFont->MeasureTextWidth("Your Points: " + to_string(playerPoints)), 50);
			enemySphere->MoveLocalZ(kSphereSpeed * frameTime / 2);
		}
	}
	
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
