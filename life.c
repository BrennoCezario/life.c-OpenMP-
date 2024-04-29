/*
 * O Jogo da Vida
 *
 * Uma célula nasce se tiver exatamente três vizinhos.
 * Uma célula morre de solidão se tiver menos de dois vizinhos.
 * Uma célula morre de superpopulação se tiver mais de três vizinhos.
 * Uma célula sobrevive à próxima geração se não morrer de solidão
 * ou superpopulação.
 *
 * Nesta versão, é usado um array 2D de ints. 1 indica célula viva, 0
 * indica célula morta. O jogo avança por um número de passos (dado pela entrada),
 * imprimindo na tela a cada passo. 'x' impresso significa célula viva, espaço significa célula morta.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef unsigned char celula_t;

celula_t **alocar_tabuleiro(int tamanho) {
    celula_t **tabuleiro = (celula_t **) malloc(sizeof(celula_t*) * tamanho);
    int i;
    for (i = 0; i < tamanho; i++)
        tabuleiro[i] = (celula_t *) malloc(sizeof(celula_t) * tamanho);
    return tabuleiro;
}

void liberar_tabuleiro(celula_t **tabuleiro, int tamanho) {
    int i;
    for (i = 0; i < tamanho; i++)
        free(tabuleiro[i]);
    free(tabuleiro);
}

/* Retorna o número de células vivas adjacentes à célula (i, j) */
int celulas_adjacentes(celula_t **tabuleiro, int tamanho, int i, int j) {
    int k, l, count = 0;
   
    int sk = (i > 0) ? i - 1 : i;
    int ek = (i + 1 < tamanho) ? i + 1 : i;
    int sl = (j > 0) ? j - 1 : j;
    int el = (j + 1 < tamanho) ? j + 1 : j;

    for (k = sk; k <= ek; k++)
        for (l = sl; l <= el; l++)
            count += tabuleiro[k][l];
    count -= tabuleiro[i][j];
   
    return count;
}

void jogar(celula_t **tabuleiro, celula_t **novo_tabuleiro, int tamanho) {
    int i, j, a;

    #pragma omp parallel for private(i, j, a) shared(tabuleiro, novo_tabuleiro, tamanho)
    for (i = 0; i < tamanho; i++) {
        for (j = 0; j < tamanho; j++) {
            a = celulas_adjacentes(tabuleiro, tamanho, i, j);
            if (a == 2)
                novo_tabuleiro[i][j] = tabuleiro[i][j];
            else if (a == 3)
                novo_tabuleiro[i][j] = 1;
            else if (a < 2 || a > 3)
                novo_tabuleiro[i][j] = 0;
        }
    }
}

/* Imprime o tabuleiro do Jogo da Vida */
void imprimir(celula_t **tabuleiro, int tamanho) {
    int i, j;
    /* Para cada linha */
    for (j = 0; j < tamanho; j++) {
        /* Imprimir cada posição da coluna... */
        for (i = 0; i < tamanho; i++)
            printf("%c", tabuleiro[i][j] ? 'x' : ' ');
        /* Seguido por uma quebra de linha */
        printf("\n");
    }
}

/* Lê um arquivo para inicializar o tabuleiro do Jogo da Vida */
void ler_arquivo(FILE *f, celula_t **tabuleiro, int tamanho) {
    int i, j;
    char *s = (char *) malloc(tamanho + 10);
    char c;
    for (j = 0; j < tamanho; j++) {
        /* Obter uma string */
        fgets(s, tamanho + 10, f);
        /* Copiar a string para o tabuleiro do Jogo da Vida */
        for (i = 0; i < tamanho; i++)
            tabuleiro[i][j] = s[i] == 'x';
    }
}

int main() {
    int tamanho, passos;
    FILE *f;
    f = stdin;
    fscanf(f, "%d %d", &tamanho, &passos);
    celula_t **anterior = alocar_tabuleiro(tamanho);
    ler_arquivo(f, anterior, tamanho);
    fclose(f);
    celula_t **proximo = alocar_tabuleiro(tamanho);
    celula_t **tmp;
    int i, j;
    #ifdef DEBUG
    printf("Inicial \n");
    imprimir(anterior, tamanho);
    printf("----------\n");
    #endif

    for (i = 0; i < passos; i++) {
        jogar(anterior, proximo, tamanho);
        #ifdef DEBUG
        printf("%d ----------\n", i);
        imprimir(proximo, tamanho);
        #endif
        tmp = proximo;
        proximo = anterior;
        anterior = tmp;
    }
    imprimir(anterior, tamanho);
    liberar_tabuleiro(anterior, tamanho);
    liberar_tabuleiro(proximo, tamanho);

    return 0;
}
