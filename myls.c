#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

void listDir(char*, bool, bool);
void listFile(char*, bool, bool);

int main(int argc, char* argv[]){

	struct stat sb;
	int opt, optcount=0;
	bool listAll = false, listLong = false; 

	while((opt = getopt(argc, argv, "al")) != -1){
		optcount++;

		switch(opt){
			case('a'):
				listAll = true;
				break;
			case('l'):
				listLong = true;
				break;
			default:
				break;
		}
	}

	if(argc - optcount == 1){
		listDir(".", listAll, listLong);
	}
	else{
		for(int i = 1; i < argc; i++){
			char* file = argv[i];
			if(file[0] == '-'){
				continue;
			}
			if(stat(file, &sb) == -1){
				perror(file);
			}
			else{
				if((sb.st_mode & S_IFMT) == S_IFDIR){
					printf("%s:\n",file);
					listDir(file, listAll, listLong);
					printf("\n");
				}
				else{
					printf("%s/n",file);
				}
			}
		}
	}
}

void listFile(char* filename, bool listAll, bool listLong){
	if(listLong){

	}
	else{

	}
}

void listDir(char* dirname, bool listAll, bool listLong){
	DIR* dirp;
	struct dirent *dirEntry;

	if((dirp = opendir(dirname)) == NULL){
		perror(dirname);
	}
	else{
		while((dirEntry = readdir(dirp)) != NULL){
			char* filename = dirEntry->d_name;
			if(filename[0] == '.'){
				if(listAll){
					printf("%s\n", dirEntry->d_name);
				}
			}else{
				printf("%s\n", dirEntry->d_name);
			}
		}
	}
}
