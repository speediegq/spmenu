/* spmenu - fancy dynamic menu
 * See LICENSE file for copyright and license details.
 */
#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// set version
#ifndef VERSION
#define VERSION "unknown"
#endif

// check if we should enable right to left language support
#ifndef RTL
#define USERTL 0
#else
#define USERTL 1
#endif

// check if we should enable image support
#ifndef IMAGE
#define USEIMAGE 0
#else
#define USEIMAGE 1
#endif

// check if we should enable multimonitor support using libXinerama
#ifndef XINERAMA
#define USEXINERAMA 0
#else
#define USEXINERAMA 1
#endif

// check if we should enable config file support using libconfig
#ifndef CONFIG
#define USECONFIG 0
#else
#define USECONFIG 1
#endif

// check if we should enable .Xresources support
#ifndef XRESOURCES
#define USEXRESOURCES 0
#else
#define USEXRESOURCES 1
#endif

// check if we should enable Wayland support
#ifndef WAYLAND
#define USEWAYLAND 0
#else
#define USEWAYLAND 1
#endif

// check if we should enable X11 support
#ifndef X11
#define USEX 0
#else
#define USEX 1
#endif

// include fribidi used for right to left language support
#if USERTL
#include <fribidi.h>
#endif

// include libraries used for image support
#if USEIMAGE
#include <errno.h>
#include <pwd.h>
#include <Imlib2.h>
#include <openssl/md5.h>
#endif

// include macros and other defines
#include "libs/define.c"

// mode
static int curMode; // 0 is command mode
static int allowkeys; // whether or not to interpret a keypress as an insertion

// various headers
#include "libs/draw/draw.h"
#include "libs/main.h"
#include "libs/draw.h"
#include "libs/stream.h"
#include "libs/schemes.h"
#include "libs/arg.h"
#include "libs/x11/inc.h" // include x11
#include "libs/wl/inc.h" // include wayland
#include "libs/sort.h"
#include "libs/history.h"

// text
static char modetext[64] = "";
static char text[BUFSIZ] = "";
static char numbers[NUMBERSBUFSIZE] = "";

// keybinds
#if USEX
static int numlockmask = 0;
#endif
static int capslockstate = 0;

static int bh, mw, mh; // height of each item, menu width, menu height
static int reallines = 0; // temporary integer which holds lines
static int inputw = 0; // input width
static int promptw; // prompt width
static int plw = 0; // powerline width
static int lrpad; // sum of left and right padding
static int vp; // vertical padding for bar
static int sp; // side padding for bar
static int cursorstate = 1; // cursor state
static int itemnumber = 0; // item number
static size_t cursor;
static struct item *items = NULL, *backup_items, *list_items;
static struct item *matches, *matchend; // matches, final match
static struct item *prev, *curr, *next, *sel; // previous, current, next, selected
static int hplength = 0; // high priority
static char **hpitems = NULL; // high priority
static int *sel_index = NULL;
static unsigned int sel_size = 0;
static int protocol_override = 0;
static int itemn = 0; // item number

#if USEX
static char *embed; // X11 embed
static int screen; // screen
#endif

// item struct
struct item {
    char *text;
    char *clntext;
#if USEIMAGE
    char *image;
#endif
    char *ex;
    struct item *left, *right;
    int hp;
    int index;
    double distance;
};

// image globals
#if USEIMAGE
static int flip = 0;
static int needredraw = 1;
static int longestedge = 0;
static int imagew = 0;
static int imageh = 0;
static int imageg = 0;
static int ow = 0;
static int oh = 0;
#endif
static int fullscreen = 0;

// set an integer to 1 if we have rtl enabled, this saves a lot of lines and duplicate code
#if USERTL
static int isrtl = 1;
#else
static int isrtl = 0;
#endif

static int ignoreconfkeys = 0; // can be set globally if you don't want to override keybinds with config file keys
static int ignoreglobalkeys = 0; // should be set in the config file, if 1, the Keys keys array is ignored
static int ignoreconfmouse = 0; // same for mouse
static int ignoreglobalmouse = 0; // same for mouse

// colors
#if USEX
static int useargb;
static int depth;
static Visual *visual;
static Colormap cmap;
#endif
static Drw *drw;

