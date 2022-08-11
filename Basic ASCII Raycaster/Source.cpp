#include <iostream>
#include <Windows.h>
#include <chrono>

int main()
{

	int screenWidth = 120;
	int screenHeight = 40;

	float playerPosX = 8.0f;
	float playerPosY = 8.0f;
	float playerPosZ = 8.0f;
	float playerFOV = 3.14159 / 4.0;

	int mapWidth = 16;
	int mapHeight = 16;
	
	float depth = 16.0f;

	wchar_t *screen = new wchar_t[screenWidth * screenHeight];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	std::wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.....####.....#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......#......#";
	map += L"#.......#......#";
	map += L"#.......#......#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();


	while (1)
	{
		//Time
		//Get the delta time
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> tdeltaTime = tp2 - tp1;
		tp1 = tp2;

		float deltaTime = tdeltaTime.count();

		//Controls
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			playerPosZ -= 0.6f * deltaTime;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			playerPosZ += 0.6f * deltaTime;
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			playerPosX += sinf(playerPosZ) * 5.0f * deltaTime;
			playerPosY += cosf(playerPosZ) * 5.0f * deltaTime;

			//Collision detection
			//Simple correctional collision detection
			if (map[(int)playerPosY * mapWidth + (int)playerPosX] == '#')
			{
				playerPosX -= sinf(playerPosZ) * 5.0f * deltaTime;
				playerPosY -= cosf(playerPosZ) * 5.0f * deltaTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			playerPosX -= sinf(playerPosZ) * 5.0f * deltaTime;
			playerPosY -= cosf(playerPosZ) * 5.0f * deltaTime;
			
			//Collision detection
			//Simple correctional collision detection
			if (map[(int)playerPosY * mapWidth + (int)playerPosX] == '#')
			{
				playerPosX += sinf(playerPosZ) * 5.0f * deltaTime;
				playerPosY += cosf(playerPosZ) * 5.0f * deltaTime;
			}
		}


		for (size_t x = 0; x < screenWidth; x++)
		{
			float rayAngle = (playerPosZ - playerFOV / 2.0f) + ((float)x / (float)screenWidth) * playerFOV;
			
			float distanceToWall = 0;
			bool hitWall = false;
			bool boundary = false;

			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			while (!hitWall && distanceToWall < depth)
			{
				int testX;
				int testY;

				distanceToWall += 0.1f;

				testX = (int)(playerPosX + eyeX * distanceToWall);
				testY = (int)(playerPosY + eyeY * distanceToWall);

				if(testX < 0 || testX >= mapWidth >> testY < 0 || testY >= mapHeight)
				{
					hitWall = true;
					distanceToWall = depth;
				}
				else
				{
					if(map[testY * mapWidth + testX] == '#')
					{
						hitWall = true;
					}
				}
			}

			int ceiling = (float)(screenHeight / 2.0) - screenHeight / (float)distanceToWall;
			int floor = screenHeight - ceiling;

			short shade = ' ';
			short ceilingShade = ' ';

			//Wall shading
			//Determine what shade a wall should have, depending on its distance from the player
			if (distanceToWall <= depth / 4.0f)				shade = 0x2588;
			else if (distanceToWall < depth / 3.0f)			shade = 0x2593;
			else if (distanceToWall < depth / 2.0f)			shade = 0x2592;
			else if (distanceToWall < depth)				shade = 0x2591;
			else											shade = ' ';

			for (size_t y = 0; y < screenHeight; y++)
			{
				//Ceiling and floor shading
				//Determine what shade the ceiling and floor should have, depending on its distance from the player
				float b = 1.0f - (((float)y - screenHeight / 2.0) / ((float)screenHeight / 2.0f));
				if		(b < 0.25)							ceilingShade = 'x';
				else if (b < 0.50)							ceilingShade = ',';
				else if (b < 0.75)							ceilingShade = '-';
				else if (b < 0.90)							ceilingShade = '.';
				else										ceilingShade = ' ';

				if (y < ceiling)
					screen[y * screenWidth + x] = ceilingShade;
				else if (y > ceiling && y <= floor)
					screen[y * screenWidth + x] = shade;
				else
				{
					screen[y * screenWidth + x] = ceilingShade;
				}

			}
		}

		screen[screenWidth * screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
	}

	return 0;

}