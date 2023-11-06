#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <raylib.h>

typedef struct SpriteAnimation
{
    Texture2D personagem;
    int framesPerSecond;

    Rectangle *rectangles;
    int rectanglesLength;

} SpriteAnimation;

SpriteAnimation CreateSpriteAnimation(Texture2D personagem, int framesPerSecond, Rectangle rectangles[], int rectanglesLength);

void DisposeSpriteAnimation(SpriteAnimation animation);

void DrawSpriteAnimationPro(SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint);

#endif // ANIMATION_H_
