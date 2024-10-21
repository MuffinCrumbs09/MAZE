#include "stdafx.h"
#include "MyGame.h"

char CMyGame::m_tileLayout[12][20];

struct Cell
{
	int x, y;
	Cell(int x, int y) : x(x), y(y) {}
};

// Check if the cell coordinates are within bound of maze grid
bool isWithinBounds(int x, int y) 
{
	return (x >= 0 && x < 20 && y >= 0 && y < 12);
}

// Randomly shuffle the list of direction cells to make a more randomised maze
// Fisher-Yates shuffle algorithm
void shuffle(std::vector<Cell>& directions) {
	for (int i = directions.size() - 1; i > 0; --i) {
		int j = rand() % (i + 1);
		std::swap(directions[i], directions[j]);
	}
}

void CMyGame::carvePath(int x, int y) {
	// Mark the current cell as a path
	m_tileLayout[y][x] = ' ';

	// List of directions (up, down, left, right)
	std::vector<Cell> directions = { Cell(0, -2), Cell(0, 2), Cell(-2, 0), Cell(2, 0) };
	shuffle(directions);       // Shuffle the directions

	// Try each direction
	for (auto& dir : directions) {
		int newX = x + dir.x;
		int newY = y + dir.y;
		int betweenX = x + dir.x / 2;
		int betweenY = y + dir.y / 2;

		// Check if the next cell is within the maze bounds and is wall
		if (isWithinBounds(newX, newY) && m_tileLayout[newY][newX] == 'X') {
			// Carve the wall between cells
			m_tileLayout[betweenY][betweenX] = ' ';

			// Move to next cell from current cell
			carvePath(newX, newY);
		}
	}
}


CMyGame::CMyGame(void) : 
	m_sprite(400, 40, "rocket.bmp", CColor::Blue(), 0)	
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	// TODO: add the game update code here
	m_sprite.Update(t);	// this will update the sample rocket sprite
}

void CMyGame::OnDraw(CGraphics* g)
{
	// TODO: add drawing code here
	m_sprite.Draw(g);	// this will draw the sample rocket sprite
	m_tiles.for_each(&CSprite::Draw, g);

	// this will print the game time
	*g << bottom << left << "Time elapsed: " << timetext(GetTime());
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// Fills the tile layout with walls for carving
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 20; x++) {
			m_tileLayout[y][x] = 'X';
		}
	}

	// Start maze generation
	// Hunt and Kill alorithm
	bool found = true;
	while (found) {
		found = false;

		// loop through gride, visiting odd numbers to allow room for walls
		for (int y = 1; y < 12; y += 2) {
			for (int x = 1; x < 20; x += 2) {
				// If the cell is a wall, carve from here
				if (m_tileLayout[y][x] == 'X') {
					carvePath(x, y);
					found = true;
				}
			}
		}
	}

	// Loop through grid again to assign tiles to the grid
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 20; x++) {
			// Skip empty spaces
			if (m_tileLayout[y][x] == ' ')
				continue;

			int nTile = 5;
			if (y > 0 && m_tileLayout[y - 1][x] == ' ') nTile -= 3;
			if (y < 11 && m_tileLayout[y + 1][x] == ' ') nTile += 3;
			if (x > 0 && m_tileLayout[y][x - 1] == ' ') nTile--;
			if (x < 19 && m_tileLayout[y][x + 1] == ' ') nTile++;
			if (nTile == 5 && x > 0 && y > 0 && m_tileLayout[y - 1][x - 1] == ' ') nTile = 14;
			if (nTile == 5 && x < 19 && y > 0 && m_tileLayout[y - 1][x + 1] == ' ') nTile = 13;
			if (nTile == 5 && x > 0 && y < 11 && m_tileLayout[y + 1][x - 1] == ' ') nTile = 11;
			if (nTile == 5 && x < 19 && y < 11 && m_tileLayout[y + 1][x + 1] == ' ') nTile = 10;

			nTile--;
			m_tiles.push_back(new CSprite(x * 64.f + 32.f, y * 64.f + 32.f, new CGraphics("tiles.png", 3, 5, nTile % 3, nTile / 3), 0));
		}
	}
}



// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
