#include "raylib.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "animation.h"

#define QTDE_TOTAL_CARTAS 16
#define QTDE_CARTAS_EMJOGO 8
#define LARGURA_CARTA 150
#define ALTURA_CARTA 200

const char* CAMINHO_IMG_PERSONAGENS[QTDE_TOTAL_CARTAS] = {
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png",
    "img/tanque.png",
    "img/soldado.png"
    };


typedef struct
{
    Rectangle areaBotao;
    Color corBotao;
    Texture2D imgBotao;
    bool visivel;
} Button_Rec;

typedef struct
{
    int centerX;
    int centerY;
    float raioX;
    float raioY;
    Color corBotao;
    Texture2D imgBotao;
    bool visivel;
} Button_Ellipse;

typedef enum {
    SCENE_MENU,
    SCENE_GAME,
    SCENE_BLABLA //nao sei um nome ainda (cena antes do jogo para escolher as cartas)
} Scene;

Scene currentScene;

typedef struct
{
    Texture2D imgPersonagem;
    char* Nome;
    int forcaAtaque;
    int forcaDefesa;
} Personagem;

typedef struct carta
{
    int id;
    Personagem *personagem;
    int posX;
    int posY;
    int posXOrigem;
    int posYOrigem;
    float width;
    float height;
    bool emJogo;

    struct carta *proxima;

} Carta;


typedef struct
{
    int tam;
    Carta *cartaInicial;
} ListaCartas;


void inserir_no_comeco(ListaCartas *listaCartas, Carta *carta)
{
    Carta *cartaNova = malloc(sizeof(Carta));

    if(cartaNova){

        cartaNova->personagem = carta->personagem;
        cartaNova->posX = carta->posX;
        cartaNova->posY = carta->posY;
        cartaNova->width = carta->width;
        cartaNova->height = carta->height;
        cartaNova->emJogo = carta->emJogo;

        cartaNova->proxima = listaCartas->cartaInicial;
        listaCartas->cartaInicial = cartaNova;
        listaCartas->tam++;
    }
    else{
        printf("Erro ao alocar memoria, inicio da lista!\n");
    }
}

void inserir_no_fim(ListaCartas *listaCartas, Carta *carta)
{
    Carta *aux, *cartaNova = malloc(sizeof(Carta));

    if(cartaNova){
        cartaNova->personagem = carta->personagem;
        cartaNova->posX = carta->posX;
        cartaNova->posY = carta->posY;
        cartaNova->width = carta->width;
        cartaNova->height = carta->height;
        cartaNova->emJogo = carta->emJogo;

        cartaNova->proxima = NULL;

        if(listaCartas->cartaInicial == NULL){
            listaCartas->cartaInicial = cartaNova;
        }
        else{
            aux = listaCartas->cartaInicial;
            while(aux->proxima){
                aux = aux->proxima;
            }
            aux->proxima = cartaNova;
        }

        listaCartas->tam++;
    }
    else{
        printf("Erro ao alocar memoria, fim da lista!\n");
    }
}

/*void inserir_no_meio(ListaCartas *listaCartas, Carta *carta)
{
    Carta *aux, *cartaNova = malloc(sizeof(Carta));

    if(cartaNova){

        cartaNova->personagem = carta->personagem;
        cartaNova->posX = carta->posX;
        cartaNova->posY = carta->posY;
        cartaNova->width = carta->width;
        cartaNova->height = carta->height;
        cartaNova->emJogo = carta->emJogo;

        if(listaCartas->cartaInicial == NULL){
            cartaNova->proxima = NULL;
            listaCartas->cartaInicial = novo;
        }
        else{
            aux = listaCartas->cartaInicial;
            while(aux->proxima && aux-> != val_anterior){
                aux = aux->proximo;
            }
            novo->proximo = aux->proximo;
            aux->proximo = novo;
        }
        lista->tam++;
    }
}*/

