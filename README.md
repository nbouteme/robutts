# Instructions
------------

## Avec GNU Make

Allez dans la racine du projet:
`cd robutts`

`make` ou `make ENABLE_AUDIO=y`

Normalement le tout compile.

Les exécutables sont placés dans le dossier build.

## Avec CMake

Allez dans le dossier build du projet:
`cd robutts/build`

`cmake ..` ou `cmake .. -DENABLE_AUDIO=y`

puis

`make`

Les exécutables sont placés dans le dossier build.

# Utilisation

Lancer une partie avec:

`./robutts assets/back.tga ./human_bot ./forward`

Le second paramêtre est le chemin vers un fichier, qui, s'il n'existe
pas, est substitué par un fichier embarqué dans l'exécutable.

