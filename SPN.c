#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    // Convertir chaque entier en binaire (4 bits) et les concaténer
    int binaryBlock[16];
    for (int i = 0; i < 4; i++) {
        intToBinary4Bits(block[i], &binaryBlock[i * 4]);
    }

    // Appliquer la permutation
    int permutedBlock[16];
    for (int j = 0; j < 16; j++) {
        permutedBlock[j] = binaryBlock[pi_pbox[1][j]];
    }

    // Diviser en blocs de 4 bits et convertir en décimal
    for (int i = 0; i < 4; i++) {
        block[i] = binaryToDecimal(&permutedBlock[i * 4]);
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
            (*s_key)[i][j] = decimalArray[i + j];
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

// Fonction pour traiter le fichier d'entrée et appliquer les opérations SPN
void processFile(const char *filename, int **s_key, int numLines, int N) {
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

    // Afficher le tableau binaire complet pour le texte
    printf("Tableau binaire complet pour le texte:\n");
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

    // Créer le tableau textBloc
    int **textBloc = malloc((numBlocks / 4) * sizeof(int *));
    for (size_t i = 0; i < numBlocks / 4; i++) {
        textBloc[i] = malloc(4 * sizeof(int));
        for (int j = 0; j < 4; j++) {
            textBloc[i][j] = decimalArray[i * 4 + j];
        }
    }

    // Afficher le tableau textBloc initial
    printf("\nTableau textBloc initial:\n");
    for (size_t i = 0; i < numBlocks / 4; i++) {
        printIntArray(textBloc[i], 4);
    }

    // Effectuer les opérations SPN pour i allant de 1 à N-1
    for (int i = 1; i < N; i++) {
        printf("\nÉtape %d:\n", i);

        // XOR entre chaque ligne de textBloc et la ligne i-1 de s_key
        for (size_t j = 0; j < numBlocks / 4; j++) {
            xorBlocks(textBloc[j], s_key[i - 1], 4);
        }

        // Afficher le tableau textBloc après XOR
        printf("Tableau textBloc après XOR avec la sous-clé %d:\n", i - 1);
        for (size_t j = 0; j < numBlocks / 4; j++) {
            printIntArray(textBloc[j], 4);
        }

        // Substitution
        for (size_t j = 0; j < numBlocks / 4; j++) {
            substituteColumns(textBloc[j], pi_sbox);
        }

        // Afficher le tableau textBloc après substitution
        printf("Tableau textBloc après substitution:\n");
        for (size_t j = 0; j < numBlocks / 4; j++) {
            printIntArray(textBloc[j], 4);
        }

        // Permutation
        for (size_t j = 0; j < numBlocks / 4; j++) {
            permuteColumns(textBloc[j], pi_pbox);
        }

        // Afficher le tableau textBloc après permutation
        printf("Tableau textBloc après permutation:\n");
        for (size_t j = 0; j < numBlocks / 4; j++) {
            printIntArray(textBloc[j], 4);
        }
    }

    // Dernière étape: XOR, substitution, XOR
    printf("\nDernière étape:\n");

    // XOR avec la sous-clé N-1
    for (size_t j = 0; j < numBlocks / 4; j++) {
        xorBlocks(textBloc[j], s_key[N - 1], 4);
    }

    // Afficher le tableau textBloc après XOR avec la sous-clé N-1
    printf("Tableau textBloc après XOR avec la sous-clé %d:\n", N - 1);
    for (size_t j = 0; j < numBlocks / 4; j++) {
        printIntArray(textBloc[j], 4);
    }

    // Substitution
    for (size_t j = 0; j < numBlocks / 4; j++) {
        substituteColumns(textBloc[j], pi_sbox);
    }

    // Afficher le tableau textBloc après substitution
    printf("Tableau textBloc après substitution:\n");
    for (size_t j = 0; j < numBlocks / 4; j++) {
        printIntArray(textBloc[j], 4);
    }

    // XOR avec la sous-clé N
    for (size_t j = 0; j < numBlocks / 4; j++) {
        xorBlocks(textBloc[j], s_key[N], 4);
    }

    // Afficher le tableau textBloc après XOR avec la sous-clé N
    printf("Tableau textBloc après XOR avec la sous-clé %d:\n", N);
    for (size_t j = 0; j < numBlocks / 4; j++) {
        printIntArray(textBloc[j], 4);
    }

    // Réunir les blocs de bits en blocs de 8 bits, convertir en ASCII et afficher
    char *finalText = malloc((numBlocks / 2) * sizeof(char));
    for (size_t i = 0; i < numBlocks / 2; i++) {
        int binary8Bits[8];
        intToBinary4Bits(textBloc[i / 2][0], &binary8Bits[0]);
        intToBinary4Bits(textBloc[i / 2][1], &binary8Bits[4]);
        finalText[i] = (char)binaryToDecimal(binary8Bits);
    }

    printf("\nTexte final:\n%s\n", finalText);

    // Libération de la mémoire allouée
    free(text);
    free(asciiCodes);
    free(binaryArray);
    for (size_t i = 0; i < numBlocks; i++) {
        free(block4Array[i]);
    }
    free(block4Array);
    free(decimalArray);
    for (size_t i = 0; i < numBlocks / 4; i++) {
        free(textBloc[i]);
    }
    free(textBloc);
    free(finalText);
}

int main() {
    int **s_key;
    int numLines;
    int N = 4;

    // Générer les sous-clés
    generateSubkeys("key.txt", &s_key, &numLines);

    // Traiter le fichier d'entrée
    processFile("file.txt", s_key, numLines, N);

    // Libération de la mémoire allouée pour s_key
    for (int i = 0; i < numLines; i++) {
        free(s_key[i]);
    }
    free(s_key);

    return 0;
}
