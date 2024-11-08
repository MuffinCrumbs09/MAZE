#pragma once

class CSpriteAgent : public CSprite 
{
	int wanderTarget; // Node that they are currently walking towards

public:
	CSpriteAgent(float x, float y, char* pFileBitmap, Uint32 time);
	CSpriteAgent(float x, float y, char* pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, Uint32 time);
	CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, CColor colorKey, Uint32 time);
	~CSpriteAgent(void);

	// Behaviour
	CVector chase(CVector targetPos, float speed);
	int wander(float speed);
};