#include "pch.h"
#include "BMPLoader.h"

float
	rotX = 20, rotY = 30,	// Скорости поворота

	ax = 1, ay = 4,	// Углы поворота изображения вокруг осей X и Y
	dx = 0, 
	dy = 0, 
	dz = -7,	// Сдвигивдолькоординат
	color[] = { 0.1f, 0.6f, 0.1f };
short
	posX, posY;	// Текущая позиция указателя мыши
bool
	leftButton, twoSide, culling, cube = true;	// Нажата левая кнопка мыши. Свет отражается от обеих поверхностей (лицевой и обратной)

float v[8][3] = { -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1, 1, 1 };

float norm[6][3] = { 0, 0, -1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0 };

float rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; // Матрица, суммирующая малые враще-ния
float
	green[] = { 0, 0.6, 0 },
	white[] = { 0.9, 0.9, 0.9 },
	sx, sy,
	texX, texY, texZ;

float rotateTex = 0.f;
float scaleTexS = 1.f;
float scaleTexT = 1.f;
float translTexS = 0.f;
float translTexT = 0.f;

int id[6][4] =
{
	0, 1, 2, 3,	// Rear (CCW - counterclockwise)
	4, 5, 6, 7,	// Front
	0, 3, 5, 4,	// Left
	7, 6, 2, 1,	// Right
	0, 4, 7, 1,	// Top
	5, 3, 2, 6,	// Bottom
};

int
	filter = GL_NEAREST,
	wrap = GL_REPEAT;
bool lighting = false, sphereMap = false, decal = false, colored = false;

UINT texture;

int texGen = GL_OBJECT_LINEAR;
GLUquadric* q;



void SetDecal()
{
	if (decal)
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}

void SetLight()
{
	if (lighting)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}
	else
		glDisable(GL_LIGHTING);
}

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
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	glPopMatrix();
}

void DrawInfo()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	/*glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);*/

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.7f, 1, 0);
	Print(80, 2800, "Decal: %d", decal);

	Print(80, 340, "SpereMap: %d", sphereMap);
	Print(80, 2650, "Colored: %d", colored);
	Print(80, 2500, "Lighting: %d", lighting);
	Print(80, 200, "Wrapping: %s", (wrap == GL_REPEAT ? "Repeat" : "Clamp"));
	Print(80, 60, "Filtering: %s", (filter == GL_NEAREST ? "Nearest" : "LinearMipmapLinaer"));
	/*Print(80, 340, "Generation Mode: %s", (texGen == GL_EYE_LINEAR ? "GL_EYE_LINEAR" :
		texGen == GL_OBJECT_LINEAR ? "GL_OBJECT_LINEAR" : "GL_SPHERE_MAP"));*/

	//Print(80, 340, "SpereMap: %s", decal);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();

	/*glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);*/
}

char* FileDlg()
{
	char *fn = new char[MAX_PATH], dir[MAX_PATH];
	fn[0] = 0;
	GetCurrentDirectory(MAX_PATH - 1, dir);
	OPENFILENAME ofn =
	{
		sizeof(OPENFILENAME), 0, 0, "BMP Files (*.bmp)\0*.bmp\0DIB Files (*.dib)\0*.dib\0", 0, 0, 1,
		fn, MAX_PATH, 0, 0,	dir, "Find a bitmap file",
		OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		0, 0, "bmp", 0, 0, 0, 0, 0, 0
	};
	return GetOpenFileName(&ofn) ? fn : 0;
}

bool LoadTexture(int id)
{
	char *fn = FileDlg();
	if (fn == 0)
		return false;
	BmpLoader* p = new BmpLoader(fn);
	if (!p)
		throw "Error reading texture.\n";
	glBindTexture(GL_TEXTURE_2D, id);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, p->sizeX, p->sizeY, GL_RGB, GL_UNSIGNED_BYTE, p->data);
	return true;
}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glPushMatrix();
	DrawInfo();
	glTranslated(dx, dy, dz);
	glMultMatrixf(rotMatrix);
	glCallList(1);
	glPopMatrix();
	glutSwapBuffers();

}

