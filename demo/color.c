#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define USERNAME_LEN 30
#define MAX_USER_COUNT 20

struct COLOR{
    char color[USERNAME_LEN];
};

struct COLOR Color[MAX_USER_COUNT];
int userItemCount = 0;


enum CHECK_COLOR_RESULT{
    CHECK_NO_COLOR, 
    CHECK_OK 
};


void numcolor(){
  strcpy(Color[0].color, "red");
  strcpy(Color[1].color, "orange");
  strcpy(Color[2].color, "yellow");
  strcpy(Color[3].color, "green");
  strcpy(Color[4].color, "cyan");
  strcpy(Color[5].color, "blue");
  strcpy(Color[6].color, "purple");
  userItemCount = 7;
}

void checkColor(char* anycolor){
    enum CHECK_COLOR_RESULT result;
    printf("%s","<HTML>\r\n");
    printf("%s","<TITLE>Index</TITLE>\r\n");
    printf("%s","<BODY>\r\n");
    printf("%s","<P>CHECK_COLOR\r\n");
    printf("%s","<H1>CGI demo\r\n");
    if(anycolor==NULL){
        result = CHECK_NO_COLOR;
    }else{
        int i = 0;
        for(; i < userItemCount; ++i){
            if(strcasecmp(Color[i].color,anycolor)==0){
                result = CHECK_OK; break;
            }
            else{
                 result = CHECK_NO_COLOR;
            }
        }
        if(i==userItemCount){
            result = CHECK_NO_COLOR;
        }
    }
    switch(result){
        case CHECK_NO_COLOR:
            printf("<H1>这不是个颜色%s\r\n",anycolor);
            break;
        case CHECK_OK:
            printf("<H1>这是个颜色%s\r\n",anycolor);
            break;
    }
    printf("%s\n","</body>");
    printf("%s\n","</html>");
}

void main(int argc, char* argv[]){
    numcolor();
    // argv[1] = "color=red";argc=2;
    if(argc <2){
        checkColor(NULL);
    }else{
        int queryStringLen = strlen(argv[1]);
        char* queryString = (char*)malloc(queryStringLen+1);
        memcpy(queryString,argv[1],queryStringLen);
        queryString[queryStringLen] = '\0';

        char colorname[USERNAME_LEN];
	memset(colorname,0,sizeof(colorname));
      
        sscanf(queryString,"color=%s",colorname);

        checkColor(colorname);
        free(queryString);
    }
    return;
}
