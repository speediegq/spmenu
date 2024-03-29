#ifndef KEY_H
#define KEY_H
/* See LICENSE file for copyright and license details. */

typedef struct {
    unsigned int mode;
    unsigned int mod;
    KeySym keysym;
    void (*func)(Arg *);
    Arg arg;
} Key;

// user friendly names for all the modifiers we're using, but there are many more
#define Ctrl ControlMask
#define Shift ShiftMask
#define Alt Mod1Mask
#define AltGr Mod3Mask
#define Super Mod4Mask
#define ShiftGr Mod5Mask

static void updatenumlockmask(void);
static void keypress_x11(XEvent *e);
static void grabkeyboard_x11(void);
static void getcapsstate(void);

static Key ckeys[256];
static Key hkeys[1] = { { -1, Ctrl|Alt, XK_Delete, quit, {0} } };
#endif
