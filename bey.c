#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>   // time()

#define NUM_BEYBLADES 2  // ベイブレードの数
#define RADIUS 1.3f      // ベイブレードの半径
#define FIELD_RADIUS 10.0f // フィールドの半径
#define M_PI 3.1415926535

typedef struct {
    float x, y, z;      // 位置
    float vx, vy, vz;   // 速度
    float angle;        // 回転角度
    float rotation_speed; // 回転速度
    float color[3];     // 色
    float damping;      // 速度の減衰率
} Beyblade;

Beyblade beyblades[NUM_BEYBLADES];

float camDistance = 20.0f, twist = 0.0f, elevation = -30.0f, azimuth = 30.0f;
int xBegin = 0, yBegin = 0;
int mButton;
int initialized = 0; // 初期化フラグ

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // 乱数の初期化
    srand(time(NULL));

    // ベイブレードの初期化
    for (int i = 0; i < NUM_BEYBLADES; i++) {
        // ランダムな速度と角度を設定
        beyblades[i].angle = 0.0f; // 初期の回転角度は0
        beyblades[i].rotation_speed = 10.0f; // 初期の回転速度
        float speed = 0.01f + ((float)rand() / RAND_MAX) * 0.12f; // 0.01 から 0.12 の範囲でランダムな速度
        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI; // 0 から 2π の範囲でランダムな角度
        beyblades[i].vx = speed * cosf(angle); // x 方向の速度成分
        beyblades[i].vy = 0.0f; // y 方向の速度成分（この例ではゼロ）
        beyblades[i].vz = speed * sinf(angle); // z 方向の速度成分
        beyblades[i].damping = 0.99999999999f; // 速度の減衰率を設定

        // 初期位置の設定
        beyblades[i].x = ((float)rand() / RAND_MAX) * (2 * FIELD_RADIUS * 0.3) - FIELD_RADIUS * 0.3; // -FIELD_RADIUS から FIELD_RADIUS の範囲でランダムな x 座標
        beyblades[i].y = FIELD_RADIUS + RADIUS; // フィールドの上端に配置
        beyblades[i].z = ((float)rand() / RAND_MAX) * (2 * FIELD_RADIUS * 0.3) - FIELD_RADIUS * 0.3; // -FIELD_RADIUS から FIELD_RADIUS の範囲でランダムな z 座標

        // ランダムな色を設定
        beyblades[i].color[0] = ((float)rand() / RAND_MAX); // 0.0 から 1.0 の範囲でランダムな色成分 R
        beyblades[i].color[1] = ((float)rand() / RAND_MAX); // 0.0 から 1.0 の範囲でランダムな色成分 G
        beyblades[i].color[2] = ((float)rand() / RAND_MAX); // 0.0 から 1.0 の範囲でランダムな色成分 B
    }
    initialized = 1;
}
void idle() {
    for (int i = 0; i < NUM_BEYBLADES; i++) {
        // 位置を更新
        if (beyblades[i].angle != 0 && beyblades[i].rotation_speed != 0) {
            beyblades[i].x += beyblades[i].vx;
            beyblades[i].z += beyblades[i].vz;
            beyblades[i].y -= 0.05f;
        }

        // ベイブレードがある一定の速度まで落ちたときに停止
        if (fabs(beyblades[i].vx) < 0.003) {
            beyblades[i].vx = 0;
            if (beyblades[i].vx == 0 && beyblades[i].vz == 0) {
                beyblades[i].rotation_speed = 0;
                beyblades[i].angle = 0;
            }
        }
        if (fabs(beyblades[i].vz) < 0.003) {
            beyblades[i].vz = 0;
            if (beyblades[i].vx == 0 && beyblades[i].vz == 0) {
                beyblades[i].angle = 0;
                beyblades[i].rotation_speed = 0;
            }
        }
        // フィールドに着地
        if (beyblades[i].y <= -0.7f) {
            beyblades[i].y = -0.7f;
            beyblades[i].vy = 0.0f; // y方向の速度をゼロにする
        }

        // フィールド外に出たら反転させる
        if (sqrt(beyblades[i].x * beyblades[i].x + beyblades[i].z * beyblades[i].z) > FIELD_RADIUS - RADIUS) {
            beyblades[i].x += -0.001 * beyblades[i].x;// 多段ヒットを起こさないための調整
            beyblades[i].z += -0.001 * beyblades[i].z;// 多段ヒットを起こさないための調整
            beyblades[i].vx *= -0.95f; // x方向の速度を反転かつ減衰
            beyblades[i].vz *= -0.95f; // z方向の速度を反転かつ減衰
            beyblades[i].rotation_speed *= 0.8f; // 回転速度の減衰
        }

        // 回転角度の更新
        beyblades[i].angle += beyblades[i].rotation_speed;
        if (beyblades[i].angle > 360) {
            beyblades[i].angle -= 360;
        }
        //ベイブレード同士の反発
        for (int j = i + 1; j < NUM_BEYBLADES; j++) {
            if (beyblades[i].y != -0.7f) {
                break;//空中でぶつかって停止しないように
            }
            float dx = beyblades[i].x - beyblades[j].x;
            float dz = beyblades[i].z - beyblades[j].z;
            float distance = sqrt(dx * dx + dz * dz);//二点間の距離

            if (distance < 2 * RADIUS) {
                // 反発
                float nx = dx / distance;
                float nz = dz / distance;

                beyblades[i].vx += nx * 0.01f;
                beyblades[i].vz += nz * 0.01f;
                beyblades[j].vx -= nx * 0.01f;
                beyblades[j].vz -= nz * 0.01f;
                beyblades[i].vx *= 0.95f;//減衰
                beyblades[i].vz *= 0.95f;
                beyblades[j].vx *= 0.95f;
                beyblades[j].vz *= 0.95f;
            }
        }
    }

    glutPostRedisplay();
}

