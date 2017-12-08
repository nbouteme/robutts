#include <robutts.h>
#include <scene.h>
#include <robot.h>
#include <audio.h>
#include <texture.h>
#include <assets.h>
#include <item.h>
#include <font.h>

#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

static const int g_60fps = ((1.0 / 60) * 1000000000);

sigjmp_buf jb;
int can_jump = 0;

game_state_t *get_game_state() {
	static game_state_t *s = 0;
	if (!s) {
		s = calloc(1, sizeof(*s));
	}
	return s;
}

void display_game_state() {
	game_state_t *gs;
	sprite_t s;
	int i;

	gs = get_game_state();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Dessine le fond...
	s = (sprite_t){gs->background_tex, (vec2_t){0, 0}, 0, 0x00FFFFFF, {1280, 720}};
	draw_sprite(gs->renderer, s);

	// ... les robots...
	for (i = 0; i < gs->n_robots; ++i) {
		draw_robot(&gs->robots[i], gs->renderer);
	}

	// ... et les objets
	for (i = 0; i < gs->n_items; ++i) {
		collectable_item_vtable[gs->items[i].type].draw(&gs->items[i], gs->renderer);
	}

	glFlush();
	glutSwapBuffers();
}

void destroy_renderer(sprite_renderer_t *sr) {
	if (!sr)
		return;
	glDeleteProgram(sr->shader);
	glDeleteVertexArrays(1, &sr->vao);
	free(sr);
}

void cleanup() {
	game_state_t *gs = get_game_state();
	wave_ctx **f = &gs->item_pick;
	wave_ctx **l = &gs->victory;

	while (f <= l)
		destroy_audio_sample(*f++);
	destroy_audio_player(gs->sound_player);
	destroy_renderer(gs->renderer);

	glDeleteTextures(1, &gs->background_tex);
	glDeleteTextures(1, &gs->robot_tex);
	glDeleteTextures(1, &gs->i_score_tex);
	glDeleteTextures(1, &gs->i_life_tex);
	glDeleteTextures(1, &gs->i_bomb_tex);
	glDeleteTextures(25, gs->explosion_tex);

	free(gs->nimbus);
	
	free(gs->robots);
	free(gs->items);
	free(gs->rspawns);
	free(gs->ispawns);
	free(gs->bmp);
	free(gs);
}

void kill_robot(robot_t *c, death_reason_t reason) {
	printf("Killed because of %d\n", reason);
	if (reason == D_LOST || reason == D_BROKEN) {
		send_command(c, CMD_DESTROY);
		exec_cmd_stream(c);
	}
	if (kill(c->process.pid, SIGKILL) == -1)
		perror("Couldn't kill robot");
	spawn_item((pitem_t){c->priv.pos, ITEM_EXPLOSION, 0});
	c->dead = reason;
}

void kill_timeout(int s) {
	(void)s;
	if (!can_jump)
		return;
	if (s == SIGALRM)
		siglongjmp(jb, D_HANGED);
	if (s == SIGPIPE) {
		siglongjmp(jb, D_EXITED);
	}
}

void exec_cmd_stream(robot_t *c) {
	request_t r;
	int idx;
	int i;
	pitem_t *item;

	//ualarm(50000, 0);
	do {
		exact_read(c->process.stdout, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			exact_read(c->process.stdout, &c->state, sizeof(int) * 2);
#define ABS(x) (x < 0 ? -x : x)
			if (c->state.lin_eng_state != 0)
				c->state.lin_eng_state /= ABS(c->state.lin_eng_state);
			if (c->state.rot_eng_state != 0)
				c->state.rot_eng_state /= ABS(c->state.rot_eng_state);
#undef ABS
			break;
		case REQ_USE_ITEM:
			exact_read(c->process.stdout, &idx, sizeof(idx));
			i = idx;
			idx = (idx < c->state.bag_size && idx >= 0);
			if (write(c->process.stdin, &idx, sizeof(idx)) == -1)
				perror("server write failed");
			if (idx) {
				item = &c->priv.bag_buffer[i];
				collectable_item_vtable[item->type].activate(item, c);
			}
			break;
		case REQ_END:
			break;
		default:
			printf("UREQ %d\n", r);
			sleep(1);
			r = REQ_END;
		}
	} while (r != REQ_END);
	ualarm(0, 0);
}

