/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 09:07:41 by alamjad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_expend(char *str, int *start, char *usable_str, t_minishell *minishell)
{
	char	buffer[BUFFER_SIZE];
	char	*err_value;
	t_env	*path_env;

	// char *expension;
	ft_bzero(buffer, BUFFER_SIZE);
	// ON INCREMENTE POUR NE PAS PRENDRE LE $
	// ATTENTION A L'EXPAND POUR LA VALEUR DE RETOUR
	if (ft_ischarset(str[*start + 1],SEPARATORS) || !str[*start + 1])
	{
		// CA C'EST GOOD
		(*start)++;
		if (ft_buffer_add(usable_str, '$') != 0)
			return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
					NULL));
		if (ft_strlcat(usable_str, buffer, BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
					NULL));
		return (0);
	}
	(*start)++;
	if (str[*start] && str[*start] == '?')
	{
		err_value = ft_itoa_gc(minishell->cached_status, &minishell->gc);
		if (!err_value)
			return (ft_error(minishell, MALLOC_FAIL, "Error malloc expands",
					NULL));
		// SI TAILLE DU BUFFER TROP PETITE
		if (ft_strlcat(usable_str, err_value, BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
					NULL));
		(*start)++;
		return (0);
	}

	// ON PARCOURT STR JUSQU'A " ou ' ou $ ou SPACES
	while (str[*start] && !ft_ischarset(str[*start], SEPARATORS))
	{
		if (ft_buffer_add(buffer, str[*start]) != 0)
			return (BUFFER_FAIL);
		(*start)++;
	}
	// ON RECUPERE L'ENV
	// ft_printf("BUFFER = %s\n", buffer);
	path_env = ft_env_find(minishell->head_env, buffer);
	// ft_printf("ENV = %s\n", env);
	if (path_env && path_env->content)
	{
		if (ft_strlcat(usable_str, path_env->content,
				BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
					NULL));
	}
	return (0);
}

// FONCTION PRINCIPALE POUR GERE LES QUOTES
void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	int		i;
	char	usable_str[BUFFER_SIZE];

	i = 0;
	minishell->state = NORMAL;
	// ON BOUCLE SUR CHAQUE ELEMENT DE TOKEN->STR
	// ft_printf("TOKEN CONTENT = %s\n", token->str);
	ft_bzero(usable_str, BUFFER_SIZE);
	// ft_tokens_print(minishell->head_token);
	while (token->str[i])
	{
		// ON AFFECTE L'ETAT ET ON GARDE LE PRECEDENT EN MEMOIRE
		ft_state_detect(token->str[i], minishell);

		// SI ON TROUVE UN DOLLAR
		if (token->str[i] == '$' && minishell->state != IN_QUOTE)
		{
			if (ft_expend(token->str, &i, usable_str, minishell))
				return ;
			continue ;
		}
		// CONDITIONS POUR RAJOUTER MALGRE LES STATES OU LES SEPARATORS (A JOLIFIER)
		else if ((token->str[i] != '\'' || minishell->state == IN_DQUOTE)
				&& (token->str[i] != '\"' || minishell->state == IN_QUOTE))
		{
			if (ft_buffer_add(usable_str, token->str[i]))
				return ;
		}
		i++;
	}
	// DUP DU BUFFER POUR REMPLACER STR DU TOKEN AVEC LES EXPENDS SI NECESSAIRE
	token->str = ft_strdup_gc(usable_str, &minishell->gc);
	if (!token->str)
		ft_error(minishell, MALLOC_FAIL, "Error malloc end of quotes handling",
			NULL);
}
