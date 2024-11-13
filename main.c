#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

int jogadorX = 34, jogadorY = MAXY - 2;  // Posição inicial do jogador
int vidas = 3;
int nivel = 1;                           // Nível atual do jogo
int numCarros = 5;                       // Número de "carros" ou obstáculos (aumenta a cada nível)
int carrosX[10] = {15, 35, 55, 25, 45, 20, 40, 60, 30, 50};  // Posições iniciais dos carros
int linhasCarros[10] = {MAXY / 2, MAXY / 2 + 2, MAXY / 2 - 2, MAXY / 2 + 4, MAXY / 2 - 4};  // Linhas dos carros
int direcaoCarros[10] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1};  // Direções dos carros (1 = direita, -1 = esquerda)

// Função para exibir mensagens temporárias
void exibirMensagem(const char *mensagem, int tempo) {
    screenClear();
    screenGotoxy(MAXX / 2 - strlen(mensagem) / 2, MAXY / 2);
    screenSetColor(YELLOW, BLACK);
    printf("%s", mensagem);
    screenUpdate();
    timerDelay(tempo);  // Pausa para exibir a mensagem
    screenClear();
}

// Função para desenhar a borda ao redor do campo de jogo
void desenharBorda() {
    screenSetColor(WHITE, BLACK);
    for (int x = 0; x < MAXX; x++) {
        screenGotoxy(x, 0); printf("#");
        screenGotoxy(x, MAXY - 1); printf("#");
    }
    for (int y = 0; y < MAXY; y++) {
        screenGotoxy(0, y); printf("#");
        screenGotoxy(MAXX - 1, y); printf("#");
    }
}

// Função para exibir as vidas restantes do jogador
void exibirVidas() {
    screenSetColor(YELLOW, BLACK);
    screenGotoxy(2, 1);
    printf("Vidas: ");
    for (int i = 0; i < 3; i++) {
        if (i < vidas) {
            printf("❤ ");  // Exibe um coração para cada vida restante
        } else {
            printf("  ");  // Espaço vazio para vidas perdidas
        }
    }
}

// Função para desenhar o jogador
void desenharJogador(int x, int y) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(x, y);
    printf("O");
}

// Função para desenhar os carros
void desenharCarros() {
    screenSetColor(RED, DARKGRAY);
    for (int i = 0; i < numCarros; i++) {
        screenGotoxy(carrosX[i], linhasCarros[i]);
        printf("===");
    }
}

// Função para limpar o jogador na posição anterior
void limparJogador(int x, int y) {
    screenGotoxy(x, y);
    printf(" ");
}

// Função para atualizar a posição dos carros
void atualizarCarros() {
    for (int i = 0; i < numCarros; i++) {
        screenGotoxy(carrosX[i], linhasCarros[i]);
        printf("   ");  // Limpa a posição anterior do carro
        carrosX[i] += direcaoCarros[i];
        if (carrosX[i] >= MAXX - 3 || carrosX[i] <= 1) {
            direcaoCarros[i] = -direcaoCarros[i];  // Inverte a direção ao atingir as bordas
        }
    }
}

// Função para verificar colisão entre jogador e carro
int verificarColisao() {
    for (int i = 0; i < numCarros; i++) {
        if (jogadorY == linhasCarros[i] && jogadorX >= carrosX[i] && jogadorX <= carrosX[i] + 2) {
            return 1;  // Colisão detectada
        }
    }
    return 0;
}

// Função para mover o jogador de acordo com a entrada do usuário
void moverJogador(int tecla) {
    limparJogador(jogadorX, jogadorY);  // Limpa a posição anterior do jogador
    if (tecla == 'w' && jogadorY > 1) jogadorY--;         // Move para cima
    if (tecla == 's' && jogadorY < MAXY - 2) jogadorY++;  // Move para baixo
    if (tecla == 'a' && jogadorX > 1) jogadorX--;         // Move para esquerda
    if (tecla == 'd' && jogadorX < MAXX - 2) jogadorX++;  // Move para direita
    desenharJogador(jogadorX, jogadorY);
}

int main() {
    int tecla = 0;
    screenInit(1);
    keyboardInit();
    timerInit(100);

    exibirMensagem("Bem-vindo! Pressione qualquer tecla para iniciar", 2);

    desenharBorda();
    desenharJogador(jogadorX, jogadorY);
    desenharCarros();
    exibirVidas();
    screenUpdate();

    while (1) {
        if (keyhit()) {
            tecla = readch();
            if (tecla == 27) break;  // Tecla Esc para sair
            moverJogador(tecla);
            screenUpdate();
        }

        if (timerTimeOver()) {
            atualizarCarros();
            desenharCarros();

            if (verificarColisao()) {
                vidas--;
                exibirVidas();
                if (vidas > 0) {
                    exibirMensagem("Colisão! Vidas restantes: ", 2);
                    jogadorX = 34;
                    jogadorY = MAXY - 2;  // Reset posição inicial do jogador
                    desenharJogador(jogadorX, jogadorY);
                } else {
                    exibirMensagem("Game Over! Pressione ESC para sair.", 3);
                    break;
                }
            }

            if (jogadorY == 1) {  // Verifica se o jogador chegou ao topo
                nivel++;
                numCarros += 2;  // Aumenta o número de carros a cada nível
                exibirMensagem("Parabéns! Próximo Nível", 2);

                for (int i = 0; i < numCarros; i++) {
                    carrosX[i] = (i * 10) % (MAXX - 3);
                    linhasCarros[i] = MAXY / 2 + (i % 5) * 2;
                }

                jogadorX = 34;
                jogadorY = MAXY - 2;
                desenharJogador(jogadorX, jogadorY);
            }
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
