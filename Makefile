NAME = ircserv

NAME_BONUS = ircserv_bonus

CXX = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = ./src/Channel.cpp ./src/Client.cpp \
	   ./src/main.cpp ./src/Parser.cpp ./src/Reply.cpp \
	   ./src/Server.cpp ./src/Utils.cpp ./src/ErrorCodes.cpp \
	   ./src/commands/Command.cpp ./src/commands/CommandHandler.cpp \
	   ./src/commands/Invite.cpp  ./src/commands/Dcc.cpp \
	   ./src/commands/Join.cpp ./src/commands/Kick.cpp \
	   ./src/commands/Mode.cpp ./src/commands/Nick.cpp \
	   ./src/commands/Ping.cpp ./src/commands/Pass.cpp \
	   ./src/commands/Pong.cpp ./src/commands/Privmsg.cpp  \
	   ./src/commands/Topic.cpp ./src/commands/User.cpp ./src/commands/Cap.cpp

SRCS_BONUS = ./src/commands/Dcc.cpp ./src/Channel.cpp ./src/Client.cpp \
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

OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

RM = rm -rf

all: $(NAME)

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(NAME)

$(NAME_BONUS): $(OBJS_BONUS)
	$(CXX) $(CFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS)

clean:
	$(RM) $(OBJS) $(OBJS_BONUS)
	$(RM) $(OBJS_BONUS)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)
	$(RM) $(NAME_BONUS)

re: fclean all

bonus: $(NAME_BONUS) $(OBJS_BONUS)

.PHONY: all clean fclean re bonus
