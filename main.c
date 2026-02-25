/**
 * NumInter - Number System Converter
 * A modern visual desktop app for number base conversions
 *
 * Features:
 *   - Real-time conversion: Decimal, Binary, Hex, Octal
 *   - Interactive 8-bit / 16-bit visual builder
 *   - Conversion history with copy support
 *   - Dark / Light theme toggle
 *   - ASCII character preview
 *   - Bitwise operation calculator (AND, OR, XOR, NOT, SHIFT)
 *   - Keyboard shortcuts throughout
 *   - Animated transitions and hover effects
 *   - Floating-point IEEE 754 viewer
 *
 * Author: Shiva Kar
 * License: MIT
 */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ================================================================
 *  CONFIGURATION
 * ================================================================ */

#define MAX_INPUT       64
#define SCREEN_WIDTH    1100
#define SCREEN_HEIGHT   800
#define HISTORY_MAX     20
#define ANIM_SPEED      8.0f

/* ================================================================
 *  THEME SYSTEM
 * ================================================================ */

typedef struct {
    Color bg;
    Color surface;
    Color surfaceHover;
    Color card;
    Color cardBorder;
    Color header;
    Color headerText;
    Color headerSub;
    Color text;
    Color textSecondary;
    Color textMuted;
    Color accent;
    Color bitOn;
    Color bitOff;
    Color bitOnText;
    Color bitOffText;
    Color shadow;
    Color success;
    Color error;
    Color warning;
    Color divider;
    Color tabActive;
    Color tabInactive;
    Color tooltip;
    Color tooltipText;
} Theme;

static const Theme THEME_LIGHT = {
    .bg            = {241, 245, 249, 255},
    .surface       = {255, 255, 255, 255},
    .surfaceHover  = {248, 250, 252, 255},
    .card          = {255, 255, 255, 255},
    .cardBorder    = {226, 232, 240, 255},
    .header        = { 15,  23,  42, 255},
    .headerText    = {255, 255, 255, 255},
    .headerSub     = {148, 163, 184, 255},
    .text          = { 15,  23,  42, 255},
    .textSecondary = { 71,  85, 105, 255},
    .textMuted     = {148, 163, 184, 255},
    .accent        = { 59, 130, 246, 255},
    .bitOn         = { 37,  99, 235, 255},
    .bitOff        = {226, 232, 240, 255},
    .bitOnText     = {255, 255, 255, 255},
    .bitOffText    = {100, 116, 139, 255},
    .shadow        = {  0,   0,   0,  20},
    .success       = { 34, 197,  94, 255},
    .error         = {239,  68,  68, 255},
    .warning       = {249, 115,  22, 255},
    .divider       = {226, 232, 240, 255},
    .tabActive     = { 59, 130, 246, 255},
    .tabInactive   = {148, 163, 184, 255},
    .tooltip       = { 30,  41,  59, 240},
    .tooltipText   = {255, 255, 255, 255},
};

static const Theme THEME_DARK = {
    .bg            = { 15,  23,  42, 255},
    .surface       = { 30,  41,  59, 255},
    .surfaceHover  = { 51,  65,  85, 255},
    .card          = { 30,  41,  59, 255},
    .cardBorder    = { 51,  65,  85, 255},
    .header        = {  2,   6,  23, 255},
    .headerText    = {255, 255, 255, 255},
    .headerSub     = {100, 116, 139, 255},
    .text          = {226, 232, 240, 255},
    .textSecondary = {148, 163, 184, 255},
    .textMuted     = {100, 116, 139, 255},
    .accent        = { 96, 165, 250, 255},
    .bitOn         = { 59, 130, 246, 255},
    .bitOff        = { 51,  65,  85, 255},
    .bitOnText     = {255, 255, 255, 255},
    .bitOffText    = {148, 163, 184, 255},
    .shadow        = {  0,   0,   0,  60},
    .success       = { 74, 222, 128, 255},
    .error         = {248, 113, 113, 255},
    .warning       = {251, 146,  60, 255},
    .divider       = { 51,  65,  85, 255},
    .tabActive     = { 96, 165, 250, 255},
    .tabInactive   = {100, 116, 139, 255},
    .tooltip       = {241, 245, 249, 240},
    .tooltipText   = { 15,  23,  42, 255},
};

/* ================================================================
 *  DATA STRUCTURES
 * ================================================================ */

typedef struct {
    char decimal[MAX_INPUT];
    char binary[MAX_INPUT];
    char hex[MAX_INPUT];
    char octal[MAX_INPUT];
    int  activeInput;           // 0=dec, 1=bin, 2=hex, 3=oct
    float cursorBlink;
    unsigned char bits[16];
    int  bitWidth;              // 8 or 16
} ConverterState;

typedef struct {
    char label[32];
    char decimal[MAX_INPUT];
    char binary[MAX_INPUT];
    char hex[MAX_INPUT];
    char octal[MAX_INPUT];
} HistoryEntry;

typedef struct {
    HistoryEntry entries[HISTORY_MAX];
    int count;
} ConversionHistory;

// Bitwise operation
typedef struct {
    char operandA[MAX_INPUT];
    char operandB[MAX_INPUT];
    int  activeField;   // 0=A, 1=B
    int  operation;     // 0=AND, 1=OR, 2=XOR, 3=NOT, 4=SHL, 5=SHR
} BitwiseState;

// Tabs
typedef enum {
    TAB_CONVERTER = 0,
    TAB_BITWISE,
    TAB_IEEE754,
    TAB_COUNT
} AppTab;

// Application state
typedef struct {
    ConverterState conv;
    ConversionHistory history;
    BitwiseState bitwise;
    AppTab currentTab;
    bool darkMode;
    Theme theme;
    Font font;
    float animTime;

    // IEEE 754
    char ieee_input[MAX_INPUT];
    bool ieee_is_double;

    // Toast notification
    char toast[128];
    float toastTimer;

    // Copy feedback
    float copyFeedback[4];  // per input box

    // Smooth animations
    float tabAnim;
    float bitAnims[16];     // per bit smooth toggle
} AppState;

