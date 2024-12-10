#pragma once

class CMyGame : public CGame
{
	// Sprites
	CSprite trolley;
	CSpriteOval front, rear;
	CSprite shelf, ball, hammer, lever, bumper, wedge;

	// Physics
	float f = 0.05f;		// friction coefficient
	float w = 0.3f;			// froction with the wedge
	float e = 0.5f;			// restitution coefficient;

	// Game condition
	bool won;

	// Geometry
	const float alpha = DEG2RAD(30.0f);	// angle
	const float yFloor = 100.f;			// y coordinate of the flat plane (the floor)
	const float slopeW = 400.f;			// slope width

	// Direct manipulation (mouse drag) variables
	bool bIsTrolleyDragged;
	bool bIsWedgeDragged;
	float xOffs;

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	void UpdateWheel(CSprite& wheel, float f);
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
};
