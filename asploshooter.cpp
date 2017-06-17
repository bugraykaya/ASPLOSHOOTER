#include "asploshooter.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#define PI 3.14159265

#define ASPLO_WIDTH 0.090
#define ASPLO_HEIGHT 0.030
#define POKI_WIDTH 0.15
#define POKI_HEIGHT 0.15
#define BULLET_WIDTH 0.04
#define BULLET_HEIGHT 0.04
#define HP_BAR_HEIGHT 0.02
#define HP_BAR_WIDTH 0.04

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    
    resize(900,900);
    display = new Display();
    setCentralWidget(display);
    display->parent = this;
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    
}
MainWindow::~MainWindow()
{
    delete display;
}

void Display::mainLoop()
{
 
    frameCounter++;
    QPoint mousePos = mapFromGlobal( QCursor::pos() );
    
    xmouse= mousePos.x();
    ymouse = height() - mousePos.y();
    realmousex = xmouse / (width() / 2) -1 ;
    realmousey = ymouse / (height() / 2) -1 ;
    
    int size= enemies.size();
    
    for(int i=0; i< size; i++)
    {
        float distToPoki = sqrt((poki->x - enemies[i]->x) *(poki->x - enemies[i]->x) + (poki->y - enemies[i]->y)*(poki->y - enemies[i]->y));
        
        if(distToPoki <= 0.15 )
        {
            if(frameCounter - enemies[i]->lastAttackFrame >= 60)
            {
                enemies[i]->lastAttackFrame = frameCounter;
                poki->hp -= 10;
                
                if(poki->hp== 0)
                {
                    for(int k=0 ; k<enemies.size(); k++ )
                    {
                        delete enemies[k];
                    }
                    enemies.clear();
                    
                    for(int k=0 ; k<bullets.size(); k++ )
                    {
                        delete bullets[k];
                    }
                    bullets.clear();
                    
                    running = false;
                    disconnect(&timer, SIGNAL(timeout()),this, SLOT(mainLoop()) );
                    timer.stop();    
                    updateGL();
                    return;
                }
                
            }
        }
        else 
        {
            enemies[i]->x += (poki->x - enemies[i]->x)/(distToPoki*500);
            enemies[i]->y += (poki->y - enemies[i]->y)/(distToPoki*500);
        }
        
    }
    
    
    for(int i=0; i<bullets.size(); i++)
    {
        
        
        bullets[i]->x += bullets[i]->xSpeed;
        bullets[i]->y += bullets[i]->ySpeed;
        
        if(bullets[i]->x > 1.5 ||bullets[i]->x < -1.5 ||bullets[i]->y > 1.5 ||bullets[i]->y < -1.5    )
        {
            delete bullets[i];
            bullets.erase(bullets.begin()+i);
            i--;
        }
        
        else
        {
            for(int j=0 ; j< enemies.size(); j++)
            {
                float distToEnemy = sqrt(pow (bullets[i]->x - enemies[j]->x ,2 ) +pow (bullets[i]->y - enemies[j]->y ,2 ));  
                
                if(distToEnemy < 0.1)
                {
                    
                    enemies[j]->hp -= 20;
                    if(enemies[j]->hp <= 0)
                    {
                        delete enemies[j];
                        enemies.erase(enemies.begin()+j);
                    }
                    
                    delete bullets[i];
                    bullets.erase(bullets.begin()+i);
                    i--;
                    break;
                    
                }
            }
        }
    }
    
    
    if(KEY_W_DOWN)
    {
        poki->y+=0.01;
    }
    if(KEY_A_DOWN)
    {
        poki->x-=0.01;
    }
    if(KEY_S_DOWN)
    {
        poki->y-=0.01;
    }
    if(KEY_D_DOWN)
    {
        poki->x+=0.01;
    }
    
    if(frameCounter %13 == 0 && MOUSE_DOWN )
    {
        float rad = atan2(realmousex- poki->x  , realmousey -poki->y );
        float deg = (rad/PI)*180;
        
        float distToMouse = sqrt(pow (realmousex- poki->x ,2 ) +pow (realmousey- poki->y ,2 ));  
        
        
        bulletSound->play();
        Bullet *bullet = new Bullet();
        
        bullet->xSpeed =( realmousex- poki->x)/(distToMouse*50);
        bullet->ySpeed =( realmousey- poki->y)/(distToMouse*50);
        bullet->x = poki->x;
        bullet->y = poki->y;
        bullet->deg = deg;
        bullets.push_back(bullet);
    }
    
    if(frameCounter % 60==0 )
    {
        Asplosions *newAsplo;
        int edge = rand()%4;
        switch(edge)
        {
            case 0://top
            newAsplo = new Asplosions( ((float)(rand()%2000))/1000 -1 , 1.5 );
            break;
            case 1://bottom
            newAsplo = new Asplosions( ((float)(rand()%2000))/1000 -1 ,-1.5 );
            break;
            case 2://left
            newAsplo = new Asplosions( -1.5 ,((float)(rand()%2000))/1000 -1 );
            break;
            case 3://right
            newAsplo = new Asplosions(  1.5 ,((float)(rand()%2000))/1000 -1 );
            break;
            
            
            
        }
        enemies.push_back(newAsplo);
        
    }
    
    updateGL();
    
}

