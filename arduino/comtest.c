#include <stdio.h>
#include <time.h>
int main()
{   
    char dataText[] = "10,10,1;5,5,2;6,7,3";
    int data[] = {10,5,13};  //Random data we want to send
    FILE *file;
    file = fopen("/dev/ttyACM0","w");  //Opening device file
    int i = 0;
    // for(i = 0 ; i < 3 ; i++)
    // {
        fprintf(file,"%s\n",dataText); //Writing to the file
        sleep(1);
    //}
    fclose(file);
}