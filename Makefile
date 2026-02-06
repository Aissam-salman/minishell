NAME= minishell

SRCS = src/main.c \
		src/errors/errors.c \
		src/parsing/lexer.c

OBJ_DIR = objs
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

CC = cc -Wall -Wextra -Werror 

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

INCS = -I. -Iincludes -I$(LIBFT_DIR)

all: $(NAME)

$(LIBFT): 
	make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT) $(INCS) -lreadline -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(INCS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(LIBFT)

re: fclean all

test: all
	@echo "Running automated tests..."
	./tests/example.sh
