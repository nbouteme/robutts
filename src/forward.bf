[
L'operation n'est pas init -
[
pas Update State -
[
Pas update -
[
Pas Destroy -
[
Pas collect -
[
Pas Collision = WTF = | (halt)
]
// Collision
| // Rien a faire donc halt
]
// Collect
TODO: utiliser immediatement l'objet pour tester
| // Rien a faire donc halt
]
// Destroy
| // Rien a faire donc halt
]

// Update
// On va juste aller tout droit
>>>>> // Passe les registres
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> // Passe les propriétés (flemme de faire une boucle)
>passe l'état rotation
+| // On met a 1 et on arrete
]

// Update State // normalement rien a faire et n'arrive jamais car l'interpreteur se charge de mettre a jour l'etat lui meme
| // halt

]

L'operation est init:

> // Passe l'operation
> // Passe Argument
> // Passe MR
> // Passe ARG
> // Passe SVC
[-]+++++++> // Met la puissance angulaire à 0, puis 7/5
[-]+++++++> // Met la puissance lineaire à 0, puis 7/5
[-]+++++> // Met la masse à 0, puis 5/5
// On laisse les couleurs par défaut
alpha>
blue>
green>
red>
+++++ // gf interpret
| // halt