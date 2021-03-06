// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

// my variables
float dzStart = -80;
float dzInit1 = -20;
float dzInit2 = -40;
float dzInit3 = -60;
float dzInit4 = dzStart;


// end of my vars

float
	rotX, rotY,	// Скорости поворота

	ax = 25, ay = -25,	// Углы поворота изображения вокруг осей X и Y
	dx = 0, 
	dy = 0, 
	dz,	// Сдвигивдолькоординат
	color[] = { 0.1f, 0.6f, 0.1f };
short
	posX, posY;	// Текущая позиция указателя мыши
bool
	leftButton, twoSide, culling;	// Нажата левая кнопка мыши. Свет отражается от обеих поверхностей (лицевой и обратной)

float v[8][3] = { -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1, 1, 1 };

float norm[6][3] = { 0, 0, -1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0 };

int id[6][4] =
{
	0, 1, 2, 3,	// Rear (CCW - counterclockwise)
	4, 5, 6, 7,	// Front
	0, 3, 5, 4,	// Left
	7, 6, 2, 1,	// Right
	0, 4, 7, 1,	// Top
	5, 3, 2, 6,	// Bottom
};

float rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // Матрица, суммирующая малые враще-ния

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);	// Будем пользоваться услугами MM

	glColor3ub(255, 0, 0);
	glLoadIdentity();
	glTranslatef(dx+1, dy+1, dzInit1);// Смещение координат точек будущих примитивов
	glRotatef(ay, 0, 1, 0);	// Вращение координат точек примитивов
	glRotatef(ax, 1, 0, 0);
	glCallList(1);

	// my 2nd cube

	glColor3ub(0, 255, 0);
	glLoadIdentity();
	glTranslatef(dx-1, dy-1, dzInit2);// Смещение координат точек будущих примитивов
	glRotatef(ay, 0, 1, 0);	// Вращение координат точек примитивов
	glRotatef(ax, 1, 0, 0);
	glCallList(1);

	// my 3 cube

	glColor3ub(0, 0, 255);
	glLoadIdentity();
	glTranslatef(dx + 1, dy - 1, dzInit3);// Смещение координат точек будущих примитивов
	glRotatef(ay, 0, 1, 0);	// Вращение координат точек примитивов
	glRotatef(ax, 1, 0, 0);
	glCallList(1);

	// my 4 cube

	glColor3ub(255, 0, 255);
	glLoadIdentity();
	glTranslatef(dx - 1, dy + 1, dzInit4);// Смещение координат точек будущих примитивов
	glRotatef(ay, 0, 1, 0);	// Вращение координат точек примитивов
	glRotatef(ax, 1, 0, 0);
	glCallList(1);


	// end of my 2nd cube

	//glColor3fv(color);

	//glBegin(GL_QUADS);		// Выбор типа примитива
	//for (int i = 0; i < 6; i++)	// Долгоготовились - быстрорисуем
	//{
	//	//glColor3ub(rand() % 256, rand() % 256, rand() % 256);
	//	glNormal3fv(norm[i]);
	//	for (int j = 0; j < 4; j++)
	//		
	//		glVertex3fv(v[id[i][j]]);
	//}
	//glEnd();


	glutSwapBuffers();

}



void DrawScene()
{
	glNewList(1, GL_COMPILE); 	// Создаем новый список команд OpenGL
	//glColor3fv(color);
	glBegin(GL_QUADS);
	for (int i = 0; i < 6; i++)
	{
		glNormal3fv(norm[i]);
		for (int j = 0; j < 4; j++)
			glVertex3fv(v[id[i][j]]);
	}
	glEnd();
	glEndList();
}

void OnIdle()
{
	// Изменяйте значения углов поворота

	ay += 0.1;
	ax += 0.1;
	dzInit1 += 0.005;
	dzInit2 += 0.005;
	dzInit3 += 0.005;
	dzInit4 += 0.005;

	if (dzInit1 > 0) dzInit1 = dzStart;
	if (dzInit2 > 0) dzInit2 = dzStart;
	if (dzInit3 > 0) dzInit3 = dzStart;
	if (dzInit4 > 0) dzInit4 = dzStart;

	

	glutPostRedisplay();
}

void OnSize(int w, int h)	// Реакция на WM_SIZE
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-5, 5, -5, 5, -10, 8);
	gluPerspective(45, double(w) / h, 0.6, 100);

}

void OnMouse(int button, int state, int x, int y)
{
	leftButton = button == GLUT_LEFT_BUTTON;
	
	if (state == GLUT_DOWN)
	{
		glutIdleFunc(0);
		rotX = rotY = 0;

	}
	else
	{
		if (fabs(rotX) > 10 || fabs(rotY) > 10)
			glutIdleFunc(OnIdle);
		else
			glutIdleFunc(0);

	}
	posX = x;	// Запоминаем координаты мыши
	posY = y;
}

void OnMouseMove(int x, int y)
{
	rotX = y - posY;
	rotY = x - posX;


	if (leftButton)
	{
		ay += (x - posX) / 2; // Измените углы поворота пропорционально смещению мыши (разностей x – posX и y – posY)
		ax += (y - posY) / 2;
	}
	else
	{
		dz += (x - posX)/2 + (y - posY)/2;
		// Вычислите степень удаления или приближения и измените величину dz пропорционально смещению мыши
	}
	posX = x;	// Запоминаем новые координаты мыши
	posY = y;
	glutPostRedisplay();

	//cout << "\nx = " << x << "  y = " << y;
}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
	case 27: exit(0); break;
	case '2': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
	case '+': dz += 0.5; break;
	case '-': dz -= 0.5; break;

	case 'a': ay -= 2; break;
	case 'd': ay += 2; break;
	case 'w': ax -= 2; break;
	case 's': ax += 2; break;


	}
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: dx -= 0.1f; break;
	case GLUT_KEY_RIGHT: dx += 0.1f; break;
	case GLUT_KEY_DOWN: dy -= 0.1f; break;
	case GLUT_KEY_UP:  dy += 0.1f; break;
	}
	
	glutPostRedisplay();
}




void Init()	// Настройки
{
	//glClearColor(1, 1, 1, 0);  // Цвет фона - белый
	glClearColor(0.3, 0.3, 0.3, 0);

	glShadeModel(GL_SMOOTH);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FRONT_AND_BACK  *******мож быть GL_LINE || GL_POINT || GL_FILL

	glEnable(GL_LIGHTING);	// "Рубильник в подвале"
	glEnable(GL_LIGHT0);		// Источник света, размещенный в точке z = ∞

	glEnable(GL_COLOR_MATERIAL);
	DrawScene();


	glPointSize(7);					// Задаем размер точки
	glEnable(GL_POINT_SMOOTH);		// Задаем «мягкий» контур для точки

	

	/*int nLights;
	glGetIntegerv(GL_MAX_LIGHTS, &nLights);
	cout << nLights << endl;*/
	

}


void main()
{
	
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("");
	Init();

	glutMouseFunc(OnMouse);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKey);
	glutSpecialFunc(OnSpecialKey);
	glutIdleFunc(OnIdle);

	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
	//===Здесь нельзя размещать код, так как предыдущий оператор является псевдо бесконечным циклом выборки сообщений о событиях

}
