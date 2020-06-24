#ifndef HTML_CONVERTER_UTILS
#define HTML_CONVERTER_UTILS

#include <glib.h>

char *add_attachments(GArray *array);
char *sidebar_info(GArray *array, char *curr);
char *image_displayer(GArray* array);

#endif
