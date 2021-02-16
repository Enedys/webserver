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

OBJ_DIR = ./bin/
OBJ_LIST = $(patsubst %.cpp, %.o, $(SRC))
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_LIST))

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	echo "Danone!"
	clang++ $(FLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)%.o: %.cpp $(HEADERS)
	clang++ -c -g $< -o $@

clean:
	rm -f $(OBJS)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run
