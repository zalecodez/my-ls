#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>


void listDir(char*, bool, bool);
void listFile(char*, bool, bool);
void generatePermissionString(bool*, bool, char*);
void getPermissions(struct stat, bool*, char*);
void getUserName(struct stat, char*);
void getGroupName(struct stat, char*);
void getModTime(struct stat, char*);

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
					listFile(file, listAll, listLong);
				}
			}
		}
	}
}

void generatePermissionString(bool *permissionList, bool isDir, char *permissionString){

	permissionString[0] = isDir ? 'd' : '-';

	permissionString[1] = permissionList[0] ? 'r' : '-';
	permissionString[2] = permissionList[1] ? 'w' : '-';
	permissionString[3] = permissionList[2] ? 'x' : '-';

	permissionString[4] = permissionList[3] ? 'r' : '-';
	permissionString[5] = permissionList[4] ? 'w' : '-';
	permissionString[6] = permissionList[5] ? 'x' : '-';

	permissionString[7] = permissionList[6] ? 'r' : '-';
	permissionString[8] = permissionList[7] ? 'w' : '-';
	permissionString[9] = permissionList[8] ? 'x' : '-';

	permissionString[10] = '\0';
}

void getPermissions(struct stat sb, bool* permissionList, char* permissionString){
	permissionList[0] = (sb.st_mode & S_IRUSR) == S_IRUSR;
	permissionList[1] = (sb.st_mode & S_IWUSR) == S_IWUSR;
	permissionList[2] = (sb.st_mode & S_IXUSR) == S_IXUSR;

	permissionList[3] = (sb.st_mode & S_IRGRP) == S_IRGRP;
	permissionList[4] = (sb.st_mode & S_IWGRP) == S_IWGRP;
	permissionList[5] = (sb.st_mode & S_IXGRP) == S_IXGRP;

	permissionList[6] = (sb.st_mode & S_IROTH) == S_IROTH;
	permissionList[7] = (sb.st_mode & S_IWOTH) == S_IWOTH;
	permissionList[8] = (sb.st_mode & S_IXOTH) == S_IXOTH;

	generatePermissionString(permissionList, S_ISDIR(sb.st_mode), permissionString);	
}
void listFile(char* filename, bool listAll, bool listLong){
	struct stat sb;
	bool permissionList[9];
	char permissionString[11]; 

	char username[256], groupname[256];

	char timestring[20];

	if(listLong){
		stat(filename, &sb);

		getPermissions(sb, permissionList, permissionString);

		getUserName(sb, username);
		getGroupName(sb, groupname);

		getModTime(sb, timestring);

		printf("%s %i %s %s %i %s %s\n",permissionString, (int)sb.st_nlink, username, groupname, (int)sb.st_size, timestring, filename);
	}
	else{
		printf("%s\n",filename);
	}
}

void getModTime(struct stat sb, char* timestring){

	struct tm  *timestamp;

	timestamp = localtime((time_t*)&sb.st_mtim);

	strftime(timestring, 20, "%b %d %R", timestamp);
}

void getGroupName(struct stat sb, char* groupname){
	struct group *groupInfo;

	groupInfo = getgrgid(sb.st_gid);	

	strcpy(groupname, groupInfo->gr_name);
}


void getUserName(struct stat sb, char* username){
	struct passwd *userInfo;

	userInfo = getpwuid(sb.st_uid);	
	strcpy(username, userInfo->pw_name);
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
					listFile(dirEntry->d_name, listAll, listLong);
					//printf("%s\n", dirEntry->d_name);
				}
			}else{
				listFile(dirEntry->d_name, listAll, listLong);
				//printf("%s\n", dirEntry->d_name);
			}
		}
	}
}