void Display::drawBullet(float x, float y, float deg)
{    
    glLoadIdentity();
    glTranslatef(x,y,0);
    glRotatef(deg,0,0,-1);
    
    glBindTexture(GL_TEXTURE_2D, bulletTexture);
    glBindBuffer(GL_ARRAY_BUFFER, asploVBO);
    glVertexPointer(3, GL_FLOAT, 0, (void*)bulletOffsetInBytes);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCOffsetInBytes);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void Display::drawAsplo(float x, float y)
{
    
    float rad = atan2(poki->x -x , poki->y -y );
    float deg = (rad/PI)*180;
    
    
    glLoadIdentity();
    glTranslatef(x,y,0);
    glRotatef(deg,0,0,-1);
    
    glBindTexture(GL_TEXTURE_2D, asploTexture);
    glBindBuffer(GL_ARRAY_BUFFER, asploVBO);
    glVertexPointer(3, GL_FLOAT, 0, (void*)asploOffsetInBytes);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCOffsetInBytes);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void Display::drawPoki(float x, float y)
{
    
    float rad = atan2(realmousey -y ,realmousex -x);
    float deg = (rad/PI)*180;
    
    glLoadIdentity();
    
    glTranslatef(x,y,0);    
    glRotatef(deg,0,0,1);
    
    glBindTexture(GL_TEXTURE_2D, pokiTexture);   
    glBindBuffer(GL_ARRAY_BUFFER, asploVBO);
    glVertexPointer(3, GL_FLOAT, 0, (void*)pokiOffsetInBytes);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCOffsetInBytes);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
}

void Display::drawHpBar()
{
    glLoadIdentity();
    glTranslatef(-0.98+HP_BAR_WIDTH, 0.98-HP_BAR_HEIGHT,0);
    
    glBindTexture(GL_TEXTURE_2D, hpBarTexture);
    glVertexPointer(3, GL_FLOAT, 0, (void*)hpBarOffsetInBytes);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCOffsetInBytes);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    for(int i=10; i<poki->hp; i+=10)
    {
        glTranslatef(HP_BAR_WIDTH+0.04,0,0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Display::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0,0,this->width(),this->height());
    
    if(running){
        
        drawPoki(poki->x , poki->y);
        
        
        int size= enemies.size();
        for(int i=0 ; i< size; i++)
        {
            drawAsplo(enemies[i]->x,enemies[i]->y);
        }
        
        size = bullets.size();
        for(int i=0 ; i< size; i++)
        {
            drawBullet(bullets[i]->x,bullets[i]->y, bullets[i]->deg );
        }
        drawHpBar();
    }
    else {
        
        glLoadIdentity();
        glScalef(1/POKI_WIDTH,1/POKI_HEIGHT,1  );
        
        glBindTexture(GL_TEXTURE_2D, menuTexture);
        glVertexPointer(3, GL_FLOAT, 0, (void*)pokiOffsetInBytes);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCOffsetInBytes);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
    }
}