/* ================================================================
 *  FONT LOADER
 * ================================================================ */

static Font LoadUniversalFont(void)
{
    Font font = {0};

    font = LoadFontEx("assets/JetBrainsMonoNerdFont-Bold.ttf", 48, 0, 250);
    if (font.texture.id != 0) return font;

#if defined(_WIN32)
    font = LoadFontEx("C:/Windows/Fonts/JetBrainsMono-Bold.ttf", 48, 0, 250);
    if (font.texture.id != 0) return font;
    font = LoadFontEx("C:/Windows/Fonts/consola.ttf", 48, 0, 250);
    if (font.texture.id != 0) return font;
#elif defined(__APPLE__)
    font = LoadFontEx("/Library/Fonts/JetBrainsMono-Bold.ttf", 48, 0, 250);
    if (font.texture.id != 0) return font;
#elif defined(__linux__)
    font = LoadFontEx("/usr/share/fonts/truetype/jetbrains-mono/JetBrainsMono-Bold.ttf", 48, 0, 250);
    if (font.texture.id != 0) return font;
#endif

    return GetFontDefault();
}

/* ================================================================
 *  CONVERSION LOGIC
 * ================================================================ */

static void UpdateBitsFromValue(int val, ConverterState *s)
{
    int max = (s->bitWidth == 16) ? 16 : 8;
    for (int i = 0; i < max; i++)
        s->bits[i] = (val >> (max - 1 - i)) & 1;
}

static int GetMaxValue(ConverterState *s)
{
    return (s->bitWidth == 16) ? 65535 : 255;
}

static void ValueToAllBases(int val, ConverterState *s)
{
    int maxVal = GetMaxValue(s);
    if (val < 0 || val > maxVal) return;

    sprintf(s->decimal, "%d", val);
    sprintf(s->hex, (s->bitWidth == 16) ? "%04X" : "%02X", val);
    sprintf(s->octal, "%o", val);

    int bits = s->bitWidth;
    for (int i = bits - 1; i >= 0; i--)
        s->binary[bits - 1 - i] = ((val >> i) & 1) + '0';
    s->binary[bits] = '\0';

    UpdateBitsFromValue(val, s);
}

static int ParseBase(const char *str, int base)
{
    if (!str || !*str) return -1;
    char *end;
    long val = strtol(str, &end, base);
    if (*end != '\0' || val < 0 || val > 65535) return -1;
    return (int)val;
}

static void UpdateConversions(ConverterState *s)
{
    int dec = -1;
    switch (s->activeInput) {
        case 0: dec = ParseBase(s->decimal, 10); break;
        case 1: dec = ParseBase(s->binary,   2); break;
        case 2: dec = ParseBase(s->hex,     16); break;
        case 3: dec = ParseBase(s->octal,    8); break;
    }
    if (dec >= 0 && dec <= GetMaxValue(s))
        ValueToAllBases(dec, s);
}

/* ================================================================
 *  HISTORY
 * ================================================================ */

static void HistoryAdd(ConversionHistory *h, ConverterState *s)
{
    if (strlen(s->decimal) == 0) return;
    // Don't add if same as last
    if (h->count > 0 && strcmp(h->entries[h->count - 1].decimal, s->decimal) == 0)
        return;

    if (h->count >= HISTORY_MAX) {
        // Shift everything down
        for (int i = 0; i < HISTORY_MAX - 1; i++)
            h->entries[i] = h->entries[i + 1];
        h->count = HISTORY_MAX - 1;
    }

    HistoryEntry *e = &h->entries[h->count];
    sprintf(e->label, "#%d", h->count + 1);
    strcpy(e->decimal, s->decimal);
    strcpy(e->binary,  s->binary);
    strcpy(e->hex,     s->hex);
    strcpy(e->octal,   s->octal);
    h->count++;
}

/* ================================================================
 *  BITWISE OPERATIONS
 * ================================================================ */

static int BitwiseCompute(int a, int b, int op)
{
    switch (op) {
        case 0: return a & b;
        case 1: return a | b;
        case 2: return a ^ b;
        case 3: return ~a & 0xFFFF;
        case 4: return (a << b) & 0xFFFF;
        case 5: return (a >> b) & 0xFFFF;
        default: return 0;
    }
}

static const char *BitwiseOpName(int op)
{
    const char *names[] = {"AND", "OR", "XOR", "NOT", "SHL", "SHR"};
    return (op >= 0 && op < 6) ? names[op] : "?";
}

static const char *BitwiseOpSymbol(int op)
{
    const char *syms[] = {"&", "|", "^", "~", "<<", ">>"};
    return (op >= 0 && op < 6) ? syms[op] : "?";
}

/* ================================================================
 *  IEEE 754
 * ================================================================ */

typedef struct {
    int sign;
    int exponent;
    unsigned long long mantissa;
    char sign_bits[2];
    char exponent_bits[16];
    char mantissa_bits[64];
    char hex_repr[32];
    char full_binary[128];
    bool is_special;        // inf, nan, denormalized
    char special_label[32];
} IEEE754Result;

