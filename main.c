#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 32
#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 720

typedef struct {
    char decimal[MAX_INPUT];
    char binary[MAX_INPUT];
    char hex[MAX_INPUT];
    char octal[MAX_INPUT];
    int activeInput;
    float cursorBlink;
    unsigned char bits[8];
} ConverterState;

Font uiFont;

/* ---------------- UNIVERSAL FONT LOADER ---------------- */

Font LoadUniversalFont(void)
{
    Font font = {0};

    // 1. Project-local font (portable)
    font = LoadFontEx("assets/JetBrainsMonoNerdFont-Bold.ttf", 96, 0, 250);
    if (font.texture.id != 0) return font;

#if defined(_WIN32)
    font = LoadFontEx("C:/Windows/Fonts/JetBrainsMono-Bold.ttf", 96, 0, 250);
    if (font.texture.id != 0) return font;

#elif defined(__APPLE__)
    font = LoadFontEx("/Library/Fonts/JetBrainsMono-Bold.ttf", 96, 0, 250);
    if (font.texture.id != 0) return font;

#elif defined(__linux__)
    font = LoadFontEx("/usr/share/fonts/truetype/jetbrains-mono/JetBrainsMono-Bold.ttf", 96, 0, 250);
    if (font.texture.id != 0) return font;
#endif

    return GetFontDefault();
}

/* ---------------- LOGIC ---------------- */

void UpdateBitsFromDecimal(int dec, ConverterState *s) {
    for (int i = 0; i < 8; i++)
        s->bits[i] = (dec >> (7 - i)) & 1;
}

void DecimalToOthers(int dec, ConverterState *s) {
    if (dec < 0 || dec > 255) return;

    sprintf(s->decimal, "%d", dec);
    sprintf(s->hex, "%02X", dec);
    sprintf(s->octal, "%03o", dec);

    for (int i = 7; i >= 0; i--)
        s->binary[7 - i] = ((dec >> i) & 1) + '0';

    s->binary[8] = '\0';
    UpdateBitsFromDecimal(dec, s);
}

int ToDecimal(const char *str, int base) {
    if (!str || !*str) return -1;

    char *end;
    long val = strtol(str, &end, base);

    if (*end != '\0' || val < 0 || val > 255)
        return -1;

    return (int)val;
}

void UpdateConversions(ConverterState *s) {
    int dec = -1;

    switch (s->activeInput) {
        case 0: dec = ToDecimal(s->decimal, 10); break;
        case 1: dec = ToDecimal(s->binary, 2);  break;
        case 2: dec = ToDecimal(s->hex, 16);     break;
        case 3: dec = ToDecimal(s->octal, 8);   break;
    }

    if (dec >= 0)
        DecimalToOthers(dec, s);
}

/* ---------------- UI ---------------- */

void DrawInputBox(const char *label, const char *value, Rectangle r,
                  bool active, Color accent, float blink)
{
    Color bg = (Color){248, 250, 252, 255};

    DrawRectangleRounded(r, 0.12f, 12, bg);

    DrawRectangleRoundedLines(
        r, 0.12f, 12,
        active ? 3 : 2,
        active ? accent : (Color){190,190,190,255}
    );

    DrawTextEx(uiFont, label, (Vector2){r.x + 16, r.y + 12}, 15, 1.1f, GRAY);
    DrawTextEx(uiFont, value, (Vector2){r.x + 16, r.y + 44}, 30, 1.25f, BLACK);

    if (active && blink < 0.5f) {
        Vector2 size = MeasureTextEx(uiFont, value, 30, 1.25f);
        DrawRectangle(r.x + 18 + size.x, r.y + 48, 2, 26, accent);
    }
}