// declare functions
static int is_selected(size_t index);
static void calcoffsets(void);
static void recalculatenumbers(void);
static void insert(const char *str, ssize_t n);
static void cleanup(void);
static void navigatehistfile(int dir);
#if USEX
static void pastesel(void);
static void grabfocus(void);
#endif
static void resizeclient(void);
static void get_width(void);
static void get_mh(void);
static void set_mode(void);
static void handle(void);
static void appenditem(struct item *item, struct item **list, struct item **last);
static int max_textw(void);
static size_t nextrune(int inc);
static char * cistrstr(const char *s, const char *sub);
static int (*fstrncmp)(const char *, const char *, size_t) = strncasecmp;
static char *(*fstrstr)(const char *, const char *) = cistrstr;

static char **list;
static size_t listsize;
static int listcount;
static int listchanged = 0;

// clicks
enum {
    ClickWindow,
    ClickPrompt,
    ClickInput,
    ClickLArrow,
    ClickItem,
    ClickSelItem,
    ClickRArrow,
    ClickNumber,
    ClickCaps,
    ClickMode,
};

// user configuration
#include "libs/options.h"
#include "libs/keybinds.h"
#include "libs/mouse.h"

static char capstext[16];
static char *fonts[] = { font };

// color array
#include "libs/colors.h"

// config file
#if USECONFIG
#include "libs/conf/config.h"
#include "libs/conf/config.c"
#endif

// include functions
#include "libs/img.h"
#include "libs/img.c"
#include "libs/rtl.h"
#include "libs/rtl.c"
#include "libs/sort.c"
#include "libs/match.h"
#include "libs/match.c"
#include "libs/draw.c"
#include "libs/schemes.c"
#include "libs/argv.h"
#include "libs/argv.c"

// include x11 code
#include "libs/x11/inc.c"
#include "libs/wl/inc.c"

// include more functions
#include "libs/history.c"
#include "libs/arg.c"
#include "libs/stream.c"

int is_selected(size_t index) {
    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] == index) {
            return 1;
        }
    }

    return 0;
}

void appenditem(struct item *item, struct item **list, struct item **last) {
    if (*last)
        (*last)->right = item;
    else
        *list = item;

    item->left = *last;
    item->right = NULL;
    *last = item;
}

void recalculatenumbers(void) {
    unsigned int numer = 0, denom = 0, selected = 0;
    struct item *item;
    if (matchend) {
        numer++;

        // walk through items that match and add to numer
        for (item = matchend; item && item->left; item = item->left)
            numer++;
    }

    // walk through all items, matching or not and add to denom
    for (item = items; item && item->text; item++)
        denom++;

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] == -1) {
            break;
        }

        selected++;
    }

    if (selected) {
        snprintf(numbers, NUMBERSBUFSIZE, "%d/%d/%d", numer, denom, selected);
    } else {
        snprintf(numbers, NUMBERSBUFSIZE, "%d/%d", numer, denom);
    }
}

void calcoffsets(void) {
    int i, n;

    if (lines > 0)
        n = lines * columns * bh;
    else { // no lines, therefore the size of items must be decreased to fit the menu elements
        int numberWidth = 0;
        int modeWidth = 0;
        int larrowWidth = 0;
        int rarrowWidth = 0;
        int capsWidth = 0;

        if (!hidematchcount) numberWidth = pango_numbers ? TEXTWM(numbers) : TEXTW(numbers);
        if (!hidemode) modeWidth = pango_mode ? TEXTWM(modetext) : TEXTW(modetext);
        if (!hidelarrow) larrowWidth = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
        if (!hiderarrow) rarrowWidth = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
        if (!hidecaps) capsWidth = pango_caps ? TEXTWM(capstext) : TEXTW(capstext);

        if (!strcmp(capstext, ""))
            capsWidth = 0;

        n = mw - (promptw + inputw + larrowWidth + rarrowWidth + modeWidth + numberWidth + capsWidth + menumarginh);
    }

    // calculate which items will begin the next page
    for (i = 0, next = curr; next; next = next->right)
        if ((i += (lines > 0) ? bh : MIN(TEXTWM(next->text) + (powerlineitems ? !lines ? 2 * plw : 0 : 0), n)) > n)
            break;

    // calculate which items will begin the previous page
    for (i = 0, prev = curr; prev && prev->left; prev = prev->left)
        if ((i += (lines > 0) ? bh : MIN(TEXTWM(prev->left->text) + (powerlineitems ? !lines ? 2 * plw : 0 : 0), n)) > n)
            break;
}

int max_textw(void) {
    int len = 0;

    for (struct item *item = items; item && item->text; item++)
        len = MAX(TEXTW(item->text), len);

    return len;
}

