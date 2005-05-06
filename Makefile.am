SUBDIRS = src man m4
ACLOCAL_AMFLAGS = -I m4

INSTALL_DIRS = \
	/ \
	/themes/default/ \
	/pieces/classiclow/ \
	/pieces/figurine/ \
	/boards/classic/

install-data-local:
	@for dir in $(INSTALL_DIRS); do \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$dir; \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file; then \
	      echo " $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)$$dir`basename $$file`"; \
	      $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)/$$dir; \
	    fi; \
	  done; \
	done

uninstall-local:
	@for dir in $(INSTALL_DIRS); do \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file; then \
	      echo " rm -f $(DESTDIR)$(pkgdatadir)$$dir`basename $$file`"; \
	      rm -f $(DESTDIR)$(pkgdatadir)/$$dir/`basename $$file`; \
	    fi; \
	  done; \
	done
