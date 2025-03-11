CC		:= gcc

CFLAGS	:= -Wall -Wextra -O2 -g

INCLUDE	:= -I $(PWD)/../../include

TARGET	:= otpctl

SRC		:= 	$(PWD)/main.c					\
			$(PWD)/commands_misc.c			\
			$(PWD)/subcommands/get.c		\
			$(PWD)/subcommands/set.c		\
			$(PWD)/subcommands/generate.c	\
			$(PWD)/subcommands/validate.c

OBJ		:= $(SRC:.c=.o)

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(INCLUDE)

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(TARGET)

re:	fclean all

.PHONY: all clean fclean re