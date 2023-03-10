/* spmenu - speedie's dmenu fork
 *
 * If you're looking for functions used to draw text, see 'libs/draw.c'
 * If you're looking for wrapper functions used inside the draw functions, see 'libs/sl/draw.c'
 * If you're looking for functions used to draw images, see 'libs/img.c'
 * If you're looking for the .Xresources array, see 'libs/xresources.h'
 *
 * You don't need to edit spmenu.c if you aren't making big changes to the software.
 *
 * After making changes, run 'make clean install' to install and 'make' to attempt a compilation.
 * `make man` will generate a man page from 'docs/docs.md', which is a Markdown document. Run this before commiting.
 *
 * See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

/* check if we should enable right to left language support */
#ifndef RTL
#define USERTL 0
#else
#define USERTL 1
#endif

/* check if we should enable pango support */
#ifndef PANGO
#define USEPANGO 0
#else
#define USEPANGO 1
#endif

/* check if we should enable image support */
#ifndef IMAGE
#define USEIMAGE 0
#else
#define USEIMAGE 1
#endif

/* check if we should enable multimonitor support using xinerama */
#ifdef XINERAMA
#define USEXINERAMA 1
#else
#define USEXINERAMA 0
#endif

/* include right to left language library */
#if USERTL
#include <fribidi.h>
#endif

/* include libraries used for image support */
#if USEIMAGE
#include <errno.h>
#include <pwd.h>
#include <Imlib2.h>
#include <openssl/md5.h>
#endif

/* include xinerama */
#if USEXINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xft/Xft.h>

#if USEPANGO
#include <pango/pango.h>
#endif

#include "libs/sl/draw.h"
#include "libs/sl/main.h"

/* macros */
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define INTERSECT(x,y,w,h,r)  (MAX(0, MIN((x)+(w),(r).x_org+(r).width)  - MAX((x),(r).x_org)) \
                             && MAX(0, MIN((y)+(h),(r).y_org+(r).height) - MAX((y),(r).y_org)))
#define LENGTH(X)             (sizeof X / sizeof X[0])
#define TEXTW(X)              (drw_font_getwidth(drw, (X), False) + lrpad)
#define TEXTWM(X)             (drw_font_getwidth(drw, (X), True) + lrpad)

/* number */
#define NUMBERSMAXDIGITS      100
#define NUMBERSBUFSIZE        (NUMBERSMAXDIGITS * 2) + 1

/* user friendly names for all the modifiers */
#define CONTROL ControlMask
#define SHIFT ShiftMask
#define ALT Mod1Mask
#define ALTR Mod3Mask
#define SUPER Mod4Mask
#define SUPERR Mod5Mask

/* alpha */
#define opaque 0xffU

/* include headers */
#include "libs/schemes.h"
#include "libs/arg.h"
#include "libs/mode.h"
#include "libs/xrdb.h"
#include "libs/key.h"
#include "libs/mouse.h"
#include "libs/sort.h"

static char text[BUFSIZ] = "";

struct item {
	char *text;
    char *image;
    char *ex;
	struct item *left, *right;
    int hp;
	double distance;
};

