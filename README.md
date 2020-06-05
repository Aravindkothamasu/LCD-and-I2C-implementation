/***********************************************************************************************
/*			LCD and I2C Driver Code Implementaton  				      */
/**********************************************************************************************/


1, 16*2 LCD 
2, I2C (Using Bit-Banging method)

Using both LCD and I2C Driver code implemented seperately as well as combined.

 - Code has Written in c language
 - Using constructor and destructor ,it will start the driver code execution.
 - In Linux Platform (Ubuntu) implemented
 - using structure file operations ,feeded the all the fuctions like read,write,open,close etc.
 - when ever ,we do any operation in the application code ,that particular operation is to be reflected in the driver code
 - Driver code is represented in (.ko) format,it is inserted into kernel space command called  " insmod _____.ko "
 - We can check ,whether it is inserted or not by doing "lsmod | head" means all the driver modules that are running in the kernel space ,display on command prompt,that to top 10
 - If you want to remove module or driver "rmmod ____.ko"
 - Once you inserted you can see the constructor called function is executed as well as destructor also.
 - Basically it is a character special file.
 - After inserting the charcter special file you have to register the character special file ,through a specific function called " register_chrdev() ", This function gives the major and minor numbering.
 - From that number,applicaion program able to communicate with kernel module.
 - This driver code written for Raspberry pi 3 ,it consists of controller (BCM 2835) ,processor ARM Cortex A-53.
 - We cannot see the kernel messages directly,By using command called "dmesg".
 - If you want to clear dmesg's "sudo dmesg -C"
 - According to the application program ,kernel module will behave all together the system calls of application should register into the structure file operations function pointer members.
 - All these kernel modules directly communicate with I/O controller ,these I/O controller will connect with pins 
 - For any more information refer BCM 2835 data sheet and raspberry pi pin description https://pinout.xyz/



------------> Done by 
		Aravind Kothamasu
		aravindkothamasu@gmail.com

