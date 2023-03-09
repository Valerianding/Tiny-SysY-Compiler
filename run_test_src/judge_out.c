#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* load_sourcefile(char* path) {
  char * buffer = 0;
  long length;
  FILE * fp = fopen (path, "r");
  if (fp)
  {
    fseek (fp, 0, SEEK_END);
    length = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    buffer = malloc (length);
    if (buffer) {
      fread (buffer, 1, length, fp);
    }
    fclose (fp);
  }
  return buffer;
}

int main(int argc , char * argv[])
{
    char * exp_str;
    char * act_str;
    
    exp_str = load_sourcefile(argv[1]);
    act_str = load_sourcefile(argv[2]);

    if(strcmp(exp_str, act_str)==0)  return 1;
    return 0;

}