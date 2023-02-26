# spmenu

# See LICENSE file for copyright and license details.

include options.mk
include host.mk
include toggle.mk

SRC = draw.c spmenu.c main.c
OBJ = $(SRC:.c=.o)

all: options spmenu

options:
	@echo spmenu build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

.c.o:
	$(CC) -c $(CFLAGS) -g $<


$(OBJ): arg.h options.h options.mk draw.h

spmenu: spmenu.o draw.o main.o
	$(CC) -o $@ spmenu.o draw.o main.o $(LDFLAGS)

clean:
	rm -f spmenu $(OBJ) spmenu-$(VERSION).tar.gz

dist: clean
	mkdir -p spmenu-$(VERSION)
	cp -rf LICENSE Makefile *.h *.mk *.c scripts/ docs/ libs/ spmenu-$(VERSION)
	tar -cf spmenu-$(VERSION).tar spmenu-$(VERSION)
	gzip spmenu-$(VERSION).tar
	rm -rf spmenu-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -rf spmenu scripts/* $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/spmenu*
	rm -f *.o
	rm -f spmenu
	rm -f a.out

compat:
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu
	rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_run
	ln -s $(DESTDIR)$(PREFIX)/bin/spmenu $(DESTDIR)$(PREFIX)/bin/dmenu
	ln -s $(DESTDIR)$(PREFIX)/bin/spmenu_run $(DESTDIR)$(PREFIX)/bin/dmenu_run

uninstall:
		  $(DESTDIR)$(PREFIX)/bin/spmenu*\

help:
	@echo install: Installs spmenu. You may need to run this as root.
	@echo uninstall: Uninstalls spmenu. You may need to run this as root.
	@echo dist: Creates a release for spmenu.
	@echo clean: Removes objects and tarballs.
	@echo compat: Installs compatibility with dmenu. WARNING: This will overwrite dmenu and dmenu_run
	@echo help: Displays this help sheet.

.PHONY: all options clean dist install uninstall help