typedef struct {
    unsigned int mode;
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

static char numbers[NUMBERSBUFSIZE] = "";
static int hplength = 0;
static char **hpitems = NULL;
static char *embed;
static int numlockmask = 0;
static int bh, mw, mh;
static int reallines = 0;
static int reqlineheight; /* required menu height */
static int clineheight; /* menu height added through argument */
static int dmx = 0; /* put spmenu at this x offset */
static int dmy = 0; /* put spmenu at this y offset (measured from the bottom if menuposition is 0) */
static unsigned int dmw = 0; /* make spmenu this wide */
static int inputw = 0, promptw, passwd = 0;
static int lrpad; /* sum of left and right padding */
static int vp;    /* vertical padding for bar */
static int sp;    /* side padding for bar */
static size_t cursor;
static struct item *items = NULL, *backup_items;
static struct item *matches, *matchend;
static struct item *prev, *curr, *next, *sel;
static int screen;
static int managed = 0;

/* image globals */
#if USEIMAGE
static int flip = 0;
static int rotation = 0;
static int needredraw = 1;
#endif
static int longestedge = 0;
static int imagew = 0;
static int imageh = 0;
static int imageg = 0;

/* set an integer if to 1 if we have right to left language support enabled
 * doing it this way, because we can reduce the amount of #if and #else lines used.
 */
#if USERTL
static int isrtl = 1;
#else
static int isrtl = 0;
static char fribidi_text[] = "";
#endif

static Atom clip, utf8, types, dock;
static Display *dpy;
static Window root, parentwin, win;
static XIC xic;

static int useargb = 0;
static Visual *visual;
static int depth;
static Colormap cmap;

static Drw *drw;
static Clr *scheme[SchemeLast];
static Clr textclrs[256];

static char *histfile;
static char **history;
static size_t histsz, histpos;
static size_t nextrune(int inc);

static void drawmenu(void);
static void calcoffsets(void);
static void readstdin(void);
static void recalculatenumbers(void);
static void usage(void);
static void match(void);
static void movewordedge(int dir);
static void insert(const char *str, ssize_t n);
static void cleanup(void);
static void navigatehistfile(int dir);
static void grabfocus(void);
static void pastesel(void);
static int max_textw(void);

/* user configuration */
#include "options.h" /* Include main header */
#include "keybinds.h" /* Include keybinds */

#include "libs/xresources.h"
#include "libs/colors.h"

static char * cistrstr(const char *s, const char *sub);
static int (*fstrncmp)(const char *, const char *, size_t) = strncasecmp;
static char *(*fstrstr)(const char *, const char *) = cistrstr;

#if USEIMAGE
#include "libs/img.h"
#include "libs/img.c"
#endif
#if USERTL
#include "libs/rtl.h"
#include "libs/rtl.c"
#else
#include "libs/rtl-none.h"
#include "libs/rtl-none.c"
#endif
#include "libs/event.h"
#include "libs/event.c"
#include "libs/key.c"
#include "libs/mouse.c"
#include "libs/sort.c"
#include "libs/draw.c"
#include "libs/schemes.c"
#include "libs/arg.c"
#include "libs/argv.h"
#include "libs/argv.c"
#include "libs/xrdb.c"
#include "libs/mode.c"
#include "libs/client.h"
#include "libs/client.c"

void
appenditem(struct item *item, struct item **list, struct item **last)
{
	if (*last)
		(*last)->right = item;
	else
		*list = item;

	item->left = *last;
	item->right = NULL;
	*last = item;
}

void
recalculatenumbers(void)
{
	unsigned int numer = 0, denom = 0;
	struct item *item;
	if (matchend) {
		numer++;
		for (item = matchend; item && item->left; item = item->left)
			numer++;
	}
	for (item = items; item && item->text; item++)
		denom++;
	snprintf(numbers, NUMBERSBUFSIZE, "%d/%d", numer, denom);
}

void
calcoffsets(void)
{
	int i, n;

	if (lines > 0)
		n = lines * columns * bh;
	else {
        int numberWidth = 0;
        int modeWidth = 0;
        int larrowWidth = 0;
        int rarrowWidth = 0;

        if (!hidematchcount) numberWidth = pango_numbers ? TEXTWM(numbers) : TEXTW(numbers);
        if (!hidemode) modeWidth = pango_mode ? TEXTWM(modetext) : TEXTW(modetext);
        if (!hidelarrow) larrowWidth = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
        if (!hiderarrow) rarrowWidth = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);

		n = mw - (promptw + inputw + larrowWidth + rarrowWidth + modeWidth + numberWidth);
    }
	/* calculate which items will begin the next page and previous page */
	for (i = 0, next = curr; next; next = next->right)
		if ((i += (lines > 0) ? bh : MIN(TEXTWM(next->text), n)) > n)
			break;
	for (i = 0, prev = curr; prev && prev->left; prev = prev->left)
		if ((i += (lines > 0) ? bh : MIN(TEXTWM(prev->left->text), n)) > n)
			break;
}

int
max_textw(void)
{
	int len = 0;
	for (struct item *item = items; item && item->text; item++)
		len = MAX(TEXTW(item->text), len);
	return len;
}

void
cleanup(void)
{
	size_t i;

    #if USEIMAGE
    cleanupimage();
    #endif

	XUngrabKey(dpy, AnyKey, AnyModifier, root);

	for (i = 0; i < SchemeLast; i++)
		free(scheme[i]);

    for (i = 0; i < hplength; ++i)
		free(hpitems[i]);

	drw_free(drw);
	XSync(dpy, False);
	XCloseDisplay(dpy);
}

