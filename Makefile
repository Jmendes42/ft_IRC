
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address
RM = rm -f

NAME = ircerver
SRCS = $(wildcard ./srcs/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all = $(NAME)

$(NAME):
		$(CC) $(SRCS) -o $(NAME)

clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
