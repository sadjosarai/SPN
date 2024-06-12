# Implémentation d'un SPN
### Réalisé par SADJO LEGBANE Saraï, Etudiant en Master à l'université de Yaoundé I, au Cameroun

## Introduction
Bonjour.
Ceci est une implémentation d'un 'Substitution-Permutation-Network', utilisé par les algorithme de chiffrement par bloc tel que DES.

## Déroulement (Fonctionnement)
    1. Le programme contenu dans 'SPN.c' va lire le texte dans dans le fichier 'file.txt', et celui du fichier 'key.txt'.

    'file.txt' contient le texte à chiffrer et 'key.txt' contient la clé de chiffrement.
    Vous pouvez changer le contenu de chaque fichier.

    2. il va ensuite convertir chaque caractère en code ascii décimal correspondant, puis en binaire, et les séparé par bloc de 4 bits ( nous avons choisi des blocs de 4 pour la simplicité et pour respecter l'exemple vu en cours). Les blocs issues de 'file.txt' sont obtenu simplement par séparation, mais ceux issue de 'key.txt' sont obtenu en prenant les 4 premiers bits pour le premier bloc, puis le bit 2 au bit 5 pour le second bloc, puis le bit 3 au bit 6 pour le troisième, et ainsi de suite jusqu'au 4 dernier bits pour le dernier bloc.

    3. Chaque bloc de 4 bits issue de est enfin converti en décimal. On obtient ainsi, deux matrice 'textbloc' et 's_key' de taille 4 x m et 4 x n, où 's_key' correspond à un système de sous clé obtenu à partir de la clé initiale.

    4. Ensuite maintenant commence les opération de Substitution, de permutation et d'addition. pour i allant de 1 à N - 1 fois, 
        a. chaque ligne de 'textbloc' est additionné (XOR) à la i-eme ligne de 's_key'

        b.puis les valeur  décimal de chaque colonne de chaque ligne du 'textbloc' obtenu sont substitué suivant 'pi_sbox' (la valeur chaque colonne de 'textbloc' correspondante à une valeur de la première ligne de 'pi_sbox est subtitué par celle de la seconde ligne de 'pi_sbox' de la même colonne).

        c. Chaque colonne de chaque ligne du 'textbloc' obtenu après substitution est converti en binaire de taille 4, puis chaque de 'textbloc' contenant à présent 16 bits subi une permutation de bit par position, suivant 'pi_pbox'.  
    
    5. à la N fois, le 'textbloc' obtenu à l'issu de la séquence précédente, subi une opération d'addition avec la N-eme ligne de 's_key', puis une subtitutiion et enfin une dernière addition avec la ligne N+1 de 's_key'.

    6. Chaque colonne de 'textbloc' est ensuite converti en binaire de 4 bits, puis concatener pour obtenir des octets, qui seront converti en décimal, et enfin en caractère ascii correspondant.



    Le N choisi ici par défaut est 4, encore une fois pour des raisons de simplicité, mais vous pouvez le modifier dans le code.

## Requis

Pour exécuter ce code, il vous faut :

    - Un système d'exploitation (windows ou linux)
    - Un Compilateur C et un débogger, gcc et gdb.
    - Un IDE (VSCode, codeBlock ou autre) supportant le langage C.