#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <iterator>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

double kvadrouravX(double y, int vpravo) {
	double x1 = (10. + std::sqrt(100. - 4. * ( y * y - 2 * y + 21))) / 2.;
	double x2 = (10. - std::sqrt(100. - 4. * ( y * y - 2 * y + 21))) / 2.;
	if (vpravo == 1 && x1 >= 5)
		return x1;
	else return x2;
}

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

Vector3 getNormal(double* start, double* end1, double* end2)
{
	Vector3 a = Vector3(start[0] - end1[0], start[1] - end1[1], start[2] - end1[2]);
	Vector3 b = Vector3(start[0] - end2[0], start[1] - end2[1], start[2] - end2[2]);
	Vector3 normal = Vector3(a.Y() * b.Z() - b.Y() * a.Z(), -a.X() * b.Z() + b.X() * a.Z(), a.X() * b.Y() - a.Y() * b.X());
	return normal;
}

std::vector<Vector3> GetRoundPoint(Vector3 start, Vector3 end) {
	Vector3 mid = Vector3((start.X() + end.X())/2, (start.Y()+end.Y())/2, start.Z());
	double radius = sqrt(pow(mid.X() - start.X(), 2) + pow(mid.Y() - start.Y(), 2));
	std::vector<Vector3> points;
	Vector3 point(start.X(), start.Y(), start.Z());
	points.push_back(point);
	double i = (acos((mid.X() * end.X() + mid.Y() * end.Y()) / (sqrt(mid.X() * mid.X() + mid.Y() * mid.Y()) * sqrt(end.X() * end.X() + end.Y() * end.Y())))) * 180 / PI - 0.45;
	double startI = i;
	while (point.X() != end.X() && point.Y() != end.Y() && i< PI + startI) {
		point.setCoords(radius * cos(i) + mid.X(), radius * sin(i) + mid.Y(), start.Z());
		points.push_back(point);
		i += 0.001;
	}
	return points;
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	double A[3]{ 0,6,0 };
	double B[3]{ 8,7 ,0};
	double C[3]{ 5, 13 ,0};
	double D[3]{ 11,11 ,0};
	double E[3]{ 11,7 ,0};
	double F[3]{ 17,4 ,0};
	double G[3]{ 10, 5 ,0};
	double H[3]{ 7,0 ,0};

	double A1[3]{ 0,6,3 };
	double B1[3]{ 8,7 ,3 };
	double C1[3]{ 5, 13 ,3 };
	double D1[3]{ 11,11,3 };
	double E1[3]{ 11,7 ,3 };
	double F1[3]{ 17,4 ,3 };
	double G1[3]{ 10,5 ,3 };
	double H1[3]{ 7,0 ,3 };
	Vector3 normal;
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	//glColor3d(0.2, 0.7, 0.8); �����
	// glColor3d(1.0, 1.0, 0.0); ������
	glColor3d(0.5, 0.2, 0.0);
	normal = getNormal(A, A1, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glTexCoord2d(1, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	normal = getNormal(B, B1, C);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(B1);
	normal = getNormal(C, C1, D);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(C1);
	normal = getNormal(D, D1, E);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(D1);
	normal = getNormal(E, E1, F);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(E1);
	normal = getNormal(F, F1, G);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(G1);
	glVertex3dv(F1);
	normal = getNormal(G, G1, H);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(H1);
	glVertex3dv(G1);
	normal = getNormal(H, H1, A);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(H1);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3d(0.2, 0.7, 0.7);
	normal = getNormal(A, B, H);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(H);

	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);

	glVertex3dv(B);
	glVertex3dv(D);
	glVertex3dv(E);

	glVertex3dv(B);
	glVertex3dv(E);
	glVertex3dv(F);

	glVertex3dv(B);
	glVertex3dv(G);
	glVertex3dv(F);

	glVertex3dv(B);
	glVertex3dv(G);
	glVertex3dv(H);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3d(0.2, 0.7, 0.7);
	normal = getNormal(A1, H1, B1);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(H1);

	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);

	glVertex3dv(B1);
	glVertex3dv(D1);
	glVertex3dv(E1);

	glVertex3dv(B1);
	glVertex3dv(E1);
	glVertex3dv(F1);

	glVertex3dv(B1);
	glVertex3dv(G1);
	glVertex3dv(F1);

	glVertex3dv(B1);
	glVertex3dv(G1);
	glVertex3dv(H1);


	glEnd();
	std::vector<Vector3>krug;
	std::vector<Vector3>krug2;


		double Midl[3]{ 3.5, 3 , 0};
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLE_FAN);
		{
			glColor3d(0, 1, 1);
			glNormal3d(0, 0, -1);
			glVertex3dv(Midl);
			Vector3 start(A[0], A[1], A[2]);
			Vector3 end(H[0], H[1], H[2]);
			krug = GetRoundPoint(start, end);
			//krug.push_back(Last);
			for (int i = 0; i < krug.size(); i++) {
				Vector3 point = krug[i];
				glVertex3d(point.X(), point.Y(), point.Z());
			}
		}
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		{
			glColor3d(0, 1, 1);
			glNormal3d(0, 0, 1);
			Midl[2] = 3;
			glVertex3dv(Midl);
			Vector3 start(A1[0], A1[1], A1[2]);
			Vector3 end(H1[0], H1[1], H1[2]);
			krug2 = GetRoundPoint(start, end);
			for (int i = 0; i < krug2.size(); i++) {
				Vector3 point = krug2[i];
				glVertex3d(point.X(), point.Y(), point.Z());
			}
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			glColor3d(1, 0, 1);
			for (int i = 1; i < krug.size(); i++)
			{
				double pointOld[3] = {krug[i-1].X(),krug[i-1].Y(),krug[i - 1].Z() };
				double pointOld1[3] = { krug2[i - 1].X(),krug2[i - 1].Y(), krug2[i - 1].Z()};
				double point[3] = { krug[i].X(),krug[i].Y(),krug[i].Z() };
				double point1[3] = { krug2[i].X(),krug2[i].Y(), krug2[i].Z() };
				Vector3 normal;
				normal = getNormal(point1, pointOld1, pointOld);
				glNormal3d(normal.X(), normal.Y(), normal.Z());

				glVertex3dv(point1);
				glVertex3dv(pointOld1);
				glVertex3dv(pointOld);
				glVertex3dv(point);
			}
		}
		glEnd();

   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}