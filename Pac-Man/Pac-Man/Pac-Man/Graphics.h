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
const sf::Vector2u GATES(((WIDTH_MAP - 1.15)* FACECOEFF_W) / 2, HEIGHT_MAP* FACECOEFF_H * 0.4);

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
				buf_X-= SPEDD_PAC;
				break;
			case RIGHT:
				buf_X+= SPEDD_PAC;
				break;
			case UP:
				buf_Y-= SPEDD_PAC;
				break;
			case DOWN:
				buf_Y+= SPEDD_PAC;
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

			// Очистка "съеденной" траектории
			gameField[(int)round(curr_Y / FACECOEFF_H)][(int)round(curr_X / FACECOEFF_W)] = '_';

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;

			return;
		}

		if (gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '.' || gameField[(int)round(buf_Y / FACECOEFF_H)][(int)round(buf_X / FACECOEFF_W)] == '_') {

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

	bool canPass(float check_Y, float check_X) {

		if (gameField[(int)round(check_Y / FACECOEFF_H)][(int)round(check_X / FACECOEFF_W)] == '.'
			|| gameField[(int)round(check_Y / FACECOEFF_H)][(int)round(check_X / FACECOEFF_W)] == '_'
			|| gameField[(int)round(check_Y / FACECOEFF_H)][(int)round(check_X / FACECOEFF_W)] == '*') {

			return true;
		}
		else {
			return false;
		}
	}
	
	// Изменение направления при движении в коробке
	objectRotation changeSideBox() {

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
			return true;
		}else if (round(buf_X) > GATES.x) {
			buf_X -= 0.1;
		}
		else if (round(buf_X) < GATES.x)
		{
			buf_X += 0.1;
		} else if (round(buf_Y) > GATES.y) {
			buf_Y -= 0.1;
		}
		else
		{
			buf_Y += 0.1;
		}

		return false;
	}

	// Проверка одинакового направления и отрицательного
	// Н-р: текущее UP - можно Left or Right при столкновении
	bool rightDir(objectRotation dir) {

		float check_X = curr_X;
		float check_Y = curr_Y;

		switch (dir)
		{
		case RIGHT:
			check_X -= speed;
			break;
		case LEFT:
			check_X += speed;
			break;
		case UP:
			check_Y -= speed;
			break;
		case DOWN:
			check_Y += speed;
			break;
		default:
			break;
		}

		if (((int(dir) >= 2 && int(rotSide) <= 1) || (int(dir) <= 1 && int(rotSide) >= 2)) && canPass(check_Y,check_X)) {
			return true;
		}

		if (dir == rotSide || dir == objectRotation(int(rotSide) + 1)) { // LEFT - 0 RIGHT - 1 ==> если текущее left, то не должно выпасть ни left, ни right также для UP и DOWN
		
			return false;
		}
		else if (dir == rotSide || dir == objectRotation(int(rotSide) - 1)) { // Наоборот RIGHT - 1 LEFT - 0 ...

			return false;
		}
		else 
			return true;
	}

	// Последовательность направлений без повторений и обратных направлений
	// Повышение уникальности построения маршрута призрака
	objectRotation plotAroute() {

		objectRotation dirOut;
		
		do
		{
			dirOut = objectRotation(random(0, 3));
		} while (!rightDir(dirOut));
		
		return dirOut;
	}

	// Возможность захода в поворот, который находится на пути призрака
	bool tryGoDironWay() {

		if (rotSide == UP || rotSide == DOWN) {

			// Right
			if (canPass(buf_Y, buf_X + FACECOEFF_W)) {
				return true;
			}
			
			// Left
			if (canPass(buf_Y, buf_X - FACECOEFF_W)) {
				return true;
			}

		}
		else if (rotSide == RIGHT || rotSide == LEFT) {

			// Up
			if (canPass(buf_Y - FACECOEFF_H, buf_X)) {
				return true;
			}

			// Down
			if (canPass(buf_Y + FACECOEFF_H, buf_X)) {
				return true;
			}

		}

		return false;
	}

	// Движение и обновление
	void update() {

		timer += STEP_TIMER;

		if (timer >= 1) {

			// Выставление прошлых данных для изменения и записи, а также сохранения тех же прошлых для возможности отката
			buf_X = curr_X;
			buf_Y = curr_Y;

			// Проверка сторонних проходов
			if(hunt)
				tryGoDironWay();

			if (flag)
			{
				// Когда срабатывает возможность выхода призрака, то он немедленно отправляется к выходу, после чего вернуться обратно он не может.
				if (goToExit()) { 

					flag = false;
					hunt = true;
				}

				// Обновление значений позиции
				curr_X = buf_X;
				curr_Y = buf_Y;

				return;
			}
			else {

				switch (rotSide)
				{
				case RIGHT:
					buf_X -= speed;
					break;
				case LEFT:
					buf_X += speed;
					break;
				case UP:
					buf_Y -= speed;
					break;
				case DOWN:
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


		if (canPass(buf_Y, buf_X)){

			// Выбор направления из имеющихся
			// Свободное движение по карте

			// Обновление значений позиции
			curr_X = buf_X;
			curr_Y = buf_Y;
		}
		else if(!flag && !hunt){
			/* Задержка в коробке*/

			rotSide = changeSideBox();
			// Возвращение к прошлой координате с целью перенаправления
			buf_X = curr_X;
			buf_Y = curr_Y;
		}
		else if(hunt){
			/* Столкновения*/

			rotSide = plotAroute();
			// Возвращение для перенаправления
			buf_X = curr_X;
			buf_Y = curr_Y;
			
		}

	}

};