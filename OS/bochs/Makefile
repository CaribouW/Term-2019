all:
	nasm -f macho64 basic.asm -o basic.o 
	ld  basic.o -o main -macosx_version_min 10.13 -lSystem

plus:
	nasm -f macho64 plus.asm -o plus.o    
	ld  plus.o -o main -macosx_version_min 10.13 -lSystem

image:
	nasm helloworld.asm -o helloworld.bin
	bximage
	dd if=helloworld.bin of=a.img bs=512 count=1 conv=notrunc
	# bochs –f bochssrc
debug:
	nasm -f macho64 -g plus.asm -o plus.o
	ld plus.o -o main -macosx_version_min 10.13 -lSystem

clean:
	rm -rf *.o *.bin *.img main