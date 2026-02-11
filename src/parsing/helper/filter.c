
#include "../../../includes/minishell.h"

static char *ft_strdup_without(char *str, t_minishell *minishell) {
	char *clear_word;

	clear_word = ft_gc_malloc(ft_strlen(str) - 2 + 1, &minishell->gc);
	if (!clear_word)
		return (NULL);
	clear_word = ft_substr_gc(str, 1, ft_strlen(str) - 2, &minishell->gc);
	return (clear_word);
}

void ft_filter_quote(t_token *token, t_minishell *minishell)
{
	char type_quote;

	if (!token || !*token->str)
		return;
	type_quote = '\0';
	if (token->str[0] == '\"' && token->str[ft_strlen(token->str) - 1] == '\"')
		type_quote = '\"';
	else if (token->str[0] == '\'' && token->str[ft_strlen(token->str) - 1] == '\'')
		type_quote = '\'';
	if (type_quote != '\0')
		token->str = ft_strdup_without(token->str, minishell);
}