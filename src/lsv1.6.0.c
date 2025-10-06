#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>

extern int errno;

// Display mode flags
#define DISPLAY_DEFAULT 0
#define DISPLAY_LONG 1
#define DISPLAY_HORIZONTAL 2

// ================== ANSI Color Codes ==================
#define COLOR_RESET     "\033[0m"
#define COLOR_BLUE      "\033[0;34m"
#define COLOR_GREEN     "\033[0;32m"
#define COLOR_RED       "\033[0;31m"
#define COLOR_PINK      "\033[0;35m"
#define COLOR_REVERSE   "\033[7m"

// Function prototypes
int gather_filenames(const char *dir, char ***filenames, int *count, int *maxlen);
void display_default(char **files, int count, int maxlen, const char *dir);
void display_horizontal(char **files, int count, int maxlen, const char *dir);
void display_long(const char *dir);
int get_terminal_width();
int cmp_str(const void *a, const void *b);
int get_file_mode(const char *dir, const char *filename, mode_t *mode);
void print_colored_file(const char *dir, const char *filename);
void do_ls(const char *dir, int display_mode);

// ================== Comparison Function ==================
int cmp_str(const void *a, const void *b) {
    const char * const *str1 = (const char * const *)a;
    const char * const *str2 = (const char * const *)b;
    return strcmp(*str1, *str2);
}

// ================== Main ==================
int main(int argc, char *argv[]) {
    int opt;
    int display_mode = DISPLAY_DEFAULT;
    int recursive_flag = 0; // New flag for -R

    // Parse options
    while ((opt = getopt(argc, argv, "lxR")) != -1) {
        switch (opt) {
            case 'l':
                display_mode = DISPLAY_LONG;
                break;
            case 'x':
                display_mode = DISPLAY_HORIZONTAL;
                break;
            case 'R':
                recursive_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l | -x | -R] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *dir = (optind < argc) ? argv[optind] : ".";

    if (display_mode == DISPLAY_LONG && !recursive_flag) {
        display_long(dir);
        return 0;
    }

    if (recursive_flag) {
        do_ls(dir, display_mode);
    } else {
        char **files = NULL;
        int count = 0, maxlen = 0;
        if (gather_filenames(dir, &files, &count, &maxlen) == -1)
            return 1;

        qsort(files, count, sizeof(char *), cmp_str);

        if (display_mode == DISPLAY_HORIZONTAL)
            display_horizontal(files, count, maxlen, dir);
        else
            display_default(files, count, maxlen, dir);

        for (int i = 0; i < count; i++) free(files[i]);
        free(files);
    }

    return 0;
}

// ================== Gather Filenames ==================
int gather_filenames(const char *dir, char ***filenames, int *count, int *maxlen) {
    DIR *dp = opendir(dir);
    if (!dp) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    int size = 0;
    char **files = NULL;
    *maxlen = 0;
    *count = 0;

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.') continue; // skip hidden files
        if (*count >= size) {
            size = size ? size * 2 : 16;
            files = realloc(files, size * sizeof(char *));
        }
        files[*count] = strdup(entry->d_name);
        int len = strlen(entry->d_name);
        if (len > *maxlen) *maxlen = len;
        (*count)++;
    }

    closedir(dp);
    *filenames = files;
    return 0;
}

// ================== Get Terminal Width ==================
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return 80; // fallback
    return w.ws_col;
}

// ================== Get File Mode ==================
int get_file_mode(const char *dir, const char *filename, mode_t *mode) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dir, filename);

    struct stat st;
    if (lstat(path, &st) == -1) {
        perror("lstat");
        return -1;
    }

    *mode = st.st_mode;
    return 0;
}

// ================== Print Colored File ==================
void print_colored_file(const char *dir, const char *filename) {
    mode_t mode;
    if (get_file_mode(dir, filename, &mode) == -1) {
        printf("%s", filename);
        return;
    }

    const char *color = COLOR_RESET;

    if (S_ISDIR(mode))
        color = COLOR_BLUE;
    else if (S_ISLNK(mode))
        color = COLOR_PINK;
    else if (mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH)
        color = COLOR_GREEN;
    else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode))
        color = COLOR_REVERSE;
    else if (strstr(filename, ".tar") || strstr(filename, ".gz") || strstr(filename, ".zip"))
        color = COLOR_RED;

    printf("%s%s%s", color, filename, COLOR_RESET);
}

