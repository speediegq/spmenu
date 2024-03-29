/* See LICENSE file for copyright and license details. */
void setupdisplay_x11(void) {
    int x, y, i;
    unsigned int du;

    Window w, dw, *dws;
    XWindowAttributes wa;

    prepare_window_size_x11();

    // resize client to image height if deemed necessary
#if IMAGE
    if (image) resizetoimageheight(img.imageheight);
#endif

    // set prompt width based on prompt size
    sp.promptw = (prompt && *prompt)
        ? pango_prompt ? TEXTWM(prompt) : TEXTW(prompt) - sp.lrpad / 4 : 0; // prompt width

    // init xinerama screens
#if XINERAMA
    XineramaScreenInfo *info;
    Window pw;
    int a, n, area = 0;
    int j, di;

    i = 0;
    if (parentwin == root && (info = XineramaQueryScreens(dpy, &n))) {
        XGetInputFocus(dpy, &w, &di);
        if (mon >= 0 && mon < n)
            i = mon;
        else if (w != root && w != PointerRoot && w != None) {
            // find top-level window containing current input focus
            do {
                if (XQueryTree(dpy, (pw = w), &dw, &w, &dws, &du) && dws)
                    XFree(dws);
            } while (w != root && w != pw);
            // find xinerama screen with which the window intersects most
            if (XGetWindowAttributes(dpy, pw, &wa))
                for (j = 0; j < n; j++)
                    if ((a = INTERSECT(wa.x, wa.y, wa.width, wa.height, info[j])) > area) {
                        area = a;
                        i = j;
                    }
        }
        // no focused window is on screen, so use pointer location instead
        if (mon < 0 && !area && XQueryPointer(dpy, root, &dw, &dw, &x, &y, &di, &di, &du))
            for (i = 0; i < n; i++)
                if (INTERSECT(x, y, 1, 1, info[i]))
                    break;

        mo.output_width = info[i].width;
        mo.output_height = info[i].height;
        mo.output_xpos = info[i].x_org;
        mo.output_ypos = info[i].y_org;

        XFree(info);
    } else
#endif
    {
        if (!XGetWindowAttributes(dpy, parentwin, &wa))
            die("spmenu: could not get embedding window attributes: 0x%lx",
                    parentwin); // die because unable to get attributes for the parent window

        mo.output_width = wa.width;
        mo.output_height = wa.height;
    }

    get_mh();

    if (menuposition == 2) { // centered
        sp.mw = MIN(MAX(max_textw() + sp.promptw, centerwidth), mo.output_width);
        x = mo.output_xpos + ((mo.output_width  - sp.mw) / 2 + xpos);
        y = mo.output_ypos + ((mo.output_height - sp.mh) / 2 - ypos);
    } else { // top or bottom
        x = mo.output_xpos + xpos;
        y = mo.output_ypos + menuposition ? (-ypos) : (mo.output_height - sp.mh - ypos);
        sp.mw = (menuwidth > 0 ? menuwidth : mo.output_width);
    }


    // create menu window and set properties for it
    create_window_x11(
            x + sp.sp,
            y + sp.vp - (menuposition == 1 ? 0 : menuposition == 2 ? borderwidth : borderwidth * 2),
            sp.mw - 2 * sp.sp - borderwidth * 2,
            sp.mh
    );

    set_window_x11();
    set_prop_x11();

#if IMAGE
    setimageopts();
#endif

    open_xim(); // open xim

    XMapRaised(dpy, win);
    XSync(dpy, False);
    XGetWindowAttributes(dpy, win, &wa);

    if (wa.map_state == IsViewable) { // must be viewable, otherwise we get a BadMatch error
        XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
    }

    // embed spmenu inside parent window
    if (x11.embed) {
        XReparentWindow(dpy, win, parentwin, x, y);
        XSelectInput(dpy, parentwin, FocusChangeMask | SubstructureNotifyMask);

        if (XQueryTree(dpy, parentwin, &dw, &w, &dws, &du) && dws) {
            for (i = 0; i < du && dws[i] != win; ++i)
                XSelectInput(dpy, dws[i], FocusChangeMask);
            XFree(dws);
        }

        grabfocus_x11();
    }

    // resize window and draw
    draw_resize(draw, sp.mw - 2 * sp.sp - borderwidth * 2, sp.mh);

    match();
    drawmenu();
}

void prepare_window_size_x11(void) {
    sp.sp = menupaddingh;
    sp.vp = (menuposition == 1) ? menupaddingv : - menupaddingv;

    sp.bh = MAX(draw->font->h, draw->font->h + 2 + lineheight);
    lines = MAX(lines, 0);
#if IMAGE
    img.setlines = lines;
#endif

    sp.lrpad = draw->font->h + textpadding;
    get_mh();

    return;
}

Display * opendisplay_x11(char *disp) {
    return XOpenDisplay(disp);
}

void set_screen_x11(Display *disp) {
    x11.screen = DefaultScreen(disp);
    root = RootWindow(disp, x11.screen);
}

void handle_x11(void) {
    XWindowAttributes wa;

    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
        fputs("warning: no locale support\n", stderr); // invalid locale, so notify the user about it

    if (!XSetLocaleModifiers(""))
        fputs("warning: no locale modifiers support\n", stderr);

    if (!(dpy = opendisplay_x11(NULL)))
        die("spmenu: cannot open display"); // failed to open display

    // set screen and root window
    set_screen_x11(dpy);

    // parent window is the root window (ie. window manager) because we're not embedding
    if (!x11.embed || !(parentwin = strtol(x11.embed, NULL, 0)))
        parentwin = root;

    if (!XGetWindowAttributes(dpy, parentwin, &wa)) {
        die("spmenu: could not get embedding window attributes: 0x%lx", parentwin);
    }

    xinitvisual(); // init visual and create drawable after
    draw = draw_create_x11(dpy, x11.screen, root, wa.width, wa.height, x11.visual, x11.depth, x11.cmap, protocol);
}

void cleanup_x11(Display *disp) {
    XUngrabKey(disp, AnyKey, AnyModifier, root);
    XSync(disp, False);
    XCloseDisplay(disp);
}
