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
                                       "<img src=\"%s\" style=\"width:60%%\" class=\"center\"/>\n",
                                       elem);
        } else if( array->len > 1) {
                g_string_append_printf(str,
                                       "<div class=\"slideshow-container\">\n");

                for(i = 0; i < array->len; i++) {
                        elem = g_array_index(array, char*, i);

                        g_string_append_printf(str, "<div class=\"mySlides fade\">\n");

                        g_string_append_printf(str,
                                               "<img src=\"images/%s\" style=\"width:60%%\" class=\"center\">\n", elem);

                        g_string_append_printf(str, "</div>\n");
                }

                g_string_append_printf(str,
                                       "<a class=\"prev\" onclick=\"plusSlides(-1)\">&#10094;</a>");

                g_string_append_printf(str,
                                       "<a class=\"next\" onclick=\"plusSlides(1)\">&#10095;</a>");

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

char *file_heading() {
        GString *str = g_string_new(NULL);

        g_string_append_printf(str, "<!doctype html>\n");
        g_string_append_printf(str, "<html>\n");
        g_string_append_printf(str, "<head>\n");
        g_string_append_printf(str, "<meta charset=\"utf-8\">\n");
        g_string_append_printf(str, "<meta name=\"description\" content=\"\">\n");
        g_string_append_printf(str, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");
        g_string_append_printf(str, "<title>Digimanity</title>\n");
        g_string_append_printf(str, "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">\n");
        g_string_append_printf(str, "<link href=\"https://fonts.googleapis.com/css?family=Nunito+Sans:300,400,600,700,800,900\" rel=\"stylesheet\">\n");
        g_string_append_printf(str, "<link rel=\"stylesheet\" href=\"css/scribbler-global.css\">\n");
        g_string_append_printf(str, "<link rel=\"stylesheet\" href=\"css/scribbler-doc.css\">\n");
        g_string_append_printf(str, "<link rel=\"author\" href=\"humans.txt\">\n");
        g_string_append_printf(str, "<link rel=\"icon\" href=\"images/favicon.ico\">\n");
        g_string_append_printf(str, "</head>\n");
        g_string_append_printf(str, "<body>\n");
        g_string_append_printf(str, "<div class=\"doc__bg\"></div>\n");
        g_string_append_printf(str, "<nav class=\"header\">\n");
        g_string_append_printf(str, "<h1 class=\"logo\">Digimanity<span class=\"logo__thin\"></span></h1>\n");
        g_string_append_printf(str, "<ul class=\"menu\">\n");
        g_string_append_printf(str, "<div class=\"menu__item toggle\"><span></span></div>\n");
        g_string_append_printf(str, "<li class=\"menu__item\"><a href=\"source.caderno\" target=\"_blank\" class=\"link link--dark\"><i class=\"fa fa-code\"></i> Wiki Code</a></li>\n");
        g_string_append_printf(str, "<li class=\"menu__item\"><a href=\"https://github.com/Syrayse/digitalHumanity\" target=\"_blank\" class=\"link link--dark\"><i class=\"fa fa-github\"></i> Github</a></li>\n");
        g_string_append_printf(str, "<li class=\"menu__item\"><a href=\"index.html\" class=\"link link--dark\"><i class=\"fa fa-home\"></i> Home</a></li>\n");
        g_string_append_printf(str, "</ul>\n");
        g_string_append_printf(str, "</nav>\n");
        g_string_append_printf(str, "<div class=\"wrapper\">\n");

        return g_string_free(str, FALSE);
}

char *file_middle() {
        GString *str = g_string_new(NULL);

        g_string_append_printf(str, "<article class=\"doc__content\">\n");
        g_string_append_printf(str, "<section class=\"js-section\">\n");

        return g_string_free(str, FALSE);
}

char *file_ending() {
        GString *str = g_string_new(NULL);

        g_string_append_printf(str, "</section>\n");
        g_string_append_printf(str, "</article>\n");
        g_string_append_printf(str, "</div>\n");
        g_string_append_printf(str, "<footer class=\"footer\">Processamento de Linguagens - 2020</footer>\n");
        g_string_append_printf(str, "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.12.0/highlight.min.js\"></script>\n");
        g_string_append_printf(str, "<script>hljs.initHighlightingOnLoad();</script>\n");
        g_string_append_printf(str, "<script src=\"js/scribbler.js\"></script>\n");
        g_string_append_printf(str, "</body>\n");
        g_string_append_printf(str, "</html>\n");

        return g_string_free(str, FALSE);
}

char *index_intro() {
        GString *str = g_string_new(NULL);

        g_string_append_printf(str, "<h2 class=\"section__title\">Digital Humanity</h2>\n");
        g_string_append_printf(str, "<img src=\"images/world.jpg\" class=\"center\" style=\"width:50%%\"/>\n");
        g_string_append_printf(str, "<p>O projeto <i>Digimanity</i> surge no âmbito da unidade curricular de Processamento de Linguagens, na tentativa de emular o comportamento de automatização de criação de enciclopédias de forma a permitir a ágil catalogação de diferentes elementos, de distintas classes.</p>\n");
        g_string_append_printf(str, "<p>Isto utiliza uma língua de domínio especifico para geração de sites HTML para representar notebooks em notação simplificada de <i>turtle</i>.</p>\n");
        g_string_append_printf(str, "<h2 class=\"section__title\">Como Utilizar</h2>\n");
        g_string_append_printf(str, "<p>Para instalar esta utilidade, deve ser simplesmente aberto ficheiro <code>install.sh</code> dispónivel na diretória principal. O código, será automaticamente compilado e o produto colocado em <code>/usr/local/bin</code> sob o nome de <code>digimanity</code>.</p>\n");
        g_string_append_printf(str, "<h2 class=\"section__title\">Autores</h2>\n");
        g_string_append_printf(str, "<ul>\n");
        g_string_append_printf(str, "<li><a href=\"https://github.com/ribeiropdiogo\" target=\"_blank\">Diogo Pinto Ribeiro</a></li>\n");
        g_string_append_printf(str, "<li><a href=\"https://github.com/Syrayse\" target=\"_blank\">Rui Pedro Neto Reis</a></li>\n");
        g_string_append_printf(str, "</ul>\n");

        return g_string_free(str, FALSE);
}
