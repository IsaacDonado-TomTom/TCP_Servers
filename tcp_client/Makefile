TCPCLIENT = tcp_client

SRCS = tcp_client.cpp ArgParser.cpp

OBJS = $(SRCS:.cpp=.o)

DEBUG_FLAGS = -g3 -fsanitize=address

all: $(TCPCLIENT)

$(TCPCLIENT): $(OBJS)
	clang++ -o $@ $^  -Wall  -Wextra -std=c++98 -pedantic

%.o: %.cpp
	clang++ -o $@ -c $^ -Wall  -Wextra -std=c++98 -pedantic -fPIC -g

clean:
	-rm -f $(OBJS)

fclean: clean
	-rm -f $(TCPCLIENT)

re: fclean all


.PHONY: tcp_client clean fclean re
