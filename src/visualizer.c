#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <GLFW/glfw3.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

static void error_callback(int error, const char* description)
{
	(void)error;
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* window;

int shader;
unsigned vao;
int du, cu;

process_t subbot;

void init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	static int already = 0;
	if (already)
		return;
	already = 1;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(256, 256, "Visualizer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
	glewExperimental = 1;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
    glfwSwapInterval(1);

	unsigned vbo;
    float *vertices = calloc(sizeof(float) * 4, 64);

	float adelta = 2 * M_PI / 64.0;
	vec2_t c = {256, 256};
	for(int i = 0; i < 64; ++i) {
		vec2_t p = vec2_rot(vec2_muls(vec2_up(), 64), adelta * i);
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
	
	int v = load_shader("v_rvisu.glsl", GL_VERTEX_SHADER);
	int f = load_shader("f_rvisu.glsl", GL_FRAGMENT_SHADER);
	shader = make_shader(v, f);
	glUseProgram(shader);

	int proju = glGetUniformLocation(shader, "proj");
	cu = glGetUniformLocation(shader, "colors");
	du = glGetUniformLocation(shader, "depth");
	
	mat4_t proj = mat4_ortho(0.0f, 4 * 128.0f, 4 * 128.0f, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(proju, 1, GL_FALSE, &proj.s[0].x);


	make_linked_process(&subbot, argv[1]);
	command_t cmd = CMD_INIT;
	write(subbot.stdin, &cmd, sizeof(cmd));
	read(subbot.stdout, &my_robot, sizeof(my_robot));
}

void exec_subcmd_stream() {
	request_t r;
	int i;
	do {
		read(subbot.stdout, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			read(subbot.stdout, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			read(subbot.stdout, &i, sizeof(int));
			i = use_item(i);
			write(subbot.stdin, &i, sizeof(int));
			break;
		default:
			break;
		}
	} while (r != REQ_END);
}

void update() {
	command_t c = CMD_UPDATE;
	write(subbot.stdin, &c, sizeof(c));
	exec_subcmd_stream();
}

void update_state() {
	command_t c = CMD_UPDATE_STATE;

	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &my_state, sizeof(my_state));
	write(subbot.stdin, my_state.bag, sizeof(item_t) * my_state.bag_size);
	write(subbot.stdin, my_state.depth_buffer, sizeof(float) * my_state.rays);
	write(subbot.stdin, my_state.obj_attr_buffer, sizeof(int) * my_state.rays);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader);

	glUniform1iv(cu, 64, (int*)my_state.obj_attr_buffer);
	glUniform1fv(du, 64, my_state.depth_buffer);

	glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 128);

    glBindVertexArray(0);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void destroy() {	
	command_t c = CMD_DESTROY;
	write(subbot.stdin, &c, sizeof(c));
	exec_subcmd_stream();
	glfwDestroyWindow(window);
    glfwTerminate();
}

void item_collected(item_t i) {
	command_t c = CMD_COLLECT;
	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &i, sizeof(i));
	exec_subcmd_stream();
}

void collision(coll_t i) {
	command_t c = CMD_COLLISION;
	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &i, sizeof(i));
	exec_subcmd_stream();
}