bool isMouseOverButtonEllipse(Button_Ellipse button)
{
    float valor = ((GetMouseX() - button.centerX) * (GetMouseX() - button.centerX)) / (button.raioX * button.raioX) + ((GetMouseY() - button.centerY) * (GetMouseY() - button.centerY)) / (button.raioY * button.raioY);
    return valor <= 1.0;
}

bool isMouseOverButtonRec(Button_Rec button)
{
    return CheckCollisionPointRec(GetMousePosition(), button.areaBotao);
}

void reposicionarListaEscolhidos(ListaCartas *cartasEscolhidas, int posXInicial, int posY)
{

    int contador = 0;
    Carta *aux = cartasEscolhidas->cartaInicial;

    while(aux)
    {
        aux->posX = posXInicial + (LARGURA_CARTA + 15) * contador;
        aux->posY = posY;

        aux->posXOrigem = aux->posX;
        aux->posYOrigem = aux->posY;

        aux = aux->proxima;
        contador++;
    }
}

void desenhaBotaoRec(Button_Rec botao)
{
    DrawRectangleRec(botao.areaBotao, botao.corBotao);
}

void desenhaBotaoEllipse(Button_Ellipse botao)
{
    DrawEllipse(botao.centerX, botao.centerY, botao.raioX, botao.raioY, botao.corBotao);
}

void desenhaFundo()
{

    ClearBackground(WHITE);

}

void desenhaCarta(Carta carta)
{
    Rectangle molde;
    Color cartaCor = carta.emJogo ? DARKGRAY : BLACK;
    Color personagemCor = carta.emJogo ? BLACK : WHITE;

    molde.x = carta.posX;
    molde.y = carta.posY;
    molde.height = carta.height;
    molde.width = carta.width;

    if (carta.personagem != NULL)
    {

        DrawRectangleRec(molde, cartaCor);
        DrawRectangleLinesEx(molde, 10, BROWN);

        DrawTexture(carta.personagem->imgPersonagem, molde.x + 20, molde.y + 25, personagemCor);
    }
    else
    {
        DrawRectangleLinesEx(molde, 5, BROWN);
    }

}

void desenhaCartasEscolhidas(ListaCartas *cartasEscolhidas)
{

    Carta *aux = cartasEscolhidas->cartaInicial;

    while(aux){
        desenhaCarta(*aux);
        aux = aux->proxima;
    }
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

Carta* cartaEscolhidasPorPosicao(ListaCartas *cartasEscolhidas, int posX, int posY)
{
    Carta *aux = cartasEscolhidas->cartaInicial;
    Rectangle areaCarta;

    while (aux)
    {
        areaCarta = (Rectangle){aux->posX, aux->posY, LARGURA_CARTA + 15, ALTURA_CARTA + 20};
        Vector2 ponto = {posX, posY};

        if(CheckCollisionPointRec(ponto, areaCarta))
        {
            return aux;
        }

        aux = aux->proxima;
    }

    return NULL;

}

//************ GAME SCENE ***************

void jogo(ListaCartas *cartasEscolhidas)
{

    Carta *cartaEmMovimento = NULL;
    reposicionarListaEscolhidos(cartasEscolhidas, GetScreenWidth() / 8, GetScreenHeight() - (LARGURA_CARTA * 2));

    while(1)
    {

        BeginDrawing();

            desenhaFundo();
            desenhaCartasEscolhidas(cartasEscolhidas);


            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (cartaEmMovimento == NULL)
                    cartaEmMovimento = cartaEscolhidasPorPosicao(cartasEscolhidas, GetMouseX(), GetMouseY());
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                if (cartaEmMovimento != NULL)
                {
                    //indexCartasEscolhidas = indexListaCartasEscolhidasPorPosicao(cartasEscolhidas, GetMouseX(), GetMouseY());

                    //if (indexCartasEscolhidas >= 0)
                        //moverCartaDisponiveisParaEscolhidas(cartasDisponiveis, cartasEscolhidas, cartaEmMovimento, indexCartasEscolhidas);

                    cartaEmMovimento->posX = cartaEmMovimento->posXOrigem;
                    cartaEmMovimento->posY = cartaEmMovimento->posYOrigem;
                }

                cartaEmMovimento = NULL;
            }

             if (cartaEmMovimento != NULL)
            {
                printf("Id carta em movimento: %d\n", cartaEmMovimento->id);

                cartaEmMovimento->posX = GetMouseX();
                cartaEmMovimento->posY = GetMouseY();

                desenhaCarta(*cartaEmMovimento);
            }


        EndDrawing();
    }
}