static IEEE754Result AnalyzeFloat(float value)
{
    IEEE754Result r = {0};
    unsigned int bits;
    memcpy(&bits, &value, sizeof(bits));

    r.sign = (bits >> 31) & 1;
    r.exponent = ((bits >> 23) & 0xFF) - 127;
    r.mantissa = bits & 0x7FFFFF;

    sprintf(r.sign_bits, "%d", r.sign);
    sprintf(r.hex_repr, "0x%08X", bits);

    // Exponent bits (8 bits)
    int raw_exp = (bits >> 23) & 0xFF;
    for (int i = 7; i >= 0; i--)
        r.exponent_bits[7 - i] = ((raw_exp >> i) & 1) + '0';
    r.exponent_bits[8] = '\0';

    // Mantissa bits (23 bits)
    for (int i = 22; i >= 0; i--)
        r.mantissa_bits[22 - i] = ((r.mantissa >> i) & 1) + '0';
    r.mantissa_bits[23] = '\0';

    // Full binary
    for (int i = 31; i >= 0; i--)
        r.full_binary[31 - i] = ((bits >> i) & 1) + '0';
    r.full_binary[32] = '\0';

    // Special values
    r.is_special = false;
    if (raw_exp == 0xFF) {
        r.is_special = true;
        if (r.mantissa == 0)
            strcpy(r.special_label, r.sign ? "-Infinity" : "+Infinity");
        else
            strcpy(r.special_label, "NaN");
    } else if (raw_exp == 0 && r.mantissa == 0) {
        r.is_special = true;
        strcpy(r.special_label, r.sign ? "-0" : "+0");
    } else if (raw_exp == 0) {
        r.is_special = true;
        strcpy(r.special_label, "Denormalized");
    }

    return r;
}

/* ================================================================
 *  CLIPBOARD
 * ================================================================ */

static void CopyToClipboard(const char *text)
{
    SetClipboardText(text);
}

/* ================================================================
 *  TOAST NOTIFICATION
 * ================================================================ */

static void ShowToast(AppState *app, const char *msg)
{
    strncpy(app->toast, msg, sizeof(app->toast) - 1);
    app->toastTimer = 2.0f;
}

/* ================================================================
 *  DRAWING HELPERS
 * ================================================================ */

static Color ColorLerp(Color a, Color b, float t)
{
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return (Color){
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t),
    };
}

static void DrawShadowRect(Rectangle r, float radius, Color shadow)
{
    Rectangle sr = {r.x + 2, r.y + 3, r.width, r.height};
    DrawRectangleRounded(sr, radius, 12, shadow);
}

static void DrawCard(Rectangle r, Theme *t)
{
    DrawShadowRect(r, 0.08f, t->shadow);
    DrawRectangleRounded(r, 0.08f, 12, t->card);
    DrawRectangleRoundedLines(r, 0.08f, 12, 1.0f, t->cardBorder);
}

static void DrawPill(int x, int y, const char *text, Color bg, Color fg, Font font)
{
    Vector2 sz = MeasureTextEx(font, text, 13, 1.0f);
    Rectangle pill = {x, y, sz.x + 16, 22};
    DrawRectangleRounded(pill, 0.5f, 8, bg);
    DrawTextEx(font, text, (Vector2){x + 8, y + 4}, 13, 1.0f, fg);
}

static bool DrawButton(Rectangle r, const char *label, Color bg, Color fg,
                       Color hoverBg, Font font, float fontSize)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);
    Color fill = hover ? hoverBg : bg;

    DrawRectangleRounded(r, 0.2f, 8, fill);
    DrawRectangleRoundedLines(r, 0.2f, 8, 1.0f, ColorBrightness(fill, -0.15f));

    Vector2 sz = MeasureTextEx(font, label, fontSize, 1.0f);
    float tx = r.x + (r.width - sz.x) / 2;
    float ty = r.y + (r.height - sz.y) / 2;
    DrawTextEx(font, label, (Vector2){tx, ty}, fontSize, 1.0f, fg);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

/* ================================================================
 *  INPUT BOX (MODERN)
 * ================================================================ */

static bool DrawInputBoxModern(AppState *app, const char *label, const char *value,
                               Rectangle r, bool active, Color accent, int index)
{
    Theme *t = &app->theme;
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // Card with shadow
    DrawShadowRect(r, 0.10f, t->shadow);
    DrawRectangleRounded(r, 0.10f, 12, active ? t->surface : t->card);

    // Left accent bar
    if (active) {
        Rectangle bar = {r.x, r.y + 8, 4, r.height - 16};
        DrawRectangleRounded(bar, 0.5f, 4, accent);
    }

    // Border
    DrawRectangleRoundedLines(r, 0.10f, 12,
        active ? 2.0f : 1.0f,
        active ? accent : (hover ? ColorBrightness(t->cardBorder, -0.1f) : t->cardBorder));

    // Label
    DrawTextEx(app->font, label, (Vector2){r.x + 20, r.y + 10}, 13, 1.0f, t->textMuted);

    // Value
    DrawTextEx(app->font, value, (Vector2){r.x + 20, r.y + 36}, 28, 1.2f, t->text);

    // Cursor
    if (active && app->conv.cursorBlink < 0.5f) {
        Vector2 sz = MeasureTextEx(app->font, value, 28, 1.2f);
        DrawRectangle(r.x + 22 + (int)sz.x, r.y + 40, 2, 24, accent);
    }

    // Copy button
    Rectangle copyBtn = {r.x + r.width - 60, r.y + 8, 50, 22};
    bool copyHover = CheckCollisionPointRec(GetMousePosition(), copyBtn);

    if (app->copyFeedback[index] > 0) {
        DrawPill(copyBtn.x, copyBtn.y, "Copied!", t->success,
                 (Color){255, 255, 255, 255}, app->font);
    } else {
        Color cbg = copyHover ? ColorBrightness(t->cardBorder, -0.1f) : t->cardBorder;
        DrawPill(copyBtn.x, copyBtn.y, "Copy", cbg, t->textSecondary, app->font);

        if (copyHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CopyToClipboard(value);
            app->copyFeedback[index] = 1.0f;
            ShowToast(app, "Copied to clipboard!");
        }
    }

    return clicked;
}

/* ================================================================
 *  BIT VISUALIZER (MODERN)
 * ================================================================ */

