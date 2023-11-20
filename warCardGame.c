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

#define NUM_ANIMACOES 4

#define VIDA_MAXIMA 100


const char* CAMINHO_IMG_PERSONAGENS[QTDE_TOTAL_CARTAS] = {
    "animations/0Idle.png",
    "animations/1Idle.png",
    "animations/2Idle.png",
    "animations/3Idle.png",
    "animations/4Idle.png",
    "animations/5Idle.png",
    "animations/6Idle.png",
    "animations/7Idle.png",
    "animations/8Idle.png",
    "animations/9Idle.png",
    "animations/10Idle.png",
    "animations/11Idle.png",
    "animations/12Idle.png",
    "animations/13Idle.png",
    "animations/14Idle.png",
    "animations/15Idle.png"
    };


typedef struct
{
    Rectangle areaBotao;
    Color corBotao;
    char* texto;
    Color corTexto;
    int fontSize;
} Button_Rec;

typedef struct
{
    int centerX;
    int centerY;
    float raioX;
    float raioY;
    Color corBotao;
    char* texto;
    Color corTexto;
    int fontSize;
} Button_Ellipse;

typedef enum {
    SCENE_MENU,
    SCENE_GAME,
    SCENE_DECK //nao sei um nome ainda (cena antes do jogo para escolher as cartas)
} Scene;

Scene currentScene;

typedef enum
{
    SOLDADO,
    CACADOR,
    SELVA,
    TANQUE
} TipoPersonagem_Enum;

typedef struct
{
    TipoPersonagem_Enum tipoPersonagem;
    int forcaAtaque;
    int vida;
} TipoPersonagem;

