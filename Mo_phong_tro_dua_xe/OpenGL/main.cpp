//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

class MatrixStack {
private: int index = 0;
		 mat4 matrixs[50];
public: void push(const mat4& matrix);
		mat4 pop();
};

void MatrixStack::push(const mat4& matrix) {
	matrixs[index] = matrix;
	index++;
}

mat4 MatrixStack::pop() {
	index--;
	return matrixs[index];
}

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}

GLuint model_loc;
GLuint projection_loc;
mat4 model;
MatrixStack ms;

enum class GOC_NHIN {
	THU_NHAT,
	THU_BA
};

// giá trị camera ở góc nhìn thứ nhất
const double EX1 = 0.5;
const double EY1 = 0.8;
const double EZ1 = 0;
const double AX1 = 5;

// giá trị camera ở góc nhìn thứ 3
const double EX3 = -2;
const double EY3 = 2;
const double EZ3 = 2;
const double AX3 = 0;

//mặc định góc nhìn thứ 3, giá trị camera thay đổi khi di chuyển
double ex = EX3;
double ey = EY3;
double ez = EZ3;
double ax = AX3;

double dichXe = 0;
double quayBanhXe = 0;
double zoom = 0;
GOC_NHIN gocNhin = GOC_NHIN::THU_BA;

// 1 - dịch X, 2 - dịch Y, 3 - dịch Z, 4 - kích thước X, 5 - kích thước Y, 6 - kích thước Z
void bienDoiHLPDV(float tx, float ty, float tz, float sx, float sy, float sz) {
	ms.push(model);
	model *= Translate(tx, ty, tz)*Scale(sx, sy, sz);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLES,0,NumPoints);
	model = ms.pop();
}

// 1 - dịch X, 2 - dịch Y, 3 - dịch Z, 4 - kích thước X, 5 - kích thước Y, 6 - kích thước Z, 7 - xoay đối tượng theo trục z
void bienDoiHLPDV2(float tx, float ty, float tz, float sx, float sy, float sz, float tx2, float ty2, float tz2,float rz) {
	ms.push(model);
	model *= Translate(tx, ty, tz)*RotateZ(rz)*Translate(tx2,ty2,tz2)*Scale(sx, sy, sz);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	model = ms.pop();
}

// VẼ ĐƯỜNG ĐUA

void veDuongDua() {
	//sau đây là code vẽ đường
	bienDoiHLPDV(0, 6.5, 0, 30, 1.5, 1);
	bienDoiHLPDV(0, 3, 0, 30, 5, 1);
	bienDoiHLPDV(0, 0, 0, 30, 1.5, 1);
	//1 là kích thước của mỗi ô trắng đen ở vạch đích,sau đây là code vẽ mấy cái ô đó
	for (float tx = -14; tx < 28; tx++)
	{
		if (tx == -14 || tx == 13) {
			for (int j = 0; j < 2; j++) {
				for (int i = 0; i < 5; i++) {
					bienDoiHLPDV(tx, 1 + i, -0.01, 1, 1, 1);
				}
				tx = tx + 1;
			}
		}
	}

}

// VẼ HÀNG RÀO

void veHangRao() {
	// hàng rào ở trước
	bienDoiHLPDV(0, -0.2, 2.6, 30, 0.2, 0.1);
	bienDoiHLPDV(0, 0.3, 2.6, 30, 0.2, 0.1);
	for (int i = 0; i <= 15; i++) {
		bienDoiHLPDV(-i, 0, 2.6, 0.2, 1.5, 0.1);
	}
	for (int i = 0; i <= 15; i++) {
		bienDoiHLPDV(i, 0, 2.6, 0.2, 1.5, 0.1);
	}
	//hàng rào ở trong
	bienDoiHLPDV(0, -0.2, -2.6, 30, 0.2, 0.1);
	bienDoiHLPDV(0, 0.3, -2.6, 30, 0.2, 0.1);
	for (int i = 0; i <= 15; i++) {
		bienDoiHLPDV(-i, 0, -2.6, 0.2, 1.5, 0.1);
	}
	for (int i = 0; i <= 15; i++) {

		bienDoiHLPDV(i, 0, -2.6, 0.2, 1.5, 0.1);
	}
}

// VẼ XE ĐUA

void veBanhXe(float tx, float ty, float tz,float rz) {
	bienDoiHLPDV(tx, ty, tz, 0.2, 0.2, 0.2);

	bienDoiHLPDV2(tx, ty, tz, 0.2, 0.2, 0.2, 0, 0.2, 0, rz-45);
	bienDoiHLPDV2(tx, ty, tz, 0.2, 0.2, 0.2, 0, -0.2, 0, rz - 45);

	bienDoiHLPDV2(tx, ty, tz, 0.2, 0.2, 0.2, 0.2, 0.1, 0, rz - 45);
	bienDoiHLPDV2(tx, ty, tz, 0.2, 0.2, 0.2, 0.2, -0.1, 0, rz - 45);

	bienDoiHLPDV2( tx, ty, tz, 0.2, 0.2, 0.2, -0.2, 0.1, 0, rz - 45);
	bienDoiHLPDV2( tx, ty, tz, 0.2, 0.2, 0.2, -0.2, -0.1, 0, rz - 45);
}

