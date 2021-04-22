PROJECT = myshell
HEADERS = $(PROJECT).h
OBJ = globals.o execute.o parser.o myshell.o execute_command.o search_path.o \
		internal_command.o subshell.o file_redirection.o pipeline.o \
	    shell_script.o background.o

C99 = cc -std=c99
CFLAGS  =  -Wall -pedantic -Werror


$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)
