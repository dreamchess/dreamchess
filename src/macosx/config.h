/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Needed for Mini-XML. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/* Define to 1 if you want UI<->engine communication via UNIX pipes. */
/* #undef COMM_PIPE_UNIX */

/* Define to 1 if you want UI<->engine communication via Win32 pipes. */
/* #undef COMM_PIPE_WIN32 */

/* Define to 1 if you want UI<->engine communication via SDL threads. */
#define COMM_SDL_THREADS 1

/* Define to 1 if the compiler supports C99 varargs macros. */
#define HAVE_C99_VARARGS_MACROS 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if the compiler supports GNU C varargs macros. */
#define HAVE_GNUC_VARARGS_MACROS 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `shlwapi' library (-lshlwapi). */
/* #undef HAVE_LIBSHLWAPI */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `usleep' function. */
#define HAVE_USLEEP 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Name of package */
#define PACKAGE "dreamchess"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "dreamchess"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "dreamchess 0.1.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "dreamchess"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1.0"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.1.0"

/* Define to 1 if the sdl user interface driver should be compiled. */
/* #undef WITH_UI_SDL */

/* Define to 1 if the sdlgl user interface driver should be compiled. */
#define WITH_UI_SDLGL 1

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef volatile */

/* Needed for Mini-XML. */
#ifndef HAVE_STRDUP
extern char *mxml_strdup(const char *);
#define strdup mxml_strdup
#endif

extern char *mxml_strdupf(const char *, va_list);

#ifndef HAVE_VSNPRINTF
extern int mxml_vsnprintf(char *, size_t, const char *, va_list);
#define vsnprintf mxml_vsnprintf
#endif
