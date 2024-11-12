#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

int playerX = 34, playerY = MAXY - 2;  // Posição inicial do jogador
int numCars = 5;                       // Número de "carros" ou obstáculos (aumentado para 5)
int carX[5] = {15, 35, 55, 25, 45};    // Novas posições iniciais dos carros
int centerY = MAXY / 2;                // Linha central onde os carros vão passar
int carSpeed[5] = {1, -1, 1, -1, 1};   // Direções dos carros (1 = direita, -1 = esquerda)

// Função para desenhar o jogador
void drawPlayer(int x, int y) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(x, y);
    printf("O");
}

// Função para desenhar os carros
void drawCars() {
    screenSetColor(RED, DARKGRAY);
    for (int i = 0; i < numCars; i++) {
        screenGotoxy(carX[i], centerY);
        printf("===");
    }
}

// Função para limpar o jogador na posição anterior
void clearPlayer(int x, int y) {
    screenGotoxy(x, y);
    printf(" ");
}

// Função para atualizar a posição dos carros
void updateCars() {
    for (int i = 0; i < numCars; i++) {
        screenGotoxy(carX[i], centerY);
        printf("   ");  // Limpa a posição anterior do carro
        carX[i] += carSpeed[i];
        if (carX[i] >= MAXX - 3 || carX[i] <= 1) {
            carSpeed[i] = -carSpeed[i];  // Inverte a direção ao atingir as bordas
        }
    }
}

// Função para verificar colisão entre jogador e carro
int checkCollision() {
    for (int i = 0; i < numCars; i++) {
        if (playerY == centerY && playerX >= carX[i] && playerX <= carX[i] + 2) {
            return 1;  // Colisão detectada
        }
    }
    return 0;
}

// Função para mover o jogador de acordo com a entrada do usuário
void movePlayer(int key) {
    clearPlayer(playerX, playerY);  // Limpa a posição anterior do jogador
    if (key == 'w' && playerY > 0) playerY--;         // Move para cima
    if (key == 's' && playerY < MAXY - 1) playerY++;  // Move para baixo
    if (key == 'a' && playerX > 0) playerX--;         // Move para esquerda
    if (key == 'd' && playerX < MAXX - 1) playerX++;  // Move para direita
    drawPlayer(playerX, playerY);
}

int main() {
    int ch = 0;
    screenInit(1);
    keyboardInit();
    timerInit(100);

    drawPlayer(playerX, playerY);
    screenUpdate();
    drawCars();  // Desenha os carros inicialmente

    while (1) {
        if (keyhit()) {
            ch = readch();
            if (ch == 27) break;  // Tecla Esc para sair
            movePlayer(ch);
            screenUpdate();
        }

        if (timerTimeOver()) {
            updateCars();
            drawCars();
            if (checkCollision()) {
                screenGotoxy(30, MAXY / 2);
                screenSetColor(LIGHTRED, BLACK);
                printf("Game Over! Pressione ESC para sair.");
                break;
            }
            screenUpdate();
        }

        if (playerY == 1) {  // Verifica se o jogador chegou ao topo
            screenGotoxy(30, MAXY / 2);
            screenSetColor(LIGHTGREEN, BLACK);
            printf("Você Venceu! Pressione ESC para sair.");
            break;
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