void DrawBitVisualizer(ConverterState *s, int x, int y)
{
    DrawTextEx(uiFont, "8-BIT VISUAL BUILDER", (Vector2){x, y - 36}, 20, 1.2f, DARKGRAY);

    const char *labels[] = {"128","64","32","16","8","4","2","1"};

    for (int i = 0; i < 8; i++) {
        Rectangle box = { x + i * 96, y, 88, 88 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), box);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            s->bits[i] ^= 1;

            int dec = 0;
            for (int b = 0; b < 8; b++)
                dec += s->bits[b] << (7 - b);

            s->activeInput = 0;
            DecimalToOthers(dec, s);
        }

        Color fill = s->bits[i]
            ? (Color){37, 99, 235, 255}
            : (Color){226,232,240,255};

        if (hover) fill = ColorBrightness(fill, -0.1f);

        DrawRectangleRounded(box, 0.18f, 12, fill);
        DrawRectangleRoundedLines(box, 0.18f, 12, 2, GRAY);

        DrawTextEx(uiFont, s->bits[i] ? "1" : "0",
            (Vector2){box.x + 34, box.y + 24}, 36, 1.25f,
            s->bits[i] ? WHITE : DARKGRAY);

        DrawTextEx(uiFont, labels[i],
            (Vector2){box.x + 30, box.y + 94}, 14, 1.1f, GRAY);
    }
}

/* ---------------- MAIN ---------------- */

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Number System Converter");
    SetTargetFPS(60);

    uiFont = LoadUniversalFont();
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    ConverterState state = {0};
    strcpy(state.decimal, "0");
    DecimalToOthers(0, &state);

    Rectangle boxes[4] = {
        {40, 100, 400, 92},
        {460, 100, 400, 92},
        {40, 220, 400, 92},
        {460, 220, 400, 92}
    };

    const char *labels[] = {
        "DECIMAL (BASE 10)",
        "BINARY (BASE 2)",
        "HEXADECIMAL (BASE 16)",
        "OCTAL (BASE 8)"
    };

    Color accents[] = {
        (Color){59,130,246,255},
        (Color){34,197,94,255},
        (Color){168,85,247,255},
        (Color){249,115,22,255}
    };

    while (!WindowShouldClose()) {
        state.cursorBlink += GetFrameTime();
        if (state.cursorBlink > 1) state.cursorBlink = 0;

        int key = GetCharPressed();
        char *target = NULL;

        if (key > 0) {
            bool ok = false;

            switch (state.activeInput) {
                case 0: target = state.decimal; ok = isdigit(key); break;
                case 1: target = state.binary;  ok = (key=='0'||key=='1'); break;
                case 2: target = state.hex;     ok = isxdigit(key); key=toupper(key); break;
                case 3: target = state.octal;   ok = (key>='0'&&key<='7'); break;
            }

            if (ok && strlen(target) < MAX_INPUT-1) {
                if (strcmp(target,"0")==0) target[0]='\0';
                int len = strlen(target);
                target[len]=key;
                target[len+1]='\0';
                UpdateConversions(&state);
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            switch (state.activeInput) {
                case 0: target = state.decimal; break;
                case 1: target = state.binary;  break;
                case 2: target = state.hex;     break;
                case 3: target = state.octal;   break;
            }
            if (target && strlen(target)>0) {
                target[strlen(target)-1]='\0';
                if (!*target) strcpy(target,"0");
                UpdateConversions(&state);
            }
        }

        for (int i=0;i<4;i++)
            if (CheckCollisionPointRec(GetMousePosition(), boxes[i]) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                state.activeInput = i;

        BeginDrawing();
        ClearBackground((Color){241,245,249,255});

        DrawRectangle(0,0,SCREEN_WIDTH,80,(Color){15,23,42,255});
        DrawTextEx(uiFont,"NUMBER SYSTEM CONVERTER",(Vector2){30,26},28,1.3f,WHITE);
        DrawTextEx(uiFont,"Decimal • Binary • Hex • Octal",(Vector2){32,54},14,1.1f,(Color){148,163,184,255});

        char *vals[] = {state.decimal,state.binary,state.hex,state.octal};

        for (int i=0;i<4;i++)
            DrawInputBox(labels[i], vals[i], boxes[i],
                state.activeInput==i, accents[i], state.cursorBlink);

        DrawBitVisualizer(&state, 60, 360);

        EndDrawing();
    }

    UnloadFont(uiFont);
    CloseWindow();
    return 0;
}