// ================== Default Display (Down-Then-Across) ==================
void display_default(char **files, int count, int maxlen, const char *dir) {
    int width = get_terminal_width();
    int spacing = 2;
    int cols = width / (maxlen + spacing);
    if (cols < 1) cols = 1;
    int rows = (count + cols - 1) / cols;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = c * rows + r;
            if (i < count) {
                print_colored_file(dir, files[i]);
                int pad = maxlen - (int)strlen(files[i]) + spacing;
                for (int p = 0; p < pad; p++) printf(" ");
            }
        }
        printf("\n");
    }
}

// ================== Horizontal Display (-x) ==================
void display_horizontal(char **files, int count, int maxlen, const char *dir) {
    int width = get_terminal_width();
    int spacing = 2;
    int col_width = maxlen + spacing;
    int curr_width = 0;

    for (int i = 0; i < count; i++) {
        if (curr_width + col_width > width) {
            printf("\n");
            curr_width = 0;
        }

        print_colored_file(dir, files[i]);
        int pad = col_width - (int)strlen(files[i]);
        for (int p = 0; p < pad; p++) printf(" ");
        curr_width += col_width;
    }
    printf("\n");
}

// ================== Long Listing (-l) ==================
void display_long(const char *dir) {
    DIR *dp = opendir(dir);
    if (!dp) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat st;
    char path[1024];

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        if (lstat(path, &st) == -1) continue;

        // File type
        printf((S_ISDIR(st.st_mode)) ? "d" :
               (S_ISLNK(st.st_mode)) ? "l" :
               (S_ISCHR(st.st_mode)) ? "c" :
               (S_ISBLK(st.st_mode)) ? "b" :
               (S_ISFIFO(st.st_mode)) ? "p" :
               (S_ISSOCK(st.st_mode)) ? "s" : "-");

        // Permissions
        printf("%c", (st.st_mode & S_IRUSR) ? 'r' : '-');
        printf("%c", (st.st_mode & S_IWUSR) ? 'w' : '-');
        printf("%c", (st.st_mode & S_IXUSR) ? 'x' : '-');
        printf("%c", (st.st_mode & S_IRGRP) ? 'r' : '-');
        printf("%c", (st.st_mode & S_IWGRP) ? 'w' : '-');
        printf("%c", (st.st_mode & S_IXGRP) ? 'x' : '-');
        printf("%c", (st.st_mode & S_IROTH) ? 'r' : '-');
        printf("%c", (st.st_mode & S_IWOTH) ? 'w' : '-');
        printf("%c", (st.st_mode & S_IXOTH) ? 'x' : '-');

        // Sticky bit
        if (st.st_mode & S_ISVTX) printf("t");

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf(" %3ld %-8s %-8s %8ld %.12s %s\n",
               st.st_nlink,
               pw ? pw->pw_name : "?",
               gr ? gr->gr_name : "?",
               st.st_size,
               ctime(&st.st_mtime) + 4,
               entry->d_name);
    }

    closedir(dp);
}

// ================== Recursive Listing (-R) ==================
void do_ls(const char *dir, int display_mode) {
    char **files = NULL;
    int count = 0, maxlen = 0;

    printf("%s:\n", dir);

    if (gather_filenames(dir, &files, &count, &maxlen) == -1)
        return;

    qsort(files, count, sizeof(char *), cmp_str);

    if (display_mode == DISPLAY_HORIZONTAL)
        display_horizontal(files, count, maxlen, dir);
    else if (display_mode == DISPLAY_DEFAULT)
        display_default(files, count, maxlen, dir);
    else
        display_long(dir);

    for (int i = 0; i < count; i++) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir, files[i]);

        struct stat st;
        if (lstat(path, &st) == -1) continue;

        if (S_ISDIR(st.st_mode)) {
            if (strcmp(files[i], ".") != 0 && strcmp(files[i], "..") != 0) {
                printf("\n");
                do_ls(path, display_mode);
            }
        }
    }

    for (int i = 0; i < count; i++) free(files[i]);
    free(files);
}
