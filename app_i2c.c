#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>

void function(char *);
int fd,op=54;
int sa=0,mr=0,data=0;

void main()
{	
	char s[20];
	fd = open("/dev/i2c",O_RDWR);
	if(fd <= 0)
	{
		perror("open");
		return ;
	}
	function(s);
	write(fd,s,strlen(s));
}

void function(char *s)
{
	printf("1,dev_write\t2,dev_read\n");
	scanf(" %d",&op);
	
	printf("Plz enter in Hex-decimal\n\nEnter slave addr\n");
	scanf(" %x",&sa);

	printf("Enter m/r\n");
	scanf(" %x",&mr);

	bzero(s,sizeof(s));
	if(op==1)
	{
		printf("Enter data\n");
		scanf(" %x",&data);
		sprintf(s,"%d %d %d",sa,mr,data);
	}
	else if(op==2)
		sprintf(s,"%d %d ",sa,mr);
	else
	{
		printf("Wrong \n");
		exit(0);
	}
	printf("s = %s\n",s);
}	
