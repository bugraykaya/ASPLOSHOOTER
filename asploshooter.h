/*
Contains class declarations for the main window and OpenGL widget classes. 
*/

#ifndef ASPLOSHOOTER_H
#define ASPLOSHOOTER_H

#include <GL/glew.h>
#include <QMainWindow>
#include <QGLWidget>
#include <string>
#include <QTimer>
#include <QTime>
#include <QKeyEvent>
#include <QSound>
#include <QPainter>

class MainWindow;
class Poki;
class Asplosions;
class Actor;
class Bullet;

#define KEY_W_DOWN pressed[0]
#define KEY_A_DOWN pressed[1]
#define KEY_S_DOWN pressed[2]
#define KEY_D_DOWN pressed[3]
#define MOUSE_DOWN pressed[4]

class Display: public QGLWidget
{
    Q_OBJECT
    
    public:
    Display();
    ~Display();
    
    
    void drawAsplo(float x, float y);
    void drawPoki(float x, float y);
    void drawBullet(float x, float y, float deg);
    void drawHpBar();
    
    MainWindow *parent;
    
    
    private slots:
    
    void mainLoop();
    
    protected:
    
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintGL();
    void initializeGL();
    
    int frameCounter=0;
    QTime time;
    QTimer timer;
    QSound *bulletSound;
    QPainter painter;
    
    GLuint asploVBO;
    GLuint pokiVBO;
    GLuint texCoordVBO;
    GLuint colorVBO;
    GLuint elementVBO;
    
    long pokiOffsetInBytes;
    long asploOffsetInBytes;
    long texCOffsetInBytes;
    long bulletOffsetInBytes;
    long hpBarOffsetInBytes;
    
    QImage *asploImage;
    int asploWidth, asploHeight;
    GLuint asploTexture;
    
    QImage *pokiImage;
    int pokiWidth, pokiHeight;
    GLuint pokiTexture;
    
    QImage *bulletImage;
    int bulletWidth, bulletHeight;
    GLuint bulletTexture;
    
    QImage *hpBarImage;
    int hpBarWidth, hpBarHeight;
    GLuint hpBarTexture;
    
    QImage *menuImage;
    int menuWidth, menuHeight;
    GLuint menuTexture;
    
    std::vector<Asplosions*> enemies;
    std::vector<Bullet*> bullets;
    
    Actor *poki;
    
    float xmouse, ymouse, realmousex, realmousey;

    bool running = true;
    bool pressed[10];
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Display *display;
};


class Actor
{
    public:
    Actor( float _x=0, float _y=0): x(_x),y(_y) 
    {
        hp = 100;
    }
    
    
    ~Actor(){}
    float x, y;
    int hp;
};

class Asplosions: public Actor
{
    public: 
    Asplosions(float _x=0 , float _y= 0) : Actor(_x , _y)
    {
        hp=60;
    }
    ~Asplosions(){}
    size_t lastAttackFrame=0;
    
};


class Bullet: public Actor
{
public: 
    Bullet(float _x=0 , float _y= 0) : Actor(_x , _y)
    {
        
    }
    ~Bullet(){}
    
    float deg, xSpeed, ySpeed;
};

#endif // MAINWINDOW_H

