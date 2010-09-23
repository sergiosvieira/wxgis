DESTDIR =

all: 
	make -C ./build/make/ -f makefile

clean:
	make -C ./build/make/ -f makefile clean	

install: 
	make -C ./build/make/ -f makefile install

uninstall:
	make -C ./build/make/ -f makefile uninstall

install_head:
	make -C ./build/make/ -f makefile install_head

.PHONY: clean
