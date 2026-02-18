/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 13:40:37 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 20:40:02 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_env_print(t_env *head, int fd)
{
	while (head)
	{
		if (head->content)
		{
			ft_putstr_fd(head->name, fd);
			ft_putstr_fd("=", fd);
			ft_putstr_fd(head->content, fd);
			ft_putstr_fd("\n", fd);
		}
		head = head->next;
	}
}

void	ft_env_print_export_no_param(t_env *head, int fd)
{
	while (head)
	{
		if (head->content)
		{
			ft_putstr_fd("export ", fd);
			ft_putstr_fd(head->name, fd);
			ft_putstr_fd("=", fd);
			ft_putstr_fd(head->content, fd);
			ft_putstr_fd("\n", fd);
		}
		head = head->next;
	}
}

// IMPRIME TOUTES LES DATAS STRUCT CMD
void	ft_cmd_print(t_cmd *head)
{
	t_cmd	*current;
	int		i;

	current = head;
	while (current)
	{
		i = 0;
		if (current->path)
			ft_printf("|{PATH = %s}|\n", current->path);
		else
			ft_printf("|{NO PATH YET}|");
		while (current->args[i++])
			ft_printf("[ARGV[%d] = %s]\n", i - 1, current->args[i - 1]);
		ft_printf("|[OUTFILE = %d || INFILE = %d]|\n", current->outfd, current->infd);
		current = current->next;
	}
}

// IMPRIME TOUTES LES DATAS STRUCT TOKEN
void	ft_tokens_print(t_token *head)
{
	t_token	*current;

	current = head;
	while (current)
	{
		ft_printf("CONTENT TOKEN = %s\n", current->str);
		ft_type_print(current);
		current = current->next;
	}
}

// IMPRIME LE TYPE EN FONCTION DE L'ENUM
void	ft_type_print(t_token *token)
{
	if (token->type == DEFAULT)
		ft_printf("TYPE = DEFAULT\n");
	else if (token->type == PIPE)
		ft_printf("TYPE = PIPE\n");
	else if (token->type == IN_CHEVRON)
		ft_printf("TYPE = IN_CHEVRON\n");
	else if (token->type == IN_DCHEVRON)
		ft_printf("TYPE = IN_DCHEVRON\n");
	else if (token->type == OUT_CHEVRON)
		ft_printf("TYPE = OUT_CHEVRON\n");
	else if (token->type == OUT_DCHEVRON)
		ft_printf("TYPE = OUT_DCHEVRON\n");
	else if (token->type == WORD)
		ft_printf("TYPE = WORD\n");
}

void	ft_state_print(char c, char *buffer, t_minishell *minishell)
{
	if (!buffer[0])
		ft_printf("EMPTY BUFFER ||", buffer);
	else
		ft_printf("BUFFER = {%s} || ", buffer);
	ft_printf("CHARACTER  = \"%c\" || STATE = ", c);
	if (minishell->state == NORMAL)
		ft_printf("NORMAL\n");
	if (minishell->state == IN_QUOTE)
		ft_printf("IN_QUOTE\n");
	if (minishell->state == IN_DQUOTE)
		ft_printf("IN_DQUOTE\n");
	if (minishell->state == WAITING)
		ft_printf("WAITING\n");
}
