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
#include <pwd.h>

#define PERMS 0666
#define MAX_BUF_SIZE 1024
#define MAX_PATH_LENG 1024
#define ANSI_COLOR "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"


void print_filetype(const struct stat *buf) // 파일의 타입을 받아오는 함수
{
    if(S_ISREG(buf->st_mode))
    {
        printf(ANSI_COLOR "Type: Regular File\n" ANSI_COLOR_RESET);
    }
    else if(S_ISDIR(buf->st_mode))
    {
        printf(ANSI_COLOR "Type: Directory File\n" ANSI_COLOR_RESET);
    }
    else
        printf(ANSI_COLOR "Type: Extra File\n" ANSI_COLOR_RESET);
}

void print_fileperm(char* filename) // 파일의 권한을 받아오는 함수
{
    int R_perm = access(filename, R_OK);
    int W_perm = access(filename, W_OK);
    int X_perm = access(filename, F_OK);

    if(R_perm == 0)
    {
        printf(ANSI_COLOR "Permitted to read\n" ANSI_COLOR_RESET);
    }
    else if (R_perm == -1 && errno == EACCES)
    {
        printf(ANSI_COLOR "Not permitted to read" ANSI_COLOR_RESET);
    }

    if(W_perm == 0)
    {
        printf(ANSI_COLOR "Permitted to write\n" ANSI_COLOR_RESET);
    }
    else if (W_perm == -1 && errno == EACCES)
    {
        printf(ANSI_COLOR "Not permitted to write\n" ANSI_COLOR_RESET);
    }

    if(X_perm == 0)
    {
        printf(ANSI_COLOR "Permitted to execute\n" ANSI_COLOR_RESET);
    }
    else if (X_perm == -1 && errno == EACCES)
    {
        printf(ANSI_COLOR "Not permitted to execute\n" ANSI_COLOR_RESET);
    }

}

void logo() 
{
	printf ("\n");
	printf ("______        _             \n");
	printf ("| ___ / (_)  | |            \n");
	printf ("| |___   _   | |   ____     \n");
	printf ("|  __/  | |  | |  / = |     \n");
	printf ("| |     | |  | | | ___/     \n");
	printf ("|_|     |_|  |_| |____|     \n");
	printf ("\n");
	printf ("(_)        | |    (_)                           \n");
	printf (" _  _____  | |     _  _____   _   _  ___   ___  \n");
	printf ("| ||  _  | | |    | ||  _  | | | | |  | | | |   \n");
	printf ("| || | | | | |____| || | | | | |_| |   >   <    \n");
	printf ("|_||_| |_| |_____/|_||_| |_| |_____| _|_| |_|_  \n");
	printf ("\n");
}

