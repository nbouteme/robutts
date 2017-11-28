#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <process.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

process_t subbot[2];
int selected = 0;
int nbots = 2;
int colors[2][4] = {
	{255, 255, 0, 0},
	{0, 255, 255, 0}
};

static void key_callback(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	if (key == ' ') {
		++selected;
		selected %= nbots;
	}
}

void display_sensors_state() {
	glClearColor(colors[selected][0], colors[selected][1], colors[selected][2], colors[selected][3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void init(int argc, char *argv[]) {
	static int already = 0;
	command_t cmd = CMD_INIT;
	int i;

	(void)argc;
	(void)argv;
	if (already)
		return;
	already = 1;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitWindowSize(256, 256);
	glutCreateWindow("Switch");
	glutDisplayFunc(display_sensors_state);
	glutKeyboardFunc(key_callback);
	glInit();
	for (i = 0; i < nbots; ++i) {
		make_linked_process(&subbot[i], argv[i + 1]);
		write(subbot[i].stdin, &cmd, sizeof(cmd));
		exact_read(subbot[i].stdout, &my_robot, sizeof(my_robot)); // ignoré
	}
}

void exec_subcmd_stream(process_t *bot) {
	request_t r;
	int i;
	do {
		exact_read(bot->stdout, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			exact_read(bot->stdout, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			exact_read(bot->stdout, &i, sizeof(int));
			i = use_item(i);
			write(bot->stdin, &i, sizeof(int));
			break;
		default:
			break;
		}
	} while (r != REQ_END);
}

void update() {
	process_t *bot = subbot + selected;
	command_t c = CMD_UPDATE;
	write(bot->stdin, &c, sizeof(c));
	exec_subcmd_stream(bot);
}

void update_state() {
	process_t *bot;
	command_t c = CMD_UPDATE_STATE;
	for (int i = 0; i < nbots; ++i) {
		bot = subbot + i;
		write(bot->stdin, &c, sizeof(c));
		write(bot->stdin, &my_state, sizeof(my_state));
		write(bot->stdin, my_state.bag, sizeof(item_t) * my_state.bag_size);
		write(bot->stdin, my_state.depth_buffer, sizeof(float) * my_state.rays);
		write(bot->stdin, my_state.obj_attr_buffer, sizeof(float) * my_state.rays);
	}
	glutPostRedisplay();
	glutMainLoopEvent();
}

void destroy() {
	command_t c = CMD_DESTROY;
	write(subbot[selected].stdin, &c, sizeof(c));
	exec_subcmd_stream(&subbot[selected]);
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
