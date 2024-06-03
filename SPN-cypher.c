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
void substituteColumns(int *block, int pi_sbox[2][16]) {
    for (int j = 0; j < 4; j++) {
        int value = block[j];
        for (int k = 0; k < 16; k++) {
            if (pi_sbox[0][k] == value) {
                block[j] = pi_sbox[1][k];
                break;
            }
        }
    }
}

// Fonction pour appliquer la permutation
void permuteColumns(int *block, int pi_pbox[2][16]) {
    int tempBlock[16];
    for (int j = 0; j < 16; j++) {
        tempBlock[j] = block[j];
    }
    for (int j = 0; j < 16; j++) {
        int index = 0;
        for (int k = 0; k < 16; k++) {
            if (pi_pbox[0][k] == j) {
                index = k;
                break;
            }
        }
        block[j] = tempBlock[pi_pbox[1][index]];
    }
}

// Fonction pour lire le fichier key.txt et générer le tableau s_key
void generateSubkeys(const char *filename, int ***s_key, int *numLines) {
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

    // Afficher le tableau binaire complet pour la clé
    printf("Tableau binaire complet pour la clé:\n");
    printBinaryArray(binaryArray, size * 8);

    // Diviser le tableau binaire en blocs de 4 bits
    size_t numBlocks = (size * 8) / 4;
    int **block4Array = malloc(numBlocks * sizeof(int *));
    if (!block4Array) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < numBlocks; i++) {
        block4Array[i] = malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            block4Array[i][j] = binaryArray[i * 4 + j];
        }
    }

    // Convertir les blocs de 4 bits en décimal
    int *decimalArray = malloc(numBlocks * sizeof(int));
    if (!decimalArray) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < numBlocks; i++) {
        decimalArray[i] = binaryToDecimal(block4Array[i]);
    }

    // Déterminer le nombre de lignes pour s_key
    *numLines = numBlocks / 4;

    // Créer le tableau s_key de numLines lignes et 4 colonnes
    *s_key = malloc(*numLines * sizeof(int *));
    for (int i = 0; i < *numLines; i++) {
        (*s_key)[i] = malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            (*s_key)[i][j] = decimalArray[i * 4 + j];
        }
    }

    // Afficher le tableau s_key
    printf("\nTableau s_key généré:\n");
    for (int i = 0; i < *numLines; i++) {
        printIntArray((*s_key)[i], 4);
    }

    // Libération de la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    for (size_t i = 0; i < numBlocks; i++) {
        free(block4Array[i]);
    }
    free(block4Array);
    free(decimalArray);
}

// Fonction pour appliquer l'opération XOR entre deux tableaux de même taille
void xorBlocks(int *block1, const int *block2, size_t size) {
    for (size_t i = 0; i < size; i++) {
        block1[i] ^= block2[i];
    }
}

// Fonction principale pour effectuer les opérations de substitution, permutation et XOR
void processFile(const char *filename, int N, int **s_key, int numLines) {
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

    // Diviser le tableau binaire en blocs de 16 bits
    size_t numBlocks = (size * 8) / 16;
    int **blocks = malloc(numBlocks * sizeof(int *));
    if (!blocks) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < numBlocks; i++) {
        blocks[i] = malloc(16 * sizeof(int));
        for (int j = 0; j < 16; j++) {
            blocks[i][j] = binaryArray[i * 16 + j];
        }
    }

    // Afficher les blocs initiaux
    printf("Blocs initiaux:\n");
    for (size_t i = 0; i < numBlocks; i++) {
        printBinaryArray(blocks[i], 16);
    }

    // Effectuer les opérations sur chaque bloc pour chaque itération
    for (int n = 0; n < N; n++) {
        for (size_t i = 0; i < numBlocks; i++) {
            // XOR avec s_key[n % numLines]
            xorBlocks(blocks[i], s_key[n % numLines], 16);

            // Convertir chaque bloc en décimal pour les opérations suivantes
            int decimalBlock[4];
            for (int j = 0; j < 4; j++) {
                int block4Bits[4];
                for (int k = 0; k < 4; k++) {
                    block4Bits[k] = blocks[i][j * 4 + k];
                }
                decimalBlock[j] = binaryToDecimal(block4Bits);
            }

            // Substitution
            substituteColumns(decimalBlock, pi_sbox);

            // Convertir de nouveau en binaire
            for (int j = 0; j < 4; j++) {
                int block4Bits[4];
                intToBinary4Bits(decimalBlock[j], block4Bits);
                for (int k = 0; k < 4; k++) {
                    blocks[i][j * 4 + k] = block4Bits[k];
                }
            }

            // Permutation
            permuteColumns(blocks[i], pi_pbox);

            // Afficher les blocs après chaque itération
            printf("Blocs après l'itération %d:\n", n + 1);
            for (size_t i = 0; i < numBlocks; i++) {
                printBinaryArray(blocks[i], 16);
            }
        }
    }

    // Dernière étape spéciale
    for (size_t i = 0; i < numBlocks; i++) {
        // XOR avec s_key[N % numLines]
        xorBlocks(blocks[i], s_key[N % numLines], 16);

        // Convertir chaque bloc en décimal pour les opérations suivantes
        int decimalBlock[4];
        for (int j = 0; j < 4; j++) {
            int block4Bits[4];
            for (int k = 0; k < 4; k++) {
                block4Bits[k] = blocks[i][j * 4 + k];
            }
            decimalBlock[j] = binaryToDecimal(block4Bits);
        }

        // Substitution
        substituteColumns(decimalBlock, pi_sbox);

        // Convertir de nouveau en binaire
        for (int j = 0; j < 4; j++) {
            int block4Bits[4];
            intToBinary4Bits(decimalBlock[j], block4Bits);
            for (int k = 0; k < 4; k++) {
                blocks[i][j * 4 + k] = block4Bits[k];
            }
        }

        // XOR avec s_key[(N + 1) % numLines]
        xorBlocks(blocks[i], s_key[(N + 1) % numLines], 16);

        // Afficher les blocs après la dernière étape
        printf("Blocs après la dernière étape:\n");
        for (size_t i = 0; i < numBlocks; i++) {
            printBinaryArray(blocks[i], 16);
        }
    }

    // Libération de la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    for (size_t i = 0; i < numBlocks; i++) {
        free(blocks[i]);
    }
    free(blocks);
}

int main() {
    int N = 4;  // Nombre d'itérations
    int **s_key;
    int numLines;

    // Générer les sous-clés
    generateSubkeys("key.txt", &s_key, &numLines);

    // Traiter le fichier avec les sous-clés
    processFile("file.txt", N, s_key, numLines);

    // Libérer la mémoire pour les sous-clés
    for (int i = 0; i < numLines; i++) {
        free(s_key[i]);
    }
    free(s_key);

    return 0;
}