char *
cistrstr(const char *h, const char *n)
{
	size_t i;

	if (!n[0])
		return (char *)h;

	for (; *h; ++h) {
		for (i = 0; n[i] && tolower((unsigned char)n[i]) ==
		            tolower((unsigned char)h[i]); ++i)
			;
		if (n[i] == '\0')
			return (char *)h;
	}
	return NULL;
}

void
grabfocus(void)
{
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000  };
	Window focuswin;
	int i, revertwin;

	for (i = 0; i < 100; ++i) {
		XGetInputFocus(dpy, &focuswin, &revertwin);
		if (focuswin == win)
			return;
		if (managed) {
		XTextProperty prop;
		char *windowtitle = prompt != NULL ? prompt : "spmenu";
		Xutf8TextListToTextProperty(dpy, &windowtitle, 1, XUTF8StringStyle, &prop);
		XSetWMName(dpy, win, &prop);
		XSetTextProperty(dpy, win, &prop, XInternAtom(dpy, "_NET_WM_NAME", False));
		XFree(prop.value);
	} else {
		XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
	}

		nanosleep(&ts, NULL);
	}
	die("cannot grab focus");
}

void
grabkeyboard(void)
{
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000  };
	int i;

	if (embed || managed)
		return;
	/* try to grab keyboard, we may have to wait for another process to ungrab */
	for (i = 0; i < 1000; i++) {
		if (XGrabKeyboard(dpy, DefaultRootWindow(dpy), True, GrabModeAsync,
		                  GrabModeAsync, CurrentTime) == GrabSuccess)
			return;
		nanosleep(&ts, NULL);
	}
	die("cannot grab keyboard");
}

int
compare_distance(const void *a, const void *b)
{
	struct item *da = *(struct item **) a;
	struct item *db = *(struct item **) b;

	if (!db)
		return 1;
	if (!da)
		return -1;

	return da->distance == db->distance ? 0 : da->distance < db->distance ? -1 : 1;
}

void
fuzzymatch(void)
{
	/* bang - we have so much memory */
	struct item *it;
	struct item **fuzzymatches = NULL;
    struct item *lhpprefix, *hpprefixend;
	lhpprefix = hpprefixend = NULL;
	char c;
	int number_of_matches = 0, i, pidx, sidx, eidx;
	int text_len = strlen(text), itext_len;

	matches = matchend = NULL;

	/* walk through all items */
	for (it = items; it && it->text; it++) {
		if (text_len) {
			itext_len = strlen(it->text);
			pidx = 0; /* pointer */
			sidx = eidx = -1; /* start of match, end of match */
			/* walk through item text */
			for (i = 0; i < itext_len && (c = it->text[i]); i++) {
				/* fuzzy match pattern */
				if (!fstrncmp(&text[pidx], &c, 1)) {
					if(sidx == -1)
						sidx = i;
					pidx++;
					if (pidx == text_len) {
						eidx = i;
						break;
					}
				}
			}
			/* build list of matches */
			if (eidx != -1) {
				/* compute distance */
				/* add penalty if match starts late (log(sidx+2))
				 * add penalty for long a match without many matching characters */
				it->distance = log(sidx + 2) + (double)(eidx - sidx - text_len);
				/* fprintf(stderr, "distance %s %f\n", it->text, it->distance); */
				appenditem(it, &matches, &matchend);
				number_of_matches++;
			}
		} else {
			appenditem(it, &matches, &matchend);
		}
	}

	if (number_of_matches) {
		/* initialize array with matches */
		if (!(fuzzymatches = realloc(fuzzymatches, number_of_matches * sizeof(struct item*))))
			die("cannot realloc %u bytes:", number_of_matches * sizeof(struct item*));
		for (i = 0, it = matches; it && i < number_of_matches; i++, it = it->right) {
			fuzzymatches[i] = it;
		}
		/* sort matches according to distance */
        if (sortmatches) qsort(fuzzymatches, number_of_matches, sizeof(struct item*), compare_distance);

		/* rebuild list of matches */
		matches = matchend = NULL;
		for (i = 0, it = fuzzymatches[i];  i < number_of_matches && it && \
				it->text; i++, it = fuzzymatches[i]) {

            if (sortmatches && it->hp)
                appenditem(it, &lhpprefix, &hpprefixend);

            appenditem(it, &matches, &matchend);
		}
		free(fuzzymatches);
	}

    if (lhpprefix) {
		hpprefixend->right = matches;
		matches = lhpprefix;
	}

	curr = sel = matches;

    for (i = 0; i < preselected; i++) {
		if (sel && sel->right && (sel = sel->right) == next) {
			curr = next;
			calcoffsets();
		}
	}

	calcoffsets();
}

