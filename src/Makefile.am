bin_PROGRAMS = gnome-paint

gnome_paint_SOURCES =         \
	gp-application.c      \
	gp-colorbutton.c      \
	gp-colorselectorbox.c \
	gp-drawingarea.c      \
	gp-imageeditor.c      \
	gp-linetool.c         \
	gp-main.c             \
	gp-rectangletool.c    \
	gp-tool.c             \
	gp-toolbox.c          \
	gp-window.c

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
