CC	= gcc
CCFLAGS = 

all:	$(OBJS)
	@echo Initializing bouncer compilation
	@echo [*] Running Conversion Tool for older psyBNC Data.
	@$(CC) tools/convconf.c -o tools/convconf
	@tools/convconf
	@echo [*] Running Autoconfig.
	@$(CC) -I. tools/autoconf.c -o tools/autoconf
	@tools/autoconf
	@echo [*] Compiling MakeSalt for Encryption..
	@$(CC) -I. -o makesalt tools/makesalt.c
	@./makesalt
	@echo [*] Compiling Bouncer..
	@make -f makefile.out
	@ls -al psybnc
	@echo done.

menuconfig:
	@echo Initializing Menu-Configuration
	@echo [*] Running Conversion Tool for older psyBNC Data.
	@$(CC) tools/convconf.c -o tools/convconf
	@tools/convconf
	@echo [*] Running Autoconfig.
	@$(CC) -I. tools/autoconf.c -o tools/autoconf
	@tools/autoconf
	@echo [*] Creating Menu, please wait.
	@echo This needs the 'ncurses' library. If it is not available, menuconf wont work. If you are using 'curses', use make menuconfig-curses instead.
	@$(CC) menuconf/menuconf.c menuconf/inputbox.c menuconf/util.c menuconf/checklist.c menuconf/menubox.c menuconf/textbox.c src/snprintf.c -I. -lncurses -lm -o menuconf/menuconf 2>/dev/null
	@menuconf/menuconf
	@clear
	@echo Now compile psyBNC using make, if not yet compiled, or if Options were changed.
	@echo done.

menuconfig-curses:
	@echo Initializing Menu-Configuration using Curses
	@echo [*] Running Conversion Tool for older psyBNC Data.
	@$(CC) tools/convconf.c -o tools/convconf
	@tools/convconf
	@echo [*] Running Autoconfig.
	@$(CC) -I. tools/autoconf.c -o tools/autoconf
	@tools/autoconf
	@echo [*] Creating Menu, please wait.
	@echo This needs the 'curses' library. If it is not available, menuconf wont work.
	@$(CC) menuconf/menuconf.c menuconf/inputbox.c menuconf/util.c menuconf/checklist.c menuconf/menubox.c menuconf/textbox.c src/snprintf.c -DNONCURSES -I. -lcurses -lm -o menuconf/menuconf 2>/dev/null
	@menuconf/menuconf
	@clear
	@echo Now compile psyBNC using make, if not yet compiled, or if Options were changed.
	@echo done.

dist:
	cd ..; tar -cvf psyBNC2.3.2-4.tar psybnc; gzip -c psyBNC2.3.2-4.tar > psyBNC2.3.2-4.tar.gz; rm psyBNC2.3.2-4.tar

clean:
	@echo Cleaning.
	rm -rf src/*.o