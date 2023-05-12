#pragma once

#include "SFML/Graphics.hpp"
// ������� ����
#include "GameField.h"

// ������� "�������" ����� (���������)
int const HEIGHT_MAP = 36;
int const WIDTH_MAP = 28;

// �����������
#define random(a,b) a + rand() % (b + 1 - a)

// �������� ������-�
float const SPEDD_PAC = 8;

// ��� �����
float const STEP_FRAME = 0.001;
// ��� �������
float const STEP_TIMER = 0.005;

// ������ ������������� ��������
float const CATCH_SPLIT_W = 14;
float const CATCH_SPLIT_H = 13.875;

// ������������ ������������ �����������
// ��������� �����
const float WELL_VIEW_H = 1.8;
const float WELL_VIEW_W = 1.4;

// ����������� ��� ������� ������������������ ����
const float FACECOEFF_W = CATCH_SPLIT_W * WELL_VIEW_W;
const float FACECOEFF_H = CATCH_SPLIT_H * WELL_VIEW_H;

// ������������ ����� ��� ������ �� �������
const sf::Vector2u GATES(13 * FACECOEFF_W + FACECOEFF_W * 0.5, 14 * FACECOEFF_H);
//(((WIDTH_MAP - 1.15)* FACECOEFF_W) / 2, HEIGHT_MAP* FACECOEFF_H * 0.4)

// ����������� NPC � ������
enum objectRotation {

	// �������� texture � TextureManager

	LEFT, // 0
	RIGHT, // 1
	UP, // 2
	DOWN // 3
};

struct PacMan {

	// ������� �������� ��������
	float curr_X;
	float curr_Y;
	// __________________________
	// Buffer position
	float buf_X = 0.0;
	float buf_Y = 200.0;
	// __________________________
	// ����������� �������
	objectRotation rotSide;
	// __________________________
	// ������� ���� ��� ��������
	float frame = 0.0;
	// __________________________
	// ���������� �����������
	float timer = 0.0;
	// __________________________
	// ���� ������
	int score = 0;
	// __________________________

	// ��������� ������������� ��������� ���������
	void startInit(float X, float Y, objectRotation rotS) {

		curr_X = X;
		curr_Y = Y;

		rotSide = rotS;
	}

	// ���������� ������� � �������� ������� (������� ������)
	void update() {

		// Animation PacMan
		frame += STEP_FRAME;
		if (frame > 1) {
			frame -= 1;
		}

		timer += STEP_TIMER;

		if (timer >= 1) {

			buf_X = curr_X;
			buf_Y = curr_Y;

			switch (rotSide)
			{
			case LEFT:
				buf_X -= SPEDD_PAC;
				break;
			case RIGHT:
				buf_X += SPEDD_PAC;
				break;
			case UP:
				buf_Y -= SPEDD_PAC;
				break;
			case DOWN:
				buf_Y += SPEDD_PAC;
				break;
			default:
				break;
			}

			// ����� �������
			timer = 0.0;
		}

		// ������
		if ((int)round(buf_Y / FACECOEFF_H) == 17 && ((int)round(buf_X / FACECOEFF_W) == 0 || (int)round((buf_X / FACECOEFF_W)) == 28))
		{
			if ((int)(round(buf_X / FACECOEFF_W)) == 0) {
				buf_X = (28 - 0.5) * FACECOEFF_W;
			}
			else {
				buf_X = 0.2 * FACECOEFF_W;
			}

			if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.')
			{
				score++;
			}
			
			// ������� "���������" ����������
			gameField[(int)round(curr_Y / FACECOEFF_H)][(int)round(curr_X / FACECOEFF_W)] = '_';

			// ���������� �������� �������
			curr_X = buf_X;
			curr_Y = buf_Y;

			return;
		}

		if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.' || gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '_') {

			if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.')
			{
				score++;
			}

			// ������� "���������" ����������
			gameField[(int)round(curr_Y / FACECOEFF_H)][(int)round(curr_X / FACECOEFF_W)] = '_';

			// ���������� �������� �������
			curr_X = buf_X;
			curr_Y = buf_Y;
		}
		
	}


};

struct Ghost {

	// ������� �������� ��������
	float curr_X;
	float curr_Y;
	// __________________________
	// Buffer position
	float buf_X = 0.0;
	float buf_Y = 200.0;
	// __________________________
	// �������� �������������
	float speed;
	// __________________________
	// ����������� �������
	objectRotation rotSide;
	// ___________________________
	// ���������� �����������
	float timer = 0.0;
	// __________________________
	// ���� ������
	bool flag = false;
	// __________________________
	// ���� �����
	bool hunt = false;
	// __________________________