void exec_cmd(robot_t *c) {
	send_command(c, CMD_UPDATE_STATE);
	write(c->process.stdin, &c->state, sizeof(c->state));
	write(c->process.stdin, c->state.bag, sizeof(item_t) * c->state.bag_size);
	write(c->process.stdin, c->state.depth_buffer, sizeof(float) * c->state.rays);
	write(c->process.stdin, c->state.obj_attr_buffer, sizeof(int) * c->state.rays);
	send_command(c, CMD_UPDATE);
	exec_cmd_stream(c);
}

void end_game(robot_t *c) {
	game_state_t *gs = get_game_state();
	int i;

	if (gs->finished)
		return;
	for (i = 0; i < gs->n_robots; ++i) {
		if (c != &gs->robots[i])
			kill_robot(&gs->robots[i], D_LOST);
	}
	play_wav_async(gs->sound_player, gs->victory);
	gs->finished = 1;
}

void coalesce_robots() {
	game_state_t *gs = get_game_state();
	int i = 0;
	robot_t * c;

	while (i < gs->n_robots) {
		if (gs->robots[i].dead) {
			c = &gs->robots[i];
			free(c->state.bag);
			free(c->state.depth_buffer);
			free(c->state.obj_attr_buffer);
			free(c->priv.obj_idx_buffer);
			free(c->priv.bag_buffer);
			glDeleteTextures(1, &c->priv.name_sprite.tid);
			gs->n_robots--;
			memmove(c, c + 1, sizeof(robot_t) * (gs->n_robots - i));
			continue;
		}
		++i;
	}
}

void load_ressources(game_state_t *gs, char *map) {
	// Charge objets et map
	const char *textures[] = {
		map,
		"assets/robot.tga",
		"assets/i_score.tga",
		"assets/i_life.tga",
		"assets/i_bomb.tga",
		"assets/i_base.tga"
	};
	int n_textures = sizeof(textures) / sizeof(textures[0]);
	const char *sfx[] = {
		"assets/item_pick.wav",
		"assets/bomb_drop.wav",
		"assets/explosion.wav",
		"assets/victory.wav"
	};
	int nsounds = sizeof(sfx) / sizeof(sfx[0]);
	bitmap_t btmp;
	char fn[25];
	bitmap_t *tex = malloc(sizeof(bitmap_t) * n_textures);
	unsigned *t = &gs->background_tex;
	int i;
	embedded_t file;
	wave_ctx **p;
	// map 
	for (i = 0; i < n_textures; ++i) {
		if (!load_tga(&tex[i], textures[i])) {
			fprintf(stderr, "Couldn't load texture '%s'\n", textures[i]);
			exit(1);
		}
		if (!make_texture(&t[i], tex[i])) {
			fprintf(stderr, "Couldn't create texture '%s'\n", textures[i]);
			exit(1);
		}
		if (i)
			free(tex[i].buff);
	}

	// Charge les sons
	gs->sound_player = make_audio_player();
	p = &gs->item_pick;
	for (i = 0; i < nsounds; ++i) {
		file = get_asset(sfx[i]);
		p[i] = load_wav_data(gs->sound_player, file.data, file.size);
	}

	// Charge l'animation d'explosion
	for (i = 24; i < 49; ++i) {
		sprintf(fn, "assets/exp/%03d.tga", i);
		if (!load_tga(&btmp, fn)) {
			fprintf(stderr, "Couldn't load texture '%s'\n", fn);
			exit(1);
		}
		if (!make_texture(&gs->explosion_tex[i - 24], btmp)) {
			fprintf(stderr, "Couldn't create texture '%s'\n", fn);
			exit(1);
		}
		free(btmp.buff);
	}
	gs->nimbus = load_font("assets/nimbus20r.tga", "assets/nimbus20r.agefnt");
	if (tex[0].width != 1280 || tex[0].height != 720) {
		puts("Wrong arena dimensions");
		exit(1);
	}
	gs->bmp = tex[0].buff;
	free(tex);
}

