#include <dirent.h>
#include <stdio.h>

int main(void)
{
    DIR *d;
    struct dirent *dir;
    char* list[];
    int count = 0;

    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
            list[count] = dir->dir_name;
            count++;
        }
        closedir(d);
    }
    return(0);
}
