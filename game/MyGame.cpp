#include "stdafx.h"
#include "MyGame.h"

char CMyGame::m_tileLayout[11][19];
bool reachedTarget = true;

struct Cell
{
	int x, y;
	Cell(int x, int y) : x(x), y(y) {}
};

// Generates a unique and random maze inside m_tileLayout.
// Hunt and Kill Algorithm
void CMyGame::MazeGeneration() 
{
	// Fills the tile layout with walls for carving
	for (int y = 0; y < 11; y++) {
		for (int x = 0; x < 19; x++) {
			m_tileLayout[y][x] = 'X';
		}
	}
	// HAK
	bool found = true;
	while (found) {
		found = false;

		// loop through gride, visiting odd numbers to allow room for walls
		for (int y = 1; y < 11; y += 2) {
			for (int x = 1; x < 19; x += 2) {
				// If the cell is a wall, carve from here
				if (m_tileLayout[y][x] == 'X') {
					CarvePath(x, y);
					found = true;
				}
			}
		}
	}
}

// Check if the cell coordinates are within bound of maze grid
bool IsWithinBounds(int x, int y) 
{
	return (x > 0 && x < 19 && y > 0 && y < 11);
}

// Randomly shuffle the list of direction cells to make a more randomised maze
// Fisher-Yates shuffle algorithm
void Shuffle(std::vector<Cell>& directions) {
	for (int i = directions.size() - 1; i > 0; --i) {
		int j = rand() % (i + 1);
		std::swap(directions[i], directions[j]);
	}
}

// Pathfind function to give the AI the direction to walk.
// Dijkstra's pathfinding algorithm
bool PathFind(vector<NODE>& graph, int nStart, int nGoal, vector<int>& path)
{
	list<int> open;

	// Mark all nodes as unvisited
	for (unsigned i = 0; i < graph.size(); i++)
		graph[i].open = false;

	// Start Node
	graph[nStart].open = true;
	graph[nStart].costSoFar = 0;
	graph[nStart].nConnection = -1;

	open.push_back(nStart);

	while (open.size() > 0)
	{
		// Find element with the smallest cost so far
		list<int>::iterator iCur = min_element(open.begin(), open.end(), [graph](int i, int j) -> bool {
			return graph[i].costSoFar < graph[j].costSoFar;
			});
		int curNode = *iCur;
		float costSoFar = graph[curNode].costSoFar;

		// Exit after reaching goal
		if (curNode == nGoal)
			break;

		// Check all connections for next waypoint
		for (CONNECTION c : graph[curNode].conlist)
		{
			int endNode = c.nEnd;
			float newCostSoFar = costSoFar + c.cost;

			// Ignore worse route
			if (graph[endNode].open && graph[endNode].costSoFar <= newCostSoFar)
				continue;

			// Better route
			graph[endNode].costSoFar = newCostSoFar;
			graph[endNode].nConnection = curNode;

			// If unvisited, add to open list
			if (!graph[endNode].open)
			{
				graph[endNode].open = true;
				open.push_back(endNode);
			}
		}

		// Close current node
		graph[curNode].closed = true;
		open.erase(iCur);
	}

	// If none is open, no path found
	if (open.size() == 0)
		return false;

	int i = nGoal;
	while (graph[i].nConnection >= 0)
	{
		path.push_back(i);
		i = graph[i].nConnection;
	}
	path.push_back(i);

	reverse(path.begin(), path.end());
	reachedTarget = false;
	return true;
}

