/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 14:27:47 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_expend(char *str, int *start, char *usable_str, t_minishell *minishell)
{
	// char *expension;
	char	buffer[BUFFER_SIZE];
	char	*err_value;
	char	*env;

	ft_bzero(buffer, BUFFER_SIZE);

	// ON INCREMENTE POUR NE PAS PRENDRE LE $
	(*start)++;

	// ATTENTION A L'EXPAND POUR LA VALEUR DE RETOUR
	if (str[*start] == '?')
	{
		// A MODIF : RECUPERE LA VARIABLE D'ERREUR GLOBAL
		err_value = ft_itoa_gc(ERR_CODE, &minishell->gc);
		if (!err_value)
			return (ft_error(MALLOC_FAIL, "Error malloc expands\n"));
		ft_strlcat(usable_str, err_value, BUFFER_SIZE);
		(*start)++;
		return (0);
	}

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
		ft_strlcat(usable_str, env, BUFFER_SIZE);
	return (0);
}

void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	int 	i;
	char	usable_str[BUFFER_SIZE];
	t_state prev_state;

	i = 0;
	minishell->state = NORMAL;
	// ON BOUCLE SUR CHAQUE ELEMENT DE TOKEN->STR
	ft_bzero(usable_str, BUFFER_SIZE);
	while (token->str[i])
	{
		// ON AFFECTE L'ETAT ET ON GARDE LE PRECEDENT EN MEMOIRE
		prev_state = minishell->state;
		ft_state_detect(token->str[i], minishell);

		// SI ON TROUVE UN DOLLAR
		if (token->str[i] == '$' && minishell->state != IN_QUOTE)
		{
			if (ft_expend(token->str, &i, usable_str, minishell))
				return;
			continue ;
		}

		// CONDITIONS POUR RAJOUTER MALGRE LES STATES OU LES SEPARATORS (A JOLIFIER)
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
	if (!token->str)
		ft_error(MALLOC_FAIL, "Error malloc end of quotes handling\n");
}