#ifndef CONFIG_H
#define CONFIG_H

#define RETSIGTYPE void
#define NANO_REG_EXTENDED REG_EXTENDED
#define VERSION "2.8.2"
#define SYSCONFDIR "/usr/local/etc"
#define PACKAGE_STRING "GNU nano 2.8.2"
#define PACKAGE "nano"

/* Define this to enable the comment/uncomment function. */
#define ENABLE_COMMENT 1

/* Define this to enable the Ctrl+G help texts. */
#define ENABLE_HELP 1

/* Define this to enable line numbering. */
#define ENABLE_LINENUMBERS 1

/* Define this to enable mouse support. */
#define ENABLE_MOUSE 1

/* Define this to enable multiple file buffers. */
#define ENABLE_MULTIBUFFER 1

/* Define this if your system has sufficient UTF-8 support. */
#define ENABLE_UTF8 1

#define HAVE_USE_DEFAULT_COLORS

#endif