void Display::initializeGL()
{
    glewInit();
    
    glClearColor(1,0,1,0);
    
    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    std::cout << "Renderer: "<<renderer<< "\n";
    std::cout << "OpenGL version supported: "<<version<< "\n";
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_BLEND); 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glPointSize(10);
    glGenTextures(1, &asploTexture);
    glBindTexture(GL_TEXTURE_2D, asploTexture);
    uchar *bits= asploImage->bits();
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, asploWidth, asploHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenTextures(1, &pokiTexture);
    glBindTexture(GL_TEXTURE_2D, pokiTexture);
    bits= pokiImage->bits();
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, pokiWidth, pokiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenTextures(1, &bulletTexture);
    glBindTexture(GL_TEXTURE_2D, bulletTexture);
    bits= bulletImage->bits();
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, bulletWidth, bulletHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenTextures(1, &hpBarTexture);
    glBindTexture(GL_TEXTURE_2D, hpBarTexture);
    bits= hpBarImage->bits();
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, hpBarWidth, hpBarHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glGenTextures(1, &menuTexture);
    glBindTexture(GL_TEXTURE_2D, menuTexture);
    bits= menuImage->bits();
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, menuWidth, menuHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    
    glGenBuffers(1, &asploVBO);
    
    
    float data[] = {
        -POKI_WIDTH, POKI_HEIGHT, -4,
        -POKI_WIDTH, -POKI_HEIGHT,-4 ,
        POKI_WIDTH ,-POKI_HEIGHT,-4,
        POKI_WIDTH, POKI_HEIGHT,-4,
        POKI_WIDTH, -POKI_HEIGHT,-4,
        -POKI_WIDTH, POKI_HEIGHT,-4,
        
        -ASPLO_WIDTH, ASPLO_HEIGHT, -4 , //asplo vertices
        -ASPLO_WIDTH, -ASPLO_HEIGHT,-4, 
        ASPLO_WIDTH, -ASPLO_HEIGHT,-4 ,
        ASPLO_WIDTH, ASPLO_HEIGHT,-4 , 
        ASPLO_WIDTH, -ASPLO_HEIGHT,-4 ,
        -ASPLO_WIDTH, ASPLO_HEIGHT, -4,
        
        -BULLET_WIDTH, BULLET_HEIGHT, -4 , //bullet vertices
        -BULLET_WIDTH, -BULLET_HEIGHT,-4, 
        BULLET_WIDTH, -BULLET_HEIGHT,-4 ,
        BULLET_WIDTH, BULLET_HEIGHT,-4 , 
        BULLET_WIDTH, -BULLET_HEIGHT,-4 ,
        -BULLET_WIDTH, BULLET_HEIGHT, -4,        
        
        -HP_BAR_WIDTH, HP_BAR_HEIGHT, -4 , //HP bar vertices
        -HP_BAR_WIDTH, -HP_BAR_HEIGHT,-4, 
        HP_BAR_WIDTH, -HP_BAR_HEIGHT,-4 ,
        HP_BAR_WIDTH, HP_BAR_HEIGHT,-4 , 
        HP_BAR_WIDTH, -HP_BAR_HEIGHT,-4 ,
        -HP_BAR_WIDTH, HP_BAR_HEIGHT, -4,        
        
        0,0,0,1,1,1,1,0,1,1,0,0//Texture coords
    };
    
    pokiOffsetInBytes = 0;
    asploOffsetInBytes = 18*sizeof(float);
    bulletOffsetInBytes = 36*sizeof(float);
    hpBarOffsetInBytes = 54*sizeof(float);
    texCOffsetInBytes = 72*sizeof(float);
    
    glBindBuffer(GL_ARRAY_BUFFER, asploVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,
            1,
            -1,
            1,
            1,
            100000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    timer.start(16.666);    
    
    
    
}

void Display::keyPressEvent(QKeyEvent *event)
{
    
    switch(event->key())
    {
        case Qt::Key_W:
        KEY_W_DOWN = true;
        break;
        
        case Qt::Key_A:
        KEY_A_DOWN = true;
        break;
        
        case Qt::Key_S:
        KEY_S_DOWN = true;
        break;
        
        case Qt::Key_D:
        KEY_D_DOWN = true;
        break;
        case Qt::Key_Return:
        if(!running)
        {
            running= true;
            poki->hp= 100;
            connect(&timer, SIGNAL(timeout()), this, SLOT(mainLoop()) );
            timer.start(16.666);    
            
        }
        break;
        default: 
        break;
    }
}
void Display::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_W:
        KEY_W_DOWN = false;
        break;
        
        case Qt::Key_A:
        KEY_A_DOWN = false;
        break;
        
        case Qt::Key_S:
        KEY_S_DOWN = false;
        break;
        
        case Qt::Key_D:
        KEY_D_DOWN = false;
        break;
        
        default: 
        break;
    }
    
}

void Display::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        MOUSE_DOWN = true;
    }
}

void Display::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        MOUSE_DOWN = false;
    }
}
    
Display::Display()
{
    KEY_W_DOWN = false;
    KEY_A_DOWN = false;
    KEY_S_DOWN = false;
    KEY_D_DOWN = false;
    MOUSE_DOWN = false;
    
    setFocusPolicy(Qt::StrongFocus);
    
    asploImage = new QImage("images/asplosions.png");
    asploWidth= asploImage->width();
    asploHeight= asploImage->height();
    asploImage->convertToFormat(QImage::Format_RGBA8888);
    
    pokiImage = new QImage("images/poki.png");
    pokiWidth= pokiImage->width();
    pokiHeight= pokiImage->height();
    pokiImage->convertToFormat(QImage::Format_RGBA8888);
    
    bulletImage = new QImage("images/bullet.png");
    bulletWidth= bulletImage->width();
    bulletHeight= bulletImage->height();
    bulletImage->convertToFormat(QImage::Format_RGBA8888);
    
    hpBarImage = new QImage("images/hp.png");
    hpBarWidth= hpBarImage->width();
    hpBarHeight= hpBarImage->height();
    hpBarImage->convertToFormat(QImage::Format_RGBA8888);
    
    menuImage = new QImage("images/menu.png");
    menuWidth= menuImage->width();
    menuHeight= menuImage->height();
    menuImage->convertToFormat(QImage::Format_RGBA8888);
    
    
    poki = new Actor();
    
    connect(&timer, SIGNAL(timeout()), this, SLOT(mainLoop()) );
    
    
    
    bulletSound= new QSound("sounds/laser.wav");
    
    
}

Display::~Display()
{
    
}
