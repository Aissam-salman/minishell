/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 20:15:42 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/07 22:07:55 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef TESTING_H
# define TESTING_H

#include "minishell.h"

int ft_check_flags(char *str);
int ft_check_redirection(char *str);
int ft_check_file_of_redirection(t_token *token);
int ft_check_heredoc_end(char *str);
int ft_check_cmd(t_minishell *minishell, t_token *ele);

#endif
