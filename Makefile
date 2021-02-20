NAME			=	webserv
FLAGS			=	-Wall -Werror -Wextra -g

HEADERS			=	NewClient.hpp \
					Debugger.hpp \
					Headers.hpp \
					Method.hpp \
					MethodGet.hpp \
					MethodHead.hpp \
					MethodOption.hpp \
					MethodPost.hpp \
					MethodPut.hpp \
					Parser.hpp \
					Request.hpp \
					Server.hpp \
					WebServer.hpp \
					include_resources.hpp \

SRC				=	NewClient.cpp \
					Debugger.cpp \
					Method.cpp \
					MethodGet.cpp \
					MethodHead.cpp \
					MethodOption.cpp \
					MethodPost.cpp \
					MethodPut.cpp \
					Parser.cpp \
					Request.cpp \
					Server.cpp \
					WebServer.cpp \
					main.cpp

OBJ = $(SRC:.cpp=.o)

.PHONY: all clean fclean re run

all: $(NAME)

$(NAME): $(OBJ)
	echo "Danone!"
	clang++ $(FLAGS) -o $(NAME) $(OBJ)

%.o:%.cpp
	clang++ -c -g $< -o $@

clean:
	rm -rf $(OBJ)

fclean:
	rm -rf $(OBJ) $(NAME)

re: fclean all