#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/string.h>
#include<linux/delay.h>
//#include<asm/io.h>
//#include<mach/platform.h>
#define print(a)	for(xyz=31;xyz>=0;xyz--)  \
				s[31-xyz] = 48+ (a>>xyz&1); \
				s[32]=0;\
			printk(#a "   %s   0x%x\n",s,a);

//unsigned int *ptr=0;
//unsigned int *status=0;

//#define GPPUD *(status)
/*#define GPFSEL0 *(ptr+0)		//For multifunctonality and output direction
#define GPFSEL1 *(ptr+1)
#define GPFSEL2 *(ptr+2)

#define GPSET0  *(ptr+7)		//To set a particular bit
#define GPSET1  *(ptr+8)	

#define GPCLR0  *(ptr+10)		//To clear a particular bit
#define GPCLR1  *(ptr+11)
#define GPLEV0  *(ptr+13)*/
static int GPCLR0,GPSET0,GPFSEL0,GPFSEL1,GPFSEL2,GPLEV0,xyz,major;
char s[33];
typedef unsigned char U8;
U8 sa,mr,dat;

#define RS (1<<23)
#define EN (1<<24)

#define D0 (1<<26)
#define D1 (1<<19)
#define D2 (1<<13) 
#define D3 (1<< 6)
#define D4 (1<< 5)
#define D5 (1<< 0)
#define D6 (1<<11)
#define D7 (1<< 9)

#define SCL 	(1<<17)
#define SDA 	(1<<27)

////////////////////////////////////////////////////

void i2c_start(void)
{
	GPSET0 |= SCL;
	GPSET0 |= SDA;
	GPCLR0 |= SDA;
}

void i2c_stop(void)
{
	GPCLR0 |= SCL;
	GPCLR0 |= SDA;
	GPSET0 |= SCL;
	GPSET0 |= SDA;
}

void i2c_write(U8 ch)
{
	signed char i;
	for(i=7;i>=0;i--)
	{
		GPCLR0 |= SCL;
		if(ch&(1<<i))
			GPSET0 |= SDA;
		else
			GPCLR0 |= SDA;
		GPSET0 |= SCL;
	}	
	printk("write\n");
}

U8 i2c_read(void)
{
	U8 temp=0;
	signed char i=0;
	for(i=7;i>=0;i--)
	{
		GPSET0 |= SCL;
		if((GPLEV0>>27)&1)
			temp |= 1<<i;
		GPCLR0 |= SCL;
	}
	printk("read\n");
	return temp;
}

void i2c_ack(void)
{
	GPCLR0 |= SCL;
	GPSET0 |= SDA;
	GPSET0 |= SCL;
	while( (GPLEV0>>27)&1);
	GPCLR0 |= SCL;
}

void i2c_noack(void)
{
	GPCLR0 |= SCL;
	GPSET0 |= SDA;	
	GPSET0 |= SCL;
}

void i2c_dev_write(U8 sa,U8 mr,U8 dat)
{
	i2c_start();
	i2c_write(sa);
	i2c_ack();
	i2c_write(mr);
	i2c_ack();
	i2c_write(dat);
	i2c_ack();
	i2c_stop();
	mdelay(10);
}

U8 i2c_dev_read(U8 sa,U8 mr)
{
	U8 temp = 0;
	i2c_start();
	i2c_write(sa);
	i2c_ack();
	i2c_write(mr);
	i2c_ack();
	i2c_start();
	i2c_write(sa|1);
	i2c_ack();
	temp =  i2c_read();
	i2c_noack();	
	i2c_stop();
	return temp;
}


//////////////////////////////////////////////////////

void data(unsigned char a)
{
	int arr[8] = {D0,D1,D2,D3,D4,D5,D6,D7};
	unsigned char i=0;

	printk("\n\nBefore data\n");
	print(GPSET0);
	print(GPCLR0);
	for(i=0;i<8;i++)
	{
		if((a>>i)&1)
			GPSET0 |= arr[i];
		else
			GPCLR0 |= arr[i];		
	}
	printk("\n\nafter data\n");
	print(GPSET0);
	print(GPCLR0);
}

void lcd_cmd(unsigned char a)
{
	printk("------------>Enter lcd_cmd \n");
	data(a);
	GPCLR0 |= RS;		//RS=0
	GPSET0 |= EN;		//EN=0
	mdelay(10);	
	GPCLR0 |= EN;		//EN=1
	print(GPCLR0);
	print(GPSET0);
}

void lcd_data(unsigned char a)
{
	data(a);
	GPSET0 |= RS;		//RS=1
	GPSET0 |= EN;		//EN=0
	mdelay(10);
	GPCLR0 |= EN;		//EN=1
	printk("\n\n");
	print(GPSET0);
	print(GPCLR0);
}

void lcd_init(void)
{
	printk("0x2\n");
	lcd_cmd(0x2);//set cursor to home
	mdelay(20);	

	printk("0x38\n");
	lcd_cmd(0x38);//8 bit mode
	mdelay(20);	

	printk("0xe\n");
	lcd_cmd(0xE);//cursor on
	mdelay(20);	
	
	printk("0x6\n");
	lcd_cmd(0x6);//right shift
	mdelay(20);	

	printk("0x1\n");
	lcd_cmd(0x1);//clear
	mdelay(20);	
	printk("I : Done lcd_init\n");
}

//////////////////////////////////////////////////////////////////

void my_string(char *kbuf)
{
	U8 i,temp=0;	
	int arr[3],count=0;
	for(i=0;kbuf[i];i++)
	{
		if(kbuf[i]==' ')
		{
			count++;
			arr[count-1] = temp;
			temp = 0;	
			continue;
		}
		temp = kbuf[i]-48 + temp*10;	
	}	
	dat = temp;
	sa = arr[0];
	mr = arr[1];
	printk("%d %d %d\n",sa,mr,dat);
}


int my_strlen(char *s)
{
	int i=0;
	for(i=0;s[i];i++);
	return i;
}

int atoi(char *s)
{
	int n=0,i=0;
	if(s[0]=='-')
		i=1;
	for( ;s[i];i++)
		n = s[i] + (n*10);
	if(s[0]=='-')
		n = n*(-1);
	return n;
}

void lcd_string(char * s)
{
	lcd_data(*s);
	s++;
}

//////////////////////////////////////////////////////////////////////////////////////

ssize_t dev_write(struct file *f, const char __user *ubuf, size_t s, loff_t *l)
{
	char kbuf[65];
	printk("---------->dev_write invoked\n");
	copy_from_user(kbuf,ubuf,sizeof(kbuf));

	printk("%s  strlen = %d\n",kbuf,my_strlen(kbuf));
	return 0;
}

ssize_t dev_read(struct file *f, char __user *ubuf, size_t s, loff_t *l)
{
	char kbuf[64]="read_fun not working";
	copy_to_user(ubuf,kbuf,sizeof(kbuf));
	return 0;
}

int dev_open(struct inode *i,struct file *f)
{
	printk("dev_open is invoked\n");
	lcd_data(0xfe);
	return 0;
}

int dev_close(struct inode *i,struct file *f)
{
	printk("dev_close is invoked\n");
	return 0;

}

struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
	.release = dev_close,
	.open = dev_open,
};

