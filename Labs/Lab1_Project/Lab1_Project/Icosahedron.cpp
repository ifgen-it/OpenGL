// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

void DrawOneSmoothNormal(float* v1);
void DrawOneFlatNormal(float* v1);

float rotationSpeed = 200;

float spec[3] = { 1, 1, 1 };
int widthNormal = 2;
float lengthNormal = 0.5;


bool twoSide;

float
	rotX, rotY,
	dx, dy, dz = -4,
	r = (1 + sqrt(5)) / 2, // Golden ratio
	v[12][3] =		// 12 vertices (x,y,z)
	{
		0, 1, r,  0,-1, r,  0, 1,-r,  0,-1,-r,
		1, r, 0, -1, r, 0,  1,-r, 0, -1,-r, 0,
		r, 0, 1, -r, 0, 1,  r, 0,-1, -r, 0,-1
	},
	rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
	color[] = { 0.1f, 0.6f, 0.1f };

float vv0[3] = {0 , 0 , 0};

float colorNorm[] = { 1.0f, 0.0f, 1.0f };	// цвет нормали Фиолетовый

int id[20][3] =	// 20 triangular faces
{
	0, 1, 8,   0, 9, 1,   0, 8, 4,   0, 4, 5,   0, 5, 9,
	2, 3, 11,  2, 11, 5,  2, 5, 4,	 2, 4, 10,  2, 10, 3,
	1, 9, 7,   1, 7, 6,   1, 6, 8,   3, 10, 6,  3, 6, 7,
	3, 7, 11,  4, 8, 10,  5, 11, 9,  6, 10, 8,  7, 9, 11
};

short posX, posY;
bool leftButton, flat = true, showNormal = false, reverseNormal = false, lightNormalOff = true, growing = true;
int depth;


void DrawTriaSmooth(float* v1, float* v2, float* v3)
{
	glColor3fv(color);
	glBegin(GL_TRIANGLES);
	glNormal3fv(v1);		glVertex3fv(v1);
	glNormal3fv(v2);		glVertex3fv(v2);
	glNormal3fv(v3);		glVertex3fv(v3);
	glEnd();

	
	// рисуем нормали для каждой вершины треугольника
	if (showNormal)
	{
		DrawOneSmoothNormal(v1);
		DrawOneSmoothNormal(v2);
		DrawOneSmoothNormal(v3);
	}
}

void DrawOneSmoothNormal(float* v1)
{
	float vvEnd[3] = {v1[0] * (1 + lengthNormal), v1[1] * (1 + lengthNormal), v1[2] * (1 + lengthNormal) };

	
	if (lightNormalOff)
	{
		glDisable(GL_LIGHTING);
	}
	
	glColor3fv(colorNorm);

	glBegin(GL_LINES);

	glVertex3fv(v1);	
	glVertex3fv(vvEnd);	

	glEnd();
	
	if (lightNormalOff)
	{
		glEnable(GL_LIGHTING);
	}
}

void ToUnit(float v[3])// Нормируемвектор
{
	float d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0)
	{
		MessageBox(0, "Zero length vector", "Error", MB_OK);
		return;
	}

	if (!reverseNormal)
	{
		v[0] /= d; v[1] /= d; v[2] /= d;   // будет колючка ->   // v[0] /= -d; v[1] /= -d; v[2] /= -d;
	}
	else
	{
		v[0] /= -d; v[1] /= -d; v[2] /= -d;   
	}
}

void GetNorm(float a[3], float b[3], float n[3])
{
	// Вычисляем координаты вектора нормали по формулам векторного произведения
	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];
	ToUnit(n);
}

void DrawTriaFlat(float* v1, float* v2, float* v3)
{
	float norm[3], a[3], b[3];
	for (int i = 0; i < 3; i++)// Векторы a и b – это стороны треугольника
	{
		a[i] = v3[i] - v2[i];
		b[i] = v1[i] - v2[i];
	}
	GetNorm(a, b, norm);	// Вычисляем нормаль и создаем треугольник

	glColor3fv(color);
	glBegin(GL_TRIANGLES);
	glNormal3fv(norm);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();


	if (showNormal)
	{
		if (lightNormalOff)
		{
			glDisable(GL_LIGHTING);
		}

		// рисуем нормаль

		//начало нормали - по центру грани треугольника
		float vvAvg[3] = { (v1[0] + v2[0] + v3[0]) / 3,  (v1[1] + v2[1] + v3[1]) / 3 , (v1[2] + v2[2] + v3[2]) / 3 };

		//конец нормали
		float vvEnd[3];
		for (int i = 0; i < 3; i++)
		{
			vvEnd[i] = vvAvg[i] + norm[i] * lengthNormal;
		}

		glColor3fv(colorNorm);

		glBegin(GL_LINES);

		glVertex3fv(vvAvg);
		glVertex3fv(vvEnd);

		glEnd();

		if (lightNormalOff)
		{
			glEnable(GL_LIGHTING);
		}
	}

}

