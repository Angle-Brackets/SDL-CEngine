#include <dirent.h>
#include "text/text.h"
#include "global.h"

static PIX_Font* default_font = NULL;

PIX_Font* load_pix_font(const char* font_path, u32 font_size, SDL_Color color){
    PIX_Font* font = FC_CreateFont();
    FC_LoadFont(font, global.render.renderer, font_path, font_size, color, TTF_STYLE_NORMAL);
    return font;
}

bool set_default_font(PIX_Font* font){
    if(font == NULL){
        return false;
    }
    default_font = font;
    return true;
}

PIX_Font* find_system_default(const char *font_dir) {
    DIR *dir = opendir(font_dir);
    if (dir == NULL) {
        perror("opendir");
        return NULL;
    }

    struct dirent *entry;
    PIX_Font *default_font = NULL;

    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..' entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[256];
        snprintf(path, sizeof(path), "%s/%s", font_dir, entry->d_name);

        if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {  // Check if it's a regular file
            const char* extension = strrchr(entry->d_name, '.');
            if (extension != NULL && strncmp(extension, ".ttf", 4) == 0) {
                if((default_font = load_pix_font(path, 12, (SDL_Color){0,0,0,255})) == NULL){
                    WARN("Failed to load default font from %s. Please provide a font manually.\n", path);
                    closedir(dir);
                    return NULL;
                }

                WARN("Found font: %s\nSet to font size 12 and black fill color. Initialize default font with set_default_font!\n", entry->d_name);
                closedir(dir);
                return default_font;
            }
        }
        else if (entry->d_type == DT_DIR) { // Handle directories
            void* font_from_subdir = find_system_default(path);
            if (font_from_subdir != NULL) {
                closedir(dir);
                return font_from_subdir;
            }
        }
    }

    closedir(dir);
    WARN("Failed to find a default font. Please provide a font manually.\n")

    return NULL;
}

PIX_Font* get_default_font(){
    //Either return the currently set default font or the default system font.
    if(default_font != NULL){
        return default_font;
    }

    WARN("Attempting to set default font to system font...\n");
    #ifdef __APPLE__
        return find_system_default("/Library/Fonts/");
    #elif _WIN32
        return find_system_default("C:\\\\Windows\\\\Fonts"); //NOT TESTED
    #elif __linux__
        return find_system_default("/usr/share/fonts"); //NOT TESTED
    #else
        WARN("Unknown Operating System. Returning NULL for default font.\n");
        return NULL;
    #endif
}

void free_pix_font(PIX_Font* font){
    FC_FreeFont(font);
    font = NULL;
}
