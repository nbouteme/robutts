VARS_OLD := $(.VARIABLES)

CFLAGS = -std=gnu11 -g -Wall -Wextra -Werror -Iinclude
LFLAGS = 
eq = $(and $(findstring x$1,x$2), $(findstring x$2,x$1))

ENABLE_AUDIO ?=

AUDIO_BACKEND = dummy
AUDIO_LIBRARIES =
AUDIO_CFLAGS =

$(shell pkg-config libpulse)
PULSEAUDIO_FOUND := $(.SHELLSTATUS)

$(shell pkg-config openal freealut)
OPENAL_FOUND := $(.SHELLSTATUS)

BACKEND = 
$(if $(call eq,$(OPENAL_FOUND),0),\
	$(eval BACKEND = OPENAL)\
	$(eval backend = openal)\
	$(eval OPENAL_LIBRARIES = pkg-config --libs openal freealut)\
	$(eval OPENAL_CFLAGS = pkg-config --cflags openal freealut))


$(if $(call eq,$(PULSEAUDIO_FOUND),0),\
	$(eval BACKEND = PULSEAUDIO)\
	$(eval backend = pulseaudio)\
	$(eval PULSEAUDIO_LIBRARIES = $(shell pkg-config --libs libpulse))\
	$(eval PULSEAUDIO_CFLAGS = $(shell pkg-config --cflags libpulse)))

$(if $(ENABLE_AUDIO),\
	$(eval AUDIO_BACKEND := $(backend))\
	$(eval AUDIO_LIBRARIES := $($(BACKEND)_LIBRARIES))\
	$(eval AUDIO_CFLAGS := $($(BACKEND)_LFLAGS))\
	,)

CFLAGS += $(AUDIO_CFLAGS)

GL_LIBRARIES = $(shell pkg-config gl --libs)
GL_CFLAGS = $(shell pkg-config gl --cflags)

TARGETS = robutts human_bot heavy_bot visualizer switch forward rbf remote_client remote_server remote_bclient remote_bserver

robutts_SRC := 	os.c fs.c robot.c shader.c texture.c sprite.c items.c\
				scene.c vec2.c vec3.c vec4.c mat3.c mat4.c age_font.c mgl.c robutt.c\
				audio$(AUDIO_BACKEND).c assets_manager.c assets.res

robutts_CFLAGS := -DASSET_MANAGER
robutts_LFLAGS := $(GL_LIBRARIES) -lm -lglut -lGLU $(AUDIO_LIBRARIES) -T build/s

human_bot_SRC := 	librobutts.c human_bot.c shader.c fs.c os.c vec2.c\
					vec3.c vec4.c mat3.c mat4.c mgl.c assets_manager.c assets.res texture.c sprite.c
human_bot_CFLAGS := -DASSET_MANAGER
human_bot_LFLAGS := $(GL_LIBRARIES) -lm -lglut -T build/s

heavy_bot_SRC := 	librobutts.c heavy_human.c shader.c fs.c os.c vec2.c\
					vec3.c vec4.c mat3.c mat4.c mgl.c assets_manager.c assets.res
heavy_bot_CFLAGS := -DASSET_MANAGER
heavy_bot_LFLAGS := $(GL_LIBRARIES) -lm -lglut -T build/s

visualizer_SRC := 	librobutts.c visualizer.c shader.c fs.c os.c vec2.c\
					vec3.c vec4.c mat3.c mat4.c mgl.c assets_manager.c assets.res
visualizer_CFLAGS := -DASSET_MANAGER
visualizer_LFLAGS := $(GL_LIBRARIES) -lm -lglut -T build/s

switch_SRC := librobutts.c switch.c os.c mgl.c
switch_LFLAGS := $(GL_LIBRARIES) -lm -lglut

forward_SRC := librobutts.c os.c forward_bot.c
rbf_SRC := librobutts.c os.c fs.c bf.c

remote_client_SRC := librobutts.c os.c remote_client.c
remote_bclient_SRC := librobutts.c os.c remote_bclient.c

remote_server_SRC := remote_server.c
remote_bserver_SRC := remote_bserver.c

remote_bclient_LFLAGS := -lpthread


BUILD_DIR = ./build/

$(foreach target, $(TARGETS), \
	$(eval $(BUILD_DIR)$(target)_SRC := $(addprefix ./src/, $($(target)_SRC)))\
	$(eval $(BUILD_DIR)$(target)_OBJ := $($(target)_SRC:.c=.o))\
	$(eval $(BUILD_DIR)$(target)_OBJP := $(addprefix ./bin/$(target)_objs/, $($(BUILD_DIR)$(target)_OBJ)))\
	$(eval $(target)_CFLAGS := $(CFLAGS) $($(target)_CFLAGS))\
	$(eval $(target)_LFLAGS := $(LFLAGS) $($(target)_LFLAGS))\
	$(eval $(BUILD_DIR)$(target)_CFLAGS := $(CFLAGS) $($(target)_CFLAGS))\
	$(eval $(BUILD_DIR)$(target)_LFLAGS := $(LFLAGS) $($(target)_LFLAGS))\
	$(eval $(BUILD_DIR)$(target)_NAME := $(target))\
)

TARGETS := $(addprefix $(BUILD_DIR), $(TARGETS))

#TODO: Utiliser des patterns static?

all: $(TARGETS)

ASSETS = $(shell find assets -type f)
ASSETS_EMBEDDED_RESSOURCES = $(addsuffix .er,$(ASSETS))
$(shell mkdir -p ./bin/assets/exp)

define BUILD_RES
bin/$1.er: $1
	gcc -DFILE=\"$$<\" -DNAME=\"$$<\" -c src/res.S -o $$@
endef

$(foreach res,$(ASSETS),\
	$(eval $(call BUILD_RES,$(res))))

ASSETS_EMBEDDED_RESSOURCES := $(addprefix ./bin/,$(ASSETS_EMBEDDED_RESSOURCES))

define BUILD_FILE_RULE
./bin/$1_objs/$2: ./src/$(2:.o=.c)
	$$(CC) $($1_CFLAGS) -c $$< -o $$@

endef

define BUILD_TARGET_RULE
$(shell mkdir -p ./bin/$($1_NAME)_objs)
./bin/$($1_NAME)_objs:
	mkdir -p $$@

./bin/$($1_NAME)_objs/assets.res: $(ASSETS_EMBEDDED_RESSOURCES)
	ld -r $$^ -o $$@

$(foreach obj,$(filter %.o,$($1_OBJ)),\
	$(call BUILD_FILE_RULE,$($1_NAME),$(obj)))

$1: $($1_OBJP)
	echo $1 $1_LFLAGS
	$$(CC) $$($1_LFLAGS) $$($1_OBJP) -o $1
endef

$(foreach target, $(TARGETS), \
	$(eval $(call BUILD_TARGET_RULE,$(target))))