typedef struct
{
    Texture2D imgPersonagem;
    int id;
    TipoPersonagem tipo;
    char* nome;
    char* caminhoSprites[NUM_ANIMACOES];
    int qtdeFrames;
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

typedef struct Timer {
    double startTime;   // Start time (seconds)
    double lifeTime;    // Lifetime (seconds)
} Timer;

Texture2D imagensPersonagens[QTDE_TOTAL_CARTAS];
Personagem personagens[QTDE_TOTAL_CARTAS];
Carta cartasDisponiveis[QTDE_TOTAL_CARTAS];


void StartTimer(Timer *timer, double lifetime)
{
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
}

bool TimerDone(Timer timer)
{
    return GetTime() - timer.startTime >= timer.lifeTime;
}

double GetElapsed(Timer timer)
{
    return GetTime() - timer.startTime;
}

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
        TraceLog(LOG_ERROR, "Erro ao alocar memoria, inicio da lista!");
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
        TraceLog(LOG_ERROR, "Erro ao alocar memoria, fim da lista!");
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

int sorteiaNumero(int min, int max)
{
    srand(time(NULL));

    int numeroAleatorio = rand() % max;

    if (numeroAleatorio < min)
        numeroAleatorio = min;

    return numeroAleatorio;
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
    DrawRectangle(botao.areaBotao.x - 10,
                  botao.areaBotao.y - 10,
                  botao.areaBotao.width,
                  botao.areaBotao.height,
                  BROWN);
    DrawRectangleRec(botao.areaBotao, botao.corBotao);

    if (botao.texto != NULL)
        DrawText(botao.texto, botao.areaBotao.x + (botao.areaBotao.width / 2 * 0.60), botao.areaBotao.y + (botao.areaBotao.height / 2 * 0.60), botao.fontSize, botao.corTexto);
}

void desenhaBotaoEllipse(Button_Ellipse botao)
{

    DrawEllipse(botao.centerX - 10,
                botao.centerY - 10,
                botao.raioX,
                botao.raioY,
                DARKGRAY);

    DrawEllipse(botao.centerX, botao.centerY, botao.raioX, botao.raioY, botao.corBotao);

    if (botao.texto != NULL)
        DrawText(botao.texto, (botao.centerX - MeasureText(botao.texto, botao.fontSize)) + 40, botao.centerY * 0.95, botao.fontSize, botao.corTexto);

}

void desenhaFundo(Scene cena)
{

    Texture2D background = {0};

    if (cena == SCENE_MENU)
        ClearBackground(DARKBROWN);
    else if (cena == SCENE_DECK)
        ClearBackground(WHITE);

    else if (cena == SCENE_GAME)
    {
        ClearBackground(RAYWHITE);
        Rectangle ground = (Rectangle){0, GetScreenHeight() / 2 + 100, GetScreenWidth(), GetScreenHeight() / 2 - 100, WHITE};

        DrawRectangleRec(ground, DARKGRAY);
    }
    else
        ClearBackground(RED);
}

void desenhaCarta(Carta carta)
{
    Rectangle moldeCarta = {0};

    Rectangle moldeImgPersonagem = {0};
    Texture2D imgPersonagem = {0};

    Texture2D imgPontosVida = {0};
    Texture2D imgPontosAtaque = {0};

    Color cartaCor = carta.emJogo ? DARKGRAY : GRAY;
    Color personagemCor = carta.emJogo ? BLACK : WHITE;

    char pontosVida[5];
    char pontosAtaque[5];

    moldeCarta.x = carta.posX;
    moldeCarta.y = carta.posY;
    moldeCarta.height = carta.height;
    moldeCarta.width = carta.width;

    if (carta.personagem != NULL)
    {

        DrawRectangleRec(moldeCarta, cartaCor);
        DrawRectangleLinesEx(moldeCarta, 10, BROWN);

        imgPersonagem = carta.personagem->imgPersonagem;

        if (IsTextureReady(imgPersonagem))
        {

            moldeImgPersonagem = (Rectangle){
                moldeCarta.x + 20,
                moldeCarta.y + 25,
                imgPersonagem.width / 4,
                imgPersonagem.height
                };

            DrawTexturePro(imgPersonagem,
                           (Rectangle){0, 0, (imgPersonagem.width / 4), imgPersonagem.height},
                           moldeImgPersonagem,
                           (Vector2){0},
                           0,
                           personagemCor);
        }
        else
        {
           TraceLog(LOG_ERROR, "Imagem do personagem nao carregada");
        }

        imgPontosVida = LoadTexture("img/shield.png");

        if (IsTextureReady(imgPontosVida))
        {

            Rectangle moldePontosVida = (Rectangle){moldeCarta.x + 5, (moldeCarta.y + moldeCarta.height) - 60, 45, 38};

            DrawTexturePro(imgPontosVida,
                           (Rectangle){0, 0, imgPontosVida.width, imgPontosVida.height, WHITE},
                           moldePontosVida,
                           (Vector2){0},
                           0,
                           WHITE);

            sprintf(pontosVida, "%d", carta.personagem->tipo.vida);
            DrawText(pontosVida, moldePontosVida.x + 40, moldePontosVida.y + 10, 20, BLACK);

        }
        else
        {
            TraceLog(LOG_ERROR, "Imagem do pontos da vida nao carregada");
        }

        imgPontosAtaque = LoadTexture("img/cross-sword.png");

        if (IsTextureReady(imgPontosAtaque))
        {

            Rectangle moldePontosAtaque = (Rectangle){moldeCarta.x + 75, (moldeCarta.y + moldeCarta.height) - 55, 35, 32};

            DrawTexturePro(imgPontosAtaque,
                           (Rectangle){0, 0, imgPontosAtaque.width, imgPontosAtaque.height, WHITE},
                           moldePontosAtaque,
                           (Vector2){0},
                           0,
                           WHITE);

            sprintf(pontosAtaque, "%d", carta.personagem->tipo.forcaAtaque);
            DrawText(pontosAtaque, moldePontosAtaque.x + 40, moldePontosAtaque.y + 5, 20, BLACK);
        }
        else
        {
            TraceLog(LOG_ERROR, "Imagem do pontos de ataque nao carregada");
        }

    }
    else
    {
        DrawRectangleLinesEx(moldeCarta, 5, BROWN);
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

void destroiTexturasPersonagens(Texture2D imagensPersonagens[], int qtde)
{
    for (int i = 0; i < qtde; i++)
    {
        UnloadTexture(imagensPersonagens[i]);
    }
}

Personagem *clonarPersonagem(Personagem *personagemOrigem)
{
    Personagem* personagem = malloc(sizeof(Personagem));

    personagem->id = personagemOrigem->id;
    personagem->imgPersonagem = personagemOrigem->imgPersonagem;

    for (int i = 0; i < NUM_ANIMACOES; i++)
        personagem->caminhoSprites[i] = personagemOrigem->caminhoSprites[i];

    personagem->tipo = personagemOrigem->tipo;
    personagem->qtdeFrames = personagemOrigem->qtdeFrames;

    return personagem;
}

Carta* clonarCarta(Carta *cartaOrigem)
{
    Carta *carta = malloc(sizeof(Carta));

    carta->id = cartaOrigem->id;
    carta->width = cartaOrigem->width;
    carta->height = cartaOrigem->height;
    carta->posX = cartaOrigem->posX;
    carta->posXOrigem = cartaOrigem->posXOrigem;
    carta->posY = cartaOrigem->posY;
    carta->posYOrigem = cartaOrigem->posYOrigem;
    carta->emJogo = cartaOrigem->emJogo;

    carta->personagem = clonarPersonagem(cartaOrigem->personagem);

    return carta;
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
            if (aux->emJogo)
                return NULL;
            else
                return aux;
        }

        aux = aux->proxima;
    }

    return NULL;

}

//************ GAME SCENE ***************

void desenhBarraVida(Personagem *personagem, int posX, int posY, int vidaMaxima)
{
    if (personagem == NULL)
        return;

    int vida = personagem->tipo.vida;

    Texture2D coracaoVida = LoadTexture("img/coracaoVida.png");
    Rectangle source = (Rectangle){0, 0, coracaoVida.width, coracaoVida.height};
    Rectangle destino = (Rectangle){posX + 20, posY - 15, 32, 18};

    Rectangle barraVidaMaxima = (Rectangle){posX + 50, posY - 10, vidaMaxima, 10};
    Rectangle barraVida = (Rectangle){posX + 50, posY - 10, vida, 10};

    DrawRectangleRec(barraVida, RED);
    DrawRectangleLinesEx(barraVidaMaxima, 2, BLACK);

    if (IsTextureReady(coracaoVida))
        DrawTexturePro(coracaoVida, source, destino, (Vector2){0}, 0, WHITE);
}

Carta* sorteiaCarta()
{
    srand(time(NULL));

    int numeroAleatorio = rand() % QTDE_TOTAL_CARTAS;

    return clonarCarta(&cartasDisponiveis[numeroAleatorio]);
}

void TextureFlipHorizontal(Texture2D *texture)
{
    Image flipedImage = LoadImageFromTexture(*texture);

    ImageFlipHorizontal(&flipedImage);

    if (IsTextureReady(*texture))
    {
        UnloadTexture(*texture);
    }

    *texture = LoadTextureFromImage(flipedImage);
}

void jogo(ListaCartas *cartasEscolhidas)
{

    const int VELOCIDADE = 3;

    SpriteAnimation animation = {0};
    Texture2D textureSprite = {0};
    Texture2D spriteMaquina = {0};
    int estadoPersonagemPlayer = 0; //0 = Idle; 1 - Run; 2 - Shot
    int estadoPersonagemMaquina = 0; //0 = Idle; 1 - Run; 2 - Shot
    int vidaMaximaPersonagemPlayer = 0;
    int vidaMaximaPersonagemMaquina = 0;
    int ataquePlayer = 0;
    int ataqueMaquina = 0;

    int indexAnimationPlayer = 0;
    int indexAnimationMaquina = 0;
    bool playerIsDead = false;
    bool maquinaIsDead = false;

    Personagem *personagemPlayer = NULL;
    Personagem *personagemMaquina = NULL;

    Timer timeAnimation;
    timeAnimation.startTime = 0;
    timeAnimation.lifeTime = 0;

    Timer timerVida;
    timerVida.startTime = 0;
    timerVida.lifeTime = 0;

    Rectangle movementRecPlayer = (Rectangle){50, GetScreenHeight() / 2, 213, 136};
    Rectangle movementRecMaquina = (Rectangle){GetScreenWidth() * 0.80, GetScreenHeight() / 2, 213, 136};

    Carta *cartaEmMovimento = NULL;
    Carta *cartaSorteada = NULL;
    reposicionarListaEscolhidos(cartasEscolhidas, (GetScreenWidth() / 2) * 0.15 , GetScreenHeight() - (LARGURA_CARTA * 2));

    while(1)
    {

        BeginDrawing();

            desenhaFundo(SCENE_GAME);
            desenhaCartasEscolhidas(cartasEscolhidas);


            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (cartaEmMovimento == NULL)
                {
                    cartaEmMovimento = cartaEscolhidasPorPosicao(cartasEscolhidas, GetMouseX(), GetMouseY());
                }

            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {

                if (cartaEmMovimento != NULL)
                {
                    cartaEmMovimento->posX = cartaEmMovimento->posXOrigem;
                    cartaEmMovimento->posY = cartaEmMovimento->posYOrigem;

                    if (personagemPlayer != NULL)
                        personagemPlayer = NULL;

                    personagemPlayer = cartaEmMovimento->personagem;


                    if (cartaSorteada != NULL)
                        cartaSorteada = NULL;

                    cartaSorteada = sorteiaCarta();

                    if (cartaSorteada != NULL)
                    {
                        if (personagemMaquina != NULL)
                            personagemMaquina = NULL;

                        personagemMaquina = cartaSorteada->personagem;
                        cartaSorteada->posX = GetScreenWidth() * 0.70;
                        cartaSorteada->posY = 35;
                        cartaSorteada->emJogo = false;
                    }
                    else
                        TraceLog(LOG_ERROR, "Erro ao sortear carta");

                    if (personagemPlayer != NULL)
                    {
                        if (IsTextureReady(textureSprite))
                        {
                            UnloadTexture(textureSprite);
                        }
                        textureSprite = LoadTexture(personagemPlayer->caminhoSprites[0]);

                        if (IsTextureReady(spriteMaquina))
                        {
                            UnloadTexture(spriteMaquina);
                        }

                        spriteMaquina = LoadTexture(personagemMaquina->caminhoSprites[0]);

                        TextureFlipHorizontal(&spriteMaquina);

                        estadoPersonagemPlayer = 0;
                        estadoPersonagemMaquina = 0;
                        vidaMaximaPersonagemPlayer = personagemPlayer->tipo.vida;
                        vidaMaximaPersonagemMaquina = personagemMaquina->tipo.vida;
                        ataquePlayer = personagemPlayer->tipo.forcaAtaque;
                        ataqueMaquina = personagemMaquina->tipo.forcaAtaque;

                        movementRecPlayer.x = 50;
                        movementRecMaquina.x = GetScreenWidth() * 0.80;
                    }

                    cartaEmMovimento->emJogo = true;
                    indexAnimationMaquina = 0;
                    indexAnimationPlayer = 0;
                    playerIsDead = false;
                    maquinaIsDead = false;
                }
                //free(cartaEmMovimento);
                cartaEmMovimento = NULL;
            }

             if (cartaEmMovimento != NULL)
            {
                cartaEmMovimento->posX = GetMouseX();
                cartaEmMovimento->posY = GetMouseY();

                desenhaCarta(*cartaEmMovimento);
            }

            if (cartaSorteada != NULL)
                desenhaCarta(*cartaSorteada);



            if (personagemPlayer != NULL)
            {
                if (IsTextureReady(textureSprite))
                {
                    desenhBarraVida(personagemPlayer, movementRecPlayer.x, movementRecPlayer.y, vidaMaximaPersonagemPlayer);

                    int width = textureSprite.width / 4;
                    int height = textureSprite.height;

                    animation = CreateSpriteAnimation(textureSprite, 4, (Rectangle[]){
                                                    (Rectangle){width * 0, 0, width, height},
                                                    (Rectangle){width * 1, 0, width, height},
                                                    (Rectangle){width * 2, 0, width, height},
                                                    (Rectangle){width * 3, 0, width, height}
                                                }, 4);
                    movementRecPlayer.width = width;
                    movementRecPlayer.height = height;
                    if (estadoPersonagemPlayer == 1)
                        movementRecPlayer.x += VELOCIDADE;


                    if (indexAnimationPlayer == animation.rectanglesLength - 1 && playerIsDead)
                    {

                        printf("AQUIIII");
                        DrawTexturePro(textureSprite,
                                       animation.rectangles[3],
                                       movementRecPlayer,
                                       (Vector2){0},
                                       0,
                                       WHITE);

                    }
                    else
                    {
                        indexAnimationPlayer = (int)(GetTime() * animation.framesPerSecond) % animation.rectanglesLength;
                        DrawSpriteAnimationPro(animation, movementRecPlayer, (Vector2){0, 0}, 0.0, WHITE);
                    }


                    if (personagemMaquina != NULL)
                    {
                        if (IsTextureReady(spriteMaquina))
                        {
                            desenhBarraVida(personagemMaquina, movementRecMaquina.x, movementRecMaquina.y, vidaMaximaPersonagemMaquina);
                            int width = spriteMaquina.width / 4;
                            int height = spriteMaquina.height;

                            animation = CreateSpriteAnimation(spriteMaquina, 4, (Rectangle[]){
                                                            (Rectangle){width * 3, 0, width, height},
                                                            (Rectangle){width * 2, 0, width, height},
                                                            (Rectangle){width * 1, 0, width, height},
                                                            (Rectangle){width * 0, 0, width, height}
                                                        }, 4);
                            movementRecMaquina.width = width;
                            movementRecMaquina.height = height;



                            if (estadoPersonagemMaquina == 1)
                                movementRecMaquina.x -= VELOCIDADE;


                            if (indexAnimationMaquina == animation.rectanglesLength - 1 && maquinaIsDead)
                            {

                                DrawTexturePro(spriteMaquina,
                                               animation.rectangles[3],
                                               movementRecMaquina,
                                               (Vector2){0},
                                               0,
                                               WHITE);

                            }
                            else
                            {
                                indexAnimationMaquina = (int)(GetTime() * animation.framesPerSecond) % animation.rectanglesLength;
                                DrawSpriteAnimationPro(animation, movementRecMaquina, (Vector2){0, 0}, 0.0, WHITE);
                            }


                        }
                        else
                            TraceLog(LOG_ERROR, "Sprite personagem sorteado nao carregado");
                    }


                    if(timeAnimation.lifeTime == 0)
                        StartTimer(&timeAnimation, 3);

                    if (TimerDone(timeAnimation))
                    {
                        //**** ANIMACAO SPRITE PERSONAGEM JOGADOR
                        if (estadoPersonagemPlayer == 0)
                        {
                            UnloadTexture(textureSprite);
                            textureSprite = LoadTexture(personagemPlayer->caminhoSprites[1]);
                            estadoPersonagemPlayer = 1;

                            StartTimer(&timeAnimation, 2.5);
                        }
                        else if (estadoPersonagemPlayer == 1)
                        {
                            UnloadTexture(textureSprite);
                            textureSprite = LoadTexture(personagemPlayer->caminhoSprites[2]);
                            estadoPersonagemPlayer = 2;
                        }
                        else if (estadoPersonagemPlayer == 3)
                        {
                            UnloadTexture(textureSprite);
                            textureSprite = LoadTexture(personagemPlayer->caminhoSprites[3]);
                        }


                        //**** ANIMACAO SPRITE PERSONAGEM MAQUINA *****
                        if (estadoPersonagemMaquina == 0)
                        {
                            UnloadTexture(spriteMaquina);
                            spriteMaquina = LoadTexture(personagemMaquina->caminhoSprites[1]);

                            TextureFlipHorizontal(&spriteMaquina);

                            estadoPersonagemMaquina = 1;

                            StartTimer(&timeAnimation, 2.5);
                        }
                        else if (estadoPersonagemMaquina == 1)
                        {
                            UnloadTexture(spriteMaquina);
                            spriteMaquina = LoadTexture(personagemMaquina->caminhoSprites[2]);
                            TextureFlipHorizontal(&spriteMaquina);

                            estadoPersonagemMaquina = 2;
                        }
                        else if (estadoPersonagemMaquina == 3)
                        {
                            UnloadTexture(spriteMaquina);
                            spriteMaquina = LoadTexture(personagemMaquina->caminhoSprites[3]);
                            TextureFlipHorizontal(&spriteMaquina);
                        }
                    }


                    if (personagemPlayer->tipo.vida <= 0 && personagemMaquina->tipo.vida > 0)
                    {
                        DrawText("YOU DIED", GetScreenWidth() / 2 * 0.60, GetScreenHeight() / 2 * 0.60, 150, RED);
                        playerIsDead = true;
                        estadoPersonagemPlayer = 3;
                    }

                    if (personagemMaquina->tipo.vida <= 0 && personagemPlayer->tipo.vida > 0)
                    {
                        DrawText("ENEMY ELIMINATED", GetScreenWidth() / 2 * 0.30, GetScreenHeight() / 2 * 0.60, 130, RED);
                        maquinaIsDead = true;
                        estadoPersonagemMaquina = 3;
                    }

                    if (TimerDone(timerVida))
                    {
                        if (estadoPersonagemPlayer == 2)
                        {
                            if (personagemMaquina->tipo.vida > 0)
                            {
                                personagemPlayer->tipo.vida -= personagemPlayer->tipo.vida > 0 ? ataqueMaquina : 0;

                                if (personagemPlayer->tipo.vida < 0)
                                    personagemPlayer->tipo.vida = 0;
                            }
                        }

                        if (estadoPersonagemMaquina == 2)
                        {
                            if (personagemPlayer->tipo.vida > 0)
                                personagemMaquina->tipo.vida -= personagemMaquina->tipo.vida > 0 ? ataquePlayer : 0;

                            if (personagemMaquina->tipo.vida < 0)
                                personagemMaquina->tipo.vida = 0;
                        }

                        if (playerIsDead || maquinaIsDead)
                            StartTimer(&timerVida, 1);
                    }
                }
                else
                {
                    TraceLog(LOG_ERROR, "Texture Not Ready");
                }
            }

        EndDrawing();
    }

    free(cartaSorteada);
    free(personagemMaquina);
}



//************ DECK SCENE *************

void preencheCaminhoSpritesPersonagem(int idPersonagem, char* caminhoSprites[], int qtde)
{
    char caminhoCompleto[100];
    char strId[20]; // Aloque memória suficiente para o número máximo de dígitos
    char strAnimacao[20]; // Aloque memória suficiente para o nome da animação

    for (int i = 0; i < qtde; i++)
    {
        strcpy(caminhoCompleto, "animations/");

        switch(i)
        {
        case 0:
            strcpy(strAnimacao, "Idle.png");
            break;
        case 1:
            strcpy(strAnimacao, "Run.png");
            break;
        case 2:
            strcpy(strAnimacao, "Shot.png");
            break;
        case 3:
            strcpy(strAnimacao, "Dead.png");
            break;
        }

        sprintf(strId, "%d", idPersonagem);
        strcat(strId, strAnimacao);

        strcat(caminhoCompleto, strId);

        caminhoSprites[i] = strdup(caminhoCompleto);

        // Lembre-se de liberar a memória alocada por strdup mais tarde

        strAnimacao[0] = '\0'; // Limpa o buffer
        caminhoCompleto[0] = '\0';
    }
}

void criaPersonagens(Personagem personagens[], Texture2D imagensPersonagens[], int qtde)
{
    Personagem personagem;
    int qtdeFrames = 4;

    TipoPersonagem tipo[4] = {
        (TipoPersonagem){SOLDADO, 10, 50},
        (TipoPersonagem){CACADOR, 25, 60},
        (TipoPersonagem){SELVA, 33, 80},
        (TipoPersonagem){TANQUE, 50, 100}
        };
    int indexTipo = 0;

    for (int i = 0; i < qtde; i++)
    {
        indexTipo = i % 4;

        personagem.id = i;
        personagem.tipo = tipo[indexTipo];
        personagem.imgPersonagem = imagensPersonagens[i];

        char* caminhoSprites[NUM_ANIMACOES];
        preencheCaminhoSpritesPersonagem(personagem.id, caminhoSprites, NUM_ANIMACOES);

        // Atribua o caminho da animação ao personagem
        for (int j = 0; j < NUM_ANIMACOES; j++) {
            personagem.caminhoSprites[j] = strdup(caminhoSprites[j]);
        }

        // Lembre-se de liberar a memória alocada por strdup mais tarde
        for (int j = 0; j < NUM_ANIMACOES; j++) {
            free(caminhoSprites[j]);
        }
        personagem.qtdeFrames = qtdeFrames;

        // Armazene o personagem no array de personagens
        personagens[i] = personagem;
    }
}

void criaCartasDisponiveis(Carta cartasDisponiveis[], Personagem personagens[], int qtdeCartas)
{

    Carta carta;
    float xInicial = 50, yInicial = (GetScreenHeight() / 2.5) ;
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
    int xInicial = 50, yInicial = 35, espacamento = 15;

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
                    carta->posY = ALTURA_CARTA + carta->posYOrigem + 15;
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


void indisponibilizarCarta(Carta cartasDisponiveis[], int id)
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
        indisponibilizarCarta(cartasDisponiveis, aux->id);

    aux->personagem = clonarPersonagem(cartaEmMovimento->personagem);

    aux->id = cartaEmMovimento->id;
}

void montarDeckCartas(ListaCartas *cartasEscolhidas)
{

    //Button_Rec botao_avancar;
    bool botaoClicado = false;
    Button_Ellipse botao_avancar;
    botao_avancar.corBotao = DARKBROWN;

    Carta *cartaEmMovimento = NULL;
    int indexCartasEscolhidas = -1;

    criaListaCartasEscolhidas(cartasEscolhidas);

    while (1)
    {

        BeginDrawing();

            desenhaFundo(SCENE_DECK);
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
                botao_avancar.centerX = GetScreenWidth() * 0.90;
                botao_avancar.centerY = (GetScreenHeight() / 2) * 0.65;
                botao_avancar.raioX = 100;
                botao_avancar.raioY = 50;
                botao_avancar.texto = "PLAY";
                botao_avancar.fontSize = 30;
                botao_avancar.corTexto = WHITE;
                desenhaBotaoEllipse(botao_avancar);
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

                    if (indexCartasEscolhidas >= 0)
                        moverCartaDisponiveisParaEscolhidas(cartasDisponiveis, cartasEscolhidas, cartaEmMovimento, indexCartasEscolhidas);

                    cartaEmMovimento->posX = cartaEmMovimento->posXOrigem;
                    cartaEmMovimento->posY = cartaEmMovimento->posYOrigem;
                }

                cartaEmMovimento = NULL;
            }

             if (cartaEmMovimento != NULL)
            {
                cartaEmMovimento->posX = GetMouseX();
                cartaEmMovimento->posY = GetMouseY();

                desenhaCarta(*cartaEmMovimento);
            }

        EndDrawing();

    }

    free(cartaEmMovimento);
    currentScene = SCENE_GAME;

    return;
}

