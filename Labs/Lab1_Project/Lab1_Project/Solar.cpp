// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"

// СОЛНЦЕ ::

float sun = 0, year = 0, day = 20, moon = 0, solarRotate = 0; // Углы вращения
float orbitEarth = 3.0f;
float orbitMoon = 0.6f;
int orbitWidth = 2;

uint shine = 128;

float lightZ = -7.;
float lightX = 0.;
float lightY = 0.;
float pos[] = { lightX, lightY, lightZ, 1 };			// Позиция источника света =  свет ближний, если pos[3] = 1

float pos1[] = { 50, 50, 50, 1 };
float white_light[] = { 1.0, 1.0, 1.0, 1.0 };

float sunEmis[] = { 0.99f, 0.8f, 0.1f };
float resetEmis[] = { 0.0f, 0.0f, 0.0f };
float earthDif[] = { 0.1f, 0.6f, 0.99f };
float moonDif[] = { 0.7f, 0.6f, 0.7f };

float spec[] = { 1, 1, 1 };	

float spec1[] = { 0.3f, 0.3f, 0.3f  };

// ОТ КУБА ОСТАЛОСЬ ::

float
	rotX, rotY,	// Скорости поворота

	ax = 1, ay = 4,	// Углы поворота изображения вокруг осей X и Y
	dx = 0, 
	dy = 0, 
	dz = -7,	// Сдвигивдолькоординат
	color[] = { 0.1f, 0.6f, 0.1f };
short
	posX, posY;	// Текущая позиция указателя мыши
bool
	leftButton, twoSide, culling, flat = true;	// Нажата левая кнопка мыши. Свет отражается от обеих поверхностей (лицевой и обратной)

float rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // Матрица, суммирующая малые враще-ния

void LightSolarUpdate()
{
	pos[0] = lightX;
	pos[1] = lightY;
	pos[2] = lightZ;
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

void OnPaint()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();	// Запоминаем единичное значение ММ в стеке
		glTranslated(dx, dy, dz);
		glMultMatrixf(rotMatrix); // ИзменяемММ

			glColor3f(1, 1, 1);
			glMaterialfv(GL_FRONT, GL_EMISSION, sunEmis);
			
			glCallList(1);
			
	glPopMatrix();		// Восстанавливаемединичноезначениеизстека
	glutSwapBuffers();


}

void AddRotation(float angle, float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, x, y, z);
	
	glMultMatrixf(rotMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
	
	glPopMatrix();
}

void DrawScene()
{
	glNewList(1, GL_COMPILE); 	// Создаем новый список команд OpenGL
			glPushMatrix();
			glRotatef(sun, 1, 1, 1);	// Вращаем систему координат
			glutSolidSphere(1, 28, 28);	// Рисуем Солнце
			glMaterialfv(GL_FRONT, GL_EMISSION, resetEmis);
			glPopMatrix();

					// Орбита Земли
					glLineWidth(orbitWidth);
					glBegin(GL_LINE_LOOP);
					for (int i = 0; i < 50; i++) {
						float a = (float)i / 50.0f * 3.1415f * 2.0f;
						glVertex3f(cos(a) * orbitEarth, 0, sin(a) * orbitEarth);
					}
					glEnd();

					// Операция со стеком (Ваш код)
					glRotatef(year, 0, 1, 0);	// Вращаем систему координат
					glTranslatef(orbitEarth, 0, 0);	// Смещаем вправо (Землю)

							// Операция со стеком (Ваш код)
							glPushMatrix();
							glRotatef(day, 0, 1, 0);	// Вращаем в смещенной системе

							glMaterialfv(GL_FRONT, GL_DIFFUSE, earthDif);
							glutSolidSphere(0.3, 20, 20); // Рисуем Землю
								// Ось Земли
								glBegin(GL_LINES);
								glVertex3f(0, 0.4, 0);
								glVertex3f(0, -0.4, 0);
								glEnd();
								
							glPopMatrix();

								// Орбита Луны
								glBegin(GL_LINE_LOOP);
								for (int i = 0; i < 50; i++) {
									float a = (float)i / 50.0f * 3.1415f * 2.0f;
									glVertex3f(0, cos(a) * orbitMoon, sin(a) * orbitMoon);
								}
								glEnd();
									glRotatef(moon, 1, 0, 0);	// Вращаем систему координат
									glTranslatef(0, orbitMoon, 0);		// Смещаем вверх (луну)

									glMaterialfv(GL_FRONT, GL_DIFFUSE, moonDif);
									glutSolidSphere(0.08, 16, 16);  // Рисуем Луну
									
	glEndList();
}

