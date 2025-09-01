################################################################################
#                                     CONFIG                                   #
################################################################################

NAME		:=  ircserv
CC		:= c++
FLAGS		:= -Wall -Wextra -Werror -std=c++98
CR		:= clear

################################################################################x
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS     := Main.cpp Client.cpp Channel.cpp Server.cpp \
			Commands/Join.cpp Commands/Nick.cpp Commands/Privmsg.cpp Commands/Pass.cpp Commands/User.cpp

OBJS     := ${SRCS:.cpp=.o}

.cpp.o:
	${CC} ${FLAGS} -c $< -o ${<:.cpp=.o}

################################################################################
#                                  Makefile  objs                              #
################################################################################

${NAME}:	$(OBJS)
			${CC} ${FLAGS} -o $@ $^


all:		${NAME}

bonus:		all

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re


