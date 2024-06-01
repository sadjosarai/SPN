#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définir la taille maximale du fichier d'entrée
#define MAX_FILE_SIZE 1000
// Définir la taille du tableau pour stocker les bits
#define MAX_BITS_SIZE (MAX_FILE_SIZE * 8)

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

// Fonction pour diviser les bits en blocs de 16 et les stocker dans des variables
int diviserEnBlocs(char* contenuInitial, char** blocs) {
    // Convertir le contenu initial en binaire
    char contenuInitialEnBinaire[MAX_BITS_SIZE];
    int index = 0;
    for (size_t i = 0; i < strlen(contenuInitial); ++i) {
        convertirEnBinaire(contenuInitial[i], contenuInitialEnBinaire, index);
        index += 8; // Avancer de 8 bits pour le prochain caractère
    }

    // Calculer le nombre total de blocs
    int tailleContenuInitial = strlen(contenuInitial);
    int nombreTotalBlocs = tailleContenuInitial / 16 + (tailleContenuInitial % 16 != 0);

    // Diviser les bits en blocs de 16
    for (int i = 0; i < nombreTotalBlocs; ++i) {
        char bloc[17]; // Un bloc contient 16 bits et un caractère de fin de chaîne
        int j;
        for (j = 0; j < 16; ++j) {
            if (i * 16 + j < tailleContenuInitial) {
                bloc[j] = contenuInitialEnBinaire[i * 16 + j];
            } else {
                bloc[j] = '0'; // Compléter avec des zéros si nécessaire
            }
        }
        bloc[j] = '\0'; // Ajouter le caractère de fin de chaîne
        strcpy(blocs[i], bloc);
    }

    return nombreTotalBlocs;
}

int main() {
    // Lire le contenu du fichier en entrée
    char* entreeFichier = lireFichier("file.txt");
    printf("Contenu du fichier : %s\n", entreeFichier);

    // Tableau pour stocker les blocs de 16 bits
    char* blocs[MAX_FILE_SIZE / 16];
    for (int i = 0; i < MAX_FILE_SIZE / 16; ++i) {
        blocs[i] = (char*)malloc(17 * sizeof(char)); // 16 bits + 1 pour le caractère de fin de chaîne
    }

    // Diviser les bits en blocs de 16
    int nombreTotalBlocs = diviserEnBlocs(entreeFichier, blocs);

    // Afficher les blocs de 16 bits
    printf("Nombre total de blocs : %d\n", nombreTotalBlocs);
    for (int i = 0; i < nombreTotalBlocs; ++i) {
        printf("Bloc %d : %s\n", i + 1, blocs[i]);
    }

    // Libérer la mémoire allouée pour le contenu du fichier et les blocs
    free(entreeFichier);
    for (int i = 0; i < MAX_FILE_SIZE / 16; ++i) {
        free(blocs[i]);
    }

    return 0;
}
