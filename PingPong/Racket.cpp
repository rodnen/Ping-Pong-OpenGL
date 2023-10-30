#include "pch.h"
#include "Racket.h"
#include <ctime>

Racket::Racket(double x, double y, double width, double height, double speed, bool EnableAI, COLORREF color, double topEdge, double bottomEdge) {
	this->x = x;
	this->y = y;

	this->width = width;
	this->height = height;

	this->speed = speed;
	
	AIisEnable = EnableAI;

	this->color = color;

	topBoundary = topEdge;
	bottomBoundary = bottomEdge;
}

Racket::Racket(double topEdge, double bottomEdge) {
	topBoundary = topEdge;
	bottomBoundary = bottomEdge;
}

void Racket::SetX(double x) {
	this->x = x;
}

void Racket::SetY(double y) {
	this->y = y;
}

void Racket::SetWidth(double width) {
	if (width < 0) {
		AfxMessageBox(L"Ширина від'ємна, це може зпричинити невірне відображення!");
	}

	this->width = width;
}

void Racket::SetHeight(double height) {
	if (height < 0) {
		AfxMessageBox(L"Висота від'ємна, це може зпричинити невірне відображення!");
	}

	this->height = height;
}

void Racket::SetSpeed(double speed) {
	this->speed = speed;
}

void Racket::SetStep(int FPS) {
	this->FPS = FPS;

	double delay = 1000 / FPS;
	double deltaTime = (double)delay / 1000.0;

	this->step = speed * deltaTime / FPS;
}

void Racket::TurnOnAi() {
	AIisEnable = true;
}

void Racket::TurnOffAi() {
	AIisEnable = false;
}

void Racket::SetBallBeated(bool beated) {
	isBallBeated = beated;
}

void Racket::SetColor(COLORREF color) {
	this->color = color;
}

void Racket::SetBottomBoundary(double bottomY) {
	bottomBoundary = bottomY;
}

void Racket::SetTopBoundary(double topY) {
	topBoundary = topY;
}

void Racket::MoveUp(double step) {	
	this->y += step;
}

void Racket::MoveDown(double step) {
	this->y -= step;
}

void Racket::SetMovingUp(bool set) {
	isMovingUp = set;
}

void Racket::SetMovingDown(bool set) {
	isMovingDown = set;
}

void Racket::AutoPlay(BallInfo ball, double deltaTime) {
	if (isBallBeated) return;

	for (int frame = 0; frame < FPS; frame++) {

		//if (ball.x > 0 && x > 0) {
			//AutoMoving(ball, step);
		//}

		//if (ball.x < 0 && x < 0) {
			AutoMoving(ball, step);
		//}
	}
}

void Racket::AutoMoving(BallInfo ball, double step) {

	if (ball.y + ball.width / 2 > y) {
		MoveUp(step);
	}
	if (ball.y + ball.width / 2 < y) {
		MoveDown(step);
	}
}


void Racket::UpdatePosition(double deltaTime) {

	if (!isMovingUp && !isMovingDown) return;

	for (int frame = 0; frame < FPS; frame++) {
		
		if (isMovingUp) {
			MoveUp(step);
		}
		if (isMovingDown) {
			MoveDown(step);
		}
	}
	
}

bool Racket::CheckTopCollision() {
	
	if (y + height / 2 > topBoundary) { y = topBoundary - height / 2; isMovingUp = false;}
	return (y + height / 2) == topBoundary;
}

bool Racket::CheckBottomCollision() {

	if ((y - height / 2) < bottomBoundary) { y = bottomBoundary + height / 2; isMovingDown = false; }

	return (y - height / 2) == bottomBoundary;
}

bool Racket::CheckCollision() {
	if (CheckBottomCollision() || CheckTopCollision()) return true;
	return false;
}

void Racket::SetDefault() {
	isMovingUp = false;
	isMovingDown = false;
	isBallBeated = false;
	TurnOffAi();
	width = 0.05;
	height = 0.2;
	speed = 1.6;
	color = RGB(204, 230, 255);
}