static void DrawBitVisualizerModern(AppState *app, int x, int y)
{
    Theme *t = &app->theme;
    ConverterState *s = &app->conv;
    int numBits = s->bitWidth;
    float dt = GetFrameTime();

    const char *labels8[]  = {"128","64","32","16","8","4","2","1"};
    const char *labels16[] = {"32768","16384","8192","4096","2048","1024","512","256",
                              "128","64","32","16","8","4","2","1"};
    const char **labels = (numBits == 16) ? labels16 : labels8;

    int bitSize = (numBits == 16) ? 48 : 76;
    int spacing = (numBits == 16) ? 54 : 84;

    // Section label
    char section_label[64];
    int totalVal = 0;
    for (int i = 0; i < numBits; i++)
        totalVal += s->bits[i] << (numBits - 1 - i);
    sprintf(section_label, "%d-BIT VISUAL BUILDER  [%d]", numBits, totalVal);
    DrawTextEx(app->font, section_label, (Vector2){x, y - 36}, 16, 1.1f, t->textMuted);

    // Toggle 8/16 button
    Rectangle toggleBtn = {x + (numBits == 16 ? 700 : 540), y - 40, 80, 26};
    char toggleLabel[16];
    sprintf(toggleLabel, "%d-bit", numBits == 8 ? 16 : 8);
    if (DrawButton(toggleBtn, toggleLabel, t->accent,
                   (Color){255,255,255,255}, ColorBrightness(t->accent, -0.15f),
                   app->font, 13))
    {
        s->bitWidth = (numBits == 8) ? 16 : 8;
        memset(s->bits, 0, sizeof(s->bits));
        ValueToAllBases(0, s);
        strcpy(s->decimal, "0");
        ShowToast(app, s->bitWidth == 16 ? "Switched to 16-bit mode" : "Switched to 8-bit mode");
    }

    for (int i = 0; i < numBits; i++) {
        // Animate bit toggle
        float target = s->bits[i] ? 1.0f : 0.0f;
        app->bitAnims[i] += (target - app->bitAnims[i]) * dt * ANIM_SPEED;

        Rectangle box = { x + i * spacing, y, bitSize, bitSize };
        bool hover = CheckCollisionPointRec(GetMousePosition(), box);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            s->bits[i] ^= 1;
            int dec = 0;
            for (int b = 0; b < numBits; b++)
                dec += s->bits[b] << (numBits - 1 - b);
            s->activeInput = 0;
            ValueToAllBases(dec, s);
            HistoryAdd(&app->history, s);
        }

        // Interpolated color
        Color fill = ColorLerp(t->bitOff, t->bitOn, app->bitAnims[i]);
        Color textCol = ColorLerp(t->bitOffText, t->bitOnText, app->bitAnims[i]);

        if (hover) fill = ColorBrightness(fill, -0.08f);

        // Draw bit with shadow
        DrawShadowRect(box, 0.18f, t->shadow);
        DrawRectangleRounded(box, 0.18f, 12, fill);
        DrawRectangleRoundedLines(box, 0.18f, 12, 1.5f,
            hover ? t->accent : ColorBrightness(fill, -0.1f));

        // Bit value
        float fontSize = (numBits == 16) ? 20 : 30;
        const char *bitText = s->bits[i] ? "1" : "0";
        Vector2 bsz = MeasureTextEx(app->font, bitText, fontSize, 1.2f);
        DrawTextEx(app->font, bitText,
            (Vector2){box.x + (bitSize - bsz.x) / 2, box.y + (bitSize - bsz.y) / 2},
            fontSize, 1.2f, textCol);

        // Weight label
        float lblFontSize = (numBits == 16) ? 10 : 13;
        Vector2 lsz = MeasureTextEx(app->font, labels[i], lblFontSize, 1.0f);
        DrawTextEx(app->font, labels[i],
            (Vector2){box.x + (bitSize - lsz.x) / 2, box.y + bitSize + 4},
            lblFontSize, 1.0f, t->textMuted);

    }
}

/* ================================================================
 *  ASCII PREVIEW CARD
 * ================================================================ */

static void DrawASCIIPreview(AppState *app, int x, int y, int width)
{
    Theme *t = &app->theme;
    int dec = ParseBase(app->conv.decimal, 10);
    if (dec < 0 || dec > 127) return;

    Rectangle card = {x, y, width, 70};
    DrawCard(card, t);

    DrawTextEx(app->font, "ASCII CHARACTER", (Vector2){x + 16, y + 8}, 12, 1.0f, t->textMuted);

    // Character display
    char ch_str[16] = {0};
    const char *display;
    if (dec < 32) {
        const char *ctrl[] = {
            "NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL",
            "BS","TAB","LF","VT","FF","CR","SO","SI",
            "DLE","DC1","DC2","DC3","DC4","NAK","SYN","ETB",
            "CAN","EM","SUB","ESC","FS","GS","RS","US"
        };
        display = ctrl[dec];
    } else if (dec == 32) {
        display = "SPACE";
    } else if (dec == 127) {
        display = "DEL";
    } else {
        sprintf(ch_str, "'%c'", (char)dec);
        display = ch_str;
    }

    DrawTextEx(app->font, display, (Vector2){x + 16, y + 30}, 28, 1.2f, t->accent);

    // Description
    char desc[64];
    sprintf(desc, "Dec: %d  |  Hex: 0x%02X", dec, dec);
    DrawTextEx(app->font, desc, (Vector2){x + 140, y + 38}, 14, 1.0f, t->textSecondary);
}

/* ================================================================
 *  CONVERSION HISTORY PANEL
 * ================================================================ */

