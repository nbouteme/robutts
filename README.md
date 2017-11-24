Instructions
------------

`git clone --depth=1 https://github.com/nbouteme/robutts`

`cd robutts/build`

`cmake ..` ou `cmake -DENABLE_AUDIO=y ..`

`make -j4`

Normalement le tout compile.

Lancer une partie avec:

`./robutts assets/back.tga ./human_bot`

Le second paramêtre est le chemin vers un fichier, qui, s'il n'existe
pas, est substitué par un fichier embarqué dans l'exécutable.

