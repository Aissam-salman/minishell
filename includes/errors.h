/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 18:43:48 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_errors {
	SUCCESS = 0,
	GENERAL_ERROR = 1,
	SYNTAX_ERROR = 2,
	PARSING_FAIL = 50,
	BUFFER_FAIL = 98,
	MALLOC_FAIL = 99,
	PERMISSION_DENIED = 126,
	CMD_NOT_FOUND = 127,
	SIGNAL = 128,
} t_errors ;

#endif
