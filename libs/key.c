void
updatenumlockmask(void)
{
	unsigned int i, j;
	XModifierKeymap *modmap;

	numlockmask = 0;
	modmap = XGetModifierMapping(dpy);
	for (i = 0; i < 8; i++)
		for (j = 0; j < modmap->max_keypermod; j++)
			if (modmap->modifiermap[i * modmap->max_keypermod + j]
				== XKeysymToKeycode(dpy, XK_Num_Lock))
				numlockmask = (1 << i);
	XFreeModifiermap(modmap);
}

void
keypress(XEvent *e)
{
   	updatenumlockmask();
    {
        unsigned int i;
        KeySym keysym;
        XKeyEvent *ev;
        char buf[64];
        KeySym ksym = NoSymbol;
        Status status;

        int len = 0;
        ev = &e->xkey;
        len = XmbLookupString(xic, ev, buf, sizeof buf, &ksym, &status);

        keysym = XkbKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0, 0);

        for (i = 0; i < LENGTH(keys); i++) {
            if (keysym == keys[i].keysym && CLEANMASK(keys[i].mod) == CLEANMASK(ev->state) && keys[i].func) {
                if ((keys[i].mode && curMode) || keys[i].mode == -1) {
                    keys[i].func(&(keys[i].arg));
                    return;
                } else if (!keys[i].mode && !curMode) {
                    keys[i].func(&(keys[i].arg));
                } else {
                    continue;
                }
            }
        }

        if (!iscntrl(*buf) && type && curMode ) {
            if (allowkeys) {
                insert(buf, len);
            } else {
                allowkeys = !allowkeys;
            }

            drawmenu();
        }
    }
}
