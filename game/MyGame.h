#pragma once

#include "SpriteAgent.h"

struct CONNECTION
{
	int nEnd; 
	float cost; 
};

struct NODE
{
	CVector pos;
	list<CONNECTION> conlist;
	float costSoFar;
	int nConnection;
	bool open, closed;
};

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSpriteAgent enemy;

	static char m_tileLayout[11][19];
	CSpriteList m_tiles;

	CSpriteList m_nodes;

	vector<NODE> m_graph;
	vector<CVector> coords;
	list<CVector> m_waypoints;

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);

	// Functions
	virtual void CarvePath(int x, int y);
	virtual void ConnectWaypoints();
	virtual int FindWaypointIndex(float x, float y);
	virtual bool AreWaypointsConnected(const CVector& start, const CVector& end);
	virtual void MazeGeneration();
};
