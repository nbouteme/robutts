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

process_t subbot[2];
int selected = 0;
int nbots = 2;
int colors[2][4] = {
	{255, 255, 0, 0},
	{0, 255, 255, 0}
};


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)window;
	(void)scancode;
	(void)mods;
	if (action == GLFW_REPEAT)
		return;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		++selected;
		selected %= nbots;
	}
}

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
    window = glfwCreateWindow(256, 256, "Switcher", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
	glewExperimental = 1;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
    glfwSwapInterval(1);
	command_t cmd = CMD_INIT;
	for (int i = 0; i < nbots; ++i) {
		make_linked_process(&subbot[i], argv[i + 1]);
		write(subbot[i].stdin, &cmd, sizeof(cmd));
		read(subbot[i].stdout, &my_robot, sizeof(my_robot)); // ignoré
	}
}

void exec_subcmd_stream(process_t *bot) {
	request_t r;
	int i;
	do {
		read(bot->stdout, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			read(bot->stdout, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			read(bot->stdout, &i, sizeof(int));
			i = use_item(i);
			write(bot->stdin, &i, sizeof(int));
			break;
		default:
			break;
		}
	} while (r != REQ_END);
}

void update() {
	process_t *bot = subbot;
	command_t c = CMD_UPDATE_STATE;
	for (int i = 0; i < nbots; ++i) {
		bot = subbot + i;
		write(bot->stdin, &c, sizeof(c));
		write(bot->stdin, &my_state, sizeof(my_state));
		write(bot->stdin, my_state.bag, sizeof(item_t) * my_state.bag_size);
		write(bot->stdin, my_state.depth_buffer, sizeof(float) * my_state.rays);
		write(bot->stdin, my_state.obj_attr_buffer, sizeof(float) * my_state.rays);
		break;
	}

	bot = subbot + selected;

	c = CMD_UPDATE;
	write(bot->stdin, &c, sizeof(c));
	exec_subcmd_stream(bot);
}

void update_state() {
	glClearColor(colors[selected][0], colors[selected][1], colors[selected][2], colors[selected][3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void destroy() {	
	command_t c = CMD_DESTROY;
	write(subbot[selected].stdin, &c, sizeof(c));
	exec_subcmd_stream(&subbot[selected]);
	glfwDestroyWindow(window);
    glfwTerminate();
}

void item_collected(item_t i) {
	command_t c = CMD_COLLECT;
	write(subbot[selected].stdin, &c, sizeof(c));
	write(subbot[selected].stdin, &i, sizeof(i));
	exec_subcmd_stream(&subbot[selected]);
}

void collision(coll_t i) {
	command_t c = CMD_COLLISION;
	write(subbot[selected].stdin, &c, sizeof(c));
	write(subbot[selected].stdin, &i, sizeof(i));
	exec_subcmd_stream(&subbot[selected]);
}
