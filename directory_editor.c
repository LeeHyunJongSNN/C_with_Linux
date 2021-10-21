#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <error.h>
#include <errno.h>

#define PERMS 0666
#define MAX_BUF_SIZE 1024
#define MAX_PATH_LENG 1024

void print_filetype(const struct stat *buf) // 파일의 타입을 받아오는 함수
{
    if(S_ISREG(buf->st_mode))
    {
        printf("Type: Regular File\n");
    }
    else if(S_ISDIR(buf->st_mode))
    {
        printf("Type: Directory File\n");
    }
    else
        printf("Type: Extra File\n");

    puts("");
}

int main(int argc, char const *argv[])
{
    char ans[6];
    char cmd[20];
    char *mk = "mkfile";
    char *ch = "chfile";
    char *list = "list";
    char *dirch = "chdir";
    char *dirmk = "mkdir";
    char *dirrm = "rmdir";
    char *end = "stop";
    char *save = "SAVE_COMMAND";
    char *dirpath = ".";
    char *pathname[100];
    char *msg [100];
    char *chmsg [101]={ '\n' };
    char* type[20];
    char cwd[MAX_PATH_LENG + 1] = { '\0', };
    char mkcwd[MAX_PATH_LENG + 1] = { '\0', };
    char rmcwd[MAX_PATH_LENG + 1] = { '\0', };
    char buf[MAX_BUF_SIZE + 1] = { '\0', };

    int fd = 0;
    int cnt;
    int idx;

    ssize_t wsize = 0;

    struct dirent **name;
    struct stat dirfile;
    
    printf("mkfile: 파일 생성, chfile: 파일 수정, list: 현재 디렉토리 내 파일 출력, stop: 프로그램 종료\n");
    printf("원하는 명령어를 입력해주세요: ");
    gets(ans);

    while (strcmp(ans, mk) != 0 && strcmp(ans, ch) != 0 && strcmp(ans, list) != 0 && strcmp(ans, end) != 0) // 올바른 명령어를 받을 때까지 반복 실행
    {
        printf("다시 입력해주세요: ");
        gets(ans);
    } 

    do // 종료 명령어를 받을 때까지 반복 실행
    {
        if(strcmp(ans, mk) == 0) // 파일 생성
        {
            printf("파일 이름(또는 경로)를 입력해주세요: ");
            gets(pathname);

            fd = open(pathname, O_CREAT | O_EXCL | O_RDWR, PERMS);

            if (fd == -1)
            {
                printf("동일한 이름의 파일이 이미 존재합니다!!\n");
                printf("파일 이름(또는 경로)를 입력해주세요: ");
                gets(pathname);
                fd = open(pathname, O_CREAT | O_EXCL | O_RDWR, PERMS);
            }
            printf("파일 내용을 입력해주세요: ");
            gets(msg);

            wsize = write(fd, (char*)msg, strlen(msg));

            if (wsize == -1)
            {
                printf ("파일 내용 입력 오류가 발생했습니다!!\n");
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            printf("파일 입력을 중지하려면 SAVE_COMMAND를 입력해주세요: ");
            gets(cmd);

            while(strcmp(cmd, save) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            if(strcmp(cmd, save) == 0)
            {
                close(fd);

                printf("mkfile: 파일 생성, chfile: 파일 수정, list: 현재 디렉토리 내 파일 출력, stop: 프로그램 종료\n");
                printf("원하는 명령어를 입력해주세요: ");
                gets(ans);
            }
        }

        else if(strcmp(ans, ch) == 0) // 파일 수정
        {
            printf("파일 이름(또는 경로)를 입력해주세요: ");
            gets(pathname);

            fd = open(pathname, O_RDWR | O_APPEND, PERMS);
            read(fd, buf, MAX_BUF_SIZE);
            puts(buf);

            if (fd == -1)
            {
                printf("파일이 존재하지 않습니다!!");
                printf("파일 이름(또는 경로)를 입력해주세요: ");
                gets(pathname);
                fd = open(pathname, O_RDWR | O_APPEND, PERMS);
            }

            printf("덧붙일 파일 내용을 입력해주세요: ");
            gets(msg);
            strcat(chmsg, msg);

            wsize = write(fd, (char*)chmsg, strlen(chmsg));
            
            if (wsize == -1)
            {
                printf ("파일 내용 입력 오류가 발생했습니다!!\n");
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            printf("파일 입력을 중지하려면 SAVE_COMMAND를 입력해주세요: ");
            gets(cmd);

            while(strcmp(cmd, save) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            if(strcmp(cmd, save) == 0)
            {
                close(fd);

                printf("mkfile: 파일 생성, chfile: 파일 수정, list: 현재 디렉토리 내 파일 출력, stop: 프로그램 종료\n");
                printf("원하는 명령어를 입력해주세요: ");
                gets(ans);
            }
        }

        else if(strcmp(ans, list) == 0) // 디렉토리 및 파일 출력
        {
            printf("디렉토리 내 파일들을 출력합니다.\n");

            if((cnt = scandir(dirpath, &name, NULL, alphasort)) == -1)
            {
                fprintf(stderr, "%s 디렉토리 조회 오류: %s\n", dirpath, strerror(errno));
                return 1;
            }

            for (idx = cnt -1; idx >=0; idx--)
            {
                printf("Name: %s\n", name[idx]->d_name);
                lstat(name[idx]->d_name, &dirfile);
                print_filetype(&dirfile);
            }   
            printf("mkfile: 파일 생성, chfile: 파일 수정, chdir: 디렉토리 이동, mkdir: 디렉토리 생성, rmdir: 디렉토리 삭제 stop: 프로그램 종료\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, mk) != 0 && strcmp(ans, ch) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(ans);
            }
        }

        else if (strcmp(ans, dirch) == 0)
        {
            if (getcwd(cwd, MAX_PATH_LENG) == NULL)
            {
                printf("현재 작업중인 디렉토리가 없습니다!!\n");
                exit(-1);
            }

            printf("현재 작업중인 디렉토리: %s\n", cwd);
            printf("이동할 디렉토리 이름을 입력해주세요: ");
            gets(cwd);
            chdir(cwd);

            if(getcwd(cwd, MAX_PATH_LENG) == NULL)
            {
                printf("해당 디렉토리가 존재하지 않습니다!!\n");
                printf("디렉토리 이름을 입력해주세요: ");
                gets(cwd);
            }

            printf("이동한 디렉토리: %s\n", cwd);
            printf("현재 디렉토리 목록을 보고 싶으면 list 명령어를 프로그램을 종료하려면 stop 명령어를 입력해주세요: ");
            gets(ans);
            while(strcmp(ans, list) != 0 && strcmp (ans, end) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(ans);
            }

        }
        
        else if (strcmp(ans, dirmk) == 0)
        {
            printf("생성할 디렉토리 이름을 입력해주세요: ");
            gets(mkcwd);
            mkdir(mkcwd, PERMS);

            printf("현재 디렉토리 목록을 보거나 다른 작업을 하고 싶으면 list 명령어를, 프로그램을 종료하려면 stop 명령어를 입력해주세요: ");
            gets(ans);
            while(strcmp(ans, list) != 0 && strcmp (ans, end) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(ans);
            }
        }

        else if (strcmp(ans, dirrm) == 0)
        {
            printf("삭제할 디렉토리 이름을 입력해주세요: ");
            gets(rmcwd);
            rmdir(rmcwd);

            printf("현재 디렉토리 목록을 보거나 다른 작업을 하고 싶으면 list 명령어를, 프로그램을 종료하려면 stop 명령어를 입력해주세요: ");
            gets(ans);
            while(strcmp(ans, list) != 0 && strcmp (ans, end) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(ans);
            }
        }
    
    } while (strcmp(ans, end) != 0);

    if(strcmp(ans, end) == 0)
    {
        printf("프로그램을 종료합니다.\n");
        exit(1);
    }

    return 0;
}