void polarview(void) {
    glTranslatef(0.0, 0.0, -camDistance);
    glRotatef(-twist, 0.0, 0.0, 1.0);
    glRotatef(-elevation, 1.0, 0.0, 0.0);
    glRotatef(-azimuth, 0.0, 1.0, 0.0);
}

//スタジアムの描画
void drawCircle(float radius, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        // 法線ベクトルを設定
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        // 法線ベクトルを設定
        glNormal3f(x, 0.0f, z);
        glVertex3f(x, 0.0f, z);    // 下の点
        glVertex3f(1.09 * x, 1.0f, 1.09 * z);    // 上の点
    }
    glEnd();
}
//ベイブレードの描画
void drawBeyblade(Beyblade* b) {
    glPushMatrix();
    glTranslatef(b->x, b->y, b->z);//初期位置
    glTranslatef(0, 2, 0);
    glRotatef(90, 1.0, 0, 0.0);
    glScalef(1.4, 1.4, 1.4);
    glRotatef(b->angle, 0.0, 0.0, 1.0);//y軸を中心とした回転
    glPushMatrix();

    // マテリアルの設定
    GLfloat material_diffuse[] = { b->color[0], b->color[1], b->color[2], 1.0 }; // をベイブレードの色に設定
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);

    // フェイス部分を描画
    glPushMatrix();
    GLUquadric* face = gluNewQuadric();
    gluQuadricDrawStyle(face, GLU_FILL); // 塗りつぶしスタイル
    gluDisk(face, 0.0, 0.3, 6, 1);
    glPopMatrix();


    glPushMatrix();
    GLUquadric* face2 = gluNewQuadric();
    gluQuadricDrawStyle(face2, GLU_FILL); // 塗りつぶしスタイル
    gluCylinder(face2, 0.3, 0.3, 0.2, 6, 1);
    glPopMatrix();

    //ホイール部分を描画
    glPushMatrix();
    glTranslatef(0, 0, 0.05);
    GLUquadric* wheel = gluNewQuadric();
    gluQuadricDrawStyle(wheel, GLU_FILL); // 塗りつぶしスタイル
    gluCylinder(wheel, 0.6, 0.8, 0.1, 32, 1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0, 0, 0.05);
    GLUquadric* wheel2 = gluNewQuadric();
    gluQuadricDrawStyle(wheel2, GLU_FILL); // 塗りつぶしスタイル
    gluCylinder(wheel2, 0.6, 0.6, 0.1, 32, 1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.11); // 次の部分の位置を調整
    int i;
    for (i = 1; i <= 2; i++) {
        glRotatef(50, 0, 0, 1);
        glPushMatrix();
        glScalef(1.4, 0.2, 0.05);
        glutSolidCube(1);
        glPopMatrix();
    }

    glPushMatrix();
    glRotatef(-25, 0, 0, 1);
    glScalef(1.4, 0.4, 0.05);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();





    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.19); // 次の部分の位置を調整
    glRotatef(90, 0, 0, 1);
    for (i = 1; i <= 2; i++) {
        glRotatef(50, 0, 0, 1);
        glPushMatrix();
        glScalef(2.0, 0.2, 0.05);
        glutSolidCube(1);
        glPopMatrix();
    }

    glPushMatrix();
    glRotatef(-25, 0, 0, 1);
    glScalef(2.0, 0.4, 0.05);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.12); // 次の部分の位置を調整
    GLUquadric* wheel3 = gluNewQuadric();
    gluQuadricDrawStyle(wheel3, GLU_FILL); // 塗りつぶしスタイル
    gluCylinder(wheel3, 1, 0.95, 0.35, 32, 1);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.30); // 次の部分の位置を調整
    GLUquadric* wheel4 = gluNewQuadric();
    gluQuadricDrawStyle(wheel4, GLU_FILL); // 塗りつぶしスタイル
    gluDisk(wheel4, 0.0, 1, 32, 1);
    glPopMatrix();

    //トラック部分の描画
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.33); // 次の部分の位置を調整
    GLUquadric* track = gluNewQuadric();
    gluQuadricDrawStyle(track, GLU_FILL); // 塗りつぶしスタイル
    gluCylinder(track, 0.4, 0.3, 0.4, 6, 1);
    glPopMatrix();




    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.73);
    GLUquadric* track2 = gluNewQuadric();
    gluQuadricDrawStyle(track2, GLU_FILL); // 塗りつぶしスタイル
    gluDisk(track2, 0.0, 0.3, 6, 1);
    glPopMatrix();

    //ボトム部分の描画
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.73);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.14, 32, 32); // 半径1.0、横32セグメント、縦32セグメントの球を描画
    gluDeleteQuadric(quad);
    glPopMatrix();


    glPopMatrix();
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // デプスバッファをクリア

    glEnable(GL_LIGHTING); // ライティングを有効にする
    glEnable(GL_LIGHT0);   // ライト0を有効にする

    
    // ライトの位置を設定
    GLfloat light_position[] = { 10.0, -10.0, 10.0, 1.0 }; // ライトを配置
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // ライトの色を設定（白色）
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    GLfloat ambient[] = { 0.8, 0.8, 0.8, 1.0 }; // 環境光
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // 拡散光を白に設定
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 }; // 鏡面反射光も白に設定
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    gluLookAt(0.0, -2.0, 13.0,  // カメラの位置
        0.0, 0.0, 0.0,  // 見ている位置
        0.0, 1.0, 0.0); // 上方向

    polarview();
    glPushMatrix();
    GLfloat field_material_diffuse[] = { 0.8, 0.8, 0.8, 1.0 }; // スタジアムの色を設定
    GLfloat field_material_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // 鏡面反射色を設定
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, field_material_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, field_material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0); // 光沢度を設定
    drawCircle(FIELD_RADIUS, 80);
    glPopMatrix();

    glPushMatrix();
    // 駒の描画
    if (initialized) {
        for (int i = 0; i < NUM_BEYBLADES; i++) {
            drawBeyblade(&beyblades[i]);
        }
    }
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers(); // バッファを交換
}


