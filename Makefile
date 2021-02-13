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
					include_resources.hpp \
					includes.hpp

SRC				=	Client.cpp \
					Debugger.cpp \
					Method.cpp \
					MethodError.cpp \
					MethodGet.cpp \
					MethodHead.cpp \
					MethodOption.cpp \
					MethodPost.cpp \
					MethodPut.cpp \
					Parser.cpp \
					Request.cpp \
					Response.cpp \
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