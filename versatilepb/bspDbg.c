void bspDbg(const char *fmt, ...);
void printcom_versatilepb(char * str)
{
    int i=0;
    while(str[i]!=0)
    {
       if(((*(volatile int *)0x101f1018) & 0x80) == 0x80)
        *(volatile int *)0x101f1000 = str[i++];
    }
}
extern int vsprintf
    (
    char *    buffer,   /* buffer to write to */
    const char *  fmt,      /* format string */
    va_list   vaList    /* optional arguments to format */
    );
static char line[1000];

#define PRINTCOM printcom_versatilepb

void bspDbg(const char *fmt, ...)
    {
    va_list args;
    int s;
   
    s=intLock();
    va_start(args, fmt);
    memset(&line[0],0,1000);
    vsprintf(line, fmt, args);
    va_end(args);
    PRINTCOM(line);
    intUnlock(s);
    return;
    }