void veXeDua() {

	// thân xe và tấm chắn trên
	bienDoiHLPDV(0 + dichXe, 0, 0, 2, 0.8, 1);
	bienDoiHLPDV(-0.3 + dichXe, 0.4, 0, 1, 0.4, 0.8);

	// tấm chắn gió
	bienDoiHLPDV2(0.75 + dichXe,0.4,0,0.5,0.1,0.7,0,0,0,-45);

	// đèn trước trái, phải
	bienDoiHLPDV(1 + dichXe,0.15,0.3,0.1,0.2,0.2);
	bienDoiHLPDV(1 + dichXe, 0.15, -0.3, 0.1, 0.2, 0.2);

	// tấm chắn sau
	bienDoiHLPDV(-1 + dichXe, -0.25, 0, 0.1, 0.05, 0.8);

	// 4 bánh xe
	veBanhXe(0.6+dichXe, -0.4, 0.5, quayBanhXe);
	veBanhXe(-0.6 + dichXe, -0.4, -0.5, quayBanhXe);
	veBanhXe(0.6 + dichXe, -0.4, -0.5, quayBanhXe);
	veBanhXe(-0.6 + dichXe, -0.4, 0.5, quayBanhXe);

}

float xoayY;

void dungMoHinh() {
	point4 eye(ex, ey, ez, 1);
	point4 at(ax, 0, 0, 1);
	point4 up(0, 1, 0, 1);
	model = LookAt(eye, at, up);

	//model *= RotateY(xoayY);

	// vẽ xe đua
	veXeDua();

	// vẽ hàng rào
	veHangRao();

	// xoay và dịch đường đua đến vị trí phù hợp so với xe
	model *= Translate(0,-1.25,-3)*RotateX(90);
	veDuongDua();
}

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	model_loc = glGetUniformLocation(program, "model");
	projection_loc = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

static void special(int specialKey, int x, int y) {
	switch (specialKey) {
	case GLUT_KEY_UP:
	{
		dichXe += 0.1;
		quayBanhXe -= 45;
		if (gocNhin == GOC_NHIN::THU_NHAT) {
			ex = EX1 + dichXe;
			ey = EY1;
			ez = EZ1;
			ax = AX1 + dichXe;
		}
		if (gocNhin == GOC_NHIN::THU_BA) {
			ex = EX3 - zoom;
			ey = EY3 + zoom;
			ez = EZ3 + zoom;
			ax = AX3 + dichXe;
		}
	}
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
	{
		dichXe -= 0.1;
		quayBanhXe += 45;
		if (gocNhin == GOC_NHIN::THU_NHAT) {
			ex = EX1 + dichXe;
			ey = EY1;
			ez = EZ1;
			ax = AX1 + dichXe;
		}
		if (gocNhin == GOC_NHIN::THU_BA) {
			ex = EX3 - zoom;
			ey = EY3 + zoom;
			ez = EZ3 + zoom;
			ax = AX3 + dichXe;
		}
	}
		glutPostRedisplay();
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case '1': {
		gocNhin = GOC_NHIN::THU_NHAT;
		ex = EX1 + dichXe;
		ey = EY1;
		ez = EZ1;
		ax = AX1 + dichXe;
	};	
		glutPostRedisplay();
		break;
	case '3': {
		gocNhin = GOC_NHIN::THU_BA;
		ex = EX3 - zoom;
		ey = EY3 + zoom;
		ez = EZ3 + zoom;
		ax = AX3 + dichXe;
	};
		glutPostRedisplay();
		break;
	case 'y': xoayY += 5;
			  glutPostRedisplay();
			  break;
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	// phóng to, thu nhỏ chỉ áp dụng cho góc nhìn thứ 3
	zoom = dir > 0 ? zoom - 0.2 : zoom + 0.2;
	if (gocNhin == GOC_NHIN::THU_BA) {
		ex = EX3 - zoom;
		ey = EY3 + zoom;
		ez = EZ3 + zoom;
		ax = AX3 + dichXe;
	}
	glutPostRedisplay();
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat l = -1, r = 1, b = -1, t = 1, zNear = 0.5, zFar = 40;
	mat4 proj = Frustum(l, r, b, t, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, proj);

	dungMoHinh();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing RacingCar");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouseWheel);
	glutSpecialFunc(special);

	glutMainLoop();
	return 0;
}
