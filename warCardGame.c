#include "raylib.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#define QTDE_PERSONAGENS 11
#define LARGURA_CARTA 250
#define ALTURA_CARTA 300

const char* CAMINHO_IMG_PERSONAGENS[11] = {
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png"
    "img/tanque.png"
    "img/tanque.png"
    "img/tanque.png"
    };


typedef struct
{
    Texture2D imgPersonagem;
    char* Nome;
    int forcaAtaque;
    int forcaDefesa;
} Personagem;

typedef struct carta
{
    Personagem personagem;
    float posX;
    float posY;
    float width;
    float height;
    bool emJogo;

    struct carta *proxima;

} Carta;


typedef struct
{
    int tam;
    Carta *carta;
} ListaCartas;

void desenhaFundo()
{

    ClearBackground(WHITE);

}

void carregarTexturas(Texture2D imagens[], int qtde)
{
    for (int i = 0; i < qtde; i++)
    {
        imagens[i] = LoadTexture(CAMINHO_IMG_PERSONAGENS[i]);
    }
}

void destroiTexturasPersonagens(Texture2D imagensPersonagens[])
{
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(imagensPersonagens[i]);
    }
}

void criaPersonagens(Personagem personagens[], Texture2D imagensPersonagens[], int qtde)
{
    Personagem personagem;

    for (int i = 0; i < qtde; i++)
    {
        personagem.Nome = "teste";
        personagem.imgPersonagem = imagensPersonagens[i];
        personagem.forcaAtaque = 10;
        personagem.forcaDefesa = 15;

        personagens[i] = personagem;
    }

}

void criaCartasDisponiveis(Carta cartasDisponiveis[], Personagem personagens[], int qtdeCartas)
{

    Carta carta;
    float xInicial = 30, yInicial = 35;
    int espacamento = 15;

    for (int i = 0; i < qtdeCartas; i++)
    {

        carta.personagem = personagens[i];
        carta.posX = xInicial + (LARGURA_CARTA + espacamento) * i;
        carta.posY = yInicial;

        carta.width = LARGURA_CARTA;
        carta.height = ALTURA_CARTA;

        carta.proxima = NULL;
        carta.emJogo = false;

        cartasDisponiveis[i] = carta;
    }
}

void desenhaCarta(Carta carta)
{
    Rectangle molde;

    molde.x = carta.posX;
    molde.y = carta.posY;
    molde.height = carta.height;
    molde.width = carta.width;

    DrawRectangleRec(molde, BLACK);
    DrawRectangleLinesEx(molde, 10, BROWN);

    DrawTexture(carta.personagem.imgPersonagem, molde.x + 20, molde.y + 25, WHITE);
}

void desenhaCartasDisponiveis(Carta cartasDisponiveis[], int qtdeCartas)
{
    Carta carta;

    int espacamento = 15;
    int qtdeColunas = (int) GetScreenWidth() / (LARGURA_CARTA + espacamento);

    if (qtdeColunas >= qtdeCartas)
    {

        for (int i = 0; i < qtdeCartas; i++)
        {
            desenhaCarta(cartasDisponiveis[i]);
        }
    }
    else
    {

        int index = 0;
        int x = -1;
        int qtdeLinhas = qtdeCartas / qtdeColunas;
        int qtde = qtdeCartas - (qtdeColunas * qtdeLinhas);

        printf("Qtde Linhas Antes %d\n", qtdeLinhas);

        qtdeLinhas += qtdeLinhas * qtdeColunas < qtdeCartas ? 1 : 0;

        printf("Qtde Cartas %d\n", qtdeCartas);
        printf("Qtde Colunas %d\n", qtdeColunas);
        printf("Qtde Linhas Depois %d\n", qtdeLinhas);

        for (int i = 0; i < qtdeLinhas; i++)
        {
            int colunas;
            if (i == qtdeLinhas - 1)
                colunas = qtde;
            else
                colunas = qtdeColunas;


            printf("Qtde Colunas %d\n", colunas);

            for (int j = 0; j < colunas; j++)
            {

                carta = cartasDisponiveis[index];

                if (i == 0 && j == 0)
                    x = carta.posX;

                if (x != -1 && i != 0)
                {
                    carta.posX = x + (LARGURA_CARTA + espacamento) * j;
                    carta.posY = ALTURA_CARTA + 50 * i;
                }

                desenhaCarta(carta);

                index ++;
            }
        }
    }
}

void jogo()
{

    Texture2D imagensPersonagens[QTDE_PERSONAGENS];
    Personagem personagens[QTDE_PERSONAGENS];
    Carta cartasDisponiveis[QTDE_PERSONAGENS];


    carregarTexturas(imagensPersonagens, QTDE_PERSONAGENS);
    criaPersonagens(personagens, imagensPersonagens, QTDE_PERSONAGENS);
    criaCartasDisponiveis(cartasDisponiveis, personagens, QTDE_PERSONAGENS);


    while (!WindowShouldClose())
    {

        BeginDrawing();

            desenhaFundo();
            desenhaCartasDisponiveis(cartasDisponiveis, QTDE_PERSONAGENS);

        EndDrawing();

    }

    destroiTexturasPersonagens(imagensPersonagens);

    CloseWindow();
}


int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    setlocale(LC_ALL, "Portuguese");

    SetTraceLogLevel(LOG_ERROR);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1500, 800, "Preguica");
    MaximizeWindow();

    SetTargetFPS(60);


    //do
    //{

        jogo();

    //} while(true);


    return 0;
}