	// ��������� ������������� ��������� ���������
	void startInit(float X, float Y, objectRotation rotS, float how_fast = SPEDD_PAC * 1.1) {

		curr_X = X;
		buf_X = curr_X;
		curr_Y = Y;
		buf_Y = curr_Y;

		speed = how_fast;
		rotSide = rotS;
	}

	// ����� �� �������
	void ghostOut() {
		flag = !flag;
	}

	// �������� ����������� ������� � �������� �����������
	bool canPass(float check_Y, float check_X) {

		int y = round(check_Y / FACECOEFF_H);
		int x = round(check_X / FACECOEFF_W);

		if (gameField[y][x] == '.'
			|| gameField[y][x] == '_'
			|| gameField[y][x] == '*') {

			return true;
		}
		else {
			return false;
		}
	}

	// ���������� ���������� �������� ��������(���������� ������� ���� �� ��� �� ����� � �������� �����������)
	bool plotWay() {

		// ����� ������������ ���������������
		bool flag_R, flag_L, flag_U, flag_D;
		float koeff = 1.2;

		// ��� ����� ��� ����
		if (rotSide == UP || rotSide == DOWN) {
		
			flag_R = false;
			flag_L = false;

			// �������� ������ � �����������(��� �����������)
			// _____________________________________________________________________________________________________________

			// Right
			if (canPass(buf_Y, buf_X + koeff * speed)) {
				flag_R = true;
			}

			// Left
			if (canPass(buf_Y, buf_X - koeff * speed)) {
				flag_L = true;
			}

			//______________________________________________________________________________________________________________

			// �������� ������� ������ �� ���������� ������ � ������������
			if (flag_R && flag_L) {

				if (canPass(buf_Y + koeff * speed * ((rotSide == UP) ? -1 : 1), buf_X)) {

					if (random(0, 100) <= 60) {

						rotSide = objectRotation(random(0, 1));
						return true;
					}
					else {
						rotSide = rotSide;
						return true;
					}
				}
				else {
					rotSide = objectRotation(random(0, 1));
					return true;
				}

			}
			else if (flag_R) {

				if (canPass(buf_Y + koeff * speed * ((rotSide == UP) ? -1 : 1), buf_X)) {

					if (random(0, 100) <= 60) {

						rotSide = RIGHT;
						return true;
					}
					else {
						return false;
					}
				}
				else {
					rotSide = RIGHT;
					return true;
				}
			} else if (flag_L) {

				if (canPass(buf_Y + koeff * speed * ((rotSide == UP) ? -1 : 1), buf_X)) {

					if (random(0, 100) <= 60) {

						rotSide = LEFT;
						return true;
					}
					else {
						return false;
					}
				}	
				else {
					rotSide = LEFT;
					return true;
				}
			}

		}
		// ��� ������ ��� �����
		else if (rotSide == RIGHT || rotSide == LEFT) {

			flag_U = false;
			flag_D = false;

			// �������� ������ � �����������(��� �����������)
			// _____________________________________________________________________________________________________________

			// Up
			if (canPass(buf_Y - koeff * speed, buf_X)) {
				flag_U = true;
			}

			// Down
			if (canPass(buf_Y + koeff * speed, buf_X)) {
				flag_D = true;
			}

			//______________________________________________________________________________________________________________

			// �������� ������� ������ �� ���������� ������ � ������������

			if (flag_U && flag_D) {

				if (canPass(buf_Y, buf_X + koeff * speed * ((rotSide == LEFT) ? 1 : -1))) {

					if (random(0, 100) <= 60) {

						rotSide = objectRotation(random(2, 3));
						return true;
					}
					else {
						return false;
					}
				}
				else {
					rotSide = objectRotation(random(2, 3));
					return true;
				}
			}
			else if (flag_U) {

				if (canPass(buf_Y, buf_X + koeff * speed * ((rotSide == LEFT) ? 1 : -1))) {

					if (random(0, 100) <= 60) {

						rotSide = UP;
						return true;
					}
					else {
						return false;
					}
				}
				else {
					rotSide = UP;
					return true;
				}
			}
			else if (flag_D) {

				if (canPass(buf_Y, buf_X + koeff * speed * ((rotSide == LEFT) ? 1 : -1))) {

					if (random(0, 100) <= 60) {

						rotSide = DOWN;
						return true;
					}
					else {
						return false;
					}
				}
				else {
					rotSide = DOWN;
					return true;
				}
			}
		}

	}