static void DrawHistoryPanel(AppState *app, int x, int y, int width, int height)
{
    Theme *t = &app->theme;
    Rectangle card = {x, y, width, height};
    DrawCard(card, t);

    DrawTextEx(app->font, "CONVERSION HISTORY", (Vector2){x + 16, y + 10}, 13, 1.0f, t->textMuted);

    if (app->history.count == 0) {
        DrawTextEx(app->font, "No conversions yet",
            (Vector2){x + 16, y + 40}, 14, 1.0f, t->textMuted);
        return;
    }

    // Clear button
    Rectangle clearBtn = {x + width - 60, y + 6, 50, 22};
    if (DrawButton(clearBtn, "Clear", t->error,
                   (Color){255,255,255,255}, ColorBrightness(t->error, -0.15f),
                   app->font, 12))
    {
        app->history.count = 0;
        ShowToast(app, "History cleared");
    }

    int startY = y + 36;
    int maxShow = (height - 50) / 24;
    int start = app->history.count > maxShow ? app->history.count - maxShow : 0;

    for (int i = start; i < app->history.count; i++) {
        HistoryEntry *e = &app->history.entries[i];
        int ey = startY + (i - start) * 24;

        // Alternating row background
        if ((i - start) % 2 == 0) {
            DrawRectangle(x + 4, ey - 2, width - 8, 22, t->surfaceHover);
        }

        char row[128];
        sprintf(row, "D:%-5s  B:%-16s  H:%-4s  O:%s",
                e->decimal, e->binary, e->hex, e->octal);
        DrawTextEx(app->font, row, (Vector2){x + 12, ey}, 12, 0.8f, t->textSecondary);

        // Click to load
        Rectangle rowRect = {x + 4, ey - 2, width - 8, 22};
        if (CheckCollisionPointRec(GetMousePosition(), rowRect) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            strcpy(app->conv.decimal, e->decimal);
            app->conv.activeInput = 0;
            UpdateConversions(&app->conv);
            ShowToast(app, "Loaded from history");
        }
    }
}

/* ================================================================
 *  TAB BAR
 * ================================================================ */

static void DrawTabBar(AppState *app, int x, int y, int width)
{
    Theme *t = &app->theme;
    const char *tabs[] = {"Converter", "Bitwise Ops", "IEEE 754"};
    const char *icons[] = {"[1]", "[2]", "[3]"};

    int tabWidth = width / TAB_COUNT;

    for (int i = 0; i < TAB_COUNT; i++) {
        Rectangle tabRect = {x + i * tabWidth, y, tabWidth, 38};
        bool hover = CheckCollisionPointRec(GetMousePosition(), tabRect);
        bool active = (app->currentTab == i);

        // Background
        Color bg = active ? t->surface : (hover ? t->surfaceHover : t->bg);
        DrawRectangleRec(tabRect, bg);

        // Active indicator
        if (active) {
            DrawRectangle(tabRect.x, tabRect.y + tabRect.height - 3,
                          tabRect.width, 3, t->accent);
        }

        // Label
        Color fg = active ? t->accent : t->tabInactive;
        char label[64];
        sprintf(label, "%s %s", icons[i], tabs[i]);
        Vector2 sz = MeasureTextEx(app->font, label, 14, 1.0f);
        DrawTextEx(app->font, label,
            (Vector2){tabRect.x + (tabRect.width - sz.x) / 2, tabRect.y + 10},
            14, 1.0f, fg);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            app->currentTab = i;
        }
    }

    // Divider
    DrawLineEx((Vector2){x, y + 38}, (Vector2){x + width, y + 38}, 1, t->divider);
}

/* ================================================================
 *  BITWISE CALCULATOR TAB
 * ================================================================ */

static void DrawBitwiseTab(AppState *app, int x, int y)
{
    Theme *t = &app->theme;
    BitwiseState *bw = &app->bitwise;

    DrawTextEx(app->font, "BITWISE OPERATION CALCULATOR",
               (Vector2){x, y}, 18, 1.2f, t->text);
    DrawTextEx(app->font, "Enter values in decimal. Results shown in all bases.",
               (Vector2){x, y + 26}, 13, 1.0f, t->textMuted);

    // Operand A input
    Rectangle boxA = {x, y + 56, 300, 65};
    bool hoverA = CheckCollisionPointRec(GetMousePosition(), boxA);
    if (hoverA && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) bw->activeField = 0;

    DrawCard(boxA, t);
    if (bw->activeField == 0) {
        Rectangle bar = {boxA.x, boxA.y + 8, 4, boxA.height - 16};
        DrawRectangleRounded(bar, 0.5f, 4, t->accent);
    }
    DrawTextEx(app->font, "OPERAND A (Decimal)", (Vector2){boxA.x + 16, boxA.y + 8}, 12, 1.0f, t->textMuted);
    DrawTextEx(app->font, bw->operandA, (Vector2){boxA.x + 16, boxA.y + 30}, 24, 1.2f, t->text);

    // Operation selector
    const char *ops[] = {"AND", "OR", "XOR", "NOT", "SHL", "SHR"};
    for (int i = 0; i < 6; i++) {
        Rectangle btn = {x + 320 + (i % 3) * 80, y + 56 + (i / 3) * 36, 72, 30};
        Color bg = (bw->operation == i) ? t->accent : t->surface;
        Color fg = (bw->operation == i) ? (Color){255,255,255,255} : t->textSecondary;
        if (DrawButton(btn, ops[i], bg, fg, ColorBrightness(bg, -0.1f), app->font, 13)) {
            bw->operation = i;
        }
    }

    // Operand B (not for NOT)
    if (bw->operation != 3) {
        Rectangle boxB = {x, y + 136, 300, 65};
        bool hoverB = CheckCollisionPointRec(GetMousePosition(), boxB);
        if (hoverB && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) bw->activeField = 1;

        DrawCard(boxB, t);
        if (bw->activeField == 1) {
            Rectangle bar = {boxB.x, boxB.y + 8, 4, boxB.height - 16};
            DrawRectangleRounded(bar, 0.5f, 4, t->accent);
        }
        DrawTextEx(app->font, "OPERAND B (Decimal)", (Vector2){boxB.x + 16, boxB.y + 8}, 12, 1.0f, t->textMuted);
        DrawTextEx(app->font, bw->operandB, (Vector2){boxB.x + 16, boxB.y + 30}, 24, 1.2f, t->text);
    }

    // Result
    int a = ParseBase(bw->operandA, 10);
    int b = ParseBase(bw->operandB, 10);
    if (a < 0) a = 0;
    if (b < 0) b = 0;

    int result = BitwiseCompute(a, b, bw->operation);

    Rectangle resCard = {x, y + 220, 560, 100};
    DrawCard(resCard, t);

    DrawTextEx(app->font, "RESULT", (Vector2){resCard.x + 16, resCard.y + 8}, 12, 1.0f, t->textMuted);

    // Expression
    char expr[128];
    if (bw->operation == 3)
        sprintf(expr, "%s %d = %d", BitwiseOpSymbol(bw->operation), a, result);
    else
        sprintf(expr, "%d %s %d = %d", a, BitwiseOpSymbol(bw->operation), b, result);
    DrawTextEx(app->font, expr, (Vector2){resCard.x + 16, resCard.y + 30}, 22, 1.2f, t->accent);

    // Multi-base result
    char resBin[MAX_INPUT] = {0};
    for (int i = 15; i >= 0; i--)
        resBin[15 - i] = ((result >> i) & 1) + '0';
    resBin[16] = '\0';

    char resLine[128];
    sprintf(resLine, "Hex: 0x%04X  |  Oct: %o  |  Bin: %s", result, result, resBin);
    DrawTextEx(app->font, resLine, (Vector2){resCard.x + 16, resCard.y + 64}, 13, 1.0f, t->textSecondary);

    // Copy result
    Rectangle copyResBtn = {resCard.x + resCard.width - 60, resCard.y + 8, 50, 22};
    if (DrawButton(copyResBtn, "Copy", t->cardBorder, t->textSecondary,
                   t->surfaceHover, app->font, 12))
    {
        char buf[32];
        sprintf(buf, "%d", result);
        CopyToClipboard(buf);
        ShowToast(app, "Result copied!");
    }
}

