// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

void Init();

double
Left = -1, Top = 1, Right = 1, Bot = -1, // Прямоугольник игрового поля (все окно)
x, y, Step = 0.0004, dx = Step,  dy = -Step, r = 0.05, yMin,	// Мяч: Координаты, Смещение, Радиус
xc = 0, x1, x2,	// Ракетка: Х-координата центра, левый и правый края,
w, h;		// Ширина, Высота

// Вычисляем скорость движения ракетки при подлете мячике к ракетке
double yStartCheckSpeed;
bool speedMeasuring, needForSpeedChecking;
double numberOfBalls = 4;		// на таком расстоянии начнется измерение скорости
int moveRacketCounter = 0;		// считает количество шагов ракетки во время замера скорости

void Set(double xx, double yy, double c, double ww)
{
	x = xx;		y = yy;    	// Переустановка параметров
	xc = c;		w = ww;		h = ww / 10;
	x1 = xc - w / 2;
	x2 = xc + w / 2;
	yMin = Bot + h + r;

	//my add
	speedMeasuring = false;
	needForSpeedChecking = true;
	yStartCheckSpeed = Bot + h + numberOfBalls * 2*r;
	
}

void OnSize(int w, int h) { glViewport(0, 0, w, h); }

void DrawScene()
{
	glColor3ub(0, 255, 0);
	glRectd(x1, Bot, x2, Bot + h);	// Ракетка

	glPushMatrix();	// Запоминаем состояние координатного пространства
	glTranslated(x, y, 0); // Изменение координат (сдвиг)
	glutSolidSphere(r, 20, 20);		// Мяч
	glPopMatrix();	// Восстанавливаем состояние пространства
}

void MoveRacket(int dir)
{
	double delta = dir * 2 * r;   //dir * 0.5 * r;
	if (Left < x1 + delta && x2 + delta < Right)
	{
		xc += delta;	x1 += delta;	x2 += delta;
	}
	//speed measuring
	if (speedMeasuring && needForSpeedChecking)
	{
		if (dir > 0)
		{
			moveRacketCounter++;
		}
		else if (dir < 0)
		{
			moveRacketCounter--;
		}
	}
}

void MoveBall()
{
	x += dx;	y += dy;

	if (x < Left + r || Right - r < x)
	{
		dx = -dx;
		x += 2. * dx;
	}
	
	//top
	if (y > Top - r)
	{
		dy = -dy;
		y += 2. * dy;
		needForSpeedChecking = true;
	}

	// racket's speed measuring
	if (y < yStartCheckSpeed && speedMeasuring == false && needForSpeedChecking)
	{
		speedMeasuring = true;
	}

	// bottom
	if (y < yMin)
	{
		if (x1 <= x && x2 >=x)
		{
			//speed params
			double addDx = 0;
			if (moveRacketCounter == 1) addDx = Step/4;
			else if (moveRacketCounter == 2) addDx = Step/2;
			else if (moveRacketCounter > 2) addDx = Step;


			else if (moveRacketCounter == -1) addDx = -Step/4;
			else if (moveRacketCounter == -2) addDx = -Step/2;
			else if (moveRacketCounter < -2) addDx = -Step;
			
			speedMeasuring = false;
			moveRacketCounter = 0;
			needForSpeedChecking = false;

			//x
			dx += addDx;
			
			// y 
			dy = -dy;
			y += 2. * dy;
		}
		else
		{
			Init();
		}
		
	}

}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT);
	MoveBall();	// Перемещаеммяч
	DrawScene();	// Рисуемсцену
	glutSwapBuffers();
}

void Init()
{
	glClearColor(0.3, 0.3, 0.3, 0);
	Set(0, 0.9, 0, 0.8);  //0.4);		// Устанавливаемпарметрыигры
	glutIdleFunc(0);	// Выключаеманимацию
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: MoveRacket(-1); break;
		case GLUT_KEY_RIGHT: MoveRacket(1); break;
	}
	glutPostRedisplay();
}

void OnKey(unsigned char ch, int x, int y)
{
	switch (ch)
	{
	case 27: exit(0); break;
	case 32: Init();	glutIdleFunc(OnPaint); break;	// Включаем анимацию
	}
	glutPostRedisplay();
}

void main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Arkanoid:");

	Init();

	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
}
