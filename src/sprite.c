#include <robutts.h>
#include <sprite.h>
#include <shaders.h>

sprite_renderer_t *make_sprite_renderer() {
	sprite_renderer_t *self = malloc(sizeof(*self));
	int v = load_shader("assets/vshader.glsl", GL_VERTEX_SHADER);
	int f = load_shader("assets/fshader.glsl", GL_FRAGMENT_SHADER);
	self->shader = make_shader(v, f);
	glUseProgram(self->shader);
	unsigned vbo;
    float vertices[] = { 
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &self->vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(self->vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	self->proj_u = glGetUniformLocation(self->shader, "proj");
	self->model_u = glGetUniformLocation(self->shader, "model");
	self->color_u = glGetUniformLocation(self->shader, "color");

	int u = glGetUniformLocation(self->shader, "tex");
	glUniform1i(u, 0);
	
	self->proj = mat4_ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

	glUniformMatrix4fv(self->proj_u, 1, GL_FALSE, &self->proj.s[0].x);
	glUseProgram(0);
	return self;
}

void draw_sprite(sprite_renderer_t *self, sprite_t sprite) {
	glUseProgram(self->shader);

	// Pas super optimal, surtout dans le cas d'objet statiques où le model change jamais 
	mat4_t model = mat4_identity();
	model = mat4_mult(model, mat4_translate((vec3_t){sprite.pos.x, sprite.pos.y, 0})); // déplacé de pos
	model = mat4_mult(model, mat4_translate((vec3_t){sprite.dims.x * 0.5f, sprite.dims.y  * 0.5f, 0})); // centré
	model = mat4_mult(model, mat4_rotation((vec3_t){0, 0, 1}, sprite.angle));
	model = mat4_mult(model, mat4_translate((vec3_t){-sprite.dims.x * 0.5f, -sprite.dims.y * 0.5f, 0})); // décentré
	model = mat4_mult(model, mat4_scale((vec3_t){sprite.dims.x, sprite.dims.y, 1.0f})); // agrandis
	glUniformMatrix4fv(self->model_u, 1, GL_TRUE, &model.s[0].x);

	float color[3];
	color[0] = (float)(sprite.color >> 16 & 0xFF) / 255.0f;
	color[1] = (float)(sprite.color >>  8 & 0xFF) / 255.0f;
	color[2] = (float)(sprite.color >>  0 & 0xFF) / 255.0f;
	glUniform3fv(self->color_u, 1, color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite.tid);
    glBindVertexArray(self->vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
	glUseProgram(0);
}
