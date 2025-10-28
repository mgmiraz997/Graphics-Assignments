//+ = Add a new circle
//- = Remove a circle
//c = Changes gradient mode
//r = Reset everything
// ESC = Exit program


#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Window dimensions
int winW = 800, winH = 600;

// Circle parameters
int numCircles = 20;
int startRadius = 30;
int gap = 10;
int gradientMode = 0; // 0 = RGB, 1 = Blue->Green ->Yellow, 2 = Purple->Pink->White

// ----------- CIRCLE DRAWING ALGORITHMS ----------- //

// Plotting 8 symmetric points for circle
void plotCirclePoints(int xc, int yc, int x, int y) {
    glBegin(GL_POINTS);
        glVertex2i(xc + x, yc + y);
        glVertex2i(xc - x, yc + y);
        glVertex2i(xc + x, yc - y);
        glVertex2i(xc - x, yc - y);
        glVertex2i(xc + y, yc + x);
        glVertex2i(xc - y, yc + x);
        glVertex2i(xc + y, yc - x);
        glVertex2i(xc - y, yc - x);
    glEnd();
}

// Midpoint Circle Drawing Algorithm with thickness
void drawCircleMidpoint(int xc, int yc, int radius, int thickness) {
    for (int t = 0; t < thickness; ++t) {
        int x = 0;
        int y = radius + t;
        int d = 1 - y;

        while (x <= y) {
            plotCirclePoints(xc, yc, x, y);
            if (d < 0)
                d += 2 * x + 3;
            else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }
}

// ----------- COLOR GRADIENT FUNCTIONS ----------- //
void setGradientColor(int i, int numCircles) {
    float t = (float)i / numCircles;

    switch (gradientMode) {
        case 0: // Rainbow (Blue -> Cyan -> Green -> Yellow -> Red)
        {
            float r = t;
            float g = 1.0f - fabs(0.5f - r) * 2.0f;
            float b = 1.0f - r;
            glColor3f(r, g, b);
            break;
        }
        case 1: // Blue -> Green > Yellow
        {
            float r = t * 1.2f;
            float g = t;
            float b = 1.0f - t;
            glColor3f(r, g, b);
            break;
        }
        case 2: // Purple -> Pink -> White
        {
            float r = 0.6f + t * 0.4f;
            float g = t * 0.5f + 0.3f;
            float b = 0.8f + t * 0.2f;
            glColor3f(r, g, b);
            break;
        }
    }
}

// ----------- DRAWING MULTIPLE CIRCLES ----------- //
void drawConcentricCircles(int xc, int yc, int numCircles, int startRadius, int gap) {
    for (int i = 0; i < numCircles; ++i) {
        int radius = startRadius + i * gap;
        int thickness = 1 + i / 3;

        setGradientColor(i, numCircles);
        drawCircleMidpoint(xc, yc, radius, thickness);
    }
}

// ----------- DISPLAY FUNCTION----------- //
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    int centerX = winW / 2;
    int centerY = winH / 2;

    drawConcentricCircles(centerX, centerY, numCircles, startRadius, gap);

    glFlush();
}

void reshape(int w, int h) {
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard control
void keyboard(unsigned char key, int, int) {
    switch (key) {
        case '+':
        case '=':
            numCircles = std::min(100, numCircles + 1);
            break;
        case '-':
            numCircles = std::max(1, numCircles - 1);
            break;
        case 'c':
        case 'C':
            gradientMode = (gradientMode + 1) % 3;
            std::cout << "Gradient mode changed to " << gradientMode << std::endl;
            break;
        case 'r':
        case 'R':
            numCircles = 20;
            gradientMode = 0;
            startRadius = 30;
            gap = 10;
            std::cout << "Reset to default.\n";
            break;
        case 27: // ESC key
            exit(0);
    }
    glutPostRedisplay();
}

void initGL() {
    glClearColor(0, 0, 0, 1);
    glColor3f(1, 1, 1);
    glPointSize(2.0f);
}

// ----------- MAIN FUNCTION ----------- //
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Interactive Concentric Circles with Color Gradient");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "=== Concentric Circle Controls ===\n";
    std::cout << "+ : Increase number of circles\n";
    std::cout << "- : Decrease number of circles\n";
    std::cout << "c : Change color gradient style\n";
    std::cout << "r : Reset to default\n";
    std::cout << "ESC : Exit program\n";

    glutMainLoop();
    return 0;
}
