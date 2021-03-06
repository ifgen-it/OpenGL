// Lab1_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"



// my variables


class Point3D
{
public:
	float x, y, z;

	Point3D() : x(0), y(0), z(0) {}

	Point3D(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

	void operator+=(Point3D& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}
};

bool flat = false;
bool lightDirectional = true;
bool ccw = true;

// end of my vars

// sphere vars
uint
nRings = 8,	// Количество колец (широта)
nSects = 8,	// Количество секций (долгота)
nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
nVert = (nRings + 1) * nSects + 2;	// Общее количество вершин

const uint clr1 = RGB(0, 255, 0), clr2 = RGB(0, 0, 255);	// Два цвета
const float
rad = 1.0f, 		// Радиус сферы
PI = acos(-1.0f);

float lightZ = 5.;
float lightX = 0.;
float
pos[] = { lightX, 0, lightZ, 0 },				// Позиция источника света
amb[] = { 0.05f, 0.05f, 0.05f },	// Фоновая подсветка
dif[] = { 0.9f, 0.9f, 0.9f },		// Свет, обеспечивающий иллюзию трехмерности
spec[] = { 0.8f, 0.8f, 0.8f };		// Свет, обеспечивающий блик
char buf[128];						// Буфер текста
int shine = 40; 					// Блескость материала

struct VERT{ Point3D v, n; uint c; };	// Координаты вершины, нормали и ее цвет
struct TRIA{ int i1, i2, i3; };			// Индексы трех вершин треугольника, выбираемых из массива вершин типа VERT


void Sphere(VERT *v, TRIA* t);
// end sphere vars


float
	rotX, rotY,	// Скорости поворота

	ax = 1, ay = 4,	// Углы поворота изображения вокруг осей X и Y
	dx = 0, 
	dy = 0, 
	dz = -5,	// Сдвигивдолькоординат
	color[] = { 0.1f, 0.6f, 0.1f };
short
	posX, posY;	// Текущая позиция указателя мыши
bool
	leftButton, twoSide = true, culling = true;	// Нажата левая кнопка мыши. Свет отражается от обеих поверхностей (лицевой и обратной)

//float v[8][3] = { -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1, 1, 1 };

//float norm[6][3] = { 0, 0, -1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0 };

float rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // Матрица, суммирующая малые враще-ния

//int id[6][4] =
//{
//	0, 1, 2, 3,	// Rear (CCW - counterclockwise)
//	4, 5, 6, 7,	// Front
//	0, 3, 5, 4,	// Left
//	7, 6, 2, 1,	// Right
//	0, 4, 7, 1,	// Top
//	5, 3, 2, 6,	// Bottom
//};


void Print(float x, float y, const char *format, ...)
{
	va_list args;
	char buffer[200];
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	glPushMatrix();
	glTranslatef(x, y, 0);
	for (char* p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p); // GLUT_STROKE_MONO_ROMAN // GLUT_STROKE_ROMAN
	glPopMatrix();
}

void DrawInfo()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.7f, 1, 0);
	Print(80, 2800, "Rings: %d", nRings);
	Print(80, 2650, "Sects: %d", nSects);
	Print(80, 2500, "Triangles: %d", nTria);
	Print(80, 200, "Light is: %s", buf);
	Print(80, 60, "Coordinates: (%3.1f, %3.1f, %3.1f)", pos[0], pos[1], pos[2]);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}


void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	DrawInfo();

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glTranslated(dx, dy, dz);
	glMultMatrixf(rotMatrix);

	glCallList(1);

	glPopMatrix();
	glutSwapBuffers();



	//  ДЛЯ КУБА 

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);	// Будем пользоваться услугами MM
	//glLoadIdentity();
	//glTranslatef(dx, dy, dz);// Смещение координат точек будущих примитивов

	//glMultMatrixf(rotMatrix);// Вместо поворотов умножаем на матрицу, вобравшую все вращения

	//glCallList(1);
	//glutSwapBuffers();

}