//************ BLABLA SCENE *************

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
    float xInicial = 30, yInicial = 500;
    int espacamento = 15;

    for (int i = 0; i < qtdeCartas; i++)
    {
        carta.id = i;
        carta.personagem = &personagens[i];
        carta.posX = xInicial + (LARGURA_CARTA + espacamento) * i;
        carta.posY = yInicial;

        carta.posXOrigem = carta.posX;
        carta.posYOrigem = carta.posY;

        carta.width = LARGURA_CARTA;
        carta.height = ALTURA_CARTA;

        carta.proxima = NULL;
        carta.emJogo = false;

        cartasDisponiveis[i] = carta;
    }
}


void criaListaCartasEscolhidas(ListaCartas *cartasEscolhidas)
{
    int xInicial = 30, yInicial = 35, espacamento = 15;

    cartasEscolhidas->cartaInicial = NULL;
    cartasEscolhidas->tam = 0;

    Carta *carta = malloc(sizeof(Carta));

    for (int i = 0; i < QTDE_CARTAS_EMJOGO; i++)
    {

        carta->id = -1;
        carta->personagem = NULL;
        carta->posX = xInicial + (LARGURA_CARTA + espacamento) * i;
        carta->posY = yInicial;

        carta->height = ALTURA_CARTA;
        carta->width = LARGURA_CARTA;
        carta->emJogo = false;

        inserir_no_fim(cartasEscolhidas, carta);
    }
}

int totalCartasEscolhidas(ListaCartas *cartasEscolhidas)
{
    int contador = 0;
    Carta *aux = cartasEscolhidas->cartaInicial;

    while(aux)
    {
        if (aux->personagem != NULL)
            contador++;

        aux = aux->proxima;
    }

    return contador;
}

int indexListaCartasEscolhidasPorPosicao(ListaCartas *cartasEscolhidas, int posX, int posY)
{
    int contador = -1;
    Carta *aux = cartasEscolhidas->cartaInicial;
    Rectangle areaCarta;

    while (aux)
    {
        contador ++;
        areaCarta = (Rectangle){aux->posX, aux->posY, LARGURA_CARTA + 15, ALTURA_CARTA + 20};
        Vector2 ponto = {posX, posY};

        if(CheckCollisionPointRec(ponto, areaCarta))
        {
            return contador;
        }

        aux = aux->proxima;
    }

    return -1;

}

void desenhaCartasDisponiveis(Carta cartasDisponiveis[], int qtdeCartas)
{
    Carta *carta = NULL;

    int espacamento = 15;
    int qtdeColunas = (int) GetScreenWidth() / (LARGURA_CARTA + espacamento);


    if (qtdeColunas > 8)
        qtdeColunas = 8;

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

        qtdeLinhas += qtdeLinhas * qtdeColunas < qtdeCartas ? 1 : 0;

        for (int i = 0; i < qtdeLinhas; i++)
        {
            int colunas;
            if (i == qtdeLinhas - 1 && qtde > 0)
                colunas = qtde;
            else
                colunas = qtdeColunas;

            for (int j = 0; j < colunas; j++)
            {

                carta = &cartasDisponiveis[index];

                if (i == 0 && j == 0)
                    x = carta->posXOrigem;

                if (x != -1 && i != 0)
                {
                    carta->posX = x + (LARGURA_CARTA + espacamento) * j;
                    carta->posY = ALTURA_CARTA + 515;
                }

                desenhaCarta(*carta);

                index ++;
            }
        }
    }
}

