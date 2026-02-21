/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 17:22:02 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 19:03:16 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "struct.h"

// ── PARSING ─────────────────────────────────────────

// PARSING/LEXER.C
void	ft_state_detect(char c, t_minishell *minishell);
int		ft_buffer_add(char *buffer, char c);
int		ft_tokenize(t_minishell *minishell);
//------
//
// PARSING/LEXER_UTILS.C
int	ft_state_inter_space_normal(t_minishell *minishell, char *buffer);
int	ft_state_inter_ope_normal(t_minishell *minishell, char *buffer, char *line, int index);
int	ft_state_inter_not_waiting(t_minishell *minishell, char *buffer, char *line, int index);
int	ft_buffer_add(char *buffer, char c);
//------

// PARSING/PARSER.C
int		checker_token(t_minishell *minishell);
//------

// PARSING/CHECK.C
int		ft_check_flags(char *str);
int		ft_check_redirection(char *str);
//------

// PARSING/CHECK2.C
int		ft_cmd_find_path(t_minishell *minishell, t_token *token);
int		is_redirection(t_token *token);
int		ft_check_pipe(char *str);
//------

// UTILS/PARSER_UTILS.C
void	ft_quotes_handle(t_minishell *minishell, t_token *token);
//------

#endif
