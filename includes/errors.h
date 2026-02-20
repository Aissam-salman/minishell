/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/20 11:19:53 by tibras           ###   ########.fr       */
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

# define ERRS_SYNT_NEAR	"Syntax error near unexpected token "
# define ERRS_SYNT_NL	"Syntax error near unexpected token 'newline'"
# define ERRS_UNCLOSED	"Syntax error: unclosed quotes"

/*
** -- Malloc / Buffer errors --
*/
# define ERRS_MALLOC_INTERP	"Fail Malloc Interpreter"
# define ERRS_MALLOC_BUF	"Fail Malloc Buffer Interpreter"
# define ERRS_MALLOC_CMD	"Error malloc cmd create"
# define ERRS_MALLOC_EXP	"Error malloc expands"
# define ERRS_MALLOC_QUOT	"Error malloc end of quotes handling"
# define ERRS_MALLOC_ENV	"Error malloc setup env"
# define ERRS_MALLOC_EXPORT	"Error malloc with export"
# define ERRS_BUF_SMALL	"Insufficient buffer size"
# define ERRS_BUF_TOO_SM	"Buffer too small"

/*
** -- Heredoc errors --
*/
# define ERRS_HDOC_PIPE	"Heredoc error :"
# define ERRS_HDOC_PFAIL	"Pipe creation failed"

/*
** -- Export / env errors --
*/
# define ERRS_EXPORT_ADD	"Error adding to export"
# define ERRS_EXPORT_PRE	"export:"
# define ERRS_EXPORT_INV	": not a valid identifier"
# define ERRS_ENV_SETUP	"Error setting up env"
# define ERRS_PWD_NOTFOUND	"PWD not found"

/*
** -- Exit errors --
*/
# define ERRS_EXIT_PRE	"exit: "
# define ERRS_EXIT_ARGS	"too many arguments"
# define ERRS_EXIT_NUM	": numeric argument required"

/*
** -- CD errors --
*/
# define ERRS_CD_ARGS	"too many arguments"
# define ERRS_CD_NOTDIR	": Not a directory"
# define ERRS_CD_PRE	"cd: "

/*
** -- Exec errors --
*/
# define ERRS_PERM_DENIED	" Permission denied"
# define ERRS_CMD_NOTFOUND	" command not found"
# define ERRS_NO_FILE_DIR	" No such file or directory"
# define ERRS_IS_DIR	" Is a directory"

#endif

//------