/* ================================================================
 *  IEEE 754 TAB
 * ================================================================ */

static void DrawIEEE754Tab(AppState *app, int x, int y)
{
    Theme *t = &app->theme;

    DrawTextEx(app->font, "IEEE 754 FLOATING POINT ANALYZER",
               (Vector2){x, y}, 18, 1.2f, t->text);
    DrawTextEx(app->font, "Enter a decimal number to see its IEEE 754 single-precision representation.",
               (Vector2){x, y + 26}, 13, 1.0f, t->textMuted);

    // Input box
    Rectangle inputBox = {x, y + 56, 400, 65};
    DrawCard(inputBox, t);
    Rectangle bar = {inputBox.x, inputBox.y + 8, 4, inputBox.height - 16};
    DrawRectangleRounded(bar, 0.5f, 4, t->warning);
    DrawTextEx(app->font, "DECIMAL VALUE", (Vector2){inputBox.x + 16, inputBox.y + 8}, 12, 1.0f, t->textMuted);
    DrawTextEx(app->font, app->ieee_input, (Vector2){inputBox.x + 16, inputBox.y + 30}, 24, 1.2f, t->text);

    // Cursor
    if (app->conv.cursorBlink < 0.5f) {
        Vector2 sz = MeasureTextEx(app->font, app->ieee_input, 24, 1.2f);
        DrawRectangle(inputBox.x + 18 + (int)sz.x, inputBox.y + 34, 2, 22, t->warning);
    }

    // Analyze
    float val = (float)atof(app->ieee_input);
    if (strlen(app->ieee_input) == 0) val = 0.0f;

    IEEE754Result ieee = AnalyzeFloat(val);

    // Bit layout visualization
    int by = y + 140;
    DrawTextEx(app->font, "BIT LAYOUT (32 bits)", (Vector2){x, by}, 14, 1.0f, t->textMuted);
    by += 22;

    // Sign bit (red)
    Color signColor = {239, 68, 68, 255};
    Color expColor  = {59, 130, 246, 255};
    Color mantColor = {34, 197, 94, 255};

    int bx = x;
    int bw = 28, bh = 32;

    // Labels
    DrawPill(bx, by - 2, "Sign", signColor, (Color){255,255,255,255}, app->font);
    DrawPill(bx + 40, by - 2, "Exponent (8)", expColor, (Color){255,255,255,255}, app->font);
    DrawPill(bx + 180, by - 2, "Mantissa (23)", mantColor, (Color){255,255,255,255}, app->font);
    by += 24;

    for (int i = 0; i < 32; i++) {
        Color bitColor;
        if (i == 0) bitColor = signColor;
        else if (i < 9) bitColor = expColor;
        else bitColor = mantColor;

        int col = i;
        Rectangle bitBox = {bx + col * (bw + 1), by, bw, bh};
        char bit[2] = {ieee.full_binary[i], '\0'};

        Color bg = ieee.full_binary[i] == '1' ? bitColor : ColorBrightness(bitColor, 0.5f);
        Color fg = ieee.full_binary[i] == '1' ? (Color){255,255,255,255} : bitColor;

        DrawRectangleRounded(bitBox, 0.15f, 4, bg);
        DrawTextEx(app->font, bit,
            (Vector2){bitBox.x + (bw - 8) / 2, bitBox.y + (bh - 12) / 2},
            12, 1.0f, fg);

        // Nibble separator
        if (i > 0 && i % 4 == 0) {
            DrawLineEx((Vector2){bitBox.x - 1, by + 2},
                       (Vector2){bitBox.x - 1, by + bh - 2}, 1.0f, t->divider);
        }
    }
    by += bh + 16;

    // Summary card
    Rectangle sumCard = {x, by, 560, 120};
    DrawCard(sumCard, t);

    DrawTextEx(app->font, "ANALYSIS", (Vector2){sumCard.x + 16, sumCard.y + 8}, 12, 1.0f, t->textMuted);

    char line1[128], line2[128], line3[128], line4[128];
    sprintf(line1, "Value: %.10g", val);
    sprintf(line2, "Sign: %s  |  Exponent: %d  (biased: %d)",
            ieee.sign ? "-" : "+", ieee.exponent, ieee.exponent + 127);
    sprintf(line3, "Hex: %s", ieee.hex_repr);
    if (ieee.is_special)
        sprintf(line4, "Special: %s", ieee.special_label);
    else
        strcpy(line4, "Normal number");

    DrawTextEx(app->font, line1, (Vector2){sumCard.x + 16, sumCard.y + 28}, 16, 1.0f, t->accent);
    DrawTextEx(app->font, line2, (Vector2){sumCard.x + 16, sumCard.y + 52}, 13, 1.0f, t->textSecondary);
    DrawTextEx(app->font, line3, (Vector2){sumCard.x + 16, sumCard.y + 72}, 13, 1.0f, t->textSecondary);
    DrawTextEx(app->font, line4, (Vector2){sumCard.x + 16, sumCard.y + 92}, 13, 1.0f,
               ieee.is_special ? t->warning : t->success);
}

