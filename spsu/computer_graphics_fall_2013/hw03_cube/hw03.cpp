#include <GL/glut.h>

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_POLYGON);
  glColor3f(0.5, 0.5, 0.5);
  glVertex3f(0, 1, 0);
  glVertex3f(0, 1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(1, 1, 0);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 1);
  glVertex3f(1, 0, 1);
  glVertex3f(1, 0, 0);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3f(1, 1, 0);
  glVertex3f(1, 1, 1);
  glVertex3f(1, 0, 1);
  glVertex3f(1, 0, 0);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0, 1, 0);
  glVertex3f(0, 1, 1);
  glVertex3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0, 1, 1);
  glVertex3f(0, 0, 1);
  glVertex3f(1, 0, 1);
  glVertex3f(1, 1, 1);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(1, 0, 0);
  glVertex3f(1, 1, 0);
  glEnd();

  glFlush();
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Cube -- A. Mikhaylova");
  glEnable(GL_DEPTH_TEST);

  glutDisplayFunc(display);

  glMatrixMode(GL_PROJECTION);
  gluPerspective(30.0, 1.0, 1.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.5, 0.5, 10.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

  glTranslatef(-0.5, -0.5, 0.0);

  glRotatef(20, 1.0, 0.0, 0.0);
  glRotatef(20, 0.0, 1.0, 0.0);

  glutMainLoop();

  return 0;
}
