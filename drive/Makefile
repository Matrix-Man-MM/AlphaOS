all: build

build:
	gcc drive.c -o drive
	./drive
	genext2fs -d ramdisk -q -b 1024 initrd

clean:
	rm drive drive.hdd initrd
