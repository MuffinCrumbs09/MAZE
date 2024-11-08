#include "stdafx.h"
#include "SpriteAgent.h"

// Constructors
CSpriteAgent::CSpriteAgent(float x, float y, char* pFileBitmap, Uint32 time) { wanderTarget = -1; }
CSpriteAgent::CSpriteAgent(float x, float y, char* pFileBitmap, CColor colorKey, Uint32 time) { wanderTarget = -1; }
CSpriteAgent::CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, Uint32 time) { wanderTarget = -1; }
CSpriteAgent::CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, CColor colorKey, Uint32 time) { wanderTarget = -1; }
CSpriteAgent::~CSpriteAgent(void) { }

// Behaviour
CVector CSpriteAgent::chase(CVector targetPos, float speed) 
{
	return CVector(0, 0);
}

int CSpriteAgent::wander(float speed) 
{
	return wanderTarget;
}