
obj-m= i2c.o
#obj-m= char3.o
#obj-m= hello.o
#obj-m= lcd1.o
#obj-m= leddd.o
#obj-m= pendrive.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	#make -C /home/iot/Desktop/buildroot_compiles/buildroot-2017.02/output/build/linux-4.4.43-v7 M=$(PWD) modules



clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	#make -C /home/iot/Desktop/buildroot_compiles/buildroot-2017.02/output/build/linux-4.4.43-v7 M=$(PWD) clean


