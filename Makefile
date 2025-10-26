NAME = ircserv

CXX = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = ./src/Channel.cpp ./src/Client.cpp \
	   ./src/main.cpp ./src/Parser.cpp ./src/Reply.cpp \
	   ./src/Server.cpp ./src/Utils.cpp ./src/ErrorCodes.cpp \
	   ./src/commands/Command.cpp ./src/commands/CommandHandler.cpp \
	   ./src/commands/Invite.cpp  \
	   ./src/commands/Join.cpp ./src/commands/Kick.cpp \
	   ./src/commands/Mode.cpp ./src/commands/Nick.cpp \
	   ./src/commands/Ping.cpp ./src/commands/Pass.cpp \
	   ./src/commands/Pong.cpp ./src/commands/Privmsg.cpp  \
	   ./src/commands/Topic.cpp ./src/commands/User.cpp ./src/commands/Cap.cpp

OBJS = $(SRCS:.cpp=.o)

RM = rm -rf

all: $(NAME)

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
