all:
	nasm helloworld.asm -o helloworld.bin
	bximage
	dd if=helloworld.bin of=a.img bs=512 count=1 conv=notrunc
	# bochs -f bochssrc

clean:
	rm -rf *.img *.bin *.o