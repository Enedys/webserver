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
					CGI.hpp \
					include_resources.hpp \
					RequestData.hpp \
					URI.hpp \

SRC				=	Debugger.cpp \
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
					NewClient.cpp \
					base_utils.cpp \
					RequestData.cpp \
					CGI.cpp \
					URI.cpp \
					main.cpp

OBJ_DIR = ./bin/
OBJ_LIST = $(patsubst %.cpp, %.o, $(SRC))
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_LIST))

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	echo "Danone!"
	clang++ -g -D_GLIBCXX_DEBUG $(FLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)%.o: %.cpp
	clang++ -c -g -D_GLIBCXX_DEBUG $< -o $@

clean:
	rm -f $(OBJS)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run