/* ================================================================
 *  HEADER
 * ================================================================ */

static void DrawHeader(AppState *app)
{
    Theme *t = &app->theme;

    // Gradient header
    int sw = GetScreenWidth();
    DrawRectangle(0, 0, sw, 64, t->header);
    DrawRectangleGradientH(0, 0, sw, 64,
        t->header, ColorBrightness(t->header, 0.05f));

    DrawTextEx(app->font, "NumInter",
        (Vector2){20, 14}, 26, 1.3f, t->headerText);
    DrawTextEx(app->font, "Number System Converter & Analyzer",
        (Vector2){22, 42}, 12, 1.0f, t->headerSub);

    // Version pill
    DrawPill(sw - 110, 22, "v2.0.0", t->accent,
             (Color){255,255,255,255}, app->font);

    // Theme toggle
    Rectangle themeBtn = {sw - 178, 18, 56, 28};
    const char *themeLabel = app->darkMode ? "Light" : "Dark";
    if (DrawButton(themeBtn, themeLabel,
                   (Color){255,255,255, 30}, t->headerText,
                   (Color){255,255,255, 50}, app->font, 13))
    {
        app->darkMode = !app->darkMode;
        app->theme = app->darkMode ? THEME_DARK : THEME_LIGHT;
        ShowToast(app, app->darkMode ? "Dark mode enabled" : "Light mode enabled");
    }

    // Keyboard shortcut hint
    DrawTextEx(app->font, "Tab: Switch  |  Ctrl+D: Theme  |  Ctrl+L: Clear",
        (Vector2){sw - 480, 48}, 11, 0.8f, t->headerSub);
}

/* ================================================================
 *  TOAST NOTIFICATION
 * ================================================================ */

static void DrawToast(AppState *app)
{
    if (app->toastTimer <= 0) return;

    float alpha = app->toastTimer > 0.3f ? 1.0f : app->toastTimer / 0.3f;
    Theme *t = &app->theme;

    Vector2 sz = MeasureTextEx(app->font, app->toast, 14, 1.0f);
    float tw = sz.x + 32;
    float tx = (GetScreenWidth() - tw) / 2;
    float ty = GetScreenHeight() - 60;

    Color bg = t->tooltip;
    bg.a = (unsigned char)(bg.a * alpha);
    Color fg = t->tooltipText;
    fg.a = (unsigned char)(255 * alpha);

    Rectangle toastRect = {tx, ty, tw, 32};
    DrawRectangleRounded(toastRect, 0.4f, 8, bg);
    DrawTextEx(app->font, app->toast, (Vector2){tx + 16, ty + 8}, 14, 1.0f, fg);
}

/* ================================================================
 *  STATUS BAR
 * ================================================================ */

static void DrawStatusBar(AppState *app)
{
    Theme *t = &app->theme;
    int sw = GetScreenWidth();
    int y = GetScreenHeight() - 24;

    DrawRectangle(0, y, sw, 24, t->header);

    char status[128];
    sprintf(status, "  %d-bit mode  |  History: %d entries  |  %s theme",
            app->conv.bitWidth, app->history.count,
            app->darkMode ? "Dark" : "Light");
    DrawTextEx(app->font, status, (Vector2){8, y + 5}, 12, 0.8f, t->headerSub);

    DrawTextEx(app->font, "NumInter v2.0",
        (Vector2){sw - 110, y + 5}, 12, 0.8f, t->headerSub);
}

/* ================================================================
 *  INPUT HANDLING
 * ================================================================ */

