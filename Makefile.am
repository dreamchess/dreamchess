SUBDIRS = desktop doc src man m4 pkg
EXTRA_DIST = data Info.plist COPYRIGHT
ACLOCAL_AMFLAGS = -I m4

INSTALL_DIRS = `find data -name '.svn' -prune -o -name 'data' -o -type d -print`

dist-hook:
	rm -rf `find $(distdir)/data -name '.svn'`

install-data-local:
	@for dir in '' $(INSTALL_DIRS); do \
	  echo $$dir; \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$dir; \
	  for file in $(srcdir)/data/$$dir/*; do \
	    echo $$file; \
	    if test -f $$file; then \
	      echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(pkgdatadir)/$$dir/`basename $$file`'"; \
	      $(INSTALL_DATA) "$$file" "$(DESTDIR)$(pkgdatadir)/$$dir/`basename $$file`"; \
	    fi; \
	  done; \
	done

uninstall-local:
	@for dir in '' $(INSTALL_DIRS); do \
	  for file in $(srcdir)/data$$dir/*; do \
	    if test -f $$file; then \
	      echo " rm -f '$(DESTDIR)$(pkgdatadir)$$dir/`basename $$file`'"; \
	      rm -f "$(DESTDIR)$(pkgdatadir)$$dir/`basename $$file`"; \
	    fi; \
	  done; \
	done
