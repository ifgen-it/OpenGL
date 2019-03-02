#include "pch.h"

byte stripes[3 * 32];
float
eyeX, eyeY, eyeZ = 10,
freq = 1,
param[][4] =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	1, 1, 1, 1,
},
spec[] = { 1, 1, 1 };
int id, texGen = GL_OBJECT_LINEAR;
bool flat, cube = true;
uint shine = 40;

void MakeStripes()
{
	for (int i = 0; i < 32; i++)
	{
		int k = 3 * i;
		if (i <= 4)
			stripes[k] = 255;
		else if (i <= 8)
			stripes[k + 1] = 255;
		else
			stripes[k + 2] = 255;
	}
}

void Freq()
{
	for (int i = 0; i < 3; i++)
		param[i][i] = freq;
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
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_GEN_S);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.7f, 1, 0);
	Print(80, 2800, "Generation Mode: %s", (texGen == GL_EYE_LINEAR ? "GL_EYE_LINEAR" :
		texGen == GL_OBJECT_LINEAR ? "GL_OBJECT_LINEAR" : "GL_SPHERE_MAP"));
	Print(80, 2600, "Plane Eqn: (%3.2f, %3.2f, %3.2f, %3.2f)",
		param[id][0], param[id][1], param[id][2], param[id][3]);
	Print(80, 200, "ShadeModel: %s", (flat ? "Flat" : "Smooth"));
	Print(80, 60, "Shinyness: %d", shine);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
	glEnable(GL_TEXTURE_GEN_S);
}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	DrawInfo();
	gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);
	if (cube)
	{
		glFrontFace(GL_CCW);
		glutSolidCube(4);
	}
	else
	{
		glFrontFace(GL_CW);
		glutSolidTeapot(2);
	}
	glPopMatrix();
	glutSwapBuffers();
}

void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
	case 'c': cube = !cube; break;
	case 't':
		texGen =
			texGen == GL_EYE_LINEAR ? GL_OBJECT_LINEAR :
			texGen == GL_OBJECT_LINEAR ? GL_SPHERE_MAP : GL_EYE_LINEAR;
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen); break;
	case 'f': glShadeModel(((flat = !flat) ? GL_FLAT : GL_SMOOTH)); break;
	case 'h': if (shine > 2) shine--; break;
	case 'H': if (shine < 128) shine++; break;
	case ' ': id = (id + 1) % 4; glTexGenfv(GL_S, GL_OBJECT_PLANE, param[id]); break;
	case 27: exit(0); break;
	}
	glMateriali(GL_FRONT, GL_SHININESS, shine);
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: eyeX += 1; break;
	case GLUT_KEY_RIGHT: eyeX -= 1; break;
	case GLUT_KEY_DOWN: eyeY -= 1; break;
	case GLUT_KEY_UP:  eyeY += 1; break;
	case GLUT_KEY_PAGE_DOWN: freq -= 0.01; Freq(); break;
	case GLUT_KEY_PAGE_UP: freq += 0.01; Freq(); break;
	}
	glTexGenfv(GL_S, GL_OBJECT_PLANE, param[id]);
	glutPostRedisplay();
}

void Init()
{
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel((flat ? GL_FLAT : GL_SMOOTH));
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, shine);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	MakeStripes();
	glEnable(GL_TEXTURE_1D);
	glEnable(GL_TEXTURE_GEN_S);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, 3, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, stripes);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, param[id]);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen);
}

void main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 800);
	glutCreateWindow("Arrows, Space, PgDn, PgUp, t, f, c, h, H");
	Init();
	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
}
