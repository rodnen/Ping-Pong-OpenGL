#pragma once

struct RacketInfo {
	double x;
	double y;
	double width;
	double height;
	double step;

	bool isMoveUp;
	bool isMoveDown;

};

struct BallInfo {
	double x;
	double y;
	double width;
};

class Racket
{
public:
	Racket() {};
	Racket(double x, double y, double width, double height, double speed, bool EnableAI, COLORREF color, double topEdge, double bottomEdge);
	Racket(double topEdge, double bottomEdge);

	~Racket() {};

private:
	short FPS;

	double x, y;
	double width, height;

	double topBoundary, bottomBoundary;

	double speed, step;
	bool AIisEnable, isMovingUp, isMovingDown, isBallBeated;
	COLORREF color;

public:
	void UpdatePosition(double deltaTime);
	void MoveUp(double step);
	void MoveDown(double step);

	void AutoPlay(BallInfo ball, double deltaTime);
	void AutoMoving(BallInfo ball, double step);

	bool CheckTopCollision();
	bool CheckBottomCollision();

	bool CheckCollision();

public:

	void SetDefault();

	void SetTopBoundary(double topY);
	void SetBottomBoundary(double bottomY);

	void SetX(double x);
	void SetY(double y);

	void SetWidth(double width);
	void SetHeight(double height);

	void SetSpeed(double speed);
	void SetStep(int FPS);
	
	void TurnOnAi();
	void TurnOffAi();

	void SetColor(COLORREF color);

	void SetMovingUp(bool set);
	void SetMovingDown(bool set);

	void SetBallBeated(bool isBeated);

public:
	double GetTopBoundary() { return topBoundary; }
	double GetBottomBoundary() { return topBoundary; }

	double GetX() { return x; }
	double GetY() { return y; }

	double GetStep() { return step; }

	double GetWidth() { return width; }
	double GetHeight() { return height; }

	double GetSpeed() { return speed; }

	bool GetAiStatus() { return AIisEnable; }
	
	bool GetBeatedStatus() { return isBallBeated; }

	COLORREF GetColor() { return color; }

	CRect GetRectArea() { return CRect(x, y, x + width, y + height); }
	
	CPoint GetLeftTop() { return CPoint(x, y); }
	CPoint GetRightBottom() { return CPoint(x + width, y + height); }

	RacketInfo GetInfoStruct() {
		RacketInfo info;

		info.x = x;
		info.y = y;
		info.width = width;
		info.height = height;
		info.step = step;

		info.isMoveDown = isMovingDown;
		info.isMoveUp = isMovingUp;

		return info;
	}

};

