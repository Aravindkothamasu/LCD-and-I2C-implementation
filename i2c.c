#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/string.h>
#include<linux/delay.h>
#include<asm/io.h>
#include<mach/platform.h>
#define print(a)	for(xyz=31;xyz>=0;xyz--)  \
				s[31-xyz] = 48+ (a>>xyz&1); \
				s[32]=0;\
			printk(#a "   %s   0x%x\n",s,a);

#define GPFSEL0 *(ptr+0)		//For multifunctonality and output direction
#define GPFSEL1 *(ptr+1)
#define GPFSEL2 *(ptr+2)

#define GPSET0  *(ptr+7)		//To set a particular bit
#define GPSET1  *(ptr+8)	

#define GPCLR0  *(ptr+10)		//To clear a particular bit
#define GPCLR1  *(ptr+11)
#define GPLEV0  *(ptr+13)

#define SCL 	(1<<17)
#define SDA 	(1<<27)


//int major,GPLEV0,GPSET0,GPSET1,GPSET2,GPCLR0,GPCLR1,GPFSEL1,GPFSEL2,xyz;
int major,xyz;
typedef unsigned char U8;
U8 sa,mr,data;
char s[33];

void i2c_start(void)
{
	GPSET0 |= SCL;
	GPSET0 |= SDA;
	GPCLR0 |= SDA;
//	print(GPSET0);
//	print(GPCLR0);

	printk("start\n");
	print(GPLEV0);
}

void i2c_stop(void)
{
	GPCLR0 |= SCL;
	GPCLR0 |= SDA;
	GPSET0 |= SCL;
	GPSET0 |= SDA;
/*	print(GPCLR0);
	print(GPSET0);*/
	printk("stop\n");
	print(GPLEV0);
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
	/*	print(GPSET0);
		print(GPCLR0);		*/
	}	
	printk("write\n");
	print(GPLEV0);
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
	printk("read %d\n",temp);
	print(GPLEV0);
	return temp;
}

void i2c_ack(void)
{
	GPCLR0 |= SCL;
	GPSET0 |= SDA;
	GPSET0 |= SCL;
	while( (GPLEV0>>27)&1);
	GPCLR0 |= SCL;
/*	print(GPCLR0);
	print(GPSET0);*/
	printk("ack\n");
	print(GPLEV0);
}

void i2c_noack(void)
{
	GPCLR0 |= SCL;
	GPSET0 |= SDA;	
	GPSET0 |= SCL;
/*	print(GPCLR0);	
	print(GPSET0);*/
	printk("no-ack\n");
	print(GPLEV0);
}

void i2c_dev_write(U8 sa,U8 mr,U8 data)
{
	i2c_start();
	i2c_write(sa);
	i2c_ack();
	i2c_write(mr);
	i2c_ack();
	i2c_write(data);
	i2c_ack();
	i2c_stop();
	mdelay(10);
	printk("device_write\n");
	print(GPLEV0);
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
	printk("device_read \n");
	print(GPLEV0);
	return temp;
}

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
	data = temp;
	sa = arr[0];
	mr = arr[1];
	printk("%d %d %d\n",sa,mr,data);
}

ssize_t dev_write(struct file *f,const char __user *ubuf,size_t s,loff_t *l)
{
	char kbuf[64];
	printk("In dev->write invoked\n");
	if(copy_from_user(kbuf,ubuf,sizeof(kbuf)))
		return -1;
//	my_string(kbuf);
//	i2c_write(0xff);
	return 0;
}

ssize_t dev_read(struct file *f,char __user *ubuf,size_t s,loff_t *l)
{
	char kbuf[64] = "ARAVIND KOTHAMASU";
	printk("dev->read invoked\n");
	if(copy_to_user(ubuf,kbuf,sizeof(kbuf)))
		return -1;
	return 0;
}

int dev_open(struct inode *i,struct file *f)
{
	printk("dev->open invoked\n");
	GPFSEL1 |= 1<<21;
	GPFSEL2 |= 1<<21;
	print(GPFSEL1);
	print(GPFSEL2);
	print(GPLEV0);
	i2c_device_write(
	return 0;
}

int dev_close(struct inode *i,struct file *f)
{
	printk("dev->close invoked\n");
	return 0;
}


struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
	.release = dev_close,
	.open = dev_open,
};

__init  static int iiiii2c_init(void)
{
	printk("iiiii2c->init is invoked\n");
	major = register_chrdev(0,"i2c",&fops);
	printk("major  = %d\n",major);
	return 0;
}

__exit static void iiiii2c_exit(void)
{
	printk("iiiii2c_exit is invoked\n");
	unregister_chrdev(major,"i2c");
}

module_init(iiiii2c_init);
module_exit(iiiii2c_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("DHANRAJ ARAVIND");
MODULE_DESCRIPTION("I2C driver");
