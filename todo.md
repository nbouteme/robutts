- Chercher toutes les lignes contenant "TODO" et faire ce qu'elles disent
- Rendre optionnelle la dépendance sur OpenAL et ALUT
- Remplacer CMake par Make
- Créer des interpréteurs de robots supplémentaires
- Faire des tests de gestions de cas d'erreur

Notes:

Pour compiler le projet, il faut cmake.
- mkdir build
- cd build
- cmake ..
- make

Le programme human_bot.c se compile comme ceci (en se mettant dans le dossier src):
gcc -DUSE_GLEW librobutts.c human_bot.c shader.c mat4.c mat3.c fs.c vec2.c -lglfw -lGLEW -lm -lGL -I../include -Wall -g -Wall -pedantic -o heavy

Le projet lui meme se lance avec la ligne suivant, en se placant dans le dossier build.
./robutts back.tga ../src/heavy ../src/heavy

Il faut avant aussi copier les shaders (.glsl) du dossier assets dans le dossier build car human_bot en utilise.
