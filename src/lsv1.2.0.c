/*
 * Programming Assignment 02: lsv1.2.0
 * Feature: Column Display (Down Then Across)
 * Author: Zaneeha Afzal
 * Instructor: Dr. Muhammad Arif Butt
 *
 * Description:
 * This version displays directory contents in multiple columns,
 * formatted "down then across", adjusting automatically to terminal width.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

extern int errno;

void do_ls_columns(const char *dir);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        do_ls_columns(".");
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);
            do_ls_columns(argv[i]);
            puts("");
        }
    }
    return 0;
}

void do_ls_columns(const char *dir)
{
    DIR *dp;
    struct dirent *entry;
    char **filenames = NULL;
    int count = 0;
    int max_len = 0;

    dp = opendir(dir);
    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;

    // --- Step 1: Read all filenames into dynamic array ---
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue; // skip hidden files

        filenames = realloc(filenames, sizeof(char *) * (count + 1));
        filenames[count] = strdup(entry->d_name);

        int len = strlen(entry->d_name);
        if (len > max_len)
            max_len = len;

        count++;
    }

    if (errno != 0)
        perror("readdir failed");

    closedir(dp);

    if (count == 0)
    {
        printf("(No visible files)\n");
        return;
    }

    // --- Step 2: Get terminal width ---
    struct winsize ws;
    int term_width = 80; // fallback default width
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
        term_width = ws.ws_col;

    int spacing = 2;
    int col_width = max_len + spacing;

    // --- Step 3: Calculate number of columns and rows ---
    int num_cols = term_width / col_width;
    if (num_cols < 1)
        num_cols = 1;

    int num_rows = (count + num_cols - 1) / num_cols;

    // --- Step 4: Print down-then-across ---
    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            int index = c * num_rows + r;
            if (index < count)
                printf("%-*s", col_width, filenames[index]);
        }
        printf("\n");
    }

    // --- Step 5: Free memory ---
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}