static struct timespec start;
void update_game_state() {
	struct timespec end;
	static int frame = 0;
	long diff;
	game_state_t *game_state = get_game_state();
	robot_t *c;
	int i, j, k, n;
	float adelta;
	ray_t r;
	vec2_t my_dir;
	intersect_data_t closest_actor, wall;
	vec2_t rot;
	float f;

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	diff = nano_diff(end, start);
	if (diff < g_60fps)
		return;
	if (game_state->finished) {
		game_state->finished += diff;
		if (game_state->finished >= 5l * 1000 * 1000 * 1000) {
			kill_robot(game_state->robots, D_EXITED);
			exit(0);
		}
	}
	start = end;
	++frame;
	if (frame == 60 * 6) {
		frame = 0;
		spawn_rand_item();
	}
	// game loop
	for (i = 0; i < game_state->n_robots; ++i) {
		c = &game_state->robots[i];

		n = sigsetjmp(jb, 1);
		can_jump = 1;
		if (n) {
			ualarm(0, 0);
			kill_robot(c, n);
		}
		if (c->dead)
			continue;
		exec_cmd(c);
		my_dir = vec2_rot(vec2_up(), -c->priv.angle);
		c->priv.linear_speed = vec2_add(c->priv.linear_speed, vec2_muls(my_dir, c->prop.linear_power / c->prop.mass * c->state.lin_eng_state));
		c->priv.angular_speed += (c->prop.angular_power / c->prop.mass) * c->state.rot_eng_state;
		c->priv.linear_speed = vec2_muls(c->priv.linear_speed, 0.8f / c->prop.mass);
		c->priv.angular_speed *= 0.8f;
		c->priv.angle += c->priv.angular_speed;
		c->priv.pos = vec2_add(c->priv.pos, c->priv.linear_speed);
		assert(c->priv.pos.x == c->priv.pos.x);
		adelta = 2 * M_PI / c->state.rays;
		j = i;
		memset(c->state.depth_buffer, 0, sizeof(float) * 64);
		for (k = 0; k < c->state.rays; ++k) {
			r.dir = vec2_rot(my_dir, k * adelta);
			r.ori = vec2_add(c->priv.pos, vec2_muls(r.dir, 32.0f));
			closest_actor = raycast_scene(r, j);
			if (raycast_bitmap(r, closest_actor.depth, &wall)) {
				c->state.depth_buffer[k] = wall.depth;
				c->state.obj_attr_buffer[k] = COLL_WALL;
			} else if (!(closest_actor.type == COLL_ROBOT && closest_actor.arg.robot_id == j)) {
				c->state.depth_buffer[k] = closest_actor.depth;
				c->state.obj_attr_buffer[k] = closest_actor.type;
				c->priv.obj_idx_buffer[k] = closest_actor.arg;
			}
		}
		c->priv.rc = 0;
		c->priv.ic = 0;
		for (k = 0; k < c->state.rays; ++k) {
			if (c->state.depth_buffer[k] > 0.001f)
				continue;
			if(c->state.obj_attr_buffer[k] != COLL_NONE) {
				send_command(c, CMD_COLLISION);
				write(c->process.stdin, &c->state.obj_attr_buffer[i], sizeof(c->state.obj_attr_buffer[i]));
				exec_cmd_stream(c);
			}
			switch(c->state.obj_attr_buffer[k]) {
			case COLL_NONE: // 🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔
				break;
			case COLL_WALL:
				//c->priv.pos = opos;
				rot = vec2_rot(my_dir, k * adelta);
				if (vec2_dot(rot, c->priv.linear_speed) <= 0)
					break;
				rot = vec2_rot(my_dir, k * adelta);
				rot = vec2_sub((vec2_t) {0, 0}, rot);
				f = sqrtf(vec2_dot(c->priv.linear_speed, c->priv.linear_speed));
				c->priv.linear_speed = vec2_muls(rot, f);
				if (f < 3)
					f = 0;
				c->state.life -= f;
				c->priv.pos = vec2_add(c->priv.pos,
									   c->priv.linear_speed);
				break;
			case COLL_ITEM:
/*				if (c->priv.ic)
					break;
				c->priv.ic = 1;
				item = &game_state->items[c->priv.obj_idx_buffer[k].item_id];
				collectable_item_vtable[item->type].activate(item, c);
*/				break;
			case COLL_ROBOT:
				collision_with_robot(c, &game_state->robots[c->priv.obj_idx_buffer[k].robot_id]);
				break;
			default: //🤔🤔🤔🤔🤔🤔🤔🤔
				break;
			}
		}
		if (c->state.life <= 0) {
			kill_robot(c, D_BROKEN);
		}
	}
	coalesce_robots();

	/* 
	   TODO: En fait, même les robots aurait pu être implémenté en tant qu'objets
	   "objet" devrait plutot s'appeler "acteur"
	 */
	
	for (i = 0; i < game_state->n_items; ++i) {
		collectable_item_vtable[game_state->items[i].type].update(&game_state->items[i]);
	}
	if (!game_state->finished && game_state->n_robots == 1) {
		end_game(game_state->robots);
	}
	for (i = 0; i < game_state->n_robots; ++i) {
		if (game_state->robots[i].state.score >= 10) {
			end_game(&game_state->robots[i]);
		}
	}
	glutPostRedisplay();
}