void
match(void)
{
	if (fuzzy) {
		fuzzymatch();
		return;
	}
	static char **tokv = NULL;
	static int tokn = 0;

	char buf[sizeof text], *s;
	int i, tokc = 0;
	size_t len, textsize;
    struct item *item, *lhpprefix, *lprefix, *lsubstr, *hpprefixend, *prefixend, *substrend;


	strcpy(buf, text);
	/* separate input text into tokens to be matched individually */
	for (s = strtok(buf, " "); s; tokv[tokc - 1] = s, s = strtok(NULL, " "))
		if (++tokc > tokn && !(tokv = realloc(tokv, ++tokn * sizeof *tokv)))
			die("cannot realloc %u bytes:", tokn * sizeof *tokv);
	len = tokc ? strlen(tokv[0]) : 0;

    matches = lhpprefix = lprefix = lsubstr = matchend = hpprefixend = prefixend = substrend = NULL;
	textsize = strlen(text) + 1;
	for (item = items; item && item->text; item++) {
		for (i = 0; i < tokc; i++)
			if (!fstrstr(item->text, tokv[i]))
				break;
		if (i != tokc) /* not all tokens match */
			continue;
        if (!sortmatches)
			appenditem(item, &matches, &matchend);
        else {
            /* exact matches go first, then prefixes with high priority, then prefixes, then substrings */
            if (item->hp && !fstrncmp(tokv[0], item->text, len))
			    appenditem(item, &lhpprefix, &hpprefixend);
            else if (!tokc || !fstrncmp(text, item->text, textsize))
				appenditem(item, &matches, &matchend);
			else if (!fstrncmp(tokv[0], item->text, len))
				appenditem(item, &lprefix, &prefixend);
			else
				appenditem(item, &lsubstr, &substrend);
		}
	}
    if (lhpprefix) {
		if (matches) {
			matchend->right = lhpprefix;
			lhpprefix->left = matchend;
		} else
			matches = lhpprefix;
		matchend = hpprefixend;
	}
	if (lprefix) {
		if (matches) {
			matchend->right = lprefix;
			lprefix->left = matchend;
		} else
			matches = lprefix;
		matchend = prefixend;
	}
	if (lsubstr) {
		if (matches) {
			matchend->right = lsubstr;
			lsubstr->left = matchend;
		} else
			matches = lsubstr;
		matchend = substrend;
	}
	curr = sel = matches;
	calcoffsets();
}

void
insert(const char *str, ssize_t n)
{
	if (strlen(text) + n > sizeof text - 1)
		return;
	/* move existing text out of the way, insert new text, and update cursor */
	memmove(&text[cursor + n], &text[cursor], sizeof text - cursor - MAX(n, 0));
	if (n > 0)
		memcpy(&text[cursor], str, n);
	cursor += n;
	match();
}

size_t
nextrune(int inc)
{
	ssize_t n;

	/* return location of next utf8 rune in the given direction (+1 or -1) */
	for (n = cursor + inc; n + inc >= 0 && (text[n] & 0xc0) == 0x80; n += inc)
		;
	return n;
}

void
movewordedge(int dir)
{
	if (dir < 0) { /* move cursor to the start of the word*/
		while (cursor > 0 && strchr(worddelimiters, text[nextrune(-1)]))
			cursor = nextrune(-1);
		while (cursor > 0 && !strchr(worddelimiters, text[nextrune(-1)]))
			cursor = nextrune(-1);
	} else { /* move cursor to the end of the word */
		while (text[cursor] && strchr(worddelimiters, text[cursor]))
			cursor = nextrune(+1);
		while (text[cursor] && !strchr(worddelimiters, text[cursor]))
			cursor = nextrune(+1);
	}
}

