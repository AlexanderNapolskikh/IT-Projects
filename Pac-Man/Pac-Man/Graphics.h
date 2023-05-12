#pragma once

#include "SFML/Graphics.hpp"
// Игровое поле
#include "GameField.h"

// Размеры "костяка" карты (лабиринта)
int const HEIGHT_MAP = 36;
int const WIDTH_MAP = 28;

// Рандомайзер
#define random(a,b) a + rand() % (b + 1 - a)

// Скорость ПакМан-а
float const SPEDD_PAC = 8;

// Шаг кадра
float const STEP_FRAME = 0.001;
// Шаг таймера
float const STEP_TIMER = 0.005;

// Размер захватываемой текстуры
float const CATCH_SPLIT_W = 14;
float const CATCH_SPLIT_H = 13.875;

// Коэффициенты растягивания изображение
// Настройка формы
const float WELL_VIEW_H = 1.8;
const float WELL_VIEW_W = 1.4;

// Коэффициент для хорошей пропорциональности игры
const float FACECOEFF_W = CATCH_SPLIT_W * WELL_VIEW_W;
const float FACECOEFF_H = CATCH_SPLIT_H * WELL_VIEW_H;

// Расположение ворот для выхода из коробки
const sf::Vector2u GATES(13 * FACECOEFF_W + FACECOEFF_W * 0.5, 14 * FACECOEFF_H);
//(((WIDTH_MAP - 1.15)* FACECOEFF_W) / 2, HEIGHT_MAP* FACECOEFF_H * 0.4)

// Направления NPC и игрока
enum objectRotation {

	// Согласно texture в TextureManager

	LEFT, // 0
	RIGHT, // 1
	UP, // 2
	DOWN // 3
};

struct PacMan {

	// Позиция текущего значения
	float curr_X;
	float curr_Y;
	// __________________________
	// Buffer position
	float buf_X = 0.0;
	float buf_Y = 200.0;
	// __________________________
	// Направление объекта
	objectRotation rotSide;
	// __________________________
	// Текущий кадр для анимации
	float frame = 0.0;
	// __________________________
	// Замедление изображения
	float timer = 0.0;
	// __________________________
	// Очки игрока
	int score = 0;
	// __________________________

	// Начальная инициализация элементов структуры
	void startInit(float X, float Y, objectRotation rotS) {

		curr_X = X;
		curr_Y = Y;

		rotSide = rotS;
	}

	// Обновление позиции и анимации объекта (игровой фигуры)
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

			// Сброс таймера
			timer = 0.0;
		}

		// Портал
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
			
			// Очистка "съеденной" траектории
			gameField[(int)round(curr_Y / FACECOEFF_H)][(int)round(curr_X / FACECOEFF_W)] = '_';

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;

			return;
		}

		if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.' || gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '_') {

			if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.')
			{
				score++;
			}

			// Очистка "съеденной" траектории
			gameField[(int)round(curr_Y / FACECOEFF_H)][(int)round(curr_X / FACECOEFF_W)] = '_';

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;
		}
		
	}


};

struct Ghost {

	// Позиция текущего значения
	float curr_X;
	float curr_Y;
	// __________________________
	// Buffer position
	float buf_X = 0.0;
	float buf_Y = 200.0;
	// __________________________
	// Скорость преследования
	float speed;
	// __________________________
	// Направление объекта
	objectRotation rotSide;
	// ___________________________
	// Замедление изображения
	float timer = 0.0;
	// __________________________
	// Флаг выхода
	bool flag = false;
	// __________________________
	// Флаг охоты
	bool hunt = false;
	// __________________________

	// Начальная инициализация элементов структуры
	void startInit(float X, float Y, objectRotation rotS, float how_fast = SPEDD_PAC * 1.1) {

		curr_X = X;
		buf_X = curr_X;
		curr_Y = Y;
		buf_Y = curr_Y;

		speed = how_fast;
		rotSide = rotS;
	}

	// Выход из коробки
	void ghostOut() {
		flag = !flag;
	}

	// Проверка возможности прохода в заданном направлении
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

	// Построение хаотичного маршрута призрака(искючается вариант хода по той же линии в обратном направлении)
	bool plotWay() {

		// Флаги возможностей перенаправления
		bool flag_R, flag_L, flag_U, flag_D;
		float koeff = 1.2;

		// Ход вверх или вниз
		if (rotSide == UP || rotSide == DOWN) {
		
			flag_R = false;
			flag_L = false;

			// Проверка выхода к пересечению(или перекрестку)
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

			// Принятие решения исходя из полученных данных в пространстве
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
		// Ход вправо или влево
		else if (rotSide == RIGHT || rotSide == LEFT) {

			flag_U = false;
			flag_D = false;

			// Проверка выхода к пересечению(или перекрестку)
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

			// Принятие решения исходя из полученных данных в пространстве

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

	// Изменение направления при движении
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

	// Проложение пути к выходу из коробки
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

	// Пауза для поворота
	float pause_rotate = 0.0;
	// ___________________________________

	// Движение и обновление
	void update() {

		// Таймер плавного перемещения
		timer += STEP_TIMER;
		pause_rotate += 0.001;

		// Прохождение временного ингтервала и обновление
		if (timer >= 1) {

			if (hunt && pause_rotate >= 1) {
				plotWay();
				pause_rotate = 0.0;
			}

			// Выставление прошлых данных для изменения и записи, а также сохранения тех же прошлых для возможности отката
			buf_X = curr_X;
			buf_Y = curr_Y;

			if (flag)
			{
				// Когда срабатывает возможность выхода призрака, то он немедленно отправляется к выходу, после чего вернуться обратно он не может.
				if (goToExit()) {
					flag = false;
					// Поворот после выхода из коробки
					rotSide = objectRotation( random(0,1) );
				}

				// Обновление значений позиции
				curr_X = buf_X;
				curr_Y = buf_Y;

				// Ускорение для выхода призракак из коробки
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

			// Сброс таймера
			timer = 0.0;
		}

		// Портал
		if ((int)round(buf_Y / FACECOEFF_H) == 17 && ((int)round(buf_X / FACECOEFF_W) == 0 || (int)round((buf_X / FACECOEFF_W)) == 28))
		{
			if ((int)(round(buf_X / FACECOEFF_W)) == 0) {
				buf_X = (28 - 0.5) * FACECOEFF_W;
			}
			else {
				buf_X = 0.2 * FACECOEFF_W;
			}

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;

			return;
		}

		// Продолжение пути
		if ( canPass(buf_Y + speed * ((rotSide == DOWN) ? 1 : -1) * ((rotSide < 2) ? 0 : 1), buf_X + speed * ( (rotSide == LEFT) ? 1 : -1) * ((rotSide > 1) ? 0 : 1)) ) {

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;
		}
		else {

			/* Задержка в коробке*/
			if(!hunt)	
				rotSide = changeSide();
		
			// Возвращение к прошлой координате с целью перенаправления
			buf_X = curr_X;
			buf_Y = curr_Y;
		}

	}

};