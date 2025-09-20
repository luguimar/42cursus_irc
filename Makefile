################################################################################
#                                     CONFIG                                   #
################################################################################

NAME		:=  ircserv
CC		:= c++ -g
FLAGS		:= -Wall -Wextra -Werror -std=c++98
CR		:= clear

################################################################################x
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS     := main.cpp Client.cpp Channel.cpp Server.cpp \
			Commands/Join.cpp Commands/Nick.cpp Commands/Privmsg.cpp Commands/Pass.cpp Commands/User.cpp \
			Commands/Mode.cpp Commands/Topic.cpp Commands/Kick.cpp Commands/Invite.cpp Commands/Quit.cpp

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


