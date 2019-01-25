#------------------------------------#
#         The Makefile V3            #
#        by Cedric Legendre          #
#                                    #
#------------------------------------#

ifeq (/bin/zsh, $(wildcard /bin/zsh))
SHELL		:=	/bin/zsh
endif

CC			=	gcc
NAME		=	malloc
CFLAGS		=	-W -Werror -Wextra -Wall
CINCS		=	-Iinclude
LIBS		=
NB_THREAD	=	-DNB_THREAD=$(shell nproc)

FILES		=	main

ifeq ($(MAKECMDGOALS),leaks)
CFLAGS		+=	-g3
else
CFLAGS		+=	-g0
endif

SRC		=	$(addsuffix .c, $(FILES))
SRC_PATH	=	$(addprefix src/, $(SRC))
OBJ		=	$(SRC_PATH:src/%.c=obj/%.o)

T_RESET	=	\033[0m
T_TITLE	=	$(T_RESET)\033[1;32m✔ \033[1;36m
T_LAUNCH	=	$(T_RESET)\033[1;32m \033[1;36m
T_COMPILE	=	$(T_RESET)\033[1;32m \033[1;36m
T_DELETE	=	$(T_RESET)\033[1;31m✘
T_FILE		=	$(T_RESET)\033[1;33m
T_TEST		=	$(T_RESET)\033[1;33m

MSG_KO		=	[\033[1;31mKO$(T_RESET)]
MSG_OK		=	[\033[1;32mOK$(T_RESET)]

.PHONY: all clean

all: $(NAME)

$(NAME): $(OBJ)
	@echo -e "\n$(T_COMPILE) Compiling binary:$(T_FILE)\t$(shell pwd)/$(NAME)$(T_RESET)\n\n"
	@$(CC) $(NB_THREAD) $(OBJ) -o $(NAME) $(LIBXML) $(LIBS)
	@echo -e "\n$(T_TITLE) $(NAME)\t\t$(T_FILE)Created$(T_RESET)\n\n$(T_RESET)"
	@echo -e "\n$(T_LAUNCH) \tYou can launch $(T_FILE)$(NAME)\033[1;36m now !\n\n$(T_RESET)"

obj/%.o : src/%.c
	@-$(CC) $(CFLAGS) $(CINCS) -o $@ -c $^
	@([ -f $@ ] && echo -e "$(MSG_OK)\t\e[1m$^$(T_RESET)  ❱❱  \033[1;32m$@$(T_RESET)") || \
	echo -e "\n$(MSG_KO)\t\e[1m$^$(T_RESET)  ❱❱  \e[41m\e[1m\e[97m\e[5m ⚠ $@ ⚠ $(T_RESET)"



clean:
	@echo -e "$(T_DELETE) Delete:$(T_FILE)\t\t$(shell pwd)/obj$(T_RESET)\n"
	@rm -rf obj

oclean:
	@echo -e "$(T_DELETE) Delete:$(T_FILE)\t\t$(shell pwd)/outputs$(T_RESET)\n"
	@rm -rf outputs

fclean: clean oclean
	@echo -e "$(T_DELETE) Delete:$(T_FILE)\t\t$(shell pwd)/$(NAME)$(T_RESET)\n"
	@find -name "*~" -delete
	@rm -f $(NAME)

re: fclean dirobj all

run: dirobj all
	@clear
	@./$(NAME)

dirobj:
	@echo -e "$(T_TITLE) Create:$(T_FILE)\t\t$(shell pwd)/obj$(T_RESET)\n"
	@mkdir -p obj
	@find src -type d -exec mkdir -p "obj/{}" \;
	@-mv obj/src/* obj/

leaks: re
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(NAME)