int main(int argc, char const *argv[])
{
    char ans[6];
    char cmd[20];
    char *ex = "exe";
    char *mk = "mkfile";
    char *ch = "chfile";
    char *rm = "rmfile";
    char *list = "list";
    char *dirch = "chdir";
    char *dirmk = "mkdir";
    char *dirrm = "rmdir";
    char *dirown = "ckown";
    char *end = "stop";
    char *endps = "stopexe";
    char *save = "save";
    char *dirpath = ".";
    char *ownname[100];
    char *pathname[100];
    char *execmd[200];
    char *msg[100];
    char *chmsg[101] = { '\n' };
    char *exmsg[101] = {"./"};
    char* type[20];
    char cwd[MAX_PATH_LENG + 1] = { '\0', };
    char mkcwd[MAX_PATH_LENG + 1] = { '\0', };
    char rmcwd[MAX_PATH_LENG + 1] = { '\0', };
    char buf[MAX_BUF_SIZE + 1] = { '\0', };

    int fd = 0;
    int own = 0;
    int rmresult;
    int cnt;
    int idx;

    ssize_t wsize = 0;

    struct dirent **name;
    struct stat dirfile;
    pid_t pid = 0;

    logo();
    printf("디렉토리 매니저 plus...\n");
    printf("디렉토리 내 파일들 및 디렉토리 정보를 출력합니다.\n");
    sleep(3);

    if((cnt = scandir(dirpath, &name, NULL, alphasort)) == -1)
    {
        fprintf(stderr, "%s 디렉토리 조회 오류: %s\n", dirpath, strerror(errno));
        return 1;
    }

    for (idx = cnt -1; idx >=0; idx--)
    {
        printf(ANSI_COLOR "Name: %s\n" ANSI_COLOR_RESET, name[idx]->d_name);
        lstat(name[idx]->d_name, &dirfile);
        print_filetype(&dirfile);
        print_fileperm(name[idx]->d_name);
        printf(ANSI_COLOR "volume: %dBytes \n" ANSI_COLOR_RESET,  dirfile.st_size);
        puts("");
    }   

    printf("---------------------------------------------------------------------------------------\n");
    printf("디렉토리 정보\n");
    printf("UID: %u, GID: %u\n", dirfile.st_uid, dirfile.st_gid);
    printf("디렉토리 용량\n");
    system("du -sh");
    printf("현재 실행중인 프로세스 PID: %d\n", getpid());
    printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("명령어를 입력해주세요: ");
    gets(ans);

    while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
    {
        printf("다시 입력해주세요: ");
        gets(ans);
    }

    do // 종료 명령어를 받을 때까지 반복 실행
    {

        if(strcmp(ans, ex) == 0)
        {
            printf("실행하고자 하는 명령어나 실행 파일을 입력해주세요: ");
            gets(execmd);
            system(execmd);
            printf("실행을 종료하려면 stopexe를 계속 실행하려면 아무 문자나 입력해주세요: ");
            gets(cmd);

            if(strcmp(cmd, endps) == 0)
            {
                printf("---------------------------------------------------------------------------------------\n");
                printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("명령어를 입력해주세요: ");
                gets(ans);

                 while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
                {
                    printf("다시 입력해주세요: ");
                    gets(ans);
                }
            }
        }

        if(strcmp(ans, mk) == 0) // 파일 생성
        {
            printf("생성할 파일 이름(또는 경로)를 입력해주세요: ");
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

            printf("파일 입력을 중지하려면 save를 입력해주세요: ");
            gets(cmd);

            while(strcmp(cmd, save) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            if(strcmp(cmd, save) == 0)
            {
                close(fd);
                
                printf("---------------------------------------------------------------------------------------\n");
                printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("명령어를 입력해주세요: ");
                gets(ans);

                 while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
                {
                    printf("다시 입력해주세요: ");
                    gets(ans);
                }
            }
        }

        else if(strcmp(ans, ch) == 0) // 파일 수정
        {
            printf("수정할 파일 이름(또는 경로)를 입력해주세요: ");
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

            printf("파일 입력을 중지하려면 save를 입력해주세요: ");
            gets(cmd);

            while(strcmp(cmd, save) != 0)
            {
                printf("다시 입력해주세요: ");
                gets(cmd);
            }

            if(strcmp(cmd, save) == 0)
            {
                close(fd);

                printf("---------------------------------------------------------------------------------------\n");
                printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("명령어를 입력해주세요: ");
                gets(ans);

                 while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
                {
                    printf("다시 입력해주세요: ");
                    gets(ans);
                }
            }
        }

        else if(strcmp(ans, rm) == 0) // 파일 삭제
        {
            printf("디렉토리 내 파일들 및 디렉토리 정보를 출력합니다.\n");

            if((cnt = scandir(dirpath, &name, NULL, alphasort)) == -1)
            {
                fprintf(stderr, "%s 디렉토리 조회 오류: %s\n", dirpath, strerror(errno));
                return 1;
            }

            for (idx = cnt -1; idx >=0; idx--)
            {
                printf(ANSI_COLOR "Name: %s\n" ANSI_COLOR_RESET, name[idx]->d_name);
                lstat(name[idx]->d_name, &dirfile);
                print_filetype(&dirfile);
                print_fileperm(name[idx]->d_name);
                printf(ANSI_COLOR "volume: %dBytes \n" ANSI_COLOR_RESET,  dirfile.st_size);
                puts("");
            }   

            printf("---------------------------------------------------------------------------------------\n");
            printf("삭제할 파일 이름(또는 경로)를 입력해주세요: ");
            gets(pathname);
            rmresult = remove(pathname);

            if (rmresult == 0)
            {
                printf("파일 삭제 성공\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("명령어를 입력해주세요: ");
                gets(ans);

                 while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
                {
                    printf("다시 입력해주세요: ");
                    gets(ans);
                }
            }

            else if (rmresult == -1)
            {
                printf("파일 삭제 실패!!\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
                printf("---------------------------------------------------------------------------------------\n");
                printf("명령어를 입력해주세요: ");
                gets(ans);

                 while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
                {
                    printf("다시 입력해주세요: ");
                    gets(ans);
                }
            }
        }
            
        

        else if(strcmp(ans, list) == 0) // 디렉토리 및 파일 출력
        {
            printf("디렉토리 내 파일들 및 디렉토리 정보를 출력합니다.\n");

            if((cnt = scandir(dirpath, &name, NULL, alphasort)) == -1)
            {
                fprintf(stderr, "%s 디렉토리 조회 오류: %s\n", dirpath, strerror(errno));
                return 1;
            }

            for (idx = cnt -1; idx >=0; idx--)
            {
                printf(ANSI_COLOR "Name: %s\n" ANSI_COLOR_RESET, name[idx]->d_name);
                lstat(name[idx]->d_name, &dirfile);
                print_filetype(&dirfile);
                print_fileperm(name[idx]->d_name);
                printf(ANSI_COLOR "volume: %dBytes \n" ANSI_COLOR_RESET,  dirfile.st_size);
                puts("");
            }   

            printf("---------------------------------------------------------------------------------------\n");
            printf("디렉토리 정보\n");
            printf("UID: %u, GID: %u\n", dirfile.st_uid, dirfile.st_gid);
            printf("디렉토리 용량\n");
            system("du -sh");
            printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
            printf("---------------------------------------------------------------------------------------\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
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
            printf("---------------------------------------------------------------------------------------\n");
            printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
            printf("---------------------------------------------------------------------------------------\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
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

            printf("---------------------------------------------------------------------------------------\n");
            printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
            printf("---------------------------------------------------------------------------------------\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
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

            printf("---------------------------------------------------------------------------------------\n");
            printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
            printf("---------------------------------------------------------------------------------------\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
            {
                printf("다시 입력해주세요: ");
                gets(ans);
            }
        }

        else if (strcmp(ans, dirown) == 0)
        {
            printf("소유권을 확인할 파일 이름을 입력해주세요: ");
            gets(ownname);

            fd = open(ownname, O_RDONLY, PERMS);

            if (fd == -1)
            {
                printf("잘못된 파일 이름(또는 경로)입니다. 파일 이름(또는 경로)를 다시 입력해주세요: ");
                gets(ownname);
            }

            stat(*ownname, &dirfile);
            printf("UID:%u GID:%u\n", dirfile.st_uid, dirfile.st_gid);

            printf("---------------------------------------------------------------------------------------\n");
            printf("exe: 파일 및 명령어 실행\nmkfile: 파일 생성\nchfile: 파일 수정\nrmfile: 파일 삭제\nlist: 디렉토리 내 파일 목록 출력\nchdir: 디렉토리 이동\nmkdir: 디렉토리 생성\nrmdir: 디렉토리 삭제\nckown: 파일 및 디렉토리 소유권 확인\nstop: 프로그램 종료\n");
            printf("---------------------------------------------------------------------------------------\n");
            printf("명령어를 입력해주세요: ");
            gets(ans);

            while (strcmp(ans, ex) !=0 && strcmp(ans, mk) != 0 && strcmp(ans, list) !=0 && strcmp(ans, ch) != 0 && strcmp(ans, rm) != 0 && strcmp(ans, dirmk) != 0 && strcmp(ans, dirch) != 0 && strcmp(ans, dirrm) != 0 && strcmp(ans, end) != 0 && strcmp(ans, dirown))
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