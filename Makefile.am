SUBDIRS = src man data m4
ACLOCAL_AMFLAGS = -I m4

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/themes/default
	@for dir in / /themes/default/; do \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file; then \
	      echo " $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)$$dir`basename $$file`"; \
	      $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)/$$dir; \
	    fi; \
	  done; \
	done

uninstall-local:
	@for dir in / /themes/default/; do \
	  for file in $(srcdir)/data$$dir*; do \
	    if test -f $$file; then \
	      echo " rm -f $(DESTDIR)$(pkgdatadir)$$dir`basename $$file`"; \
	      rm -f $(DESTDIR)$(pkgdatadir)/$$dir/`basename $$file`; \
	    fi; \
	  done; \
	done
