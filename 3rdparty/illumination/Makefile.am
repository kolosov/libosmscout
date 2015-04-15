ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src include tests
EXTRA_DIST = ./config.rpath autogen.sh \
	     catalogs/*.xml \
	     packaging/maemo/README.Debian \
	     packaging/maemo/changelog \
	     packaging/maemo/compat \
	     packaging/maemo/control \
	     packaging/maemo/copyright \
	     packaging/maemo/dirs \
	     packaging/maemo/docs \
	     packaging/maemo/libillumination1-dev.install \
	     packaging/maemo/libillumination1.install \
	     packaging/maemo/libillumination1.postinst \
	     packaging/maemo/libillumination1.postrm \
	     packaging/maemo/optify \
	     packaging/maemo/rules

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = illumination.pc illumination-full.pc

dist-hook:
	rm -rf `find $(distdir)/doc -name ".svn"`