void
loadhistory(void)
{
	FILE *fp = NULL;
	static size_t cap = 0;
	size_t llen;
	char *line;

	if (!histfile) {
		return;
	}

	fp = fopen(histfile, "r");
	if (!fp) {
		return;
	}

	for (;;) {
		line = NULL;
		llen = 0;
		if (-1 == getline(&line, &llen, fp)) {
			if (ferror(fp)) {
				die("failed to read history");
			}
			free(line);
			break;
		}

		if (cap == histsz) {
			cap += 64 * sizeof(char*);
			history = realloc(history, cap);
			if (!history) {
				die("failed to realloc memory");
			}
		}
		strtok(line, "\n");
		history[histsz] = line;
		histsz++;
	}
	histpos = histsz;

	if (fclose(fp)) {
		die("failed to close file %s", histfile);
	}
}

void
navigatehistfile(int dir)
{
	static char def[BUFSIZ];
	char *p = NULL;
	size_t len = 0;

	if (!history || histpos + 1 == 0)
		return;

	if (histsz == histpos) {
		strncpy(def, text, sizeof(def));
	}

	switch (dir) {
        case 1:
            if (histpos < histsz - 1) {
                p = history[++histpos];
            } else if (histpos == histsz - 1) {
                p = def;
                histpos++;
            }
            break;
        case -1:
            if (histpos > 0) {
                p = history[--histpos];
            }
            break;
	}

	if (p == NULL) {
		return;
	}

	len = MIN(strlen(p), BUFSIZ - 1);
	strcpy(text, p);
	text[len] = '\0';
	cursor = len;
	match();
}

void
pastesel(void)
{
	char *p, *q;
	int di;
	unsigned long dl;
	Atom da;

	/* we have been given the current selection, now insert it into input */
	if (XGetWindowProperty(dpy, win, utf8, 0, (sizeof text / 4) + 1, False,
	                   utf8, &da, &di, &dl, &dl, (unsigned char **)&p)
	    == Success && p) {
		insert(p, (q = strchr(p, '\n')) ? q - p : (ssize_t)strlen(p));
		XFree(p);
	}
	drawmenu();
}

void
xinitvisual()
{
	XVisualInfo *infos;
	XRenderPictFormat *fmt;
	int nitems;
	int i;

	XVisualInfo tpl = {
		.screen = screen,
		.depth = 32,
		.class = TrueColor
	};
	long masks = VisualScreenMask | VisualDepthMask | VisualClassMask;

	infos = XGetVisualInfo(dpy, masks, &tpl, &nitems);
	visual = NULL;
	for(i = 0; i < nitems; i ++) {
		fmt = XRenderFindVisualFormat(dpy, infos[i].visual);
		if (fmt->type == PictTypeDirect && fmt->direct.alphaMask) {
			visual = infos[i].visual;
			depth = infos[i].depth;
			cmap = XCreateColormap(dpy, root, visual, AllocNone);
			useargb = 1;
			break;
		}
	}

	XFree(infos);

	if (!visual || !alpha) {
		visual = DefaultVisual(dpy, screen);
		depth = DefaultDepth(dpy, screen);
		cmap = DefaultColormap(dpy, screen);
	}
}