Carta* pegaCartaDisponivelPorPosicao(Carta cartasDisponiveis[], int posX, int posY)
{

    Rectangle areaCarta;

    for (int i = 0; i < QTDE_TOTAL_CARTAS; i++)
    {
        areaCarta = (Rectangle){cartasDisponiveis[i].posX, cartasDisponiveis[i].posY, LARGURA_CARTA, ALTURA_CARTA};
        Vector2 ponto = {posX, posY};

        //printf("Id: %d\n", cartasDisponiveis[i].id);
        //printf("PosX: %d\n", cartasDisponiveis[i].posX);
        //printf("PosY: %d\n", cartasDisponiveis[i].posY);

        if(CheckCollisionPointRec(ponto, areaCarta))
        {
            if (cartasDisponiveis[i].emJogo == false)
                return &cartasDisponiveis[i];
            else
                return NULL;
        }
    }

    return NULL;
}

void devolverCartaParaDisponiveis(Carta cartasDisponiveis[], int id)
{
    if (id >= 0 && id < QTDE_TOTAL_CARTAS)
        cartasDisponiveis[id].emJogo = false;
}

void moverCartaDisponiveisParaEscolhidas(Carta cartasDisponiveis[], ListaCartas *cartasEscolhidas, Carta *cartaEmMovimento, int indexCartasEscolhidas)
{

    int contador = 0;
    Carta *aux = cartasEscolhidas->cartaInicial;

    for (int i = 0; i < QTDE_TOTAL_CARTAS; i++)
    {

        if (cartaEmMovimento->id == cartasDisponiveis[i].id)
            cartasDisponiveis[i].emJogo = true;
    }

    while (aux && contador < indexCartasEscolhidas)
    {
        aux = aux->proxima;
        contador ++;
    }

    if (aux->id != -1)
        devolverCartaParaDisponiveis(cartasDisponiveis, aux->id);

    aux->personagem = cartaEmMovimento->personagem;
    aux->id = cartaEmMovimento->id;
}

void montarDeckCartas(ListaCartas *cartasEscolhidas, Carta cartasDisponiveis[])
{

    //Button_Rec botao_avancar;
    bool botaoClicado = false;
    Button_Ellipse botao_avancar;
    botao_avancar.corBotao = DARKBROWN;

    //Texture2D imagensPersonagens[QTDE_TOTAL_CARTAS];
    //Personagem personagens[QTDE_TOTAL_CARTAS];
    //Carta cartasDisponiveis[QTDE_TOTAL_CARTAS];

    Carta *cartaEmMovimento = NULL;
    int indexCartasEscolhidas = -1;

    //carregarTexturas(imagensPersonagens, QTDE_TOTAL_CARTAS);
    //criaPersonagens(personagens, imagensPersonagens, QTDE_TOTAL_CARTAS);
    //criaCartasDisponiveis(cartasDisponiveis, personagens, QTDE_TOTAL_CARTAS);

    criaListaCartasEscolhidas(cartasEscolhidas);

    while (1)
    {

        BeginDrawing();

            desenhaFundo();
            desenhaCartasDisponiveis(cartasDisponiveis, QTDE_TOTAL_CARTAS);
            desenhaCartasEscolhidas(cartasEscolhidas);

            /*
            if (totalCartasEscolhidas(cartasEscolhidas) >= 1)
            {
                botao_avancar.areaBotao = (Rectangle) {(GetScreenWidth() / 2) * 1.55, (GetScreenHeight() / 2) * 0.80, 150, 100};
                desenhaBotaoRec(botao_avancar);
            }

            if (isMouseOverButtonRec(botao_avancar))
                botao_avancar.corBotao = BROWN;
            else
                botao_avancar.corBotao = DARKBROWN;
            */

            if (totalCartasEscolhidas(cartasEscolhidas) >= QTDE_CARTAS_EMJOGO)
            {
                botao_avancar.centerX = (GetScreenWidth() / 2) * 1.55;
                botao_avancar.centerY = (GetScreenHeight() / 2) * 0.80;
                botao_avancar.raioX = 100;
                botao_avancar.raioY = 50;
                desenhaBotaoEllipse(botao_avancar);
                DrawText("JOGAR", (botao_avancar.centerX - MeasureText("JOGAR", 30)) + 50, botao_avancar.centerY * 0.96, 30, WHITE);
            }

            if (isMouseOverButtonEllipse(botao_avancar))
            {
                botao_avancar.corBotao = BROWN;

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    botaoClicado = true;
            }
            else
                botao_avancar.corBotao = DARKBROWN;

            if (botaoClicado)
                break;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (cartaEmMovimento == NULL)
                    cartaEmMovimento = pegaCartaDisponivelPorPosicao(cartasDisponiveis, GetMouseX(), GetMouseY());
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                if (cartaEmMovimento != NULL)
                {
                    indexCartasEscolhidas = indexListaCartasEscolhidasPorPosicao(cartasEscolhidas, GetMouseX(), GetMouseY());

                    printf("%d index\n", indexCartasEscolhidas);

                    if (indexCartasEscolhidas >= 0)
                        moverCartaDisponiveisParaEscolhidas(cartasDisponiveis, cartasEscolhidas, cartaEmMovimento, indexCartasEscolhidas);

                    cartaEmMovimento->posX = cartaEmMovimento->posXOrigem;
                    cartaEmMovimento->posY = cartaEmMovimento->posYOrigem;
                }

                cartaEmMovimento = NULL;
            }

             if (cartaEmMovimento != NULL)
            {
                printf("Id carta em movimento: %d\n", cartaEmMovimento->id);

                cartaEmMovimento->posX = GetMouseX();
                cartaEmMovimento->posY = GetMouseY();

                desenhaCarta(*cartaEmMovimento);
            }

        EndDrawing();

    }

    //destroiTexturasPersonagens(imagensPersonagens);
    currentScene = SCENE_GAME;

    return;
}