void OnIdle()
{
	// Изменяйте значения углов поворота

	AddRotation(rotX / 200, 1, 0, 0);
	AddRotation(rotY / 200, 0, 1, 0);

	glutPostRedisplay();
}

void OnSize(int w, int h)	// Реакция на WM_SIZE
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, double(w) / h, 0.6, 100);
	glMatrixMode(GL_MODELVIEW);
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
		if (fabs(rotX) > 1 || fabs(rotY) > 1)
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
		 // Измените углы поворота пропорционально смещению мыши (разностей x – posX и y – posY)
		
		AddRotation(rotX / 10, 1, 0, 0);
		AddRotation(rotY / 10, 0, 1, 0);

	}
	else
	{
		dz += (rotX + rotY) / 60;
		lightZ = dz;
		LightSolarUpdate();
		// Вычислите степень удаления или приближения и измените величину dz пропорционально смещению мыши
	}
	posX = x;	// Запоминаем новые координаты мыши
	posY = y;
	glutPostRedisplay();

}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
	case 27: exit(0); break;
	case '2': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
	case '+': dz += 0.5; lightZ = dz; LightSolarUpdate(); break;
	case '-': dz -= 0.5; lightZ = dz; LightSolarUpdate(); break;

	case 's': sun += 4; DrawScene(); break;
	case 'y': year += 4; DrawScene(); break;
	case 'd': day += 4; DrawScene(); break;
	case 'm': moon += 4; DrawScene(); break;

	case 'a':    day -= 30; moon += 30; year += 1 / 12.0f; sun += 1;
		DrawScene(); break;

	case 'f': 
		flat = !flat;
		if (flat)
		{
			glShadeModel(GL_FLAT);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
		}
		else
		{
			glShadeModel(GL_SMOOTH);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
		}
		break;

	}
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y)
{
	float delta = -0.005 * dz;

	switch (key)
	{
	case GLUT_KEY_LEFT: dx -= delta; lightX = dx; break;
	case GLUT_KEY_RIGHT: dx += delta; lightX = dx; break;
	case GLUT_KEY_DOWN: dy -= delta; lightY = dy; break;
	case GLUT_KEY_UP:  dy += delta; lightY = dy; break;
	}

	LightSolarUpdate();
	glutPostRedisplay();
}

void Init()	// Настройки
{
	//glClearColor(1, 1, 1, 0);  // Цвет фона - белый
	glClearColor(0.3, 0.3, 0.3, 0);
	if (flat)
	{
		glShadeModel(GL_FLAT);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
	}
	else
	{
		glShadeModel(GL_SMOOTH);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
	}
	
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FRONT_AND_BACK  *******мож быть GL_LINE || GL_POINT || GL_FILL

	glEnable(GL_LIGHTING);	// "Рубильник в подвале"
	glEnable(GL_LIGHT0);		// Источник света, размещенный в точке z = ∞
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glEnable(GL_LIGHT1);	
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);

	

	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);		
	glMateriali(GL_FRONT, GL_SHININESS, shine);

	DrawScene();


	glPointSize(7);					// Задаем размер точки
	glEnable(GL_POINT_SMOOTH);		// Задаем «мягкий» контур для точки

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
