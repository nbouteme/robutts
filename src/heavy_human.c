#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <shaders.h>
#include <mat4.h>

robot_properties my_robot = {
	1.0f, 1.6f, 1.5f, 0xF0FFF0, "heavy"
};

int up, right, left, down, use;

int shader;
unsigned vao;
int du, cu;

/*
  La gestion des touches de glut est maladroite
 */
static void key_callback(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	if (key == 'k') {
		exit(43);
	}
	if (key == 'l') {
		while(1);
	}
	if (key == ' ') {
		use = 1;
	}
}

static void skey_callback(int key, int x, int y)
{
	(void)x;
	(void)y;
	if (key == GLUT_KEY_UP) {
		up = 1;
	}
	if (key == GLUT_KEY_DOWN) {
		down = 1;
	}
	if (key == GLUT_KEY_LEFT) {
		left = 1;
	}
	if (key == GLUT_KEY_RIGHT) {
		right = 1;
	}
}

static void skeyup_callback(int key, int x, int y)
{
	(void)x;
	(void)y;
	if (key == GLUT_KEY_UP) {
		up = 0;
	}
	if (key == GLUT_KEY_DOWN) {
		down = 0;
	}
	if (key == GLUT_KEY_LEFT) {
		left = 0;
	}
	if (key == GLUT_KEY_RIGHT) {
		right = 0;
	}
}

void display_sensors_state() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader);

	glUniform1iv(cu, 64, (int*)my_state.obj_attr_buffer);
	glUniform1fv(du, 64, my_state.depth_buffer);

	glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 128);

	glUseProgram(0);
    glBindVertexArray(0);
	glutSwapBuffers();
}

void init(int argc, char *argv[]) {
	static int already = 0;
	unsigned vbo;
    float *vertices;
	float adelta;
	vec2_t c;
	vec2_t p;
	int i;
	int v;
	int f;
	int proju;
	mat4_t proj;

	(void)argc;
	(void)argv;
	if (already)
		return;
	already = 1;
	use = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitWindowSize(256, 256);
	glutCreateWindow("Heavy bot");
	glutDisplayFunc(display_sensors_state);
	glutKeyboardFunc(key_callback);
	glutSpecialFunc(skey_callback);
	glutSpecialUpFunc(skeyup_callback);
	glInit();

	vertices = calloc(sizeof(float) * 4, 64);
	adelta = 2 * M_PI / 64.0;
	c = (vec2_t){256, 256};
	for(i = 0; i < 64; ++i) {
		p = vec2_rot(vec2_muls(vec2_up(), 64), adelta * i);
		p = vec2_add(p, c);
		vertices[4 * i] = p.x;
		vertices[4 * i + 1] = p.y;
		vertices[4 * i + 2] = p.x;
		vertices[4 * i + 3] = p.y;
	}

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 64, vertices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	
	v = load_shader("assets/v_rvisu.glsl", GL_VERTEX_SHADER);
	f = load_shader("assets/f_rvisu.glsl", GL_FRAGMENT_SHADER);
	shader = make_shader(v, f);
	glUseProgram(shader);

	proju = glGetUniformLocation(shader, "proj");
	cu = glGetUniformLocation(shader, "colors");
	du = glGetUniformLocation(shader, "depth");
	
	proj = mat4_ortho(0.0f, 4 * 128.0f, 4 * 128.0f, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(proju, 1, GL_FALSE, &proj.s[0].x);
}

void update_state() {
	glutPostRedisplay();
	glutMainLoopEvent();
}

void update() {
	my_state.lin_eng_state = up - down;
	my_state.rot_eng_state = left - right;
	if (use) {
		use = 0;
		use_item(0);
	}
}
