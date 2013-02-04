#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <ar.h>
#include <dirent.h>

#define STR_SIZE sizeof("rwxrwxrwx")
#define FP_SPECIAL 0
#define BLOCKSIZE 1024



void table_cont(int argc, char *argv[], int flag);
void append_files(int argc, char *argv[], int flag, int arch_fd);
void append_archive(int argc, char *argv[]);
int file_position(int argc, char *argv[], int *arch_fd, struct ar_hdr *v);
void extract_file(int argc, char *argv[]);
void delete_files(int argc, char *argv[]);
char **dir_args(char argc, char *argv[]);
int dir_count;

int main(int argc, char *argv[])
{
    int i;
    int opt;
    int flag;

    if((opt = getopt(argc, argv, "vtqxdA")) != -1) {
        switch (opt) {
            case 't':
                printf("flag %s has been set \n",argv[1]);
                table_cont(argc, argv, 0);
                break;
            case 'v':
                printf("flag %s has been set \n",argv[1]);
                table_cont(argc, argv, 1);
                break;
            case 'q':
                printf("flag %s has been set \n",argv[1]);
                append_archive(argc, argv);
                break;
            case 'x':
                extract_file(argc, argv);
                break;
            case 'd':
                printf("flag %s has been set \n",argv[1]);
                delete_files(argc, argv);
                break;
            case 'A':
                printf("flag %s has been set \n",argv[1]);
                append_archive(dir_count, dir_args(argc, argv) );
                break;
            default: /* '?' */
                fprintf(stderr, "No option exisits\n");
                exit(EXIT_FAILURE);
        }
    }
    else{
        fprintf(stderr, "Selected option that is not supported by this program\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

char * file_perm_string(mode_t perm, int flags)
{
	static char str[STR_SIZE];
	snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	         (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
	         (perm & S_IXUSR) ?
	         (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	         (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	         (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
	         (perm & S_IXGRP) ?
	         (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	         (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	         (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
	         (perm & S_IXOTH) ?
	         (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
	         (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
	return str;
}

void table_cont(int argc, char *argv[], int flag)
{
    char outstr_buff[20];
    struct ar_hdr buffer;
	int fd;

	if((fd = open(argv[2], O_RDONLY)) == -1){
		printf("could not open file or does not exisit, Error: %d\n", errno);
		exit(errno);
	}
	else{
		printf("opening '%s' was sucessful\n", argv[2]);

        if(lseek(fd, SARMAG, SEEK_SET) == -1){
            printf("could not lseek set past Error: %d\n", errno);
            exit(errno);
        }
        else{
            while(read(fd, &buffer, 60) != 0){
                time_t rawtime = atoi(buffer.ar_date);
                struct tm *newtime;

                newtime = localtime(&rawtime);
                strftime(outstr_buff, 20, "%b %e %R %Y" ,newtime);
                if (flag == 1){
                    printf("%s %d/%d %d \t %s %.*s\n", file_perm_string(strtol(buffer.ar_mode, NULL, 8), 1),atoi(buffer.ar_uid),atoi(buffer.ar_gid),atoi(buffer.ar_size), outstr_buff, 16, buffer.ar_name);
                }
                else{
                    printf("%.*s\n", 16, buffer.ar_name);
                }
                lseek(fd, atoi(buffer.ar_size), SEEK_CUR);
            }
        }
	}
	if((close(fd) == -1)){
		printf("Could not close fd%d:, Error: %d\n",fd, errno);
		exit(errno);
	}
    else{
        printf("file fd:%d closed sucessfully\n",fd);
    }
}

void append_archive(int argc, char *argv[])
{
    int arch_fd;
    int create_flag;

    if((arch_fd = open(argv[2], O_APPEND | O_WRONLY, 00666)) == -1){
        if(ENOENT){
            arch_fd = open(argv[2], O_CREAT | O_APPEND | O_WRONLY, 00666);
            create_flag = 1;
            printf("Archive by the name of '%s' does not exist, let make it for you!\n", argv[2]);
            append_files(argc, argv, create_flag, arch_fd);
            close(arch_fd);
        }
        else{
            printf("could not open or create archive '%s', Error: %d\n", argv[2], errno);
            exit(errno);
        }
    }
    else{
        printf("opening archive '%s' was sucessful with fd:%d\n", argv[2], arch_fd);
        create_flag = 0;
        append_files(argc, argv, create_flag, arch_fd);
        close(arch_fd);
    }
}


void append_files(int argc, char *argv[], int flag, int arch_fd)
{
    int file_fd;
    int i;
    int n;
    struct stat file_stat;
    char hdr_buff[59];
    char filename[16];
    char read_buff[BLOCKSIZE];
    int total_bytes_read;


    if(flag){
        write(arch_fd, ARMAG, SARMAG);
    }
    for(i = 3; i < argc; i++){
        if((file_fd = open(argv[i], O_RDONLY)) == -1 | sizeof(argv[i]) >= 16){
            printf("could not append file '%s', length of filename is either too big or there was an error. Goodbye! \n", argv[i]);
            exit(errno);
        }
        else{
            printf("opening file '%s' was sucessful with fd:%d\n", argv[i], file_fd);
            stat(argv[i], &file_stat);
            strcpy(filename, argv[i]);
            snprintf(hdr_buff, 59, "%-''*s%-''*d%-''*d%-''*d%-''*o%-''*d", 16, strcat(filename, "/"), 12, (int)file_stat.st_mtime, 6, file_stat.st_uid, 6, file_stat.st_gid, 8, file_stat.st_mode, 10, (int)file_stat.st_size);
            write(arch_fd, hdr_buff, 58);
            write(arch_fd, &ARFMAG, 2);

            while((n = read(file_fd, read_buff, BLOCKSIZE)) != 0){
                if((write(arch_fd, read_buff, n) == -1)){
                    printf("Error (%d): Could not write file '%s'\n", errno, argv[i]);
                    exit(errno);
                }
                total_bytes_read += n;
            }
            printf("%d total bytes written for file '%s'\n", total_bytes_read, argv[i]);

        }
        close(file_fd);
    }
}

int file_position(int argc, char *argv[], int *arch_fd, struct ar_hdr *header)
{
    int i;
    int j;

    for (i = 0; i < 16; i++){
        if(header->ar_name[i] == '/'){
            header->ar_name[i] = '\0';
        }
    }
    for (j = 0; j < argc; j++){
        if((strcmp(header->ar_name, argv[j]) == 0)){
            return 0;
        }

    }
    return -1;
}

void extract_file(int argc, char *argv[])
{
    int arch_fd;
    int file_fd;
    int bytes_read;
    struct ar_hdr header;
    char read_buff[BLOCKSIZE];
    struct stat file_stat;
    struct utimbuf timestamp;

    if((arch_fd = open(argv[2], O_RDONLY)) == -1){
        printf("could not open file or does not exisit, Error: %d\n", errno);
		exit(errno);
	}
    else{
        printf("opening '%s' was sucessful\n", argv[2]);
		lseek(arch_fd, SARMAG, SEEK_SET);
        while((read(arch_fd, &header, 60)) != 0){

            if(file_position(argc, argv, &arch_fd, &header) == 0){
                if((file_fd = open(header.ar_name, O_CREAT | O_WRONLY | O_EXCL, strtol(header.ar_mode, NULL, 8))) == -1){
                    if(EEXIST){
                        printf("'%s' aready exisits, cannot extract archive!\n", header.ar_name);
                        exit(errno);
                    }
                    else if(EACCES){
                        printf("Set permissions not allowed!\n");
                        exit(errno);
                    }
                    else{
                        printf("Error(%d), Could not open file\n", errno);
                        exit(errno);
                    }

                }
                else{
                    fstat(file_fd, &file_stat);
                    timestamp.actime = atoi(header.ar_date);
                    timestamp.modtime = atoi(header.ar_date);
                    utime(header.ar_name, &timestamp);

                    int total_bytes_read = 0;
                    while(total_bytes_read < atoi(header.ar_size)){
                        if((total_bytes_read + BLOCKSIZE) > atoi(header.ar_size)){
                            bytes_read = read(arch_fd, read_buff, (atoi(header.ar_size) - total_bytes_read));
                            total_bytes_read += bytes_read;
                            write(file_fd, read_buff, bytes_read);
                        }
                        else{
                            bytes_read = read(arch_fd, read_buff, BLOCKSIZE);
                            total_bytes_read += bytes_read;
                            write(file_fd, read_buff, BLOCKSIZE);
                        }
                    }

                }
            }
            else{
                printf("No match for current argument, moving on to next header\n");
                lseek(arch_fd, atoi(header.ar_size), SEEK_CUR);
            }

        }
    close(file_fd);
    close(arch_fd);
    }
}

void delete_files(int argc, char *argv[])
{
    int old_arch_fd;
    int new_arch_fd;
    struct stat old_arch_stat;
    struct ar_hdr header;
    struct ar_hdr org_header;
    int bytes_read;
    int total_bytes_read = 0;
    char read_buff[BLOCKSIZE];

    if((old_arch_fd = open(argv[2], O_RDONLY)) == -1){
        printf("could not open file or does not exisit, Error: %d\n", errno);
		exit(errno);
	}
    else{
        printf("opening '%s' was sucessful\n", argv[2]);
        fstat(old_arch_fd, &old_arch_stat);
        unlink(argv[2]);
		if((new_arch_fd = open(argv[2], O_CREAT | O_WRONLY | O_EXCL | O_APPEND, old_arch_stat.st_mode)) == -1){
            if(EEXIST){
                printf("cannot create temporary archive, it already exisits!\n");
                exit(errno);
            }
            else if(EACCES){
                printf("Set permissions not allowed!\n");
                exit(errno);
            }
            else{
                printf("Error(%d), Could not open archive\n", errno);
                exit(errno);
            }

        }
        else{
            write(new_arch_fd, ARMAG ,SARMAG);
            lseek(old_arch_fd, SARMAG, SEEK_SET);
            while((read(old_arch_fd, &header, 60)) != 0){
                org_header = header;
                if(file_position(argc, argv, &old_arch_fd, &header) == 0){
                    lseek(old_arch_fd, atoi(header.ar_size), SEEK_CUR);
                }
                else{
                    write(new_arch_fd, &org_header, 60);

                    int total_bytes_read = 0;
                    while(total_bytes_read < atoi(header.ar_size)){
                        if((total_bytes_read + BLOCKSIZE) > atoi(header.ar_size)){
                            bytes_read = read(old_arch_fd, read_buff, (atoi(header.ar_size) - total_bytes_read));
                            total_bytes_read += bytes_read;
                            write(new_arch_fd, read_buff, bytes_read);
                        }
                        else{
                            bytes_read = read(old_arch_fd, read_buff, BLOCKSIZE);
                            total_bytes_read += bytes_read;
                            write(new_arch_fd, read_buff, BLOCKSIZE);
                        }
                    }
                }
            }
        }
    close(old_arch_fd);
    close(new_arch_fd);
    }
}

char** dir_args(char argc, char *argv[]){
    char cwd_buffer[255];
    static char *filenames[100];
    struct dirent *dir_ptr;
    int i = 3;
    char dirlist[20];
    struct stat statbuf;

    getcwd(cwd_buffer, 255);
    DIR *dir = opendir(cwd_buffer);
    filenames[2] = argv[2];
    while((dir_ptr = readdir(dir)) != NULL){
        char *tmp = dir_ptr->d_name;
        stat(tmp, &statbuf);
        if((statbuf.st_mode & S_IFMT) == S_IFREG){
            filenames[i] = dir_ptr->d_name;
            i++;
            printf("name of file = %s\n", filenames[i]);
        }
    }

    dir_count = i;

    return filenames;
}


