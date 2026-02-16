/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/16 12:05:28 by tibras           ###   ########.fr       */
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
		err_value = ft_itoa_gc(GENERAL_ERROR, &minishell->gc);
		if (!err_value)
			return (ft_error(MALLOC_FAIL, "Error malloc expands", NULL));

		// SI TAILLE DU BUFFER TROP PETITE
		if (ft_strlcat(usable_str, err_value, BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(BUFFER_FAIL, "Insufficient buffer size", NULL));
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
	env = getenv(buffer);

	// ft_printf("ENV = %s\n", env);
	if (env)
	{
		if (ft_strlcat(usable_str, env, BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(BUFFER_FAIL, "Insufficient buffer size", NULL));
	}
	return (0);
}

// FONCTION PRINCIPALE POUR GERE LES QUOTES
void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	int 	i;
	char	usable_str[BUFFER_SIZE];

	i = 0;
	minishell->state = NORMAL;
	// ON BOUCLE SUR CHAQUE ELEMENT DE TOKEN->STR
	ft_bzero(usable_str, BUFFER_SIZE);
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
		// A MODIFIER : REVOIR VU QUE C'EST PAS BEAU
		else if (token->str[i] == '\'' && minishell->state == IN_DQUOTE)
		{
			if (ft_buffer_add(usable_str, token->str[i]))
				return ;
		}
		else if (token->str[i] == '\"' && minishell->state == IN_QUOTE)
		{
			if (ft_buffer_add(usable_str, token->str[i]))
				return ;
		}
		else if (token->str[i] != '\'' && token->str[i] != '\"')
		{
			if (ft_buffer_add(usable_str, token->str[i]))
				return ;
		}
		i++;
	}

	// DUP DU BUFFER POUR REMPLACER STR DU TOKEN AVEC LES EXPENDS SI NECESSAIRE
	token->str = ft_strdup_gc(usable_str, &minishell->gc);
	if (!token->str)
		ft_error(MALLOC_FAIL, "Error malloc end of quotes handling", NULL);
}