//************ MAIN MENU *****************

void MessageBox(char* message, Rectangle box, Button_Rec buttons[], int qtdeButtons)
{

    ClearBackground(DARKBROWN);

    DrawRectangleLinesEx(box, 2, WHITE);
    DrawText(message, box.x + 10, box.y + 20, 25, RED);

    for (int i = 0; i < qtdeButtons; i++)
        desenhaBotaoRec(buttons[i]);
}

void menu()
{

    bool botaoStart_Clicado = false;
    bool botaoExit_Clicado = false;
    bool isPaused = false;

    Button_Rec botao_start = (Button_Rec){
        (Rectangle){GetScreenWidth() / 2 * 0.80, GetScreenHeight() / 3 * 0.90, 300, 100, WHITE},
        GRAY,
        "START",
        BLACK,
        40,
        false};


    Button_Rec botao_exit = (Button_Rec){
        (Rectangle){botao_start.areaBotao.x, botao_start.areaBotao.y + botao_start.areaBotao.height + 50, 300, 100, WHITE},
        GRAY,
        "EXIT",
        BLACK,
        40,
        false};

    while (1)
    {
        BeginDrawing();
            desenhaFundo(SCENE_MENU);

            if (!isPaused)
            {
               if (isMouseOverButtonRec(botao_start))
               {
                   botao_start.corBotao = DARKGRAY;
                   botao_start.corTexto = RAYWHITE;

                   if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                       botaoStart_Clicado = true;
               }
               else
               {
                   botao_start.corBotao = GRAY;
                   botao_start.corTexto = BLACK;
               }

               if (isMouseOverButtonRec(botao_exit))
               {
                   botao_exit.corBotao = DARKGRAY;
                   botao_exit.corTexto = RAYWHITE;

                   if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                   {
                       botaoExit_Clicado = true;
                       isPaused = true;
                   }

               }
               else
               {
                   botao_exit.corBotao = GRAY;
                   botao_exit.corTexto = BLACK;
               }

               desenhaBotaoRec(botao_start);
               desenhaBotaoRec(botao_exit);

               if (botaoStart_Clicado)
                break;
            }

            if (isPaused && botaoExit_Clicado)
            {

                Rectangle box = (Rectangle){GetScreenWidth() / 2 * 0.80, GetScreenHeight() / 3 * 0.90, 300, 200, WHITE};

                Button_Rec botao_yes = (Button_Rec){
                    (Rectangle){box.x + 50 , box.y + (box.height * 0.60), 100, 50, WHITE},
                    GRAY,
                    "YES",
                    BLACK,
                    20,
                    false};


                Button_Rec botao_no = (Button_Rec){
                    (Rectangle){botao_yes.areaBotao.x + botao_yes.areaBotao.width + 20, botao_yes.areaBotao.y, 100, 50, WHITE},
                    GRAY,
                    "NO",
                    BLACK,
                    20,
                    false};

                if (isMouseOverButtonRec(botao_yes))
                {
                    botao_yes.corBotao = DARKGRAY;
                    botao_yes.corTexto = RAYWHITE;

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        CloseWindow();

                }
                else
                {
                    botao_yes.corBotao = GRAY;
                    botao_yes.corTexto = BLACK;
                }

                if (isMouseOverButtonRec(botao_no))
                {
                    botao_no.corBotao = DARKGRAY;
                    botao_no.corTexto = RAYWHITE;

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        botaoExit_Clicado = false;
                        isPaused = false;
                    }

                }
                else
                {
                    botao_no.corBotao = GRAY;
                    botao_no.corTexto = BLACK;
                }


                MessageBox("Deseja realmente sair?", box, (Button_Rec[]){botao_yes, botao_no}, 2);
            }

        EndDrawing();
    }

    currentScene = SCENE_DECK;
    return;
}

void DesenhaCena()
{
    ListaCartas cartasEscolhidas;

    switch (currentScene)
    {
    case SCENE_MENU:
        menu();
        break;
    case SCENE_DECK:
        montarDeckCartas(&cartasEscolhidas);
        break;
    case SCENE_GAME:
        jogo(&cartasEscolhidas);
        break;
    }

}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    currentScene = SCENE_MENU;

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(0 , 0, "War Card Game");
    SetTargetFPS(60);

    carregarTexturas(imagensPersonagens, QTDE_TOTAL_CARTAS);
    criaCartasDisponiveis(cartasDisponiveis, personagens, QTDE_TOTAL_CARTAS);
    criaPersonagens(personagens, imagensPersonagens, QTDE_TOTAL_CARTAS);

    while (!WindowShouldClose())
    {
        DesenhaCena();


        /*BeginDrawing();

        EndDrawing();*/


    }

    destroiTexturasPersonagens(imagensPersonagens, QTDE_TOTAL_CARTAS);
    CloseWindow();

    return 0;
}