// Connects waypoints by checking neigbouring waypoints
void CMyGame::ConnectWaypoints()
{
	// Total waypoints
	int totalWaypoints = coords.size();

	// Loop through each to find the neighbours
	for (int i = 0; i < totalWaypoints; i++) {
		// Gets the current position
		CVector pos = coords[i];

		// Potential neighbor positions relative to the current waypoint
		std::vector<std::pair<float, float>> neighborPositions = {
			{pos.m_x + (64.f * 2), pos.m_y},     // Right
			{pos.m_x - (64.f * 2), pos.m_y},     // Left
			{pos.m_x, pos.m_y + (64.f * 2)},     // Up
			{pos.m_x, pos.m_y - (64.f * 2)}      // Down
		};

		// Go through each calculated neighbour
		for (const auto& [neighborX, neighborY] : neighborPositions) {
			// Get its index
			int nIdx = FindWaypointIndex(neighborX, neighborY);

			if (nIdx != -1) {  // Neighbor found
				// Check if there is a wall between the waypoints
				if (AreWaypointsConnected(pos, coords[nIdx])) {
					float distance = Distance(pos, coords[nIdx]);
					m_graph[i].conlist.push_back({ nIdx, distance });
				}
			}
		}
	}
}

int CMyGame::FindWaypointIndex(float x, float y)
{
	for (int i = 0; i < coords.size(); ++i)
	{
		if (std::abs(coords[i].m_x - x) < 1e-2 && std::abs(coords[i].m_y - y) < 1e-2)
			return i;  // Return the index if position matches
	}
	return -1; // Return -1 if no matching waypoint found
}

// Function to check if two waypoints are connected without walls in between
bool CMyGame::AreWaypointsConnected(const CVector& start, const CVector& end)
{
	int x1 = static_cast<int>(start.m_x / 64.f);
	int y1 = static_cast<int>(start.m_y / 64.f);
	int x2 = static_cast<int>(end.m_x / 64.f);
	int y2 = static_cast<int>(end.m_y / 64.f);

	if (x1 == x2) { // Vertical connection
		int step = (y2 > y1) ? 1 : -1;
		for (int y = y1 + step; y != y2; y += step) {
			if (m_tileLayout[y][x1] == 'X') {
				return false; // Wall found
			}
		}
	}
	else if (y1 == y2) { // Horizontal connection
		int step = (x2 > x1) ? 1 : -1;
		for (int x = x1 + step; x != x2; x += step) {
			if (m_tileLayout[y1][x] == 'X') {
				return false; // Wall found
			}
		}
	}

	return true; // No walls found
}

void CMyGame::CarvePath(int x, int y) {
	// Mark the current cell as a path
	m_tileLayout[y][x] = ' ';

	// Add cell as waypoint
	coords.push_back(CVector(x * 64.f + 32.f, y * 64.f + 32.f));

	// List of directions (up, down, left, right)
	std::vector<Cell> directions = { Cell(0, -2), Cell(0, 2), Cell(-2, 0), Cell(2, 0) };
	Shuffle(directions);       // Shuffle the directions

	// Try each direction
	for (auto& dir : directions) {
		int newX = x + dir.x;
		int newY = y + dir.y;
		int betweenX = x + dir.x / 2;
		int betweenY = y + dir.y / 2;

		// Check if the next cell is within the maze bounds and is wall
		if (IsWithinBounds(newX, newY) && m_tileLayout[newY][newX] == 'X') {
			// Carve the wall between cells
			m_tileLayout[betweenY][betweenX] = ' ';

			// Move to next cell from current cell
			CarvePath(newX, newY);
		}
	}
}


CMyGame::CMyGame(void) :
	enemy(400, 40, 119, 162, "fox.png", CColor::Green(128), 0)
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

	if (!m_waypoints.empty()) 
	{
		if (enemy.GetSpeed() < 1) 
		{
			enemy.SetSpeed(500);
			enemy.SetDirection(m_waypoints.front() - enemy.GetPosition());
			enemy.SetRotation(enemy.GetDirection() - 90);
		}

		CVector v = m_waypoints.front() - enemy.GetPosition();
		if (Dot(enemy.GetVelocity(), v) < 0)
		{
			m_waypoints.pop_front();
			enemy.SetVelocity(0, 0);
			enemy.SetRotation(0);

			if (m_waypoints.empty() && !reachedTarget) 
			{
				reachedTarget = true;
				std::cout << "Reached target" << endl;
			}
		}
	}

	// TODO: add the game update code here
	enemy.Update(t);	// this will update the sample rocket sprite
}