/*
  Chaque pixel rouge est le point de spawn d'un robot
  Chaque pixel bleu est le point de spawn d'un robot

  Cette fonction sauvegarde les positions de ces pixels

  Il faut entre 2 et 8 spawn pour les robots, et plus de 6 pour les objets.
 */
void fill_rspawns() {
	game_state_t *gs = get_game_state();
	int x, y;

	gs->nrspawns = 0;
	gs->nispawns = 0;
	gs->rspawns = malloc(sizeof(vec2_t) * gs->nrspawns);
	gs->ispawns = malloc(sizeof(vec2_t) * gs->nispawns);
	for (y = 0; y < 720; ++y) {
		for (x = 0; x < 1280; ++x) { // On regarde tout les pixels de l'image
			if ((gs->bmp[y * 1280 + x] & 0x00FFFFFF) == 0x00FF0000) {
				// Ajoute un spawn de robot
				gs->rspawns = realloc(gs->rspawns, sizeof(vec2_t) * (gs->nrspawns + 1));
				gs->rspawns[gs->nrspawns++] = (vec2_t) {x, y};
			}
			if ((gs->bmp[y * 1280 + x] & 0x00FFFFFF) == 0x000000FF) {
				// Ajoute un spawn d'objet
				gs->ispawns = realloc(gs->ispawns, sizeof(vec2_t) * (gs->nispawns + 1));
				gs->ispawns[gs->nispawns++] = (vec2_t) {x, y};
			}
		}
	}
	if (gs->nrspawns > 8 || gs->nrspawns < 2 || gs->nispawns < 6) {
		fputs("Not enough spawn points found", stderr);
		exit(2);
	}
}

int main(int argc, char *argv[argc])
{
	struct sigaction s;
	sigset_t ss;
	game_state_t *gs;
	robot_t *ro;
	int all;
	int i;
	robot_t *c;

	if (argc < 3) {
		return 1;
	}

	/*
	  On a besoin d'intercepter certains signaux.
	  SIGALRM pour interompre une execution trop lente
	  SIGPIPE pour au cas ou on fait de l'entrée/sortie sur un fils mort 
	*/
	sigemptyset(&ss);
	s.sa_handler = kill_timeout;
	s.sa_mask = ss;
	s.sa_flags = 0;
	sigaction(SIGALRM, &s, 0);
	s.sa_flags = SA_RESTART;
	sigaction(SIGPIPE, &s, 0);

	// Boilerplate glut/glew/gl
	atexit(cleanup);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitWindowSize(1280, 720);
	// Impossible de créer une fenetre de taille fixe... Thanks glut.
	glutCreateWindow("Robutt");
	glutDisplayFunc(display_game_state);
	glutIdleFunc(update_game_state);
	srand(time(0));
	glInit();
	// ./a.out map.bmp [bots]...
	gs = get_game_state();
	load_ressources(gs, argv[1]);
	fill_rspawns();

	gs->renderer = make_sprite_renderer();
	if (argc - 2 > 8) {
		puts("Il y a trop de robots!");
		exit(1);
	}

	ro = make_robots(argc - 2, argv + 2);
	all = 0;
	for (i = 0; i < argc - 2; ++i) {
		c = ro + i;
		if (!init_robot(c, gs->rspawns[all++]))
			c->state.life = -100.0f;
		spawn_item((pitem_t) {
				c->priv.pos,
				ITEM_BASE, c
		});
	}
	
	gs->robots = ro;
	gs->n_robots = argc - 2;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	glutMainLoop();
    return 0;
}
