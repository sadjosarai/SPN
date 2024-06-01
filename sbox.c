#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définir la taille maximale du fichier d'entrée
#define MAX_FILE_SIZE 1000
// Définir la taille du tableau pour stocker les bits
#define MAX_BITS_SIZE (MAX_FILE_SIZE * 8)

// Définir la S-boîte comme un tableau 2D
// Cet exemple suppose une entrée de 4 bits et une sortie de 4 bits
int S_BOX[16] = {
    0x0, 0x4, 0x2, 0xc,
    0x3, 0x8, 0xa, 0x5,
    0xe, 0x7, 0x1, 0xf,
    0xd, 0xb, 0x9, 0x6
};

// Fonction pour effectuer la substitution en utilisant la S-boîte
int substituer(int entree) {
    return S_BOX[entree];
}

// Fonction pour lire un fichier en entrée et stocker son contenu dans une variable
char* lireFichier(const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", nomFichier);
        exit(EXIT_FAILURE);
    }

    char* contenu = (char*)malloc(MAX_FILE_SIZE * sizeof(char));
    if (contenu == NULL) {
        printf("Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }

    int c;
    size_t i = 0;
    while ((c = fgetc(fichier)) != EOF && i < MAX_FILE_SIZE - 1) {
        contenu[i++] = (char)c;
    }
    contenu[i] = '\0'; // Ajouter le caractère de fin de chaîne

    fclose(fichier);
    return contenu;
}

// Fonction pour convertir un caractère en binaire et le stocker dans un tableau
void convertirEnBinaire(char caractere, char* binaire, int index) {
    // Convertir le caractère en binaire
    for (int i = 7; i >= 0; --i) {
        binaire[index++] = (caractere & (1 << i)) ? '1' : '0';
    }
}

int main() {
    // Lire le contenu du fichier en entrée
    char* entreeFichier = lireFichier("./file.txt");
    printf("Contenu du fichier : %s\n", entreeFichier);

    // Tableau pour stocker les bits
    char bits[MAX_BITS_SIZE];
    int index = 0;

    // Convertir chaque caractère du fichier en binaire
    for (size_t i = 0; i < strlen(entreeFichier); ++i) {
        convertirEnBinaire(entreeFichier[i], bits, index);
        index += 8; // Avancer de 8 bits pour le prochain caractère
    }

    // Afficher les bits convertis
    printf("Bits convertis : %s\n", bits);

    // Exemple d'utilisation de la S-boîte
    int entree = 0x5; // Exemple d'entrée
    int sortie = substituer(entree);
    printf("Entrée: 0x%x, Sortie: 0x%x\n", entree, sortie);

    // Libérer la mémoire allouée pour le contenu du fichier
    free(entreeFichier);

    return 0;
}
