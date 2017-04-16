bin_PROGRAMS = gnome-paint

gp-marshal.h: marshal.list
	glib-genmarshal --prefix=gp ./marshal.list --header > gp-marshal.h
gp-marshal.c: marshal.list gp-marshal.h
	glib-genmarshal --prefix=gp ./marshal.list --body > gp-marshal.c

gnome_paint_SOURCES =         \
	gp-application.c      \
	gp-basetool.c         \
	gp-colorbutton.c      \
	gp-colormanager.c     \
	gp-colorselectorbox.c \
	gp-dialogutils.c      \
	gp-document.c	      \
	gp-documentmanager.c  \
	gp-drawingarea.c      \
	gp-headerbar.c        \
	gp-help-commands.c    \
	gp-imageeditor.c      \
	gp-linetool.c         \
	gp-main.c             \
	gp-marshal.c          \
	gp-rectangleselectiontool.c \
	gp-rectangletool.c    \
	gp-selectiontool.c    \
	gp-sizetoolproperty.c \
	gp-toolproperty.c     \
	gp-toolbox.c          \
	gp-tool.c             \
	gp-toolmanager.c      \
	gp-widget-init.c      \
	gp-window.c           \
	gp-window-commands.c

gnome_paint_CFLAGS = $(GNOME_PAINT_CFLAGS) -DLOCALEDIR=\""$(localedir)"\"

gnome_paint_LDADD = $(GNOME_PAINT_LIBS)

nodist_gnome_paint_SOURCES = \
	gp-resource.c

noinst_resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(top_srcdir)/data $(top_srcdir)/data/org.gnome.Paint.gresource.xml)

dist_noinst_DATA = \
	$(noinst_resource_files)

gp-resource.c gp-resource.h: $(top_srcdir)/data/org.gnome.Paint.gresource.xml Makefile $(noinst_resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/data --generate --c-name gl $<

-include $(top_srcdir)/git.mk