void myKbd(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESCキー
    else if (key == 'r' || key == 'R') {
        // 'r' キーが押されたとき、駒を再生成
        init();
    }
}

void myMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        mButton = button;
        xBegin = x;
        yBegin = y;
    }
}

void myMotion(int x, int y) {
    int xDisp, yDisp;

    xDisp = x - xBegin;
    yDisp = y - yBegin;
    switch (mButton) {
    case GLUT_LEFT_BUTTON:
        azimuth += (float)xDisp / 2.0f;
        elevation -= (float)yDisp / 2.0f;
        break;
    case GLUT_MIDDLE_BUTTON:
        twist = fmod(twist + xDisp, 360.0f);
        break;
    case GLUT_RIGHT_BUTTON:
        camDistance -= (float)yDisp / 40.0f;
        break;
    }
    xBegin = x;
    yBegin = y;
    glutPostRedisplay();
}

void myInit(char* progname) {
    int width = 800, height = 800;
    float aspect = (float)width / (float)height;

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(progname);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glutKeyboardFunc(myKbd);
    glutMouseFunc(myMouse); // マウスイベントを登録
    glutMotionFunc(myMotion); // マウスドラッグイベントを登録

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 1.0f, 50.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST); // デプステストを有効にする
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    myInit(argv[0]);
    glutDisplayFunc(display);
    glutIdleFunc(idle); // idle関数を登録
    glutMainLoop();
    return 0;
}