void DrawOneFlatNormal(float* v1)
{
	float vvEnd[3] = { v1[0] * 2, v1[1] * 2, v1[2] * 2 };

	//glDisable(GL_LIGHTING);
	glColor3fv(colorNorm);

	glBegin(GL_LINES);

	glVertex3fv(v1);	// вершина треугольника
	glVertex3fv(vvEnd);	// ноль коородинат

	glEnd();
	//
	glEnable(GL_LIGHTING);
}




void DrawRecursive(float *v1, float *v2, float *v3, int depth)
{
	float v12[3], v23[3], v31[3];
	if (depth == 0)
	{
		if (flat)
			DrawTriaFlat(v1, v2, v3);
		else
			DrawTriaSmooth(v1, v2, v3);
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];
	}

	ToUnit(v12);	ToUnit(v23);	ToUnit(v31);
	DrawRecursive(v1, v12, v31, depth - 1);
	DrawRecursive(v2, v23, v12, depth - 1);
	DrawRecursive(v3, v31, v23, depth - 1);
	DrawRecursive(v12, v23, v31, depth - 1);
}


void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);	// Будем пользоваться услугами MM

	//glColor3ub(255, 0, 0);
	glLoadIdentity();
	
	glTranslatef(dx, dy, dz);// Смещение координат точек будущих примитивов

	glMultMatrixf(rotMatrix);// Вместо поворотов умножаем на матрицу, вобравшую все вращения


	glCallList(1);


	glutSwapBuffers();

}



void DrawScene()
{
	glNewList(1, GL_COMPILE);
	for (int i = 0; i < 20; i++)

		//DrawTriaSmooth(v[id[i][0]], v[id[i][1]], v[id[i][2]]);
		//DrawTriaFlat(v[id[i][0]], v[id[i][1]], v[id[i][2]]);
		DrawRecursive(v[id[i][0]], v[id[i][1]], v[id[i][2]], depth);

	glEndList();

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


void OnIdle()
{
	// Изменяйте значения углов поворота

	AddRotation(rotX / rotationSpeed , 1, 0, 0);
	AddRotation(rotY / rotationSpeed , 0, 1, 0);


	glutPostRedisplay();
}

void OnSize(int w, int h)	// Реакция на WM_SIZE
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-5, 5, -5, 5, -10, 8);
	gluPerspective(45, double(w) / h, 0.6, 100);

	glMatrixMode(GL_MODELVIEW);	// чтобы при изменении размеров оставался куб в центре
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
	case 32: 
		depth += growing ? 1 : -1;
		if (!depth || depth == 4)
			growing = !growing;
		DrawScene();
		break;

	case 'n':
		flat = !flat;
		DrawScene();
		break;

	case 's':
		showNormal = !showNormal;
		DrawScene();
		break;

	case 'r':
		reverseNormal = !reverseNormal;
		DrawScene();
		break;

	case 'l':
		lightNormalOff = !lightNormalOff;
		DrawScene();
		break;


	}
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y)
{
	float delta = -0.005 * dz;

	switch (key)
	{
	case GLUT_KEY_LEFT: dx -= delta; break;
	case GLUT_KEY_RIGHT: dx += delta; break;
	case GLUT_KEY_DOWN: dy -= delta; break;
	case GLUT_KEY_UP:  dy += delta; break;
	}
	
	glutPostRedisplay();
}




void Init()	// Настройки
{
	//glClearColor(1, 1, 1, 0);  // Цвет фона - белый
	glClearColor(0.3, 0.3, 0.3, 0);


	float d = sqrt(1 + r * r);
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 3; j++)
			v[i][j] /= d;
	}

	glLineWidth(widthNormal); 	// толщина нормали

	// только для куба //glShadeModel(GL_SMOOTH);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FRONT_AND_BACK  *******мож быть GL_LINE || GL_POINT || GL_FILL

	glEnable(GL_LIGHTING);	// "Рубильник в подвале"
	glEnable(GL_LIGHT0);		// Источник света, размещенный в точке z = ∞

	glEnable(GL_COLOR_MATERIAL);

	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, 128);	// для блика

	glEnable(GL_NORMALIZE);		// и без этого работает

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
