/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 12:25:48 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_errors
{
	SUCCESS = 0,
	GENERAL_ERROR = 1,
	ERR_SYNTAX = 2,
	NO_SUCH_FILE_O_DIR = 3,
	ERR_CHECK_TOKEN = 4,
	ERR_HANDLE_PIPE = 5,
	ERR_REDIRECTION = 6,
	ERR_OPEN = 7,
	PARSING_FAIL = 50,
	BUFFER_FAIL = 98,
	MALLOC_FAIL = 99,
	PERMISSION_DENIED = 126,
	IS_DIRECTORY = 126,
	CMD_NOT_FOUND = 127,
	SIGNAL = 128,
}	t_errors;

# define ERRS_SYNT_NEAR "Syntax error near unexpected token "

#define ERR_QUOTES "Syntax error: unclosed quotes"

#endif

//------
