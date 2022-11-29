// Shperes.io.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\aserdyukov\\source\\repos\\Games-Concepts-Spheres\\Assessment1Resources");

	/**** Set up your scene here ****/
	ICamera* myCamera = myEngine->CreateCamera(kFPS);
	//TODO: Change camera to Manual
	//myCamera->RotateLocalX(90);
	//myCamera->SetPosition(0, 200, 0);

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

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