void CMyGame::OnDraw(CGraphics* g)
{
	// TODO: add drawing code here
	enemy.Draw(g);	// this will draw the sample rocket sprite

	m_tiles.for_each(&CSprite::Draw, g);
	m_nodes.for_each(&CSprite::Draw, g);

	for (NODE n : m_graph)
		for (CONNECTION c : n.conlist)
			g->DrawLine(n.pos, m_graph[c.nEnd].pos, CColor::Black());

	// this will print the game time
	*g << bottom << left << "Time elapsed: " << timetext(GetTime());
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// Start maze generation
	MazeGeneration();

	// Loop through grid again to assign tiles to the grid
	for (int y = 0; y < 11; y++) {
		for (int x = 0; x < 19; x++) {
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

	for (int y = 0; y < 11; y++) {
		for (int x = 0; x < 19; x++) {
			std::cout << m_tileLayout[y][x];
		}
		std::cout << endl;
	}

	std::cout << coords.size() << endl;

	for (CVector v : coords)
		m_graph.push_back(NODE{ v });

	ConnectWaypoints();

	for (int i = 0; i < m_graph.size(); i++) {
		NODE& node = m_graph[i];
		std::cout << "Waypoint " << i << " has connections : \n";

		for (const auto& connection : node.conlist) {
			int neighborIndex = connection.nEnd;
			std::cout << "    - Connected to waypoint " << neighborIndex
				<< endl;
		}

		std::cout << std::endl;  // Separate each waypoint's output with a blank line
	}

	int i = 0;
	for (NODE n : m_graph)
	{
		stringstream s;
		s << i++;
		m_nodes.push_back(new CSpriteOval(n.pos, 12, CColor::White(), CColor::Black(), 0));
		m_nodes.push_back(new CSpriteText(n.pos, "arial.ttf", 14, s.str(), CColor::Black(), 0));
	}
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	enemy.SetPosition(m_graph[0].pos);
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame() { }

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel) { }

// called when the game is over
void CMyGame::OnGameOver() { }

// one time termination code
void CMyGame::OnTerminate() { }

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
void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) { }

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle) { }
void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	CVector v(x, y);

	// Check if clicked point is on a valid path
	if (m_tileLayout[y / 64][x / 64] != ' ')
		return;

	// Find closest node to NPC
	vector<NODE>::iterator iFirst = min_element(m_graph.begin(), m_graph.end(),
		[this](NODE& n1, NODE& n2) -> bool {
			return Distance(n1.pos, enemy.GetPos()) < Distance(n2.pos, enemy.GetPos());
		});
	int nFirst = iFirst - m_graph.begin();
	std::cout << "Closest node to NPC: " << nFirst << std::endl;

	// Find closest node to click
	vector<NODE>::iterator iLast = min_element(m_graph.begin(), m_graph.end(),
		[v](NODE& n1, NODE& n2) -> bool {
			return Distance(n1.pos, v) < Distance(n2.pos, v);
		});
	int nLast = iLast - m_graph.begin();
	std::cout << "Closest node to click: " << nLast << std::endl;

	// Clear current waypoints
	if (!m_waypoints.empty()) {
		m_waypoints.clear();
		enemy.SetVelocity(0, 0);
	}

	// Calculate path
	vector<int> path;
	if (PathFind(m_graph, nFirst, nLast, path)) {
		for (int i : path)
			m_waypoints.push_back(m_graph[i].pos);
		m_waypoints.push_back(v); // End position at click point
	}
}
void CMyGame::OnLButtonUp(Uint16 x,Uint16 y) { }
void CMyGame::OnRButtonDown(Uint16 x,Uint16 y) { }
void CMyGame::OnRButtonUp(Uint16 x,Uint16 y) { }
void CMyGame::OnMButtonDown(Uint16 x,Uint16 y) { }
void CMyGame::OnMButtonUp(Uint16 x,Uint16 y) { }