	// ��������� ����������� ��� ��������
	objectRotation changeSide() {

		if (rotSide == UP) {
			return DOWN;
		}
		else if (rotSide == DOWN) {
			return UP;
		}
		else if (rotSide == RIGHT) {
			return LEFT;
		}
		else if (rotSide == LEFT) {
			return RIGHT;
		}
	}

	// ���������� ���� � ������ �� �������
	bool goToExit() {

		if (round(buf_X) == GATES.x && round(buf_Y) == GATES.y) {

			hunt = true;
			return true;
		}
		else if (round(buf_X) > GATES.x) {
			buf_X -= 0.1;
		}
		else if (round(buf_X) < GATES.x)
		{
			buf_X += 0.1;
		}
		else if (round(buf_Y) > GATES.y) {
			buf_Y -= 0.1;
		}
		else
		{
			buf_Y += 0.1;
		}

		return false;
	}

	// ����� ��� ��������
	float pause_rotate = 0.0;
	// ___________________________________

	// �������� � ����������
	void update() {

		// ������ �������� �����������
		timer += STEP_TIMER;
		pause_rotate += 0.001;

		// ����������� ���������� ���������� � ����������
		if (timer >= 1) {

			if (hunt && pause_rotate >= 1) {
				plotWay();
				pause_rotate = 0.0;
			}

			// ����������� ������� ������ ��� ��������� � ������, � ����� ���������� ��� �� ������� ��� ����������� ������
			buf_X = curr_X;
			buf_Y = curr_Y;

			if (flag)
			{
				// ����� ����������� ����������� ������ ��������, �� �� ���������� ������������ � ������, ����� ���� ��������� ������� �� �� �����.
				if (goToExit()) {
					flag = false;
					// ������� ����� ������ �� �������
					rotSide = objectRotation( random(0,1) );
				}

				// ���������� �������� �������
				curr_X = buf_X;
				curr_Y = buf_Y;

				// ��������� ��� ������ ��������� �� �������
				return;
			}
			else {

				switch (rotSide)
				{
					// LEFT ~ RIGHT
				case RIGHT:
					if(gameField[int(round(curr_Y/FACECOEFF_H))][int(round((buf_X - speed)/FACECOEFF_W))] != '|')
						buf_X -= speed;
					break;
				case LEFT:
					if (gameField[int(round(curr_Y / FACECOEFF_H))][int(round((buf_X + speed) / FACECOEFF_W))] != '|')
						buf_X += speed;
					break;
				case UP:
					if (gameField[int(round((curr_Y - speed) / FACECOEFF_H))][int(round(buf_X / FACECOEFF_W))] != '|')
						buf_Y -= speed;
					break;
				case DOWN:
					if (gameField[int(round((curr_Y + speed) / FACECOEFF_H))][int(round(buf_X / FACECOEFF_W))] != '|')
						buf_Y += speed;
					break;
				default:
					break;
				}
			}

			// ����� �������
			timer = 0.0;
		}

		// ������
		if ((int)round(buf_Y / FACECOEFF_H) == 17 && ((int)round(buf_X / FACECOEFF_W) == 0 || (int)round((buf_X / FACECOEFF_W)) == 28))
		{
			if ((int)(round(buf_X / FACECOEFF_W)) == 0) {
				buf_X = (28 - 0.5) * FACECOEFF_W;
			}
			else {
				buf_X = 0.2 * FACECOEFF_W;
			}

			// ���������� �������� �������
			curr_X = buf_X;
			curr_Y = buf_Y;

			return;
		}

		// ����������� ����
		if ( canPass(buf_Y + speed * ((rotSide == DOWN) ? 1 : -1) * ((rotSide < 2) ? 0 : 1), buf_X + speed * ( (rotSide == LEFT) ? 1 : -1) * ((rotSide > 1) ? 0 : 1)) ) {

			// ���������� �������� �������
			curr_X = buf_X;
			curr_Y = buf_Y;
		}
		else {

			/* �������� � �������*/
			if(!hunt)	
				rotSide = changeSide();
		
			// ����������� � ������� ���������� � ����� ���������������
			buf_X = curr_X;
			buf_Y = curr_Y;
		}

	}

};