void
readstdin(void)
{
	char buf[sizeof text], *p;
	size_t i, imax = 0, itemsiz = 0;
	unsigned int tmpmax = 0;
    #if USEIMAGE
    int w, h;
    char *limg = NULL;
    #endif

	if (passwd){
    	inputw = lines = 0;
    	return;
  	}

	/* read each line from stdin and add it to the item list */
	for (i = 0; fgets(buf, sizeof buf, stdin); i++) {
      	if (i + 1 >= itemsiz) {
			itemsiz += 256;
			if (!(items = realloc(items, itemsiz * sizeof(*items))))
				die("cannot realloc %zu bytes:", itemsiz * sizeof(*items));
		}
		if ((p = strchr(buf, '\n')))
			*p = '\0';
		if (!(items[i].text = strdup(buf)))
			die("cannot strdup %u bytes:", strlen(buf) + 1);
        items[i].hp = arrayhas(hpitems, hplength, items[i].text);
		drw_font_getexts(drw->font, buf, strlen(buf), &tmpmax, NULL, True);
		if (tmpmax > inputw) {
			inputw = tmpmax;
			imax = i;
		}

        /* parse image markup */
        #if USEIMAGE
        if(!strncmp("IMG:", items[i].text, strlen("IMG:"))) {
            if(!(items[i].image = malloc(strlen(items[i].text)+1)))
                fprintf(stderr, "spmenu: cannot malloc %lu bytes\n", strlen(items[i].text));
            if(sscanf(items[i].text, "IMG:%[^\t]", items[i].image)) {
                items[i].text += strlen("IMG:")+strlen(items[i].image)+1;
            } else {
                free(items[i].image);
                items[i].image = NULL;
            }
        } else {
            items[i].image = NULL;
        }

        if (generatecache && longestedge <= 256 && items[i].image && strcmp(items[i].image, limg?limg:"")) {
            loadimagecache(items[i].image, &w, &h);
            fprintf(stdout, "spmenu: generating thumbnail for: %s\n", items[i].image);
        }

        if(items[i].image)
            limg = items[i].image;
        #endif

        /* todo: use this for something
         * current usage is not very useful, however it's here to be used later.
         */
        if(!(items[i].ex = malloc(strlen(items[i].text)+1)))
                fprintf(stderr, "spmenu: cannot malloc %lu bytes\n", strlen(items[i].text));
        if (!strncmp("spmenu:", items[i].text, strlen("spmenu:"))) {
            if (sscanf(items[i].text, "spmenu:%[^\t]", items[i].ex)) {
                items[i].text += strlen("spmenu:")+strlen(items[i].ex)+1;
            }

            if (!strncmp("version", items[i].ex, strlen("version"))) {
                die("spmenu version %s", VERSION);
            }

            if (!strncmp("license", items[i].ex, strlen("license"))) {
                die("spmenu is licensed under the MIT license. See the included LICENSE file for more information.");
            }
        }
	}

	if (items) {
        #if USEIMAGE
        items[i].image = NULL;
        #endif
		items[i].text = NULL;
    }

    #if USEIMAGE
    if (!limg) longestedge = imagegaps = 0;
    #endif
	inputw = items ? TEXTWM(items[imax].text) : 0;
	lines = MIN(lines, i);
}

