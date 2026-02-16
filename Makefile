NAME= minishell

SRCS = src/main.c \
		src/parsing/lexer.c \
		src/parsing/parser.c \
		src/parsing/expander.c \
		src/parsing/check.c \
		src/parsing/check2.c \
		src/parsing/filter.c \
		src/exec/exec.c \
		src/exec/child_exec.c \
		src/exec/handler.c \
		src/exec/wait.c \
		src/exec/cntrl.c \
		src/built_in/env.c \
		src/built_in/export.c \
		src/built_in/pwd.c \
		src/built_in/echo.c \
		src/built_in/exit.c \
		src/built_in/cd.c \
		src/built_in/unset.c \
		src/built_in/is_built_in.c \
		src/utils/cmds.c \
		src/utils/output.c \
		src/utils/tokens.c \
		src/utils/heredoc.c \
		src/utils/env_setup.c \
		src/utils/parser_utils.c \
		src/utils/errors.c \
		src/utils/signal_core.c

OBJ_DIR = objs
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
CC = cc -Wall -Wextra -Werror -fsanitize=address -g3
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCS = -I. -Iincludes -I$(LIBFT_DIR)

all: $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	@echo "Compiling Minishell..."
	@$(CC) $(OBJS) $(LIBFT) $(INCS) -lreadline -o $(NAME)
	@echo "Build finished: ./minishell created."

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP $(INCS) -c $< -o $@

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

SRCS_NO_MAIN = src/parsing/lexer.c \
			src/parsing/parser.c \
			src/parsing/expander.c \
			src/parsing/check.c \
			src/parsing/check2.c \
			src/parsing/filter.c \
			src/exec/exec.c \
			src/exec/child_exec.c \
			src/exec/handler.c \
			src/exec/wait.c \
			src/exec/cntrl.c \
			src/utils/cmds.c \
			src/utils/output.c \
			src/utils/tokens.c \
			src/utils/errors.c \
			src/utils/signal_core.c

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
