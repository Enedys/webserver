# Colors
_GREY		=	\e[30m
_RED		=	\e[31m
_GREEN		=	\e[32m
_YELLOW		=	\e[33m
_BLUE		=	\e[34m
_FUXIA		=	\e[35m
_CYAN		=	\e[36m
_WHITE		=	\e[37m
_END		=	\e[0m

NAME			=	webserv
FLAGS			=	-Wall -Werror -Wextra -g

HEADERS			=	NewClient.hpp \
					Debugger.hpp \
					include_resources.hpp \
					Header.hpp \
					Method.hpp \
					MethodDelete.hpp \
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
					RequestData.hpp \
					URI.hpp \
					Configurator.hpp

SRC				=	Debugger.cpp \
					Header.cpp \
					Method.cpp \
					MethodGet.cpp \
					MethodDelete.cpp \
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
					InputConfigurator.cpp \
					Configurator.cpp \
					main.cpp

OBJ_DIR = ./bin/
OBJ_LIST = $(patsubst %.cpp, %.o, $(SRC))
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_LIST))

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	@clang++ -g -D_GLIBCXX_DEBUG -DTESTER $(FLAGS) $(OBJS) -o $(NAME)
	@printf "$(_GREEN)\nDanone! ✅\n$(NAME) is ready to run! ✅\n$(_END)"

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(OBJ_DIR)%.o: %.cpp
	@clang++ -c -g -D_GLIBCXX_DEBUG -DTESTER $< -o $@
	@printf "\033[2K\r $(_YELLOW)Compiling... $< $(_END)⌛"

clean:
	@rm -f $(OBJS)
	@rm -rf $(OBJ_DIR)
	@printf "$(_RED) '"$(OBJ_DIR)"' has been deleted. $(_END)🗑️\n"

fclean: clean
	@rm -f $(NAME)
	@printf "$(_RED) '"$(NAME)"' has been deleted. $(_END)🗑️\n"

re: fclean all

.PHONY: all clean fclean re run
