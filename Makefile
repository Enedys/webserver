NAME			=	webserv
FLAGS			=	-Wall -Werror -Wextra -g

HEADERS			=	Client.hpp \
					Debugger.hpp \
					Headers.hpp \
					Method.hpp \
					MethodError.hpp \
					MethodGet.hpp \
					MethodHead.hpp \
					MethodOption.hpp \
					MethodPost.hpp \
					MethodPut.hpp \
					Parser.hpp \
					Request.hpp \
					Response.hpp \
					Server.hpp \
					WebServer.hpp \
					includes.hpp

SRC				=	MethodGet.cpp \
					Parser.cpp \
					main.cpp \
					mainololo.cpp \
					test.cpp

OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re run

all: $(NAME)

$(NAME): $(OBJ)
	echo "out make"
	gcc $(FLAGS) -o $(NAME) $(OBJ)

%.o:%.c $(HEADERS)
	gcc -c -g $< -o $@

clean:
	rm -rf $(OBJ)

fclean:
	rm -rf $(OBJ) $(NAME)

re: fclean all