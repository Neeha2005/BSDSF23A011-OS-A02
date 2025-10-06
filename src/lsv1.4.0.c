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

// Function prototypes
int gather_filenames(const char *dir, char ***filenames, int *count, int *maxlen);
void display_default(char **files, int count, int maxlen);
void display_horizontal(char **files, int count, int maxlen);
void display_long(const char *dir);
int get_terminal_width();

// ================== New Comparison Function ==================
int cmp_str(const void *a, const void *b) {
    const char * const *str1 = (const char * const *)a;
    const char * const *str2 = (const char * const *)b;
    return strcmp(*str1, *str2);
}

// ====================== Main ======================
int main(int argc, char *argv[]) {
    int opt;
    int display_mode = DISPLAY_DEFAULT;

    // Parse options
    while ((opt = getopt(argc, argv, "lx")) != -1) {
        switch (opt) {
        case 'l':
            display_mode = DISPLAY_LONG;
            break;
        case 'x':
            display_mode = DISPLAY_HORIZONTAL;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l | -x] [directory]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    const char *dir = (optind < argc) ? argv[optind] : ".";

    if (display_mode == DISPLAY_LONG) {
        display_long(dir);
        return 0;
    }

    char **files = NULL;
    int count = 0, maxlen = 0;
    if (gather_filenames(dir, &files, &count, &maxlen) == -1)
        return 1;

    // ================= Alphabetical Sorting =================
    qsort(files, count, sizeof(char *), cmp_str);

    // Call display functions
    if (display_mode == DISPLAY_HORIZONTAL)
        display_horizontal(files, count, maxlen);
    else
        display_default(files, count, maxlen);

    // Free memory
    for (int i = 0; i < count; i++) free(files[i]);
    free(files);
    return 0;
}

// ================= Gather Filenames ==================
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
        if (entry->d_name[0] == '.') continue;
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

// ================= Default (Down-Then-Across) Display ==================
void display_default(char **files, int count, int maxlen) {
    int width = get_terminal_width();
    int spacing = 2;
    int cols = width / (maxlen + spacing);
    if (cols < 1) cols = 1;
    int rows = (count + cols - 1) / cols;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = c * rows + r;
            if (i < count)
                printf("%-*s", maxlen + spacing, files[i]);
        }
        printf("\n");
    }
}

// ================= Horizontal Display (-x) ==================
void display_horizontal(char **files, int count, int maxlen) {
    int width = get_terminal_width();
    int spacing = 2;
    int col_width = maxlen + spacing;
    int curr_width = 0;

    for (int i = 0; i < count; i++) {
        if (curr_width + col_width > width) {
            printf("\n");
            curr_width = 0;
        }
        printf("%-*s", col_width, files[i]);
        curr_width += col_width;
    }
    printf("\n");
}

// ================= Long Listing (-l) ==================
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

        // Sticky bit handling
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

int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        return 80; // fallback
    return w.ws_col;
}
