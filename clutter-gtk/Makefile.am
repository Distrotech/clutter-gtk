include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

INCLUDES = -I$(srcdir) -I$(top_srcdir) -I$(top_builddir)

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Clutter-Gtk\" 		\
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\"		\
	-DCLUTTER_GTK_COMPILATION		\
	$(CLUTTER_GTK_DEPRECATED_CFLAGS)	\
	$(CLUTTER_GTK_DEBUG_CFLAGS) 		\
	$(NULL)

AM_CFLAGS = $(MAINTAINER_CFLAGS) $(CLUTTER_GTK_DEPS_CFLAGS)

lib_LTLIBRARIES = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la

source_c = $(srcdir)/subsurface-protocol.c $(srcdir)/subsurface-client-protocol.h
source_c += \
	$(srcdir)/gtk-clutter-actor.c 		\
	$(srcdir)/gtk-clutter-embed.c 		\
	$(srcdir)/gtk-clutter-offscreen.c	\
	$(srcdir)/gtk-clutter-texture.c		\
	$(srcdir)/gtk-clutter-util.c 		\
	$(srcdir)/gtk-clutter-window.c		\
	$(NULL)

source_h_public = \
	$(srcdir)/gtk-clutter-actor.h 	\
	$(srcdir)/gtk-clutter-embed.h 	\
	$(srcdir)/gtk-clutter-texture.h	\
	$(srcdir)/gtk-clutter-util.h 	\
	$(srcdir)/gtk-clutter-window.h	\
	$(NULL)

source_h_private = \
	$(srcdir)/gtk-clutter-offscreen.h	\
	$(srcdir)/gtk-clutter-actor-internal.h	\
	$(NULL)

# please, keep the list sorted alphabetically
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_SOURCES = $(source_c) $(source_h) $(source_h_private)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LIBADD  = $(CLUTTER_GTK_DEPS_LIBS) $(LIBM)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LDFLAGS = \
	$(CLUTTER_LT_LDFLAGS) \
	-export-symbols-regex "^gtk_clutter.*"

cluttergtkheadersdir = $(includedir)/clutter-gtk-@CLUTTER_GTK_API_VERSION@/clutter-gtk

# please, keep the list sorted alphabetically
cluttergtkheaders_HEADERS = \
	$(top_srcdir)/clutter-gtk/clutter-gtk.h 		\
	$(top_builddir)/clutter-gtk/gtk-clutter-version.h	\
	$(source_h_public)					\
	$(NULL)

EXTRA_DIST += gtk-clutter-version.h.in


@wayland_scanner_rules@

-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =

if HAVE_INTROSPECTION
GtkClutter-@CLUTTER_GTK_API_VERSION@.gir: libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la Makefile

GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_SCANNERFLAGS = \
	--identifier-prefix=GtkClutter \
	--warn-all \
	--pkg-export clutter-gtk-@CLUTTER_GTK_API_VERSION@ \
	--c-include='clutter-gtk/clutter-gtk.h'
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_NAMESPACE    = GtkClutter
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_VERSION      = 1.0
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_INCLUDES     = Gtk-3.0 Clutter-1.0
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_PACKAGES     = gtk+-3.0 clutter-1.0
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_CFLAGS       = $(INCLUDES) $(AM_CPPFLAGS)
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_LIBS         = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la
GtkClutter_@CLUTTER_GTK_API_VERSION_AM@_gir_FILES        = $(source_h_public) $(source_c)

INTROSPECTION_GIRS += GtkClutter-@CLUTTER_GTK_API_VERSION@.gir

girdir = $(datadir)/gir-1.0/
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif # HAVE_INTROSPECTION
