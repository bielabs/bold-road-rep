#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Defina MAXX e MAXY se não estiverem definidos nos headers incluídos
#ifndef MAXX
#define MAXX 80
#endif

#ifndef MAXY
#define MAXY 25
#endif

int jogadorX = 34, jogadorY = MAXY - 2;  // Posição inicial do jogador
int vidas = 3;
int nivel = 1;                           // Nível atual do jogo
int numCarros = 5;                       // Número de "carros" ou obstáculos (aumenta a cada nível)
int carrosX[15] = {15, 35, 55, 25, 45, 20, 40, 60, 30, 50, 10, 70, 5, 75, 65};  // Aumentei para 15 para suportar níveis superiores
int linhasCarros[15] = {MAXY / 2, MAXY / 2 + 2, MAXY / 2 - 2, MAXY / 2 + 4, MAXY / 2 - 4, MAXY / 2 + 6, MAXY / 2 - 6, MAXY / 2 + 8, MAXY / 2 - 8, MAXY / 2 + 10, MAXY / 2 - 10, MAXY / 2 + 12, MAXY / 2 - 12, MAXY / 2 + 14, MAXY / 2 - 14};  // Adicionado para suportar mais carros
int direcaoCarros[15] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1};  // Direções dos carros (1 = direita, -1 = esquerda)

// Função para selecionar o emoji do carro baseado no nível atual
const char* selecionarEmojiCarro(int i) {
    if (nivel == 1) return (i % 3 == 0) ? "👩🏻‍🦼" : (i % 3 == 1) ? "🚴🏻" : "👨🏻‍🦽";
    if (nivel == 2) return (i % 3 == 0) ? "🚙" : (i % 3 == 1) ? "🚗" : "🚕";
    if (nivel == 3) return (i % 3 == 0) ? "🚌" : (i % 3 == 1) ? "🚚" : "🚐";
    if (nivel == 4) return (i % 3 == 0) ? "🚓" : (i % 3 == 1) ? "🚑" : "🚒";
    if (nivel == 5) return (i % 3 == 0) ? "🚙" : (i % 3 == 1) ? "🚗" : "🚕"; 
    return (i % 15 == 0) ? "🛸" : (i % 15 == 1) ? "👩🏻‍🦼" : (i % 15 == 2) ? "🚴🏻" :
           (i % 15 == 3) ? "👨🏻‍🦽" : (i % 15 == 4) ? "🚙" : (i % 15 == 5) ? "🚗" :
           (i % 15 == 6) ? "🚕" : (i % 15 == 7) ? "🚌" : (i % 15 == 8) ? "🚚" :
           (i % 15 == 9) ? "🚐" : (i % 15 == 10) ? "🚓" : (i % 15 == 11) ? "🚑" :
           (i % 15 == 12) ? "🚒" : (i % 15 == 13) ? "🚙" : "🚗";  // Substituição de 🏍 e 🏎
}

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
void desenharJogador(int x, int y, int emMovimento) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(x, y);
    printf("%s", emMovimento ? "🏃" : "🧍‍♂️");
}

// Função para desenhar os carros
void desenharCarros() {
    screenSetColor(RED, DARKGRAY);
    for (int i = 0; i < numCarros; i++) {
        screenGotoxy(carrosX[i], linhasCarros[i]);
        printf("%s", selecionarEmojiCarro(i));
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
        printf(" ");  // Limpa a posição anterior do carro
        carrosX[i] += direcaoCarros[i];
        if (carrosX[i] >= MAXX - 2 || carrosX[i] <= 1) {  // Ajustei para -2 para evitar sobreposição com a borda
            direcaoCarros[i] = -direcaoCarros[i];  // Inverte a direção ao atingir as bordas
        }
    }
}

// Função para verificar colisão entre jogador e carro
int verificarColisao() {
    for (int i = 0; i < numCarros; i++) {
        if (jogadorY == linhasCarros[i] && 
            jogadorX >= carrosX[i] && jogadorX <= carrosX[i] + 1) {  // Ajustei para +1 para considerar largura dos emojis
            return 1;  // Colisão detectada
        }
    }
    return 0;
}

// Função para mover o jogador de acordo com a entrada do usuário
void moverJogador(int tecla) {
    limparJogador(jogadorX, jogadorY);  // Limpa a posição anterior do jogador
    int emMovimento = 0;  // Indica se o jogador está se movendo

    if (tecla == 'w' && jogadorY > 1) { jogadorY--; emMovimento = 1; }         // Move para cima
    if (tecla == 's' && jogadorY < MAXY - 2) { jogadorY++; emMovimento = 1; }  // Move para baixo
    if (tecla == 'a' && jogadorX > 1) { jogadorX--; emMovimento = 1; }         // Move para esquerda
    if (tecla == 'd' && jogadorX < MAXX - 2) { jogadorX++; emMovimento = 1; }  // Move para direita

    desenharJogador(jogadorX, jogadorY, emMovimento);
}

int main() {
    int tecla = 0;
    screenInit(1);
    keyboardInit();
    timerInit(100);

    exibirMensagem("Bem-vindo! Pressione qualquer tecla para iniciar", 2);

    desenharBorda();
    desenharJogador(jogadorX, jogadorY, 0);  // Passa 0 para emMovimento
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
                    char mensagem[50];
                    snprintf(mensagem, sizeof(mensagem), "Colisão! Vidas restantes: %d", vidas);
                    exibirMensagem(mensagem, 2);
                    jogadorX = 34;
                    jogadorY = MAXY - 2;  // Reset posição inicial do jogador
                    desenharJogador(jogadorX, jogadorY, 0);  // Passa 0 para emMovimento
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
                    linhasCarros[i] = MAXY / 2 + (i % 7) * 2;  // Ajustei para suportar mais linhas
                }

                jogadorX = 34;
                jogadorY = MAXY - 2;
                desenharJogador(jogadorX, jogadorY, 0);  // Passa 0 para emMovimento
            }
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

