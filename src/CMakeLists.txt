LINK_DIRECTORIES(
	${CMAKE_SOURCE_DIRECTORY}
)

INCLUDE_DIRECTORIES(
	${PROJECT_BINARY_DIR}/include
	${AUDIO_INCLUDE_DIRS}
)

FUNCTION(APPEND var suffix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${f}${suffix}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(APPEND)

########## SONS ############

FILE(GLOB SOUNDFILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/assets/*.wav)

APPEND(SOUNDFILESO .o ${SOUNDFILES})
FOREACH(SFX IN LISTS SOUNDFILES)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${SFX}.o
	COMMAND mkdir -p assets
	COMMAND gcc '-DFILE="${PROJECT_SOURCE_DIR}/${SFX}"' '-DNAME="${SFX}"' -c ../../src/res.S -o ${SFX}.o
	DEPENDS ../${SFX} res.S
	)
ENDFOREACH(SFX IN LISTS SOUNDFILES)


ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/sounds.o
  COMMAND ld -r ${SOUNDFILESO} -o ${PROJECT_SOURCE_DIR}/src/sounds.o
  DEPENDS ${SOUNDFILESO}
)

########### FIN SONS ###############

########## SHADERS ############

FILE(GLOB SHADERSFILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/assets/*.glsl)

APPEND(SHADERSFILESO .o ${SHADERSFILES})
FOREACH(SFX IN LISTS SHADERSFILES)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${SFX}.o
	COMMAND mkdir -p assets
	COMMAND gcc '-DFILE="${PROJECT_SOURCE_DIR}/${SFX}"' '-DNAME="${SFX}"' -c ../../src/res.S -o ${SFX}.o
	DEPENDS ../${SFX} res.S
	)
ENDFOREACH(SFX IN LISTS SHADERSFILES)


ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/shaders.o
  COMMAND ld -r ${SHADERSFILESO} -o ${PROJECT_SOURCE_DIR}/src/shaders.o
  DEPENDS ${SHADERSFILESO}
)

########### FIN SHADERS ###############

########## POLICE ############

FILE(GLOB POLICEFILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/assets/*.agefnt)

APPEND(POLICEFILESO .o ${POLICEFILES})

FOREACH(SFX IN LISTS POLICEFILES)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${SFX}.o
	COMMAND mkdir -p assets
	COMMAND gcc '-DFILE="${PROJECT_SOURCE_DIR}/${SFX}"' '-DNAME="${SFX}"' -c ../../src/res.S -o ${SFX}.o
	DEPENDS ../${SFX} res.S
	)
ENDFOREACH(SFX IN LISTS POLICEFILES)

ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/fonts.o
  COMMAND ld -r ${POLICEFILESO} -o ${PROJECT_SOURCE_DIR}/src/fonts.o
  DEPENDS ${POLICEFILESO}
)

########### FIN POLICE ###############

########## TEXTURES ############

FILE(GLOB_RECURSE TEXTUREFILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/assets/*.tga)

APPEND(TEXTUREFILESO .o ${TEXTUREFILES})
FOREACH(SFX IN LISTS TEXTUREFILES)
  get_filename_component(SFX_DIR ${SFX} DIRECTORY)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${SFX}.o
	COMMAND mkdir -p ${SFX_DIR}
	COMMAND gcc '-DFILE="${PROJECT_SOURCE_DIR}/${SFX}"' '-DNAME="${SFX}"' -c ../../src/res.S -o ${SFX}.o
	DEPENDS ../${SFX} res.S
	)
ENDFOREACH(SFX IN LISTS TEXTUREFILES)


ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/textures.o
  COMMAND ld -r ${TEXTUREFILESO} -o ${PROJECT_SOURCE_DIR}/src/textures.o
  DEPENDS ${TEXTUREFILESO}
)

########### FIN SHADERS ###############

ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/assets.o
  COMMAND ld -r ${PROJECT_SOURCE_DIR}/src/sounds.o ${PROJECT_SOURCE_DIR}/src/shaders.o ${PROJECT_SOURCE_DIR}/src/fonts.o ${PROJECT_SOURCE_DIR}/src/textures.o -o ${PROJECT_SOURCE_DIR}/src/assets.o
  DEPENDS sounds.o shaders.o fonts.o textures.o)


ADD_EXECUTABLE(${PROJECT_NAME}
  # helpers
  os.c
  fs.c
  # Components
  robot.c
  shader.c
  texture.c
  sprite.c
  items.c
  scene.c
  # Math
  vec2.c
  vec3.c
  vec4.c
  mat3.c
  mat4.c
  # Polices
  age_font.c
  # Main
  mgl.c
  robutt.c
  audio${AUDIO_BACKEND}.c
  assets_manager.c
  assets.o
  )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  ${GL_LIBRARIES}
  glut
  GLU
  m
  ${AUDIO_LIBRARIES}
)

SET_TARGET_PROPERTIES(${PROJECT_NAME}
  PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -DASSET_MANAGER"
  LINK_FLAGS "-T ${PROJECT_SOURCE_DIR}/build/s")

ADD_EXECUTABLE(human_bot
  # helpers
  librobutts.c
  human_bot.c
  shader.c
  fs.c
  vec2.c
  vec3.c
  vec4.c
  mat3.c
  mat4.c
  mgl.c
  assets_manager.c
  assets.o
)

TARGET_LINK_LIBRARIES(human_bot
  glfw
  ${GL_LIBRARIES}
  glut
  m
)

SET_TARGET_PROPERTIES(human_bot
  PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -DASSET_MANAGER"
  LINK_FLAGS "-T ${PROJECT_SOURCE_DIR}/build/s")

ADD_EXECUTABLE(visualizer
  # helpers
  librobutts.c
  visualizer.c
  shader.c
  fs.c
  os.c
  mgl.c
  vec2.c
  vec3.c
  vec4.c
  mat3.c
  mat4.c
  assets_manager.c
  assets.o
)

TARGET_LINK_LIBRARIES(visualizer
  glfw
  ${GL_LIBRARIES}
  glut
  m
)

SET_TARGET_PROPERTIES(visualizer
  PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -DASSET_MANAGER"
  LINK_FLAGS "-T ${PROJECT_SOURCE_DIR}/build/s")

ADD_EXECUTABLE(switch
  # helpers
  librobutts.c
  switch.c
  os.c
  mgl.c
)

TARGET_LINK_LIBRARIES(switch
  glfw
  ${GL_LIBRARIES}
  m
  glut
)

ADD_EXECUTABLE(forward
  # helpers
  librobutts.c
  forward_bot.c
)

ADD_EXECUTABLE(rbf
  # helpers
  librobutts.c
  fs.c
  bf.c
)

ADD_EXECUTABLE(remote_client
  librobutts.c
  remote_client.c
)


INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
