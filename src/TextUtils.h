typedef struct {
    const char *accented;
    const char *unaccented;
} AccentMapping;

char *Utf8RemoveAccents(const char *input);