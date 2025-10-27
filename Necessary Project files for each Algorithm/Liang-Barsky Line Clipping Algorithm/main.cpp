// Left Click twice= Draw a line
// Arrow keys= Move clipping window
// [ / ]= Resize clipping window
//  C= Clear all lines
//  R= Reset window to default
//  ESC= exit program

#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ---------- Clipping window boundaries ----------
float xmin = 200, ymin = 150, xmax = 600, ymax = 450;
float winMoveStep = 10;
float winResizeStep = 10;

// ---------- Line Structure ----------
struct Line {
    float x1, y1, x2, y2;
};
vector<Line> lines;

// ---------- Mouse Clicks ----------
bool firstClick = true;
float xStart, yStart;

// ---------- Drawing a Rectangle which is my clipping window ----------
void drawRect(float xmin, float ymin, float xmax, float ymax) {
    glColor3f(1, 1, 1); // white color
    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin, ymin);
    glVertex2f(xmax, ymin);
    glVertex2f(xmax, ymax);
    glVertex2f(xmin, ymax);
    glEnd();
}

// ---------- Drawing Line ----------
void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

// ---------- Liang–Barsky Algorithm ----------
bool liangBarskyClip(float x1, float y1, float x2, float y2,
                     float &cx1, float &cy1, float &cx2, float &cy2)
{
    float dx = x2 - x1, dy = y2 - y1;
    float p[4] = {-dx, dx, -dy, dy};
    float q[4] = {x1 - xmin, xmax - x1, y1 - ymin, ymax - y1};
    float tE = 0.0f, tL = 1.0f;

    for (int i = 0; i < 4; i++) {
        if (p[i] == 0) {
            if (q[i] < 0) return false; // parallel & outside
        } else {
            float t = q[i] / p[i];
            if (p[i] < 0)
                tE = max(tE, t); // I am entering
            else
                tL = min(tL, t); // I am leaving
        }
    }

    if (tE > tL) return false; // I am at outside

    cx1 = x1 + tE * dx;
    cy1 = y1 + tE * dy;
    cx2 = x1 + tL * dx;
    cy2 = y1 + tL * dy;
    return true;
}

// ---------- Display Function ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawRect(xmin, ymin, xmax, ymax); // clipping window

    for (auto &ln : lines) {
        // Drawing original line (red color)
        drawLine(ln.x1, ln.y1, ln.x2, ln.y2, 1, 0, 0);

        // My clip and drawing the visible portion (green)
        float cx1, cy1, cx2, cy2;
        if (liangBarskyClip(ln.x1, ln.y1, ln.x2, ln.y2, cx1, cy1, cx2, cy2)) {
            drawLine(cx1, cy1, cx2, cy2, 0, 1, 0);
        }
    }
    glFlush();
}

// ---------- Mouse Handler ----------
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = 600 - y; // invert Y for getting the OpenGL coordinates
        if (firstClick) {
            xStart = x; yStart = y;
            firstClick = false;
        } else {
            Line l = {xStart, yStart, (float)x, (float)y};
            lines.push_back(l);
            firstClick = true;
        }
        glutPostRedisplay();
    }
}

// ---------- Keyboard Handler ----------
void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'c':
        case 'C':
            lines.clear();
            std::cout << "Cleared all lines.\n";
            break;
        case 'r':
        case 'R':
            xmin = 200; ymin = 150; xmax = 600; ymax = 450;
            std::cout << "Window reset.\n";
            break;
        case '[': // chotto kortesi
            xmin += winResizeStep;
            ymin += winResizeStep;
            xmax -= winResizeStep;
            ymax -= winResizeStep;
            break;
        case ']': // expand
            xmin -= winResizeStep;
            ymin -= winResizeStep;
            xmax += winResizeStep;
            ymax += winResizeStep;
            break;
        case 27: // ESC
            exit(0);
    }
    glutPostRedisplay();
}

// ---------- Special Keys for Moving Window ----------
void specialKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:
            ymin += winMoveStep; ymax += winMoveStep;
            break;
        case GLUT_KEY_DOWN:
            ymin -= winMoveStep; ymax -= winMoveStep;
            break;
        case GLUT_KEY_LEFT:
            xmin -= winMoveStep; xmax -= winMoveStep;
            break;
        case GLUT_KEY_RIGHT:
            xmin += winMoveStep; xmax += winMoveStep;
            break;
    }
    glutPostRedisplay();
}

// ---------- Initialization ----------
void initGL() {
    glClearColor(0, 0, 0, 1); // black background of the window
    glColor3f(1, 1, 1);
    glLineWidth(2.0f);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ---------- MAIN ----------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Interactive Liang–Barsky Line Clipping");
    initGL();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);

    std::cout << "=== Liang–Barsky Interactive Visualization ===\n";
    std::cout << "Left click twice -> draw line\n";
    std::cout << "Arrow keys -> move window\n";
    std::cout << "[ / ] -> resize window\n";
    std::cout << "C -> clear lines, R -> reset window\n";
    std::cout << "ESC -> exit\n";
    std::cout << "Red = original lines, Green = clipped segments\n";

    glutMainLoop();
    return 0;
}
