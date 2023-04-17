#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_FILE_SIZE 1024*1024
char* read_file(const char* filename) {

    /* 打开文件，读取内容到缓冲区 */
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "无法打开文件 %s\n", filename);
        return NULL;
    }

    char* buffer = (char*)malloc(MAX_FILE_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "内存不足，无法读取文件 %s\n", filename);
        fclose(fp);
        return NULL;
    }

    size_t len = fread(buffer, 1, MAX_FILE_SIZE, fp);
    fclose(fp);

    /* 在缓冲区末尾添加空字符 '\0'，以便将其作为 C 字符串使用 */
    buffer[len] = '\0';

    return buffer;
}

char * delet_lf(char * str)
{
  int len=strlen(str);
  int ljw=0;
  char * ret=(char *)malloc(len);
  for(int i=0;i<len;i++)
  {
    if(str[i]=='\n')
    {
      continue;
    }
    else
    {
      ret[ljw]=str[i];
      ljw++;
    }
  }
  ret[ljw]=0;
  
  free(str);
  return ret;
}

int main(int argc , char * argv[])
{
    char * exp_str;
    char * act_str;
    
    exp_str = read_file(argv[1]);
    act_str = read_file(argv[2]);
    exp_str = delet_lf(exp_str);
    act_str = delet_lf(act_str);
    if(strcmp(exp_str, act_str)==0)  return 1;
    return 0;

}