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

FILE(GLOB_RECURSE ASSETSFILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/assets/*)

APPEND(ASSETSFILESO .o ${ASSETSFILES})
FOREACH(SFX IN LISTS ASSETSFILES)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${SFX}.o
	COMMAND mkdir -p assets/exp
	COMMAND gcc '-DFILE="${PROJECT_SOURCE_DIR}/${SFX}"' '-DNAME="${SFX}"' -c ../../src/res.S -o ${SFX}.o
	DEPENDS ../${SFX} res.S
	)
ENDFOREACH(SFX IN LISTS ASSETSFILES)

ADD_CUSTOM_COMMAND(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/assets.o
  COMMAND ld -r ${ASSETSFILESO} -o ${PROJECT_SOURCE_DIR}/src/assets.o
  DEPENDS ${ASSETSFILESO}
)

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
  sprite.c
  texture.c
  os.c
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
  ${GL_LIBRARIES}
  glut
  m
)

SET_TARGET_PROPERTIES(human_bot
  PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -DASSET_MANAGER"
  LINK_FLAGS "-T ${PROJECT_SOURCE_DIR}/build/s")

ADD_EXECUTABLE(heavy_bot
  # helpers
  librobutts.c
  heavy_human.c
  shader.c
  fs.c
  os.c
  vec2.c
  vec3.c
  vec4.c
  mat3.c
  mat4.c
  mgl.c
  assets_manager.c
  assets.o
)

TARGET_LINK_LIBRARIES(heavy_bot
  ${GL_LIBRARIES}
  glut
  m
)

SET_TARGET_PROPERTIES(heavy_bot
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
  ${GL_LIBRARIES}
  m
  glut
)

ADD_EXECUTABLE(forward
  # helpers
  librobutts.c
  os.c
  forward_bot.c
)

ADD_EXECUTABLE(rbf
  # helpers
  librobutts.c
  fs.c
  os.c
  bf.c
)

ADD_EXECUTABLE(remote_client
  librobutts.c
  remote_client.c
  os.c
)

ADD_EXECUTABLE(remote_server
  remote_server.c
)

ADD_EXECUTABLE(remote_bclient
  librobutts.c
  remote_bclient.c
  os.c
)

TARGET_LINK_LIBRARIES(remote_bclient
  pthread
)

ADD_EXECUTABLE(remote_bserver
  remote_bserver.c
  os.c
)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
