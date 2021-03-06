// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT);	// Стираем буфер цветаframebuffer)
	glBegin(GL_POINTS);			// Включаем режим задания точек
	glColor3f(1, 0, 0);		// Выбираем цвет рисования
	glVertex2f(-0.8, -0.8);
	glVertex2f(-0.8, 0.8);
	glColor3f(0, 0, 1);
	glVertex2f(0.8, 0.8);
	glVertex2f(0.8, -0.8);
	glEnd();
	glFlush();
}
void OnSize(int w, int h)	// Реакция на WM_SIZE
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);	// Режим ортографической проекции

}
void Init()	// Настройки
{
	glClearColor(1, 1, 1, 0);	// Цвет фона - белый
	glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
	glPointSize(7);					// Задаем размер точки
	glEnable(GL_POINT_SMOOTH);		// Задаем «мягкий» контур для точки
}


void main()
{
	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("");
	Init();
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
	//===Здесь нельзя размещать код, так как предыдущий оператор является псевдо бесконечным циклом выборки сообщений о событиях

}
