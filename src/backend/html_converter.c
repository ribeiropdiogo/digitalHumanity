#include "html_converter.h"

char *add_attachments(GArray *array) {
        int i;
        char *elem;
        GString *str = g_string_new(NULL);

        if(array->len > 0) {
                g_string_append_printf( str,
                                        "<h2 class=\"section__title\">Anexos</h2>");

                g_string_append_printf(str, "<ul>\n");

                for(i = 0; i < array->len; i++) {
                        elem = g_array_index(array, char*, i);
                        g_string_append_printf(str,
                                               "<li><a href=\"%s\">%s</a></li>\n"
                                               , elem, elem);
                }

                g_string_append_printf(str, "</ul>\n");
        }

        return g_string_free(str, FALSE);
}

char *sidebar_info(GArray *array, char *curr) {
        int i;
        char *elem;
        GString *str = g_string_new(NULL);

        if( array->len > 0 ) {
                g_string_append_printf(str,
                                       "<aside class=\"doc__nav\">\n");

                g_string_append_printf(str, "<ul>\n");

                for(i = 0; i < array->len; i++) {
                        elem = g_array_index(array, char*, i);
                        g_string_append_printf(str,
                                               "<li class=\"js-btn %s\"><a href=\"%s.html\">%s</a></li>\n",
                                               (!curr || strcmp(elem,curr)) ? "" : "selected", elem, elem);
                }

                g_string_append_printf(str, "</ul>\n");

                g_string_append_printf(str,
                                       "</aside>\n");
        }

        return g_string_free(str, FALSE);
}

char *image_displayer(GArray* array) {
        int i;
        char *elem;
        GString *str = g_string_new(NULL);

        if( array->len == 1 ) {
                elem = g_array_index(array, char*, 0);
                g_string_append_printf(str,
                                       "<img src=\"%s\" style=\"width:60%\" class=\"center\"/>\n",
                                       elem);
        } else if( array->len > 1) {
                g_string_append_printf(str,
                                       "<div class=\"slideshow-container\">\n");

                for(i = 0; i < array->len; i++) {
                        elem = g_array_index(array, char*, i);

                        g_string_append_printf(str, "<div class=\"mySlides fade\">\n");

                        g_string_append_printf(str,
                                               "<img src=\"images/%s\" style=\"width:60%\" class=\"center\">\n", elem);

                        g_string_append_printf(str, "</div>\n");
                }

                g_string_append_printf(str,
                                       "<a class=\"prev\" onclick=\"plusSlides(-1)\">&#10094;</a>");

                g_string_append_printf(str,
                                       "<a class=\"next\" onclick=\"plusSlides(1)\">&#10095;</a>")

                g_string_append_printf(str,
                                       "</div>\n\n");

                g_string_append_printf(str,
                                       "<div style=\"text-align:center\">\n");

                for(i = 0; i < array->len; i++) {
                        g_string_append_printf(str,
                                               "<span class=\"dot\" onclick=\"currentSlide(%d)\"></span>\n", i+1);
                }

                g_string_append_printf(str, "</div>\n");
        }

        return g_string_free(str, FALSE);
}
