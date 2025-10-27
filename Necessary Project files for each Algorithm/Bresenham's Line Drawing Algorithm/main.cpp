// Keys:
//   + / = : increase thickness
//   -     : decrease thickness
//   r     : toggle rounded brush (circle) for thickness
//   c     : clear / reset

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

int winW = 800, winH = 600;
struct Pt { int x, y; };
Pt p1 = {-1,-1}, p2 = {-1,-1};
bool p1set = false;
int lineWidth = 1;         // W
bool roundBrush = false;   // if true use filled circle brush; else filled square
std::vector<std::pair<Pt,Pt>> drawnLines; // store drawn lines along with width/round flag
std::vector<int> drawnWidths;
std::vector<char> drawnRoundFlags;

// setting a single pixel
void setPixel(int x, int y){
    glBegin(GL_POINTS);
      glVertex2i(x, y);
    glEnd();
}

// draw filled square centered at (cx,cy) with side size W
void drawSquareBrush(int cx, int cy, int W){
    int half = W / 2;
    for(int dy = -half; dy <= half; ++dy){
        for(int dx = -half; dx <= half; ++dx){
            setPixel(cx + dx, cy + dy);
        }
    }
}

// drawing filled circle centered at (cx,cy) with radius r
void drawCircleBrush(int cx, int cy, int r){
    // for each dy,we are calculating half-width
    for (int dy = -r; dy <= r; ++dy){
        int hh = (int)std::floor(std::sqrt((double)r*r - dy*dy));
        for (int dx = -hh; dx <= hh; ++dx){
            setPixel(cx + dx, cy + dy);
        }
    }
}

// Bresenham factor that returns the list of pixels(we'll draw brush per pixel)
void bresenhamPixels(int x1, int y1, int x2, int y2, std::vector<Pt> &outPixels){
    outPixels.clear();
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int x = x1;
    int y = y1;

    while(true){
        outPixels.push_back({x,y});
        if (x == x2 && y == y2) break;
        int e2 = 2*err;
        if (e2 > -dy){
            err -= dy;
            x += sx;
        }
        if (e2 < dx){
            err += dx;
            y += sy;
        }
    }
}

// Drawing a Bresenham line with thickness W and chosen brush style
void drawLineThick(int x1, int y1, int x2, int y2, int W, bool useRoundBrush){
    std::vector<Pt> pixels;
    bresenhamPixels(x1,y1,x2,y2,pixels);

    // For each Bresenham pixel,we are drawing brush centered at that pixel
    for (const Pt &p : pixels){
        if (useRoundBrush){
            drawCircleBrush(p.x, p.y, W/2);
        } else {
            drawSquareBrush(p.x, p.y, W);
        }
    }

    // Drawing for more continuous thick line:
    if (useRoundBrush){
        // Draw circle at endpoints (to make caps look round even when line short)
        drawCircleBrush(x1, y1, W/2);
        drawCircleBrush(x2, y2, W/2);
    } else {

    }
}

// My display function
void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f); // white points
    glPointSize(1.0f);

    // redrawing previously drawn lines
    for (size_t i=0; i<drawnLines.size(); ++i){
        Pt a = drawnLines[i].first;
        Pt b = drawnLines[i].second;
        int W = drawnWidths[i];
        bool rf = (drawnRoundFlags[i] != 0);
        drawLineThick(a.x,a.y,b.x,b.y,W,rf);
    }

    // if P1 set but P2 not set, showing a marker
    if (p1set){
        glColor3f(1.0f, 0.2f, 0.2f);
        drawSquareBrush(p1.x, p1.y, 5);
    }
    // if both set is showing the live line
    if (p1set && p2.x!=-1){
        glColor3f(0.2f, 1.0f, 0.2f);
        drawLineThick(p1.x, p1.y, p2.x, p2.y, lineWidth, roundBrush);
    }

    glFlush();
}

// mouse handler -> left click -> setting P1 and P2
void mouse(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        int mx = x;
        int my = winH - y; // converting orthographic coordinates (origin bottom-left)
        if (!p1set){
            p1.x = mx; p1.y = my; p1set = true;
            p2 = {-1,-1};
        } else {
            // second click setting p2 and storing the drawn line
            p2.x = mx; p2.y = my;
            drawnLines.push_back({p1,p2});
            drawnWidths.push_back(lineWidth);
            drawnRoundFlags.push_back(roundBrush ? 1 : 0);
            // resetting p1 to allow set of new pair
            p1set = false;
            p1 = {-1,-1}; p2 = {-1,-1};
        }
        glutPostRedisplay();
    }
}

// mouse motion -> showing live preview line while dragging with left button pressed
void motion(int x, int y){
    if (p1set){
        p2.x = x;
        p2.y = winH - y;
        glutPostRedisplay();
    }
}

// keyboard controls
void keyboard(unsigned char key, int x, int y){
    if (key == '+' || key == '=' ){
        lineWidth = std::min(100, lineWidth + 1);
        std::cout << "Width: " << lineWidth << "\n";
    } else if (key == '-'){
        lineWidth = std::max(1, lineWidth - 1);
        std::cout << "Width: " << lineWidth << "\n";
    } else if (key == 'r' || key == 'R'){
        roundBrush = !roundBrush;
        std::cout << "Round brush: " << (roundBrush ? "ON" : "OFF") << "\n";
    } else if (key == 'c' || key == 'C'){
        drawnLines.clear();
        drawnWidths.clear();
        drawnRoundFlags.clear();
        p1set = false; p1 = {-1,-1}; p2 = {-1,-1};
        std::cout << "Cleared.\n";
    }
    glutPostRedisplay();
}

// reshape function
void reshape(int w, int h){
    winW = w; winH = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); // coordinate system matches window pixels, origin bottom-left
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void initGL(){
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glColor3f(1.0f,1.0f,1.0f);
    glPointSize(1.0f);
    glEnable(GL_POINT_SMOOTH); // optional
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Bresenham Line Drawing - Thick Lines Demo");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    std::cout << "Left-click to set P1 and P2.\n"
              << "+ / = : increase thickness\n"
              << "-     : decrease thickness\n"
              << "r     : toggle rounded brush\n"
              << "c     : clear\n";

    glutMainLoop();
    return 0;
}