//************ MAIN MENU *****************


void menu()
{

    while (GetKeyPressed() != KEY_SPACE)
    {
        BeginDrawing();
            ClearBackground(DARKGREEN);


        EndDrawing();

    }

    currentScene = SCENE_GAME;

    return;
}


void DesenhaCena()
{
    ListaCartas cartasEscolhidas;

    Texture2D imagensPersonagens[QTDE_TOTAL_CARTAS];
    Personagem personagens[QTDE_TOTAL_CARTAS];
    Carta cartasDisponiveis[QTDE_TOTAL_CARTAS];

    carregarTexturas(imagensPersonagens, QTDE_TOTAL_CARTAS);
    criaPersonagens(personagens, imagensPersonagens, QTDE_TOTAL_CARTAS);
    criaCartasDisponiveis(cartasDisponiveis, personagens, QTDE_TOTAL_CARTAS);

    switch (currentScene)
    {
    case SCENE_MENU:
        menu();
        break;
    case SCENE_BLABLA:
        montarDeckCartas(&cartasEscolhidas, cartasDisponiveis);
        break;
    case SCENE_GAME:
        jogo(&cartasEscolhidas);
        break;
    }

}

Texture2D _texture;
SpriteAnimation _animation;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    currentScene = SCENE_BLABLA;

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(0 , 0, "War Card Game");
    SetTargetFPS(60);

    _texture = LoadTexture("animations/Shot.png");
    _animation = CreateSpriteAnimation(_texture, 2, (Rectangle[]){

        (Rectangle){0, 0, 128, 128},
        (Rectangle){128, 0, 128, 128},
        (Rectangle){256, 0, 128, 128},
        (Rectangle){384, 0, 128, 128}
    }, 4);


    printf("%f", _animation.rectangles[2].x);

    while (!WindowShouldClose())
    {
        //DesenhaCena();

        BeginDrawing();

            ClearBackground(GRAY);

            Rectangle source = {0, 0, 128, 128};

            Rectangle dest = {200, 200, 128, 128};
            DrawTexturePro(_texture, source, dest, (Vector2){0, 0}, 1.0, WHITE);


            Vector2 origin = {0};
            DrawSpriteAnimationPro(_animation, dest, origin, 0, WHITE);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