void cleanup(void) {
    size_t i;

#if USEIMAGE
    cleanupimage(); // function frees images
#endif

    // free high priority items
    for (i = 0; i < hplength; ++i)
        free(hpitems[i]);

    // free drawing and close the display
    drw_free(drw);

#if USEX
    if (!protocol) {
        cleanup_x11(dpy);
    }
#endif

    free(sel_index);
}

char * cistrstr(const char *h, const char *n) {
    size_t i;

    if (!n[0])
        return (char *)h;

    for (; *h; ++h) {
        for (i = 0; n[i] && tolower((unsigned char)n[i]) ==
                tolower((unsigned char)h[i]); ++i);

        if (n[i] == '\0')
            return (char *)h;
    }

    return NULL;
}

#if USEX
void grabfocus(void) {
    grabfocus_x11();
}
#endif

void insert(const char *str, ssize_t n) {
    if (strlen(text) + n > sizeof text - 1)
        return;

    static char l[BUFSIZ] = "";
    if (requirematch) memcpy(l, text, BUFSIZ);

    // move existing text out of the way, insert new text, and update cursor
    memmove(&text[cursor + n], &text[cursor], sizeof text - cursor - MAX(n, 0));

    // update cursor
    if (n > 0 && str && n)
        memcpy(&text[cursor], str, n);

    // add to cursor position and continue matching
    cursor += n;
    match();

    if (!matches && requirematch) {
        memcpy(text, l, BUFSIZ);
        cursor -= n;
        match();
    }
}

size_t nextrune(int inc) {
    ssize_t n;

    // return location of next utf8 rune in the given direction (+1 or -1)
    for (n = cursor + inc; n + inc >= 0 && (text[n] & 0xc0) == 0x80; n += inc)
        ;
    return n;
}

#if USEX
void pastesel(void) {
    if (!protocol) {
        pastesel_x11();
    }
}
#endif

void resizeclient(void) {
#if USEWAYLAND
    if (protocol) {
        resizeclient_wl(&state);
    } else {
#if USEX
        resizeclient_x11();
#endif
    }
#elif USEX
    resizeclient_x11();
#endif
}

void get_width(void) {
    inputw = mw / 3;
}

void get_mh(void) {
    mh = (lines + 1) * bh;
    mh += 2 * menumarginv;

    if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) && lines) {
        mh -= bh;
    }
}

void set_mode(void) {
    if (!type) { // no typing allowed, require normal mode
        mode = 0;
    }

    // set default mode, must be done before the event loop or keybindings will not work
    if (mode) {
        curMode = 1;
        allowkeys = 1;

        sp_strncpy(modetext, instext, sizeof(modetext));
    } else {
        curMode = 0;
        allowkeys = !curMode;

        sp_strncpy(modetext, normtext, sizeof(modetext));
    }
}

void handle(void) {
    if (!protocol) {
#if USEX
        handle_x11();

        if (!drw_font_create(drw, fonts, LENGTH(fonts))) {
            die("no fonts could be loaded.");
        }

        loadhistory(); // read history entries
#if USEIMAGE
        store_image_vars();
#endif

        // fast (-f) means we grab keyboard before reading standard input
        if (fast && !isatty(0)) {
            grabkeyboard_x11();
            readstdin();
        } else {
            readstdin();
            grabkeyboard_x11();
        }

        set_mode();

        init_appearance(); // init colorschemes by reading arrays
        setupdisplay_x11(); // set up display and create window
        eventloop_x11(); // function is a loop which checks X11 events and calls other functions accordingly
#endif
#if USEWAYLAND
    } else {
        loadhistory();
#if USEIMAGE
        store_image_vars();
        setimageopts();
#endif

        // Disable some X11 only features
        menupaddingv = menupaddingh = 0;
        xpos = ypos = 0;
        borderwidth = 0;
        managed = 0;

        drw = drw_create_wl(protocol);

        if (!drw_font_create(drw, fonts, LENGTH(fonts))) {
            die("no fonts could be loaded.");
        }

        readstdin();
        set_mode();
        init_appearance();

        handle_wl();
#endif
    }
}

int main(int argc, char *argv[]) {
    readargs(argc, argv); // start by reading arguments

    // pledge limits what programs can do, so here we specify what spmenu should be allowed to do
#ifdef __OpenBSD__
    if (pledge("stdio rpath wpath cpath", NULL) == -1)
        die("pledge");
#endif

    handle();

    return 1; // should be unreachable
}
