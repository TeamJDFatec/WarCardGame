#include <animation.h>
#include <stdlib.h>

SpriteAnimation CreateSpriteAnimation(Texture2D personagem, int framesPerSecond, Rectangle rectangles[], int rectanglesLength)
{
    SpriteAnimation spriteAnimation =
    {
        .personagem = personagem,
        .framesPerSecond = framesPerSecond,
        .rectanglesLength = rectanglesLength
    };

    Rectangle* mem = (Rectangle*)malloc(sizeof(Rectangle) * rectanglesLength);
    if (mem == NULL)
    {
        TraceLog(LOG_FATAL, "No memory for CreateSpriteAnimation");
        spriteAnimation.rectanglesLength = 0;
        return spriteAnimation;
    }

    spriteAnimation.rectangles = mem;
    spriteAnimation.rectangles = rectangles;

    return spriteAnimation;
}

void DisposeSpriteAniation(SpriteAnimation animation)
{
    free(animation.rectangles);
}

void DrawSpriteAnimationPro(SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint)
{
    int index = (int)(GetTime() * animation.framesPerSecond) % animation.rectanglesLength;

    Rectangle source = animation.rectangles[index];
    DrawTexturePro(animation.personagem, source, dest, origin, rotation, tint);
}
