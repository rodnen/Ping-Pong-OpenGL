#pragma once

#include "Racket.h"

enum {
	LEFT = 1,
	RIGHT = 2,
	TOP = 4,
	BOTTOM = 16,
	CENTER = 32
};


class Ball
{

public:
	Ball() {};
	Ball(double x, double y, double width, double speedX, double speedY, COLORREF color, double windowHeight, double windowWidth);
	Ball(double windowHeight, double windowWidth);

	~Ball() {};

private:
	short FPS;

	double x, y;
	double width;
	UINT beatedSide;
	UINT leftSide;
	double windowHeight, windowWidth;

	double speedX, speedY, stepX, stepY;
	double angle = 0;
	COLORREF color;

	bool inGame = true, speedYReversed = false;

public:
	void Move(Racket* rOne, Racket* rTwo);
	bool IsCollision(RacketInfo racket);

	void CheckSpeed();
	void IncreaseSpeed();

	void SetDefault();

public:
	void SetWindowHeight(double height);
	void SetWindowWidth(double width);

	void SetX(double x);
	void SetY(double y);

	void SetStep(short FPS);

	void SetWidth(double width);

	void SetSpeedX(double speed);
	void SetSpeedY(double speed);

	void SetColor(COLORREF color);

	void SetRotateAngle(double angle);

	void SetSide(UINT side) { this->beatedSide = side; }

	void BackToGame() { inGame = true; }
	
public:
	double GetWindowWidth() { return windowWidth; }
	double GetWindowHeight() { return windowHeight; }

	double GetX() { return x; }
	double GetY() { return y; }

	double GetStepX() { return stepX; }
	double GetStepY() { return stepY; }

	double GetWidth() { return width; }
	
	double GetSpeedX() { return speedX; }
	double GetSpeedY() { return speedY; }

	double GetRotateAngle() { return angle; }

	UINT GetSideCollision() { return beatedSide; }
	UINT GetLeftSide() { return leftSide; }

	COLORREF GetColor() { return color; }

	bool InGame() { return inGame; }

	BallInfo GetInfoStruct() {
		BallInfo info;

		info.x = x;
		info.y = y;
		info.width = width;

		return info;
	}
};