void
setup(void)
{
	int x, y, i;
    #if USEXINERAMA
    int j, di;
    #endif
	unsigned int du;
    unsigned int tmp, minstrlen = 0, curstrlen = 0;
    int numwidthchecks = 100;
    struct item *item;
	XIM xim;
	Window w, dw, *dws;
    XWindowAttributes wa;
    #if USEXINERAMA
	XineramaScreenInfo *info;
	Window pw;
	int a, n, area = 0;
    #endif

    /* init appearance */
    init_appearance();

	clip = XInternAtom(dpy, "CLIPBOARD",   False);
	utf8 = XInternAtom(dpy, "UTF8_STRING", False);
   	types = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
	dock = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);

    if (!clineheight) {
        reqlineheight += lineheight;
    } else {
        reqlineheight = clineheight;
    }

	/* calculate menu geometry */
	bh = drw->font->h + 2 + reqlineheight;
	lines = MAX(lines, 0);
    reallines = lines;

    #if USEIMAGE
    if (image)
        resizetoimageheight(imageheight);
    #endif

	mh = (lines + 1) * bh;
	promptw = (prompt && *prompt) ? TEXTWM(prompt) - lrpad / 4 : 0;

    if (accuratewidth) {
        for (item = items; !lines && item && item->text; ++item) {
            curstrlen = strlen(item->text);
            if (numwidthchecks || minstrlen < curstrlen) {
                numwidthchecks = MAX(numwidthchecks - 1, 0);
                minstrlen = MAX(minstrlen, curstrlen);
                if ((tmp = MIN(TEXTW(item->text), mw/3) > inputw)) {
                    inputw = tmp;
                    if (tmp == mw/3)
                        break;
                }
            }
        }
    }

    #if USEXINERAMA
	i = 0;
	if (parentwin == root && (info = XineramaQueryScreens(dpy, &n))) {
		XGetInputFocus(dpy, &w, &di);
		if (mon >= 0 && mon < n)
			i = mon;
		else if (w != root && w != PointerRoot && w != None) {
			/* find top-level window containing current input focus */
			do {
				if (XQueryTree(dpy, (pw = w), &dw, &w, &dws, &du) && dws)
					XFree(dws);
			} while (w != root && w != pw);
			/* find xinerama screen with which the window intersects most */
			if (XGetWindowAttributes(dpy, pw, &wa))
				for (j = 0; j < n; j++)
					if ((a = INTERSECT(wa.x, wa.y, wa.width, wa.height, info[j])) > area) {
						area = a;
						i = j;
					}
		}
		/* no focused window is on screen, so use pointer location instead */
		if (mon < 0 && !area && XQueryPointer(dpy, root, &dw, &dw, &x, &y, &di, &di, &du))
			for (i = 0; i < n; i++)
				if (INTERSECT(x, y, 1, 1, info[i]))
					break;

		if (centered) {
			mw = MIN(MAX(max_textw() + promptw, minwidth), info[i].width);
			x = info[i].x_org + ((info[i].width  - mw) / 2);
			//y = info[i].y_org + 0;
			y = info[i].y_org + ((info[i].height - mh) / 2);
		} else {
		    x = info[i].x_org + dmx;
			y = info[i].y_org + (menuposition ? 0 : info[i].height - mh - dmy);
			//y = info[i].y_org + 0;
			mw = (dmw>0 ? dmw : info[i].width);
		}

		XFree(info);
	} else
    #endif
	{
		if (!XGetWindowAttributes(dpy, parentwin, &wa))
			die("could not get embedding window attributes: 0x%lx",
			    parentwin);

		if (centered) {
			mw = MIN(MAX(max_textw() + promptw, minwidth), wa.width);
			x = (wa.width  - mw) / 2;
			y = (wa.height - mh) / 2;
		} else {
			x = 0;
			y = 0;
			mw = wa.width;
		}
	}

	/* might be faster in some instances, most of the time unnecessary */
    if (!accuratewidth) inputw = MIN(inputw, mw/3);

	match();

	/* create menu window */
    create_window(x + sp, y + vp, mw - 2 * sp, mh);
    set_window();
    set_prop();

    #if USEIMAGE
    setimageopts();
    #endif

	/* input methods */
	if ((xim = XOpenIM(dpy, NULL, NULL, NULL)) == NULL)
		die("XOpenIM failed: could not open input device");

	xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
	                XNClientWindow, win, XNFocusWindow, win, NULL);

	XMapRaised(dpy, win);
	if (embed) {
		XSelectInput(dpy, parentwin, FocusChangeMask | SubstructureNotifyMask);
		if (XQueryTree(dpy, parentwin, &dw, &w, &dws, &du) && dws) {
			for (i = 0; i < du && dws[i] != win; ++i)
				XSelectInput(dpy, dws[i], FocusChangeMask);
			XFree(dws);
		}
		grabfocus();
	}
	drw_resize(drw, mw, mh);
	drawmenu();
}

int
main(int argc, char *argv[])
{
	XWindowAttributes wa;

    readargs(argc, argv);

    longestedge = MAX(imagewidth, imageheight);

    if (mode) {
        curMode = 1;
        allowkeys = 1;

        strcpy(modetext, instext);
    } else {
        curMode = 0;
        allowkeys = !curMode;

        strcpy(modetext, normtext);
    }

	if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
		fputs("warning: no locale support\n", stderr);

        if (!(dpy = XOpenDisplay(NULL)))
		die("spmenu: cannot open display");

    screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

    if (!embed || !(parentwin = strtol(embed, NULL, 0)))
		parentwin = root;

    if (!XGetWindowAttributes(dpy, parentwin, &wa))
		die("spmenu: could not get embedding window attributes: 0x%lx", parentwin);

	xinitvisual();
	drw = drw_create(dpy, screen, root, wa.width, wa.height, visual, depth, cmap);

	if (!drw_font_create(drw, font, LENGTH(font)))
	    die("no fonts could be loaded.");
	lrpad = drw->font->h;

    prepare_window_size();

    #ifdef __OpenBSD__
	if (pledge("stdio rpath", NULL) == -1)
		die("pledge");
    #endif

	loadhistory();

	if (fast && !isatty(0)) {
		grabkeyboard();
		readstdin();
	} else {
		readstdin();
		grabkeyboard();
	}

    /* set values which we can restore later */
    if (!imagew || !imageh || !imageg) {
        imagew = imagewidth;
        imageh = imageheight;
        imagegaps = imagegaps;
    }

	setup();
	eventloop();

	return 1; /* unreachable */
}
