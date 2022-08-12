#include <iostream>
#include <algorithm>
#include "olcConsoleGameEngine.h"

class Game : public olcConsoleGameEngine
{
public:
	Game()
	{
		m_sAppName = L"Textured Raycaster";
	}

	virtual bool OnUserCreate()
	{
		//2D map
		//This is the 2D map. 
		//# = wall
		//. = floor
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


		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//Controls
		if (m_keys[L'A'].bHeld)
			playerPosZ -= turningSpeed * fElapsedTime;
		if (m_keys[L'D'].bHeld)
			playerPosZ += turningSpeed * fElapsedTime;
		if (m_keys[L'W'].bHeld)
		{
			playerPosX += sinf(playerPosZ) * speed * fElapsedTime;
			playerPosY += cosf(playerPosZ) * speed * fElapsedTime;

			//Collision detection
			//Simple correctional collision detection
			if (map[(int)playerPosY * mapWidth + (int)playerPosX] == '#')
			{
				playerPosX -= sinf(playerPosZ) * speed * fElapsedTime;
				playerPosY -= cosf(playerPosZ) * speed * fElapsedTime;
			}
		}
		if (m_keys[L'S'].bHeld)
		{
			playerPosX -= sinf(playerPosZ) * speed * fElapsedTime;
			playerPosY -= cosf(playerPosZ) * speed * fElapsedTime;

			//Collision detection
			//Simple correctional collision detection
			if (map[(int)playerPosY * mapWidth + (int)playerPosX] == '#')
			{
				playerPosX += sinf(playerPosZ) * speed * fElapsedTime;
				playerPosY += cosf(playerPosZ) * speed * fElapsedTime;
			}
		}

		//Drawing
		for (size_t x = 0; x < ScreenWidth(); x++)
		{
			float rayAngle = (playerPosZ - playerFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * playerFOV;

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

				if (testX < 0 || testX >= mapWidth >> testY < 0 || testY >= mapHeight)
				{
					hitWall = true;
					distanceToWall = depth;
				}
				else
				{
					if (map[testY * mapWidth + testX] == '#')
					{
						hitWall = true;
					}
				}
			}

			int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / (float)distanceToWall;
			int floor = ScreenHeight() - ceiling;

			short shade = ' ';
			short ceilingShade = ' ';

			//Wall shading
			//Determine what shade a wall should have, depending on its distance from the player
			if (distanceToWall <= depth / 4.0f)				shade = 0x2588;
			else if (distanceToWall < depth / 3.0f)			shade = 0x2593;
			else if (distanceToWall < depth / 2.0f)			shade = 0x2592;
			else if (distanceToWall < depth)				shade = 0x2591;
			else											shade = ' ';

			for (size_t y = 0; y < ScreenHeight(); y++)
			{
				//Ceiling and floor shading
				//Determine what shade the ceiling and floor should have, depending on its distance from the player
				float b = 1.0f - (((float)y - ScreenHeight() / 2.0) / ((float)ScreenHeight() / 2.0f));
				if (b < 0.25)							ceilingShade = 'x';
				else if (b < 0.50)							ceilingShade = ',';
				else if (b < 0.75)							ceilingShade = '-';
				else if (b < 0.90)							ceilingShade = '.';
				else										ceilingShade = ' ';

				if (y < ceiling)
					Draw(x, y, L' ');
				else if (y > ceiling && y <= floor)
					Draw(x, y, shade);
				else
				{
					Draw(x, y, ceilingShade);
				}

			}
		}
		return true;
	}

private:
	float playerPosX = 8.0f;
	float playerPosY = 8.0f;
	float playerPosZ = 8.0f;
	float playerFOV = 3.14159 / 4.0;

	int mapWidth = 16;
	int mapHeight = 16;

	float depth = 16.0f;

	float speed = 5.0f;
	float turningSpeed = 9.0f;


	std::wstring map;

};

int main()
{
	Game game;
	game.ConstructConsole(120, 80, 8, 8);
	game.Start();

	return 0;
}