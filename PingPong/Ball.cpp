#include "pch.h"
#include "Ball.h"

Ball::Ball(double x, double y, double width, double speedX, double speedY, COLORREF color, double windowHeight, double windowWidth) {
	this->x = x;
	this->y = y;

	this->width = width;

	this->speedX = speedX;
	this->speedY = speedY;

	this->color = color;

	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
}

Ball::Ball(double windowHeight, double windowWidth) {
	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
}

void Ball::SetX(double x) {
	this->x = x;
}

void Ball::SetY(double y) {
	this->y = y;
}

void Ball::SetWidth(double width) {
	if (width < 0) {
		AfxMessageBox(L"Ширина від'ємна, це може зпричинити невірне відображення!");
	}

	this->width = width;
}

void Ball::SetSpeedX(double speed) {
	this->speedX = speed;
}

void Ball::SetSpeedY(double speed) {
	this->speedY = speed;
}

void Ball::SetStep(short FPS) {
	this->FPS = FPS;

	double delay = 1000 / FPS;
	double deltaTime = (double)delay / 1000.0;

	this->stepX = speedX * deltaTime / FPS;
	this->stepY = speedY * deltaTime / FPS;
}

void Ball::SetColor(COLORREF color) {
	this->color = color;
}

void Ball::SetWindowHeight(double height) {
	windowHeight = height;
}

void Ball::SetWindowWidth(double width) {
	windowWidth = width;
}

void Ball::SetRotateAngle(double angle){
	this->angle = angle;
}

void Ball::Move(Racket* rOne, Racket* rTwo) {
	
	SetStep(FPS);

	for (int frame = 0; frame < FPS; frame++) {
		x += stepX;
		y += stepY;

		bool leftWindowSide = x - width / 2 < -windowWidth;
		bool rightWindowSide = x + width / 2 > windowWidth;

		if (leftWindowSide || rightWindowSide) {
			if (leftWindowSide) {
				leftSide = LEFT;
			}
			if (rightWindowSide) {
				leftSide = RIGHT;
			}
			inGame = false;
			break;
		}

		if (y - width / 2 < -windowHeight || y + width / 2 > windowHeight) {
			speedY = -speedY;

			if (y - width / 2 < -windowHeight)
				y = -windowHeight + width / 2;

			if (y + width / 2 > windowHeight)
				y = windowHeight - width / 2;

			break;
		}

		bool collisionOne = IsCollision(rOne->GetInfoStruct());
		bool collisionTwo = IsCollision(rTwo->GetInfoStruct());

		if (collisionOne || collisionTwo) {
	
			if (collisionOne) {
				rOne->SetBallBeated(true);
				rTwo->SetBallBeated(false);
			}
			else if (collisionTwo) {
				rOne->SetBallBeated(false);
				rTwo->SetBallBeated(true);
			}

			CheckSpeed();
			angle = -angle;

			IncreaseSpeed();
			break;
		}
	}
}

void Ball::CheckSpeed() {
	if (speedY == 0.0) {
		speedY = 0.5;
	}


	if (beatedSide & TOP) {
		if (speedY < 0)
			speedY = -speedY;
	}

	if (beatedSide & BOTTOM) {
		if (speedY > 0)
			speedY = -speedY;
	}

	//speedY = -speedY;
	speedX = -speedX;
}

bool Ball::IsCollision(RacketInfo racket) {
	bool collision = 
		x - width / 2 < racket.x + racket.width / 2 &&
		x + width / 2 > racket.x - racket.width / 2 &&
		y + width / 2 > racket.y - racket.height / 2 &&
		y - width / 2 < racket.y + racket.height / 2;

	if (collision) {

		double centerY = racket.y;
		double centerX = racket.x;

		double topY = centerY + racket.height / 2;
		double bottomY = centerY - racket.height / 2;
		double leftX = centerX - racket.width / 2;
		double rightX = centerX + racket.width / 2;
		
		double racketMovement = racket.step * FPS;
		double rmBySide = racket.step * FPS;
		if (racket.x > 0)
			rmBySide = -rmBySide;

		//Перевірка колізії лівої та правої сторони ракетки 
		bool leftCollision = (x - width / 2 <= leftX && x - width / 2 >= centerX) || (x + width / 2 >= leftX && x + width / 2 <= centerX);
		bool rightCollision = (x - width / 2 <= rightX && x - width / 2 >= centerX) || (x + width / 2 >= rightX && x + width / 2 <= centerX);

		//Перевірка на яку частину сторони потрапив м'яч
		bool topCollision = ((y - width / 2 >= topY && y - width / 2 <= centerY) || (y + width / 2 <= topY && y + width / 2 >= centerY)) && (leftCollision || rightCollision);
		bool bottomCollision = ((y - width / 2 <= bottomY && y - width / 2 >= centerY) || (y + width / 2 >= bottomY && y + width / 2 <= centerY)) && (leftCollision || rightCollision);

		//Перевірка колізії із верхньою та нижньою граню
		bool topSide = (y - width / 2 <= topY && y + width / 2 >= topY);
		bool bottomSide = (y - width / 2 <= bottomY && y + width / 2 >= bottomY);

		if (topCollision || topSide) {
			beatedSide = TOP;
		}
		else if (bottomCollision || bottomSide) {
			beatedSide = BOTTOM;
		}
		else {
			beatedSide = CENTER;
		}

		if (racket.isMoveUp) {
			if (topSide) {
				y += racketMovement;
				x += rmBySide;
			}
			if (bottomSide) {
				y -= racketMovement;
				x += rmBySide;
			}
		}

		if (racket.isMoveDown) {
			if (topSide) {
				y += racketMovement;
				x += rmBySide;
			}
			if (bottomSide) {
				y -= racketMovement;
				x += rmBySide;
			}
		}


		return true;
	}
	return false;
}




void Ball::IncreaseSpeed() {
	if (speedX > 0) speedX += 0.1;
	else speedX -= 0.1;
		
	if(speedY > 0) speedY += 0.05;
	else speedY -= 0.05;
}

void Ball::SetDefault() {
	width = 0.08;
	speedX = 0.5;
	speedY = 0;
	
	angle = 10;

	beatedSide = CENTER;
	color = RGB(204, 230, 255);
}