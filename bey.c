#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>   // time()

#define NUM_BEYBLADES 2  // �x�C�u���[�h�̐�
#define RADIUS 1.3f      // �x�C�u���[�h�̔��a
#define FIELD_RADIUS 10.0f // �t�B�[���h�̔��a
#define M_PI 3.1415926535

typedef struct {
    float x, y, z;      // �ʒu
    float vx, vy, vz;   // ���x
    float angle;        // ��]�p�x
    float rotation_speed; // ��]���x
    float color[3];     // �F
    float damping;      // ���x�̌�����
} Beyblade;

Beyblade beyblades[NUM_BEYBLADES];

float camDistance = 20.0f, twist = 0.0f, elevation = -30.0f, azimuth = 30.0f;
int xBegin = 0, yBegin = 0;
int mButton;
int initialized = 0; // �������t���O

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // �����̏�����
    srand(time(NULL));

    // �x�C�u���[�h�̏�����
    for (int i = 0; i < NUM_BEYBLADES; i++) {
        // �����_���ȑ��x�Ɗp�x��ݒ�
        beyblades[i].angle = 0.0f; // �����̉�]�p�x��0
        beyblades[i].rotation_speed = 10.0f; // �����̉�]���x
        float speed = 0.01f + ((float)rand() / RAND_MAX) * 0.12f; // 0.01 ���� 0.12 �͈̔͂Ń����_���ȑ��x
        float angle = ((float)rand() / RAND_MAX) * 2 * M_PI; // 0 ���� 2�� �͈̔͂Ń����_���Ȋp�x
        beyblades[i].vx = speed * cosf(angle); // x �����̑��x����
        beyblades[i].vy = 0.0f; // y �����̑��x�����i���̗�ł̓[���j
        beyblades[i].vz = speed * sinf(angle); // z �����̑��x����
        beyblades[i].damping = 0.99999999999f; // ���x�̌�������ݒ�

        // �����ʒu�̐ݒ�
        beyblades[i].x = ((float)rand() / RAND_MAX) * (2 * FIELD_RADIUS * 0.3) - FIELD_RADIUS * 0.3; // -FIELD_RADIUS ���� FIELD_RADIUS �͈̔͂Ń����_���� x ���W
        beyblades[i].y = FIELD_RADIUS + RADIUS; // �t�B�[���h�̏�[�ɔz�u
        beyblades[i].z = ((float)rand() / RAND_MAX) * (2 * FIELD_RADIUS * 0.3) - FIELD_RADIUS * 0.3; // -FIELD_RADIUS ���� FIELD_RADIUS �͈̔͂Ń����_���� z ���W

        // �����_���ȐF��ݒ�
        beyblades[i].color[0] = ((float)rand() / RAND_MAX); // 0.0 ���� 1.0 �͈̔͂Ń����_���ȐF���� R
        beyblades[i].color[1] = ((float)rand() / RAND_MAX); // 0.0 ���� 1.0 �͈̔͂Ń����_���ȐF���� G
        beyblades[i].color[2] = ((float)rand() / RAND_MAX); // 0.0 ���� 1.0 �͈̔͂Ń����_���ȐF���� B
    }
    initialized = 1;
}
void idle() {
    for (int i = 0; i < NUM_BEYBLADES; i++) {
        // �ʒu���X�V
        if (beyblades[i].angle != 0 && beyblades[i].rotation_speed != 0) {
            beyblades[i].x += beyblades[i].vx;
            beyblades[i].z += beyblades[i].vz;
            beyblades[i].y -= 0.05f;
        }

        // �x�C�u���[�h��������̑��x�܂ŗ������Ƃ��ɒ�~
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
        // �t�B�[���h�ɒ��n
        if (beyblades[i].y <= -0.7f) {
            beyblades[i].y = -0.7f;
            beyblades[i].vy = 0.0f; // y�����̑��x���[���ɂ���
        }

        // �t�B�[���h�O�ɏo���甽�]������
        if (sqrt(beyblades[i].x * beyblades[i].x + beyblades[i].z * beyblades[i].z) > FIELD_RADIUS - RADIUS) {
            beyblades[i].x += -0.001 * beyblades[i].x;// ���i�q�b�g���N�����Ȃ����߂̒���
            beyblades[i].z += -0.001 * beyblades[i].z;// ���i�q�b�g���N�����Ȃ����߂̒���
            beyblades[i].vx *= -0.95f; // x�����̑��x�𔽓]������
            beyblades[i].vz *= -0.95f; // z�����̑��x�𔽓]������
            beyblades[i].rotation_speed *= 0.8f; // ��]���x�̌���
        }

        // ��]�p�x�̍X�V
        beyblades[i].angle += beyblades[i].rotation_speed;
        if (beyblades[i].angle > 360) {
            beyblades[i].angle -= 360;
        }
        //�x�C�u���[�h���m�̔���
        for (int j = i + 1; j < NUM_BEYBLADES; j++) {
            if (beyblades[i].y != -0.7f) {
                break;//�󒆂łԂ����Ē�~���Ȃ��悤��
            }
            float dx = beyblades[i].x - beyblades[j].x;
            float dz = beyblades[i].z - beyblades[j].z;
            float distance = sqrt(dx * dx + dz * dz);//��_�Ԃ̋���

            if (distance < 2 * RADIUS) {
                // ����
                float nx = dx / distance;
                float nz = dz / distance;

                beyblades[i].vx += nx * 0.01f;
                beyblades[i].vz += nz * 0.01f;
                beyblades[j].vx -= nx * 0.01f;
                beyblades[j].vz -= nz * 0.01f;
                beyblades[i].vx *= 0.95f;//����
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

//�X�^�W�A���̕`��
void drawCircle(float radius, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        // �@���x�N�g����ݒ�
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)segments;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        // �@���x�N�g����ݒ�
        glNormal3f(x, 0.0f, z);
        glVertex3f(x, 0.0f, z);    // ���̓_
        glVertex3f(1.09 * x, 1.0f, 1.09 * z);    // ��̓_
    }
    glEnd();
}
//�x�C�u���[�h�̕`��
void drawBeyblade(Beyblade* b) {
    glPushMatrix();
    glTranslatef(b->x, b->y, b->z);//�����ʒu
    glTranslatef(0, 2, 0);
    glRotatef(90, 1.0, 0, 0.0);
    glScalef(1.4, 1.4, 1.4);
    glRotatef(b->angle, 0.0, 0.0, 1.0);//y���𒆐S�Ƃ�����]
    glPushMatrix();

    // �}�e���A���̐ݒ�
    GLfloat material_diffuse[] = { b->color[0], b->color[1], b->color[2], 1.0 }; // ���x�C�u���[�h�̐F�ɐݒ�
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);

    // �t�F�C�X������`��
    glPushMatrix();
    GLUquadric* face = gluNewQuadric();
    gluQuadricDrawStyle(face, GLU_FILL); // �h��Ԃ��X�^�C��
    gluDisk(face, 0.0, 0.3, 6, 1);
    glPopMatrix();


    glPushMatrix();
    GLUquadric* face2 = gluNewQuadric();
    gluQuadricDrawStyle(face2, GLU_FILL); // �h��Ԃ��X�^�C��
    gluCylinder(face2, 0.3, 0.3, 0.2, 6, 1);
    glPopMatrix();

    //�z�C�[��������`��
    glPushMatrix();
    glTranslatef(0, 0, 0.05);
    GLUquadric* wheel = gluNewQuadric();
    gluQuadricDrawStyle(wheel, GLU_FILL); // �h��Ԃ��X�^�C��
    gluCylinder(wheel, 0.6, 0.8, 0.1, 32, 1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0, 0, 0.05);
    GLUquadric* wheel2 = gluNewQuadric();
    gluQuadricDrawStyle(wheel2, GLU_FILL); // �h��Ԃ��X�^�C��
    gluCylinder(wheel2, 0.6, 0.6, 0.1, 32, 1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.11); // ���̕����̈ʒu�𒲐�
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
    glTranslatef(0.0, 0.0, 0.19); // ���̕����̈ʒu�𒲐�
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
    glTranslatef(0.0, 0.0, 0.12); // ���̕����̈ʒu�𒲐�
    GLUquadric* wheel3 = gluNewQuadric();
    gluQuadricDrawStyle(wheel3, GLU_FILL); // �h��Ԃ��X�^�C��
    gluCylinder(wheel3, 1, 0.95, 0.35, 32, 1);
    glPopMatrix();



    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.30); // ���̕����̈ʒu�𒲐�
    GLUquadric* wheel4 = gluNewQuadric();
    gluQuadricDrawStyle(wheel4, GLU_FILL); // �h��Ԃ��X�^�C��
    gluDisk(wheel4, 0.0, 1, 32, 1);
    glPopMatrix();

    //�g���b�N�����̕`��
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.33); // ���̕����̈ʒu�𒲐�
    GLUquadric* track = gluNewQuadric();
    gluQuadricDrawStyle(track, GLU_FILL); // �h��Ԃ��X�^�C��
    gluCylinder(track, 0.4, 0.3, 0.4, 6, 1);
    glPopMatrix();




    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.73);
    GLUquadric* track2 = gluNewQuadric();
    gluQuadricDrawStyle(track2, GLU_FILL); // �h��Ԃ��X�^�C��
    gluDisk(track2, 0.0, 0.3, 6, 1);
    glPopMatrix();

    //�{�g�������̕`��
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.73);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.14, 32, 32); // ���a1.0�A��32�Z�O�����g�A�c32�Z�O�����g�̋���`��
    gluDeleteQuadric(quad);
    glPopMatrix();


    glPopMatrix();
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �f�v�X�o�b�t�@���N���A

    glEnable(GL_LIGHTING); // ���C�e�B���O��L���ɂ���
    glEnable(GL_LIGHT0);   // ���C�g0��L���ɂ���

    
    // ���C�g�̈ʒu��ݒ�
    GLfloat light_position[] = { 10.0, -10.0, 10.0, 1.0 }; // ���C�g��z�u
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // ���C�g�̐F��ݒ�i���F�j
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    GLfloat ambient[] = { 0.8, 0.8, 0.8, 1.0 }; // ����
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // �g�U���𔒂ɐݒ�
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 }; // ���ʔ��ˌ������ɐݒ�
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    gluLookAt(0.0, -2.0, 13.0,  // �J�����̈ʒu
        0.0, 0.0, 0.0,  // ���Ă���ʒu
        0.0, 1.0, 0.0); // �����

    polarview();
    glPushMatrix();
    GLfloat field_material_diffuse[] = { 0.8, 0.8, 0.8, 1.0 }; // �X�^�W�A���̐F��ݒ�
    GLfloat field_material_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // ���ʔ��ːF��ݒ�
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, field_material_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, field_material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0); // ����x��ݒ�
    drawCircle(FIELD_RADIUS, 80);
    glPopMatrix();

    glPushMatrix();
    // ��̕`��
    if (initialized) {
        for (int i = 0; i < NUM_BEYBLADES; i++) {
            drawBeyblade(&beyblades[i]);
        }
    }
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers(); // �o�b�t�@������
}


void myKbd(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC�L�[
    else if (key == 'r' || key == 'R') {
        // 'r' �L�[�������ꂽ�Ƃ��A����Đ���
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
    glutMouseFunc(myMouse); // �}�E�X�C�x���g��o�^
    glutMotionFunc(myMotion); // �}�E�X�h���b�O�C�x���g��o�^

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 1.0f, 50.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST); // �f�v�X�e�X�g��L���ɂ���
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    myInit(argv[0]);
    glutDisplayFunc(display);
    glutIdleFunc(idle); // idle�֐���o�^
    glutMainLoop();
    return 0;
}
