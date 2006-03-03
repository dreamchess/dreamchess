SUBDIRS = desktop src man m4
EXTRA_DIST = data
ACLOCAL_AMFLAGS = -I m4

INSTALL_DIRS = \
	/ \
	/themes/ \
	/styles/default/ \
	/styles/sketch/ \
	/styles/elements/ \
	/pieces/classiclow/ \
	/pieces/figurine/ \
	/pieces/sketch/ \
	/pieces/elements/ \
	/boards/classic/ \
	/boards/sketch/ \
	/boards/elements/
	

dist-hook:
	rm -rf `find $(distdir)/data -name '.svn'`

install-data-local:
	@for dir in $(INSTALL_DIRS); do \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)$$dir; \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file -a $$file != ".svn"; then \
	      echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(pkgdatadir)$$dir`basename $$file`'"; \
	      $(INSTALL_DATA) "$$file" "$(DESTDIR)$(pkgdatadir)$$dir`basename $$file`"; \
	    fi; \
	  done; \
	done

uninstall-local:
	@for dir in $(INSTALL_DIRS); do \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file; then \
	      echo " rm -f '$(DESTDIR)$(pkgdatadir)$$dir`basename $$file`'"; \
	      rm -f "$(DESTDIR)$(pkgdatadir)$$dir/`basename $$file`"; \
	    fi; \
	  done; \
	done
