#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	trolley(100, 400, "trolley.png", CColor::Green(), 0),
	front(40, 380, 40, 40, CColor::Red(), 0),
	rear(160, 380, 40, 40, CColor::Red(), 0),
	shelf(620, 511, 140, 20, "shelf.png", CColor::Green(), 0),
	ball(560, 540, "ball.png", CColor::Green(), 0),
	hammer(550, 266, "hammer.png", CColor::Green(), 0),
	lever(520, 472, "lever.png", CColor::Green(), 0),
	wedge(848, 106, "wedge.png", CColor::Green(), 0),
	bumper(1200, 120, "bumper.png", CColor::Green(), 0)
{
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::UpdateWheel(CSprite &wheel, float f)
{
	Uint32 t = GetTime();
	CVector normal;

	// Normal
	if (wheel.GetX() <= slopeW)
		normal = CVector(sin(alpha), cos(alpha)); // slope
	else
		normal = CVector(0, 1); // flat floor

	// gravity and reaction (normal force)
	CVector gravity(0, -10);
	wheel.Accelerate(gravity);
	wheel.Accelerate(-Dot(gravity, normal) * normal);

	// collision
	CVector xnormal(normal.m_y, -normal.m_x);
	wheel.SetVelocity(Cross(wheel.GetVelocity(), normal) * xnormal);

	// friction
	float fAcc = f * Dot(gravity, normal);
	fAcc = max(fAcc, -wheel.GetSpeed());	// cannot exceed speed - must not get it backwards!
	wheel.Accelerate(fAcc * wheel.GetNormalisedVelocity());

	wheel.Update(t);
}

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	// simulation!
	if (IsGameMode())
	{
		// TROLLEY

		// Test position of the wedge
		bool bWedge = (trolley.GetX() + 73 >= wedge.GetX());

		// Update the wheels
		UpdateWheel(front, bWedge ? w : f);		// friction depends on the wedge position
		UpdateWheel(rear, bWedge ? w : f);

		// Average the speed of the wheels
		float speed = (front.GetSpeed() + rear.GetSpeed()) / 2;
		front.SetSpeed(speed);
		rear.SetSpeed(speed);

		// span the trolley on top of the wheels
		CVector position = (front.GetPosition() + rear.GetPosition()) / 2;
		trolley.SetPosition(position);

		// setup the trolley rotation (based on the position of the wheels)
		CVector d = front.GetPosition() - rear.GetPosition();
		trolley.SetRotation(RAD2DEG(atan2(-d.m_y, d.m_x)));

		// Push the wedge in front of the trolley
		wedge.SetX(max(wedge.GetX(), trolley.GetX() + 73));

		// did the trolley hit the bumper?
		CVector normal(-1, 0);	// collision surface normal
		if (trolley.HitTest(&bumper) && Dot(front.GetVelocity(), normal) < 0)
		{
			front.SetVelocity(e * Reflect(front.GetVelocity(), normal));
			rear.SetVelocity(e * Reflect(rear.GetVelocity(), normal));
		}

		// Did trolley hit the hammer?
		if (trolley.HitTest(&hammer) && hammer.GetRotation() == 0)
			hammer.SetOmega(RAD2DEG(-speed / hammer.GetHeight() * 2));

		// Did the hammer hit the lever?
		if (hammer.HitTest(&lever) && hammer.GetOmega() != 0)
		{
			lever.SetOmega(-hammer.GetOmega() * hammer.GetHeight() / lever.GetHeight());
			hammer.SetOmega(0);
		}

		// Did the lever hit the ball?
		if (lever.HitTest(&ball) && lever.GetOmega() != 0)
			ball.SetVelocity(e * DEG2RAD(lever.GetOmega()) * lever.GetHeight() / 2, 0);

		// Did the lever hit the shelf?
		if (lever.HitTest(&shelf) && lever.GetOmega() != 0)
			lever.SetOmega(0);

		// Ball mechanics
		CVector gravity(0, -10);
		ball.Accelerate(gravity);
		if (ball.HitTest(&shelf))
			ball.Accelerate(-gravity);

		// Update (only moving parts - trolley was updated earlier)
		ball.Update(t);
		hammer.Update(t);
		lever.Update(t);

		// Winning condition test - must land on the platform, between the wheels
		if (ball.GetY() < yFloor + 57 && abs(ball.GetX() - trolley.GetX()) < 70)
			won = true;

		// End of the game condition (win or not)
		if (won || ball.GetBottom() <= yFloor)
			GameOver();
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
	g->DrawLine(CVector(0, yFloor + slopeW * tan(alpha)), CVector(slopeW, yFloor), 3, CColor::Black());
	g->DrawLine(CVector(slopeW, yFloor), CVector(GetWidth(), yFloor), 3, CColor::Black());
	trolley.Draw(g);
	//front.Draw(g);
 	//rear.Draw(g);

	shelf.Draw(g);
	hammer.Draw(g);
	lever.Draw(g);
	wedge.Draw(g);
	bumper.Draw(g);
	ball.Draw(g);

		
	// draw messages
	*g << font(14);
	*g << bottom << "Version 5.0" << endl << endl;
	if (IsMenuMode())
		*g << "Drag the trolley and the triangular wedge, press SPACE when ready";
	else if (IsGameMode())
		*g << "Simulating...";
	else if (won)
		*g << color(CColor::Red()) << "CONGRATULATIONS! YOU WON!" << endl << "Press F2 to play again";
	else
		*g << color(CColor::Red()) << "MISSED!" << endl << "Press F2 to play again";
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	trolley.SetPivotFromCenter(0, -20);
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	trolley.SetPos(100, 400);
	trolley.SetVelocity(0, 0);

	CVector normal(sin(alpha), cos(alpha));
	trolley.SetY(yFloor + (slopeW - trolley.GetX()) * tan(alpha));
	trolley.Move(normal * 20);
	trolley.SetRotation(RAD2DEG(alpha));

	front.SetVelocity(0, 0);
	rear.SetVelocity(0, 0);

	CVector xnormal(cos(alpha), -sin(alpha));
	front.SetPosition(trolley.GetPosition() + 60 * xnormal);
	rear.SetPosition(trolley.GetPosition() - 60 * xnormal);

	ball.SetPos(560, 540);
	ball.SetVelocity(0, 0);
	wedge.SetPos(848, 106);
	hammer.SetRotation(0);
	hammer.SetOmega(0);
	lever.SetRotation(0);
	lever.SetOmega(0);
	
	bIsTrolleyDragged = bIsWedgeDragged = false;
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	bIsTrolleyDragged = bIsWedgeDragged = false;
	won = false;
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
		if (IsMenuMode())
			StartGame();
		else
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
	if (bIsTrolleyDragged && x + xOffs + 60 < slopeW)
	{
		trolley.SetX(x + xOffs);
		
		CVector normal(sin(alpha), cos(alpha));
		trolley.SetY(yFloor + (slopeW - trolley.GetX()) * tan(alpha));
		trolley.Move(normal * 20);

		CVector xnormal(cos(alpha), -sin(alpha));
		front.SetPosition(trolley.GetPosition() + 60 * xnormal);
		rear.SetPosition(trolley.GetPosition() - 60 * xnormal);
	}
	if (bIsWedgeDragged && x + xOffs > slopeW + 20)
		wedge.SetX(x + xOffs);;
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (trolley.HitTest(CVector(x, y)))
	{
		bIsTrolleyDragged = true;
		xOffs = trolley.GetX() - x;
	}
	if (wedge.HitTest(CVector(x, y)))
	{
		bIsWedgeDragged = true;
		xOffs = wedge.GetX() - x;
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	bIsTrolleyDragged = bIsWedgeDragged = false;
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
