#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Tableaux statiques pi_sbox et pi_pbox
int pi_sbox[2][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
};

int pi_pbox[2][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
};

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

// Fonction pour imprimer un tableau d'entiers
void printIntArray(const int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

// Fonction pour convertir un entier en binaire (sur 8 bits)
void intToBinary8Bits(int n, int *binaryArray) {
    for (int i = 7; i >= 0; i--) {
        binaryArray[i] = n % 2;
        n /= 2;
    }
}

// Fonction pour convertir un entier en binaire (sur 4 bits)
void intToBinary4Bits(int n, int *binaryArray) {
    for (int i = 3; i >= 0; i--) {
        binaryArray[i] = n % 2;
        n /= 2;
    }
}

// Fonction pour imprimer un tableau binaire
void printBinaryArray(const int *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d", array[i]);
        if ((i + 1) % 4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

// Fonction pour convertir un tableau de binaire (4 bits) en entier
int binaryToDecimal(const int *binaryArray) {
    int value = 0;
    for (int i = 0; i < 4; i++) {
        value = value * 2 + binaryArray[i];
    }
    return value;
}

// Fonction pour appliquer la substitution
void substituteColumns(int **tables, size_t numTables, int pi_sbox[2][16]) {
    for (size_t i = 0; i < numTables; i++) {
        for (int j = 0; j < 4; j++) {
            int value = tables[i][j];
            for (int k = 0; k < 16; k++) {
                if (pi_sbox[0][k] == value) {
                    tables[i][j] = pi_sbox[1][k];
                    break;
                }
            }
        }
    }
}

// Fonction pour appliquer la permutation
void permuteColumns(int **tables, size_t numTables, int pi_pbox[2][16]) {
    for (size_t i = 0; i < numTables; i++) {
        int tempTable[16];
        for (int j = 0; j < 16; j++) {
            tempTable[j] = tables[i][j];
        }
        for (int j = 0; j < 16; j++) {
            int index = 0;
            for (int k = 0; k < 16; k++) {
                if (pi_pbox[0][k] == j) {
                    index = k;
                    break;
                }
            }
            tables[i][j] = tempTable[pi_pbox[1][index]];
        }
    }
}

// Fonction pour lire le fichier key.txt et générer le tableau s_key
void generateSubkeys(const char *filename) {
    size_t size;

    // Lire le fichier et stocker le contenu dans un tableau
    char *text = readFile(filename, &size);

    // Convertir chaque caractère en code ASCII
    int *asciiCodes = malloc(size * sizeof(int));
    if (!asciiCodes) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        asciiCodes[i] = (int)text[i];
    }

    // Convertir chaque code ASCII en binaire
    int *binaryArray = malloc(size * 8 * sizeof(int));
    if (!binaryArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        intToBinary8Bits(asciiCodes[i], &binaryArray[i * 8]);
    }

    // Afficher le tableau binaire complet
    printf("Tableau binaire complet pour la clé:\n");
    printBinaryArray(binaryArray, size * 8);

    // Convertir chaque bloc de 4 bits en décimal
    size_t numBlocks = (size * 8) / 4;
    int *decimalArray = malloc(numBlocks * sizeof(int));
    if (!decimalArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    int tempBlock[4];
    for (size_t i = 0; i < numBlocks; i++) {
        for (int j = 0; j < 4; j++) {
            tempBlock[j] = binaryArray[i * 4 + j];
        }
        decimalArray[i] = binaryToDecimal(tempBlock);
    }

    // Déterminer le nombre de lignes pour s_key (entre 5 et 8)
    srand(time(NULL));
    int numLines = (rand() % 4) + 5;

    // Créer le tableau s_key
    int s_key[numLines][16];
    int index = 0;
    for (int i = 0; i < numLines; i++) {
        for (int j = 0; j < 16; j++) {
            if (index < numBlocks) {
                s_key[i][j] = decimalArray[index++];
            } else {
                s_key[i][j] = 0; // Remplir avec des zéros si les données sont insuffisantes
            }
        }
    }

    // Afficher le tableau s_key
    printf("\nTableau s_key généré:\n");
    for (int i = 0; i < numLines; i++) {
        printIntArray(s_key[i], 16);
    }

    // Libération de la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    free(decimalArray);
}

// Fonction pour traiter le fichier file.txt et effectuer les opérations de substitution et permutation
void processFile(const char *filename) {
    size_t size;

    // Lire le fichier et stocker le contenu dans un tableau
    char *text = readFile(filename, &size);

    // Convertir chaque caractère en code ASCII
    int *asciiCodes = malloc(size * sizeof(int));
    if (!asciiCodes) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        asciiCodes[i] = (int)text[i];
    }

    // Convertir chaque code ASCII en binaire
    int *binaryArray = malloc(size * 8 * sizeof(int));
    if (!binaryArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        intToBinary8Bits(asciiCodes[i], &binaryArray[i * 8]);
    }

    // Afficher le tableau binaire complet
    printf("\nTableau binaire complet pour file.txt:\n");
    printBinaryArray(binaryArray, size * 8);

    // Convertir chaque bloc de 4 bits en décimal
    size_t numBlocks = (size * 8) / 4;
    int *decimalArray = malloc(numBlocks * sizeof(int));
    if (!decimalArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    int tempBlock[4];
    for (size_t i = 0; i < numBlocks; i++) {
        for (int j = 0; j < 4; j++) {
            tempBlock[j] = binaryArray[i * 4 + j];
        }
        decimalArray[i] = binaryToDecimal(tempBlock);
    }

    // Créer des tableaux de blocs de 4 pour les opérations suivantes
    size_t numTables = numBlocks / 4;
    int **tables = malloc(numTables * sizeof(int *));
    for (size_t i = 0; i < numTables; i++) {
        tables[i] = malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            tables[i][j] = decimalArray[i * 4 + j];
        }
    }

    // Afficher les tableaux avant substitution
    printf("\nTableaux avant substitution:\n");
    for (size_t i = 0; i < numTables; i++) {
        printIntArray(tables[i], 4);
    }

    // Effectuer la substitution
    substituteColumns(tables, numTables, pi_sbox);

    // Afficher les tableaux après substitution
    printf("\nTableaux après substitution:\n");
    for (size_t i = 0; i < numTables; i++) {
        printIntArray(tables[i], 4);
    }

    // Convertir les tableaux après substitution en binaire
    int **binaryTables = malloc(numTables * sizeof(int *));
    for (size_t i = 0; i < numTables; i++) {
        binaryTables[i] = malloc(16 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            intToBinary4Bits(tables[i][j], &binaryTables[i][j * 4]);
        }
    }

    // Afficher les tableaux en binaire après substitution
    printf("\nTableaux en binaire après substitution:\n");
    for (size_t i = 0; i < numTables; i++) {
        printBinaryArray(binaryTables[i], 16);
    }

    // Effectuer la permutation
    permuteColumns(binaryTables, numTables, pi_pbox);

    // Afficher les tableaux après permutation
    printf("\nTableaux après permutation:\n");
    for (size_t i = 0; i < numTables; i++) {
        printBinaryArray(binaryTables[i], 16);
    }

    // Libération de la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    free(decimalArray);
    for (size_t i = 0; i < numTables; i++) {
        free(tables[i]);
        free(binaryTables[i]);
    }
    free(tables);
    free(binaryTables);
}

int main() {
    const char *keyFile = "key.txt";  // Fichier pour générer les sous-clés
    const char *dataFile = "file.txt"; // Fichier pour les opérations avec pi_sbox et pi_pbox

    // Générer le système de sous-clés à partir du fichier key.txt
    generateSubkeys(keyFile);

    // Traiter le fichier file.txt
    processFile(dataFile);

    return 0;
}
