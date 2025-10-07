/*
 * lsv1.1.0 - Long Listing Implementation
 * Feature: -l option support
 * Author : Zaneeha
 * Instructor: Dr. Muhammad Arif Butt
 */

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

extern int errno;

void do_ls(const char *dir);
void do_ls_long(const char *dir);
void print_permissions(mode_t mode);

int main(int argc, char *argv[])
{
    int opt;
    int long_listing = 0;

    // Parse command-line options
    while ((opt = getopt(argc, argv, "l")) != -1)
    {
        switch (opt)
        {
        case 'l':
            long_listing = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [directory...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // If no directory specified, default to current
    if (optind == argc)
    {
        if (long_listing)
            do_ls_long(".");
        else
            do_ls(".");
    }
    else
    {
        // Process each directory argument
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);
            if (long_listing)
                do_ls_long(argv[i]);
            else
                do_ls(argv[i]);
            puts("");
        }
    }

    return 0;
}

// Original simple listing
void do_ls(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        printf("%s\n", entry->d_name);
    }

    if (errno != 0)
        perror("readdir failed");

    closedir(dp);
}

// -------- NEW FUNCTION --------
// Long listing format implementation
void do_ls_long(const char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    struct stat statbuf;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        // Build full file path
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        // Get file metadata
        if (lstat(path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        // 1️⃣ File type & permissions
        print_permissions(statbuf.st_mode);

        // 2️⃣ Number of hard links
        printf(" %2ld", statbuf.st_nlink);

        // 3️⃣ Owner and group names
        struct passwd *pw = getpwuid(statbuf.st_uid);
        struct group *gr = getgrgid(statbuf.st_gid);
        printf(" %-8s %-8s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        // 4️⃣ File size in bytes
        printf(" %8ld", statbuf.st_size);

        // 5️⃣ Last modification time (ctime converts to string)
        char *mtime = ctime(&statbuf.st_mtime);
        mtime[strlen(mtime) - 1] = '\0'; // remove newline
        printf(" %s", mtime);

        // 6️⃣ File name
        printf(" %s\n", entry->d_name);
    }

    closedir(dp);
}

// Helper function to print permission bits (rwx)
void print_permissions(mode_t mode)
{
    // File type
    if (S_ISREG(mode))
        printf("-");
    else if (S_ISDIR(mode))
        printf("d");
    else if (S_ISLNK(mode))
        printf("l");
    else
        printf("?");

    // User permissions
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');

    // Group permissions
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');

    // Others permissions
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

