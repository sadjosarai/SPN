#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Fonction pour lire le contenu d'un fichier et le stocker dans un tableau
char* readFile(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Aller à la fin du fichier pour obtenir sa taille
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    // Allouer de la mémoire pour stocker le contenu du fichier
    char *buffer = malloc(*size * sizeof(char));
    if (!buffer) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Lire le fichier dans le buffer
    fread(buffer, sizeof(char), *size, file);
    fclose(file);

    return buffer;
}

// Fonction pour imprimer un tableau de caractères
void printCharArray(const char *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%c", array[i]);
    }
    printf("\n");
}

// Fonction pour imprimer un tableau d'entiers
void printIntArray(const int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

// Fonction pour convertir un entier en binaire (sur 8 bits)
void intToBinary(int n, int *binaryArray) {
    for (int i = 7; i >= 0; i--) {
        binaryArray[i] = n % 2;
        n /= 2;
    }
}

// Fonction pour convertir un bloc de 4 bits en décimal
int binaryToDecimal(int *binaryArray) {
    int decimal = 0;
    for (int i = 0; i < 4; i++) {
        decimal = decimal * 2 + binaryArray[i];
    }
    return decimal;
}

// Fonction pour imprimer un tableau binaire
void printBinaryArray(const int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d", array[i]);
        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

// Fonction pour substituer les valeurs des colonnes selon pi_sbox
void substituteColumns(int **tables, size_t numTables, const int pi_sbox[2][16]) {
    for (size_t t = 0; t < numTables; t++) {
        for (int col = 0; col < 16; col++) {
            int value = tables[t][col];
            // Rechercher la valeur dans la ligne 1 de pi_sbox
            for (int i = 0; i < 16; i++) {
                if (pi_sbox[0][i] == value) {
                    // Substituer avec la valeur correspondante de la ligne 2
                    tables[t][col] = pi_sbox[1][i];
                    break;
                }
            }
        }
    }
}

// Fonction pour permuter les valeurs des tableaux selon pi_pbox
void permuteColumns(int **tables, size_t numTables, const int pi_pbox[2][16]) {
    for (size_t t = 0; t < numTables; t++) {
        int tempTable[16];
        for (int col = 0; col < 16; col++) {
            int index1 = -1;
            // Trouver l'indice correspondant dans la ligne 1 de pi_pbox
            for (int i = 0; i < 16; i++) {
                if (pi_pbox[0][i] == col) {
                    index1 = i;
                    break;
                }
            }
            // Utiliser cet indice pour trouver la nouvelle position dans la ligne 2 de pi_pbox
            if (index1 != -1) {
                int newIndex = pi_pbox[1][index1];
                tempTable[col] = tables[t][newIndex];
            }
        }
        // Copier les valeurs permutées dans le tableau original
        for (int col = 0; col < 16; col++) {
            tables[t][col] = tempTable[col];
        }
    }
}

int main() {
    // Initialiser les tableaux pi_sbox et pi_pbox de manière statique
    int pi_sbox[2][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
    };

    int pi_pbox[2][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
    };

    // Imprimer pi_sbox
    printf("pi_sbox:\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            printf("%d ", pi_sbox[i][j]);
        }
        printf("\n");
    }

    // Imprimer pi_pbox
    printf("\npi_pbox:\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            printf("%d ", pi_pbox[i][j]);
        }
        printf("\n");
    }

    const char *filename = "file.txt";
    size_t size;

    // Lire le fichier et stocker le contenu dans un tableau
    char *text = readFile(filename, &size);
    
    // Imprimer le tableau de caractères
    printf("\nContenu du fichier:\n");
    printCharArray(text, size);

    // Créer un tableau pour les codes ASCII
    int *asciiCodes = malloc(size * sizeof(int));
    if (!asciiCodes) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Convertir chaque caractère en code ASCII
    for (size_t i = 0; i < size; i++) {
        asciiCodes[i] = (int)text[i];
    }

    // Imprimer le tableau de codes ASCII
    printf("\nCodes ASCII:\n");
    printIntArray(asciiCodes, size);

    // Créer un tableau pour les valeurs binaires (chaque code ASCII est représenté par 8 bits)
    int *binaryArray = malloc(size * 8 * sizeof(int));
    if (!binaryArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Convertir chaque code ASCII en binaire
    for (size_t i = 0; i < size; i++) {
        intToBinary(asciiCodes[i], &binaryArray[i * 8]);
    }

    // Imprimer le tableau binaire
    printf("\nValeurs binaires:\n");
    printBinaryArray(binaryArray, size * 8);

    // Créer un tableau pour les valeurs décimales (chaque bloc de 4 bits)
    size_t numBlocks = (size * 8) / 4;
    int *decimalArray = malloc(numBlocks * sizeof(int));
    if (!decimalArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Convertir chaque bloc de 4 bits en décimal
    int tempBlock[4];
    for (size_t i = 0; i < numBlocks; i++) {
        for (int j = 0; j < 4; j++) {
            tempBlock[j] = binaryArray[i * 4 + j];
        }
        decimalArray[i] = binaryToDecimal(tempBlock);
    }

    // Imprimer les blocs de 4 bits convertis en décimal
    printf("\nValeurs décimales par bloc de 4 bits:\n");
    printIntArray(decimalArray, numBlocks);

    // Créer et imprimer les tableaux de taille 16 avant substitution
    size_t numTables = (numBlocks + 15) / 16; // Nombre de tableaux nécessaires
    int **tables = malloc(numTables * sizeof(int*));
    for (size_t i = 0; i < numTables; i++) {
        tables[i] = malloc(16 * sizeof(int));
        size_t start = i * 16;
        size_t end = (start + 16 < numBlocks) ? start + 16 : numBlocks;
        for (size_t j = start; j < end; j++) {
            tables[i][j - start] = decimalArray[j];
        }
        for (size_t j = end; j < start + 16; j++) {
            tables[i][j - start] = 0; // Remplir avec des zéros si nécessaire
        }
    }

    // Imprimer les tableaux de taille 16 avant substitution
    printf("\nTableaux de taille 16 avant substitution:\n");
    for (size_t i = 0; i < numTables; i++) {
        printf("Tableau %zu:\n", i + 1);
        printIntArray(tables[i], 16);
    }

    // Substituer les valeurs des colonnes selon pi_sbox
    substituteColumns(tables, numTables, pi_sbox);

    // Imprimer les tableaux de taille 16 après substitution
    printf("\nTableaux de taille 16 après substitution:\n");
    for (size_t i = 0; i < numTables; i++) {
        printf("Tableau %zu:\n", i + 1);
        printIntArray(tables[i], 16);
    }

    // Permuter les valeurs des colonnes selon pi_pbox
    permuteColumns(tables, numTables, pi_pbox);

    // Imprimer les tableaux de taille 16 après permutation
    printf("\nTableaux de taille 16 après permutation:\n");
    for (size_t i = 0; i < numTables; i++) {
        printf("Tableau %zu:\n", i + 1);
        printIntArray(tables[i], 16);
    }

    // Libérer la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    free(decimalArray);
    for (size_t i = 0; i < numTables; i++) {
        free(tables[i]);
    }
    free(tables);

    return 0;
}
