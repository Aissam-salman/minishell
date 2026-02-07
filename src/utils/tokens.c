#include "minishell.h"

// CREATION DU TOKEN A PARTIR DU BUFFER
t_token *ft_token_create(t_minishell *minishell, char *buffer)
{
	t_token *new_token;

	new_token = ft_calloc_gc(1, sizeof(t_token), &minishell->gc);
	if (!new_token)
		return (NULL);
	new_token->str = ft_strdup_gc(buffer, &minishell->gc);
	new_token->type = DEFAULT;
	new_token->next = NULL;
	return (new_token);
}

// AJOUT DU TOKEN A LA LISTE CHAINEE *head_token
void	ft_token_add(t_minishell *minishell, t_token *to_add)
{
	t_token	*tmp;

	if (!minishell || !to_add)
		return ;
	if (!minishell->head_token)
	{
		minishell->head_token = to_add;
		return ;
	}
	tmp = minishell->head_token;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = to_add;
}
