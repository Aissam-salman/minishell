NAME= minishell

SRCS = src/main.c \
		src/errors/errors.c \
		src/parsing/lexer.c \
		src/parsing/parser.c \
		src/parsing/support/expander.c \
		src/parsing/support/check.c \
		src/parsing/support/check2.c \
		src/built_in/env.c \
		src/built_in/export.c \
		src/utils/cmds.c \
		src/utils/output.c \
		src/utils/tokens.c \
		src/utils/heredoc.c \
		src/utils/env_setup.c \
		src/utils/parser_utils.c \
		src/helpers/cntrl.c \
		src/execution/exec.c \
		src/execution/child_exec.c \
		src/execution/handler.c \
		src/execution/wait.c 

OBJ_DIR = objs
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
CC = cc -Wall -Wextra -Werror -g
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCS = -I. -Iincludes -I$(LIBFT_DIR)

all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT) $(INCS) -lreadline -o $(NAME)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -MP $(INCS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)

re: fclean all

###############################################
###############################################
############## TESTING ########################

SRCS_NO_MAIN = src/errors/errors.c \
			src/parsing/lexer.c \
			src/parsing/parser.c \
			src/parsing/helper/expander.c \
			src/parsing/helper/check.c \
			src/parsing/helper/check2.c \
			src/utils/cmds.c \
			src/utils/output.c \
			src/utils/tokens.c \
			src/helpers/cntrl.c \
			src/exec.c

OBJS_NO_MAIN = $(SRCS_NO_MAIN:%.c=$(OBJ_DIR)/%.o)

CRITERION_DIR = lib/criterion
CRITERION_INC = -I$(CRITERION_DIR)/include
CRITERION_LIB = -L$(CRITERION_DIR)/lib -Wl,-rpath,$(CRITERION_DIR)/lib

TEST_SRCS = $(addprefix tests/units/, mocks.c test_parsing.c)
TEST_OBJS = $(TEST_SRCS:%.c=%.o)
TEST_NAME = unit_tests

testf: all
	@echo "Running automated tests..."
	./tests/example.sh

units: $(LIBFT) $(OBJS_NO_MAIN) $(TEST_OBJS)
	@echo "Compiling unit tests..."
	@$(CC) -o $(TEST_NAME) $(TEST_OBJS) $(OBJS_NO_MAIN) $(LIBFT) $(CRITERION_LIB) -lcriterion -lreadline

tests/units/%.o: tests/units/%.c
	$(CC) $(INCS) $(CRITERION_INC) -c $< -o $@
