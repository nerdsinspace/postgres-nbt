#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(nbt_decode);
Datum
nbt_decode(PG_FUNCTION_ARGS)
{
	bytea *nbtdata = PG_GETARG_BYTEA_P(0);
	int datalen = VARSIZE(nbtdata) - VARHDRSZ; // "variable header size" aka "length of the body not including the header that defines said length"
	char* nbt = (char*)VARDATA(nbtdata);
	if (nbt[0]!=10 || nbt[1]!=0 || nbt[2]!=0){
		PG_RETURN_TEXT_P(cstring_to_text("Malformed 1"));
	}
	char* signTextRows[4];
	for (int i=0; i<4; i++)signTextRows[i]=NULL;
	size_t pos=3;
	size_t total_len=0;
	while(pos<datalen){
		int tagType = nbt[pos];
		if (tagType==0){
			break;
		}
		if (nbt[pos+1]!=0){
			PG_RETURN_TEXT_P(cstring_to_text("Malformed 2"));
		}
		int namelen = nbt[pos+2];
		char name[512];
		memcpy(name, &nbt[pos+3], namelen);
		name[namelen] = 0;
		pos += 3 + namelen;
		if (!strcmp(name, "x") || !strcmp(name, "y") || !strcmp(name, "z")) {
			if (tagType!=3){
				PG_RETURN_TEXT_P(cstring_to_text("Malformed 3"));
			}
			pos += 4;
			continue;
		}
		if (tagType == 8) {
			if (nbt[pos]!=0){
				PG_RETURN_TEXT_P(cstring_to_text("Malformed 4"));
			}
			pos++;
			int linelen = nbt[pos];
			if (linelen < 0) linelen += 256; // deal with unsigned / signed
			pos++;
			char* data = palloc(linelen+1);
			memcpy(data, &nbt[pos], linelen);
			data[linelen]=0;
			total_len+=linelen;
			pos+=linelen;
			if (namelen==5 && !strncmp(name, "Text", 4)){
				int signTextRow = name[4] - '1';
				if (signTextRow<0 || signTextRow>=4){
					PG_RETURN_TEXT_P(cstring_to_text("Malformed 5"));
				}
				signTextRows[signTextRow] = data;
			}else{
				if (strcmp(name, "id") || strcmp(data, "minecraft:sign")){
					PG_RETURN_TEXT_P(cstring_to_text("Malformed 6"));
				}
			}
			continue;
		}
		PG_RETURN_TEXT_P(cstring_to_text("Malformed 7"));
	}
	char* ret = palloc(total_len+6); // +6 for [ , , , ] NULL
	ret[0]=0;
	strcat(ret, "[");
	for (int i=0; i<4; i++){
		if (!signTextRows[i]){
			PG_RETURN_TEXT_P(cstring_to_text("Malformed 8"));
		}
		strcat(ret, signTextRows[i]);
		if (i!=3) strcat(ret, ",");
	}
	strcat(ret, "]");
	PG_RETURN_TEXT_P(cstring_to_text(ret));
}