static void HandleConverterInput(AppState *app)
{
    ConverterState *s = &app->conv;
    s->cursorBlink += GetFrameTime();
    if (s->cursorBlink > 1.0f) s->cursorBlink = 0;

    int key = GetCharPressed();
    char *target = NULL;

    if (key > 0) {
        bool ok = false;
        switch (s->activeInput) {
            case 0: target = s->decimal; ok = isdigit(key); break;
            case 1: target = s->binary;  ok = (key == '0' || key == '1'); break;
            case 2: target = s->hex;     ok = isxdigit(key); key = toupper(key); break;
            case 3: target = s->octal;   ok = (key >= '0' && key <= '7'); break;
        }

        if (ok && strlen(target) < MAX_INPUT - 1) {
            if (strcmp(target, "0") == 0) target[0] = '\0';
            int len = strlen(target);
            target[len] = key;
            target[len + 1] = '\0';
            UpdateConversions(s);
            HistoryAdd(&app->history, s);
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        switch (s->activeInput) {
            case 0: target = s->decimal; break;
            case 1: target = s->binary;  break;
            case 2: target = s->hex;     break;
            case 3: target = s->octal;   break;
        }
        if (target && strlen(target) > 0) {
            target[strlen(target) - 1] = '\0';
            if (!*target) strcpy(target, "0");
            UpdateConversions(s);
        }
    }
}

static void HandleBitwiseInput(AppState *app)
{
    BitwiseState *bw = &app->bitwise;
    int key = GetCharPressed();

    if (key > 0 && isdigit(key)) {
        char *target = (bw->activeField == 0) ? bw->operandA : bw->operandB;
        if (strlen(target) < MAX_INPUT - 1) {
            if (strcmp(target, "0") == 0) target[0] = '\0';
            int len = strlen(target);
            target[len] = key;
            target[len + 1] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        char *target = (bw->activeField == 0) ? bw->operandA : bw->operandB;
        if (strlen(target) > 0) {
            target[strlen(target) - 1] = '\0';
            if (!*target) strcpy(target, "0");
        }
    }
}

static void HandleIEEE754Input(AppState *app)
{
    int key = GetCharPressed();

    if (key > 0 && (isdigit(key) || key == '.' || key == '-' || key == 'e' || key == 'E' || key == '+')) {
        int len = strlen(app->ieee_input);
        if (len < MAX_INPUT - 1) {
            app->ieee_input[len] = key;
            app->ieee_input[len + 1] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = strlen(app->ieee_input);
        if (len > 0) {
            app->ieee_input[len - 1] = '\0';
        }
    }
}

static void HandleGlobalKeys(AppState *app)
{
    // Tab switching with keyboard
    if (IsKeyPressed(KEY_TAB)) {
        app->currentTab = (app->currentTab + 1) % TAB_COUNT;
    }

    // Ctrl+D: Toggle dark mode
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
        app->darkMode = !app->darkMode;
        app->theme = app->darkMode ? THEME_DARK : THEME_LIGHT;
        ShowToast(app, app->darkMode ? "Dark mode" : "Light mode");
    }

    // Ctrl+L: Clear input
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) {
        if (app->currentTab == TAB_CONVERTER) {
            strcpy(app->conv.decimal, "0");
            app->conv.activeInput = 0;
            ValueToAllBases(0, &app->conv);
            ShowToast(app, "Cleared");
        } else if (app->currentTab == TAB_BITWISE) {
            strcpy(app->bitwise.operandA, "0");
            strcpy(app->bitwise.operandB, "0");
            ShowToast(app, "Cleared");
        } else {
            app->ieee_input[0] = '\0';
            ShowToast(app, "Cleared");
        }
    }

    // Number keys 1-3 to switch tabs with Ctrl
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        if (IsKeyPressed(KEY_ONE))   app->currentTab = TAB_CONVERTER;
        if (IsKeyPressed(KEY_TWO))   app->currentTab = TAB_BITWISE;
        if (IsKeyPressed(KEY_THREE)) app->currentTab = TAB_IEEE754;
    }
}

/* ================================================================
 *  MAIN
 * ================================================================ */

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NumInter - Number System Converter & Analyzer");
    SetWindowMinSize(900, 700);
    SetTargetFPS(60);

    AppState app = {0};
    app.font = LoadUniversalFont();
    SetTextureFilter(app.font.texture, TEXTURE_FILTER_BILINEAR);

    // Defaults
    app.darkMode = false;
    app.theme = THEME_LIGHT;
    app.conv.bitWidth = 8;
    strcpy(app.conv.decimal, "0");
    ValueToAllBases(0, &app.conv);

    strcpy(app.bitwise.operandA, "0");
    strcpy(app.bitwise.operandB, "0");
    strcpy(app.ieee_input, "3.14");

    app.currentTab = TAB_CONVERTER;

    Color accents[] = {
        { 59, 130, 246, 255},   // Blue (decimal)
        { 34, 197,  94, 255},   // Green (binary)
        {168,  85, 247, 255},   // Purple (hex)
        {249, 115,  22, 255},   // Orange (octal)
    };

    const char *labels[] = {
        "DECIMAL (BASE 10)",
        "BINARY (BASE 2)",
        "HEXADECIMAL (BASE 16)",
        "OCTAL (BASE 8)",
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        app.animTime += dt;

        // Update timers
        if (app.toastTimer > 0) app.toastTimer -= dt;
        for (int i = 0; i < 4; i++) {
            if (app.copyFeedback[i] > 0) app.copyFeedback[i] -= dt;
        }

        // Handle input
        HandleGlobalKeys(&app);

        switch (app.currentTab) {
            case TAB_CONVERTER: HandleConverterInput(&app); break;
            case TAB_BITWISE:   HandleBitwiseInput(&app);   break;
            case TAB_IEEE754:   HandleIEEE754Input(&app);   break;
            default: break;
        }

        // =================== DRAW ===================
        BeginDrawing();
        ClearBackground(app.theme.bg);

        // Header
        DrawHeader(&app);

        // Tab bar
        DrawTabBar(&app, 0, 64, GetScreenWidth());

        int contentY = 110;

        switch (app.currentTab) {
        case TAB_CONVERTER: {
            // Input boxes (2x2 grid)
            Rectangle boxes[4] = {
                { 30, contentY,      340, 82},
                {380, contentY,      340, 82},
                { 30, contentY + 96, 340, 82},
                {380, contentY + 96, 340, 82},
            };

            char *vals[] = {
                app.conv.decimal, app.conv.binary,
                app.conv.hex, app.conv.octal,
            };

            for (int i = 0; i < 4; i++) {
                if (DrawInputBoxModern(&app, labels[i], vals[i], boxes[i],
                        app.conv.activeInput == i, accents[i], i))
                {
                    app.conv.activeInput = i;
                }
            }

            // ASCII preview
            DrawASCIIPreview(&app, 730, contentY, 340);

            // Bit visualizer (full width, below input boxes)
            int bvY = contentY + 210;
            DrawBitVisualizerModern(&app, 30, bvY);

            // History panel below bit visualizer, spanning full width
            int bitH = (app.conv.bitWidth == 16) ? 85 : 115;
            int histY = bvY + bitH;
            DrawHistoryPanel(&app, 30, histY,
                             GetScreenWidth() - 60, GetScreenHeight() - histY - 30);
            break;
        }

        case TAB_BITWISE:
            DrawBitwiseTab(&app, 30, contentY);
            break;

        case TAB_IEEE754:
            DrawIEEE754Tab(&app, 30, contentY);
            break;

        default: break;
        }

        // Status bar
        DrawStatusBar(&app);

        // Toast overlay
        DrawToast(&app);

        EndDrawing();
    }

    UnloadFont(app.font);
    CloseWindow();
    return 0;
}
