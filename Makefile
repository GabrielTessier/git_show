
compile:
	gcc -o git_show view.c dict.c sdl_utils.c window.c -lSDL2
	$(info Pour executer faire : )
	$(info /path/to/this/folder/git_show)

run:
	./git_show
