NAME = webserv
SOURCES = main.cpp parsing/webserv.cpp parsing/location.cpp parsing/parce_server.cpp \
					server/client.cpp server/server.cpp server/socket.cpp \
					request/request.cpp request/post.cpp request/get.cpp \
					request/check_path.cpp request/normal_post.cpp request/delete.cpp \
					request/chunked_post.cpp request/boundary.cpp
OBJ_PATH  = objs
SRC_PATH = srcs

CC = c++
 CFLAGS =  -Wall -Wextra -Werror

OBJS = $(addprefix $(OBJ_PATH)/,$(SOURCES:.cpp=.o))
.PHONY: all clean fclean re

all : $(OBJ_PATH) $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -std=c++98 $(OBJS) -o  $(NAME)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_PATH):
	@mkdir objs
	@mkdir objs/parsing
	@mkdir objs/request
	@mkdir objs/server

clean:
	rm -fr objs

fclean: clean
	rm -f $(NAME)
re : fclean all