void DrawScene()
{
	
	VERT *Vert = new VERT[nVert];
	TRIA *Tria = new TRIA[nTria];

	Sphere(Vert, Tria);	// Вычислениекоординат, нормалей и индексов

	// Задание адресов трех массивов (вершин, нормалей и цветов),  а также шага перемещения по ним
	glVertexPointer(3, GL_FLOAT, sizeof(VERT), &Vert->v);
	glNormalPointer(GL_FLOAT, sizeof(VERT), &Vert->n);
	glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(VERT), &Vert->c);

	glNewList(1, GL_COMPILE);	// Формирование списка рисующих команд
	glDrawElements(GL_TRIANGLES, nTria * 3, GL_UNSIGNED_INT, Tria);
	glEndList();


	delete[] Vert;
	delete[] Tria;

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

	/*ay += 0.1;
	ax += 0.1;*/
	AddRotation(rotX / 200, 1, 0, 0);
	AddRotation(rotY / 200, 0, 1, 0);


	glutPostRedisplay();
}

void OnSize(int w, int h)	// Реакция на WM_SIZE
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-5, 5, -5, 5, -10, 8);
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
		//ay += (x - posX) / 2; // Измените углы поворота пропорционально смещению мыши (разностей x – posX и y – posY)
		//ax += (y - posY) / 2;
		AddRotation(rotX / 10, 1, 0, 0);
		AddRotation(rotY / 10, 0, 1, 0);

	}
	else
	{
		dz += (rotX + rotY) / 60;
		//dz += (x - posX)/2 + (y - posY)/2;
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
	case '5':
		AddRotation(-10, 1, 0, 0);
		; break;
	case '6':
		AddRotation(10, 1, 0, 0);
		; break;
	case '7':
		AddRotation(-10, 0, 1, 0);
		; break;
	case '8':
		AddRotation(10, 0, 1, 0);
		; break;
	case '9':
		AddRotation(-10, 0, 0, 1);
		; break;
	case '0':
		AddRotation(10, 0, 0, 1);
		; break;

	case 27: exit(0); break;
	case '2': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;

	case '1': 
		culling = !culling;
		if (culling)
		{
			glEnable(GL_CULL_FACE); 		// Culling говорит о том, что не видимые поверхности можно не рисовать
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
		; break;

	case '+': dz += 0.5; break;
	case '-': dz -= 0.5; break;

		

	case 'r':
		ccw = !ccw;
		if (ccw)
		{
			glFrontFace(GL_CCW);
		}
		else
		{
			glFrontFace(GL_CW);
		}
		break;

	case 'n':
		flat = !flat;
		glShadeModel(flat ? GL_FLAT : GL_SMOOTH);
		break;

	case 'a':
		lightX -= 1;
		pos[0] = lightX;
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		break;

	case 'd':
		lightX += 1;
		pos[0] = lightX;
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		break;
	case 'w':
		lightZ -= 1;
		pos[2] = lightZ;
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		break;

	case 's':
		lightZ += 1;
		pos[2] = lightZ;
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		break;

	case 'p': 
		lightDirectional = !lightDirectional;
		if (lightDirectional)
		{
			strcpy_s(buf, "Directional");
			pos[3] = 0;
		}
		else
		{
			strcpy_s(buf, "Positional");
			pos[3] = 1;
		}
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		break;

	case 'z': 
		nRings -= 1 + (nRings / 10);
		if (nRings < 1) nRings = 1;
		nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
		nVert = (nRings + 1) * nSects + 2;	// Общее количество вершин
		DrawScene();
		break;
	case 'x':
		
		nRings += 1 + (nRings / 10);
		nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
		nVert = (nRings + 1) * nSects + 2;	// Общее количество вершин
		DrawScene();
		break;

	case 'c':
		nSects -= 1 + (nSects / 10);
		if (nSects < 1) nSects = 1;
		nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
		nVert = (nRings + 1) * nSects + 2;	// Общее количество вершин
		DrawScene();
		break;
	case 'v':
		nSects += 1 +(nSects / 10);
		nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
		nVert = (nRings + 1) * nSects + 2;	// Общее количество вершин
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



void Sphere(VERT *v, TRIA* t)
{
	uint last = nVert - 1;// Индекс последней вершины (на южном полюсе)

	v[0].v = Point3D(0, rad, 0);// Формирование массива вершин. Северный полюс
	v[0].n = Point3D(0, 1, 0);
	v[0].c = clr2;

	v[last].v = Point3D(0, -rad, 0); // Южный полюс
	v[last].n = Point3D(0, -1, 0);
	v[last].c = nVert & 1 ? clr2 : clr1;

	float
		da = PI / (nRings + 2),
		db = 2 * PI / nSects,
		af = PI - da / 2,
		bf = 2 * PI - db / 2;

	uint n = 1;	// Индекс вершины, следующей за северным полюсом

	for (float a = da; a < af; a += da) 	// Циклпошироте
	{
		float
			y = rad * cos(a), 		// Координата y постоянна для всего кольца
			xz = rad * sin(a);

		for (float b = 0.; b < bf; n++, b += db)// Циклподолготе
		{
			float// Координаты проекции в экваториальной плоскости
				x = xz * sin(b),
				z = xz * cos(b);

			v[n].v = Point3D(x, y, z);
			v[n].n = Point3D(x / rad, y / rad, z / rad);
			v[n].c = n & 1 ? clr1 : clr2;
		}
	}

	for (n = 0; n < nSects; n++)	// Формирование массива индексов. Треугольники вблизи полюсов
	{
		t[n].i1 = 0;// Индекс общей вершины (северный полюс)
		t[n].i2 = n + 1;// Индекс текущей вершины
		t[n].i3 = n == nSects - 1 ? 1 : n + 2;// Замыкание

		t[nTria - nSects + n].i1 = nVert - 2 - ((1 + n) % nSects);	// Южныйполюс
		t[nTria - nSects + n].i2 = nVert - 1;
		t[nTria - nSects + n].i3 = nVert - 2 - n;
	}

	int k = 1;		// Вершина, следующая за полюсом
	n = nSects;
	for (uint i = 0; i < nRings; i++, k += nSects)// Треугольникиразбиенияколец
	{
		for (uint j = 0; j < nSects; j++, n += 2)
		{
			t[n].i1 = k + j;
			t[n].i2 = k + nSects + j;
			t[n].i3 = k + nSects + ((j + 1) % nSects);

			t[n + 1].i1 = t[n].i1;
			t[n + 1].i2 = t[n].i3;
			t[n + 1].i3 = k + ((j + 1) % nSects);
		}
	}
}



void Init()	// Настройки
{
	if (lightDirectional)
	{
		strcpy_s(buf, "Directional");
		pos[3] = 0;
	}
	else
	{
		strcpy_s(buf, "Positional");
		pos[3] = 1;
	}
	if (ccw)
	{
		glFrontFace(GL_CCW);
	}
	else
	{
		glFrontFace(GL_CW);
	}
	
	glClearColor(0, 0, 0, 0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	if (culling)
	{
		glEnable(GL_CULL_FACE); 		// Culling говорит о том, что не видимые поверхности можно не рисовать
	}
	
	glShadeModel(flat ? GL_FLAT : GL_SMOOTH);
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, twoSide);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);// Небольшая фоновая подсветка
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	DrawScene();

	//VERT *Vert = new VERT[nVert];
	//TRIA *Tria = new TRIA[nTria];

	//Sphere(Vert, Tria);	// Вычислениекоординат, нормалей и индексов

	//// Задание адресов трех массивов (вершин, нормалей и цветов),  а также шага перемещения по ним
	//glVertexPointer(3, GL_FLOAT, sizeof(VERT), &Vert->v);
	//glNormalPointer(GL_FLOAT, sizeof(VERT), &Vert->n);
	//glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(VERT), &Vert->c);

	//glNewList(1, GL_COMPILE);	// Формирование списка рисующих команд
	//glDrawElements(GL_TRIANGLES, nTria * 3, GL_UNSIGNED_INT, Tria);
	//glEndList();


	//delete[] Vert;
	//delete[] Tria;



	//  ЭТО ДЛЯ КУБА

	//glClearColor(1, 1, 1, 0);  // Цвет фона - белый
	//glClearColor(0.3, 0.3, 0.3, 0);

	//glShadeModel(GL_SMOOTH);  //glShadeModel(GL_FLAT);		// Нет интерполяции цветов при растеризации
	//glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FRONT_AND_BACK  *******мож быть GL_LINE || GL_POINT || GL_FILL

	//glEnable(GL_LIGHTING);	// "Рубильник в подвале"
	//glEnable(GL_LIGHT0);		// Источник света, размещенный в точке z = ∞

	//glEnable(GL_COLOR_MATERIAL);
	//DrawScene();


	//glPointSize(7);					// Задаем размер точки
	//glEnable(GL_POINT_SMOOTH);		// Задаем «мягкий» контур для точки

	

	/*int nLights;
	glGetIntegerv(GL_MAX_LIGHTS, &nLights);
	cout << nLights << endl;*/
	

}


void main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Sphere");
	Init();

	int dep;
	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &dep);
	cout << "ModelView stack size = " << dep;
	

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