///////////////////////////                                               //////////////////////////////////

__init static int i2c_lcd_init(void)
{
	printk("1-->i2c_lcd_init is invoked\n");
	
	major = register_chrdev(0,"my_i2c_LED",&fops);
//	ptr=(unsigned int *)__io_address(GPIO_BASE);
	GPLEV0 = GPFSEL0 = GPFSEL1 = GPFSEL2 = 0;

					/* 	SETTING RS EN AND D0 -7 AS OUTPUT PINS	*/
	GPFSEL2 |= (1<<9)|(1<<12);	//FOR BCM 23,24(RS,EN)

	GPFSEL2 |= (1<<18);		//D0
	GPFSEL1 |= (1<<27);		//D1
	GPFSEL1 |= (1<<9);		//D2
	GPFSEL0 |= (1<<18);		//D3
	GPFSEL0 |= (1<<15);		//D4
	GPFSEL0 |= (1<<0);		//D5
	GPFSEL1 |= (1<<3);		//D6
	GPFSEL0 |= (1<<27);		//D7

	GPFSEL1 |= 1<<21;		//SDA
	GPFSEL2 |= 1<<21;		//SCL

	printk("For Setting Direction...\n");
	print(GPFSEL0);
	print(GPFSEL1);
	print(GPFSEL2);

	print(GPLEV0);
	GPLEV0 = 0;
	print(GPLEV0);

	printk("major = %d\n",major);
	printk("__init done...\n");
	return 0;
}

__exit static void i2c_lcd_exit(void)
{
	printk("i2c_lcd_exit is invoked\n");
	unregister_chrdev(major,"my_i2c_LED");
}


module_init(i2c_lcd_init);
module_exit(i2c_lcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ARAVIND-DHANRAJ");
MODULE_DESCRIPTION("I2C Driver Dhanraj-Aravind");







/*"	if(strlen(kbuf)<2)
	{
		op=atoi(kbuf);
		printk("in if %d\n",op);
	}

	if(op==(0x1||0x2||0x6||0xc||0xe||0xf||0x38||0x28||0x80||0xc0||0xd0||0x94))
		lcd_cmd(op);
	else
		lcd_string(kbuf);		//Problem is LEV register is not 0

*/
/*	if(op==0)
		lcd_string(kbuf); */
		
/*	if(kbuf[0]=='0')			//For lcd_cmd
	{
		for(i=2;kbuf[i];i++)		//Pending
		{
	else if(kbuf[1]=='1')			//For lcd_data
	{
		//Pending
	}*/
////////////////////////////////////////////////////////////////////////////////////
