/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/11 20:02:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_expend(char *str, int *start, char *usable_str)
{
	// char *expension;
	char	buffer[5000];
	char	*env;

	ft_bzero(buffer, 5000);

	// ON INCREMENTE POUR NE PAS PRENDRE LE $
	(*start)++;

	// ON PARCOURT STR JUSQU'A " ou ' ou $ ou SPACES
	while (str[*start] && !ft_ischarset(str[*start], SEPARATORS))
	{
		ft_buffer_add(buffer, str[*start]);
		(*start)++;
	}
	// ON RECUPERE L'ENV
	printf("BUFFER = %s\n", buffer);
	env = getenv(buffer);
	// ft_printf("ENV = %s\n", env);
	if (env)
		ft_strlcat(usable_str, env, 5000);
	// printf("USABLE = %s\n", usable_str);
}

void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	int 	i;
	char	usable_str[5000];
	t_state prev_state;

	i = 0;
	minishell->state = NORMAL;
	// ON BOUCLE SUR CHAQUE ELEMENT DE TOKEN->STR
	ft_bzero(usable_str, 5000);
	// printf("TOKEN = %s\n", token->str);
	while (token->str[i])
	{
		// ON AFFECTE L'ETAT ET ON GARDE LE PRECEDENT EN MEMOIRE
		prev_state = minishell->state;
		ft_state_detect(token->str[i], minishell);

		// SI ON TROUVE UN DOLLAR
		if (token->str[i] == '$' && minishell->state != IN_QUOTE)
		{
			ft_expend(token->str, &i, usable_str);
			continue ;
		}
		// CONDITIONS POUR RAJOUTER MALGRE LES STATES OU LES SEPARATORS
		else if (token->str[i] == '\'' && minishell->state == IN_DQUOTE)
			ft_buffer_add(usable_str, token->str[i]);
		else if (token->str[i] == '\"' && minishell->state == IN_QUOTE)
			ft_buffer_add(usable_str, token->str[i]);
		else if (token->str[i] != '\'' && token->str[i] != '\"')
			ft_buffer_add(usable_str, token->str[i]);
		i++;
	}
	// DUP DU BUFFER POUR REMPLACER STR DU TOKEN AVEC LES EXPENDS SI NECESSAIRE
	token->str = ft_strdup_gc(usable_str, &minishell->gc);
	// if (!token->str)
		// PROTEGER SI !token->str
}