void DrawScene()
{
	float t[4][2] = { 0, 1, 0, 0, 1, 0, 1, 1 };
	glNewList(1, GL_COMPILE);
	if (cube)
	{
		colored ? glColor3fv(green) : glColor3fv(white);

		glBegin(GL_QUADS);
		for (int i = 0; i < 6; i++)
		{
			glNormal3fv(norm[i]);
			for (int j = 0; j < 4; j++)
			{
				glTexCoord2fv(t[j]);
				glVertex3fv(v[id[i][j]]);
			}


		}
		glEnd();

		//glMatrixMode(GL_TEXTURE),
		//glRotatef(rotateTex, 0, 0, 1);
		//glTranslatef(0, 0, rotateTex);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();



		glScalef(scaleTexS, scaleTexT, 1);

		glTranslatef(translTexS + 0.5, translTexT + 0.5, 0.0);
		glRotatef(rotateTex, 0.0, 0.0, 1.0);
		glTranslatef(-translTexS - 0.5, -translTexT - 0.5, 0.0);

		glTranslatef(translTexS, translTexT, 0.0);

		glMatrixMode(GL_MODELVIEW);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		colored ? glColor3fv(green) : glColor3fv(white);
		gluSphere(q, 2, 32, 32);

	}
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
	glLoadIdentity();
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

}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
	case 27: exit(0); break;
	case 'z': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
	case '+': dz += 0.5; break;
	case '-': dz -= 0.5; break;

	case 'c': // colored
		colored = !colored;
		DrawScene();
		break;
	case 'u': cube = !cube;  DrawScene(); break;
	case 'd': // decal
		decal = !decal;
		SetDecal();
		break;

	case 's': // sphereMap
		/*texGen =
			texGen == GL_EYE_LINEAR ? GL_OBJECT_LINEAR :
			texGen == GL_OBJECT_LINEAR ? GL_SPHERE_MAP : GL_EYE_LINEAR;
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen);*/
		sphereMap = !sphereMap;
		if (sphereMap)
		{
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
		else
		{
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
		break;

	case 'l': // lighting
		lighting = !lighting;
		SetLight();
		break;

	case 'o': // loadTexture(1)
		if (LoadTexture(1))
			DrawScene();
		break;

	case 'w': // wrap
		wrap = wrap == GL_REPEAT ? GL_CLAMP : GL_REPEAT;
		break;

	case 'f': // filter
		filter = filter == GL_NEAREST ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST;
		break;


	case '4': // rotate texture
		rotateTex += 3;
		DrawScene();
		break;
	case '5': // rotate texture
		rotateTex += -3;
		DrawScene();
		break;


	case '6': // translate texture
		translTexS += 0.05;
		DrawScene();
		break;
	case '7': // translate texture
		translTexS += -0.05;
		DrawScene();
		break;


	case '8': // translate texture
		translTexT += 0.05;
		DrawScene();
		break;
	case '9': // translate texture
		translTexT += -0.05;
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


	case GLUT_KEY_F1:  // scale texture
		scaleTexS -= 0.05f;
		DrawScene();
		break;
	case GLUT_KEY_F2:  // scale texture
		scaleTexS += 0.05f;
		DrawScene();
		break;

	case GLUT_KEY_F3:  // scale texture
		scaleTexT -= 0.05f;
		DrawScene();
		break;
	case GLUT_KEY_F4:  // scale texture
		scaleTexT += 0.05f;
		DrawScene();
		break;

	}
	
	glutPostRedisplay();
}




void Init()	// Настройки
{
	q = gluNewQuadric();
	gluQuadricTexture(q, true);

	glClearColor(0, 0, 0, 0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	SetLight();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);


	SetDecal();
	sphereMap = false;
		/*glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, texGen);*/

	if (LoadTexture(1))
		DrawScene();



	// ОТ КУБА ОСТАЛОСЬ :
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

	
}


void main()
{
	
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Textures");
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
