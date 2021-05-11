#include "Application.h"
#include <raylib.h>
#include <vector>

#include "MotionCapter.h"
#include "Keypoint.h"

using namespace std;

void openmocap2::Application::Start()
{
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

	// Define the camera to look into our 3d world
	Camera3D camera = { 0 };
	camera.position = Vector3 { 0.0f, 10.0f, 10.0f };  // Camera position
	camera.target = Vector3 { 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = Vector3 { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------


	MotionCapter mocaper;
	vector<vector<Keypoint>> keypoints;
	mocaper.Start(keypoints);

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		auto& kps = keypoints[0];
		for (size_t i = 0; i < kps.size(); i++) {
			auto& k = kps[i];
			DrawSphere(Vector3{ k.position.x, k.position.y, k.position.z }, 2, RED);
		}

		/*DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
		DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);*/

		DrawGrid(10, 1.0f);

		EndMode3D();

		DrawText("Welcome to the third dimension!", 10, 40, 20, DARKGRAY);

		DrawFPS(10, 10);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}

