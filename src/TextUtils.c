#include "TextUtils.h"
#include "Settings.h"

AccentMapping accent_mappings[] = {
    {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "Zo"}, {"ú", "u"},
    {"Á", "A"}, {"É", "E"}, {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"},
    {"à", "a"}, {"è", "e"}, {"ì", "i"}, {"ò", "o"}, {"ù", "u"},
    {"À", "A"}, {"È", "E"}, {"Ì", "I"}, {"Ò", "O"}, {"Ù", "U"},
    {"â", "a"}, {"ê", "e"}, {"î", "i"}, {"ô", "o"}, {"û", "u"},
    {"Â", "A"}, {"Ê", "E"}, {"Î", "I"}, {"Ô", "O"}, {"Û", "U"},
    {"ä", "a"}, {"ë", "e"}, {"ï", "i"}, {"ö", "o"}, {"ü", "u"},
    {"Ä", "A"}, {"Ë", "E"}, {"Ï", "I"}, {"Ö", "O"}, {"Ü", "U"},
    {"ã", "a"}, {"õ", "o"},
    {"Ã", "A"}, {"Õ", "O"},
    {"ç", "c"}, {"Ç", "C"},
    {"ñ", "n"}, {"Ñ", "N"}
};

char *Utf8RemoveAccents(const char *input) {
    size_t input_len = strlen(input);
    char *output = malloc(input_len + 1);
    AssertNew(output);

    const char *input_ptr = input;
    char *output_ptr = output;

    while (*input_ptr) {
        int found = 0;
        for (size_t i = 0; i < sizeof(accent_mappings) / sizeof(accent_mappings[0]); ++i) {
            size_t accented_len = strlen(accent_mappings[i].accented);
            if (strncmp(input_ptr, accent_mappings[i].accented, accented_len) == 0) {
                strcpy(output_ptr, accent_mappings[i].unaccented);
                input_ptr += accented_len;
                output_ptr += strlen(accent_mappings[i].unaccented);
                found = 1;
                break;
            }
        }
        if (!found) {
            *output_ptr++ = *input_ptr++;
        }
    }

    *output_ptr = '\0';
    return output;
}