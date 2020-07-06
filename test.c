#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	int ch;
	char data[1024];
	int i=0;
	while ((ch=getchar())>=0){
		data[i++]=ch;
	}


	char* nbt = data;
	if (nbt[0]!=10 || nbt[1]!=0 || nbt[2]!=0){
		printf("Wrong!\n");
		return -1;
	}
	char* signTextRows[4];
	for (int i=0; i<4; i++)signTextRows[i]=NULL;
	size_t pos=3;
	size_t total_len=0;
	while(pos<i){
		int tagType = nbt[pos];
		printf("TagType %d\n", tagType);
		if (tagType==0){
			break;
		}
		if (nbt[pos+1]!=0){
			printf("Wrong 1\n");
			return -1;
		}
		int namelen = nbt[pos+2];
		if (namelen < 0) namelen += 256;
		char name[512];
		memcpy(name, &nbt[pos+3], namelen);
		name[namelen] = 0;
		pos += 3 + namelen;
		printf("Tag name: %s\n", name);
		if (!strcmp(name, "x") || !strcmp(name, "y") || !strcmp(name, "z")) {
			if (tagType!=3){
				printf("Wrong2\n");
				return -1;
			}
			pos += 4;
			continue;
		}
		// "Text1", "Text2", "Text3", "Text4" are the possibles
		if (tagType == 8) {
			if (nbt[pos]!=0){
				printf("Wrong3\n");
				return -1;
			}
			pos++;
			int linelen = nbt[pos];
			if (linelen < 0) linelen += 256;
			printf("Line len: %d\n", linelen);
			pos++;
			char* data = malloc(linelen+1);
			memcpy(data, &nbt[pos], linelen);
			data[linelen]=0;
			total_len+=linelen;
			pos+=linelen;
			if (namelen==5 && !strncmp(name, "Text", 4)){
				int signTextRow = name[4] - '1';
				if (signTextRow<0 || signTextRow>=4){
					printf("Wrong4\n");
					return -1;
				}
				printf("Sign text row %d is %s\n", signTextRow, data);
				signTextRows[signTextRow] = data;
			}else{
				printf("%s is %s\n", name, data);
			}
			continue;
		}
		printf("Wrong5\n");
		return -1;
	}
	char* ret = malloc(total_len+6); // +6 for [ , , , ] NULL
	ret[0]=0;
	strcat(ret, "[");
	for (int i=0; i<4; i++){
		if (!signTextRows[i]){
			printf("Wrong6\n");
			return -1;
		}
		strcat(ret, signTextRows[i]);
		if (i!=3) strcat(ret, ",");
	}
	strcat(ret, "]");
	printf("Final: %s\n", ret);
}

