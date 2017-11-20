Robutt (Robot-Headbutt)
======

Les spécifications minimales sont écrites normalement, et les avancées
sont en italique.

Par spécification minimales, j'entends par la le minimum de
fonctionnalitées requise pour considérer le projet comme fonctionel.

Le programme prend un parametre une map au format tga 32 bit non
compressé, et une suite de nom du fichier de programmes de robots, *ou
bien une chaine correspondant à la commande (pour passer des
arguments)*.

Tout les pixels rouges de la map correspondent aux zone de spawn des
robots, les pixels bleus correspondent aux spawn des items. Il ne doit
pas y avoir plus de robots que de point de spawn. Les items spawnent
sur un point bleu aléatoirement. *Si un robot est sur le point bleu au
moment de générer l'item, une explosion est générée à la place.*
Un amas de pixels noirs placé dans un arrangement très spécifique
correspond à un mur impassable, sinon traversable.

Les robots possèdent des caractéristique:
- La puissance de leur moteur linéraire
- La puissance de leur moteur angulaire
- Leur masse

La somme des trois ne peut dépasser 50. Les unités sont arrangées par
le biais d'une formule pour équilibrer.

Une plus grand puissance de moteur permet de se déplacer et de freiner
plus rapidement. Une grande masse demande plus de puissance pour se
déplacer et freiner mais cause de plus gros dégat lors d'un choc et
subit moins de dégats en contrepartie. Mais entrer en collision avec
un mur cause de lourds dégats. L'inertie fait que le robot parcourera
une plus longue distance lorsqu'il coupera son moteur linéraire.
Etre plus léger rend plus mobile mais plus fragile.

Lorsqu'un robot meurt, *une explosion à sa position est générée* et il
disparait. Il  y a  deux causes  possibles de mort  d'un robot  en cas
normal:
- Son programme a mis trop de temps à répondre
- Sa vie est tombée en dessous de 0

En plus de cela s'ajoute des causes un peu moins naturelles:
- Son programme est mort
- Son programme a fournit une réponse que le serveur ne reconnait pas

Pour perdre de la vie, un robot doit foncer dans un mur, ou dans un
autre robot, ou etre pris dans une explosion.  Lorsqu'il y a un
contact avec un mur, la vie perdue est proportionelle avec la vitesse
du robot, sa masse, et l'angle d'approche (toucher un mur par le coté
cause moins de dégats que foncer droit dedans)

Lorsqu'il y a un contact avec un autre robot, les deux prennent des
dégats en fonction de leur vitesse, masse, et direction respective.

Si un robot est pris dans une explosion, il prends des dégats et un
knockback selon sa position par rapport au centre de l'explosion.  Un
robot prendra de lourds dégats si le souffle d'une explosion le jette
dans un mur, par exemple.

Pour gagner, un robot doit être le seul robot vivant, ou bien
collecter un certain nombre de points. Un point est un item dont
l'effet est activé des sa collecte.

Lorsqu'un robot collecte un item collectable, il est ajouté à une
liste d'objets, et pour l'utiliser il communique au serveur l'indice
dans le sac de l'objet à utiliser.

Il existe 6 objets, dont 3 collectable:

Le "point", qui donne un point au robot qui le ramasse.
La vie, qui rends 20% de point de vie;

Une bombe, qui peut etre placée et détonnée à distance.
Lorsque la bombe est placée, elle est invisible, elle est retiré de l'inventaire du robot, qui se voit ensuite donné un détonateur.
Lorsque le détonateur est activé, une explosion est générée aux coordonnées du placement de la bombe.
Un accelerateur, qui double la vitesse tout en divisant par 4 les dégats subits suite aux chocs, pendant 20 secondes.
*Un laser, 5px de large et actif 2 frames, qui tue tout les robots qui entrent en contact avec*

Détails d'implementation
=======

Les robots sont implémentés sous forme de sous processus qui
communiquent avec le serveur (processsus parent) par le biais de
l'entrée et sortie standard.

Cela permet d'assurer l'intégritée du serveur dans le cas ou on tente
de lancer des robots malformés et/ou malintentionnés dessus.

Cela permet aussi de ne pas limiter substantiellement la possibilité
d'implementation d'un robot, qui peut utiliser n'importe quelle
bibliotheque indépendamment de ce que supporte le serveur.

Par exemple un robot peut servir de proxy pour un autre robot sur le
réseau, ou bien de proxy pour un joueur humain. Ou bien un robot peut
servir de proxy pour lire un programme qui défini le robot.

Pour cela un robot doit implémenter l'interface définie par librobutt,
qui à son minimum peut etre absolument rien.