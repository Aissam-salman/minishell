#include "../../includes/minishell.h"
#include "../../includes/testing.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

t_elements *create_mocks_element1();
// t_elements *create_mocks_element2();

Test(Parser, test_check_flags) {
    cr_expect(ft_check_flags("-l") == 1);
    cr_expect(ft_check_flags("-la") == 1);
    cr_expect(ft_check_flags("-lamsdsdsd") == 1);
    cr_expect(ft_check_flags("--format") == 1);
    cr_expect(ft_check_flags("--foo") == 1);
    cr_expect(ft_check_flags("--l0912") == 1);
    cr_expect(ft_check_flags("--1") == 1);
    cr_expect(ft_check_flags("-1") == 1);

    cr_expect(ft_check_flags("-") == 0);
    cr_expect(ft_check_flags("- ") == 0);
    cr_expect(ft_check_flags("") == 0);
    cr_expect(ft_check_flags("--") == 0);
    cr_expect(ft_check_flags("-- ") == 0);
    cr_expect(ft_check_flags("--l -la") == 0);
}

Test(Parser, test_check_redirection)
{
	cr_expect(ft_check_redirection(">") == 1);
	cr_expect(ft_check_redirection("<") == 1);
	cr_expect(ft_check_redirection(">>") == 1);
	cr_expect(ft_check_redirection("<<") == 1);

	cr_expect(ft_check_redirection("") == 0);
	cr_expect(ft_check_redirection(" ") == 0);
	cr_expect(ft_check_redirection("      ") == 0);
	cr_expect(ft_check_redirection("  <") == 0);
	cr_expect(ft_check_redirection("><") == 0);
	cr_expect(ft_check_redirection(">>>") == 0);
	cr_expect(ft_check_redirection("<<<") == 0);
	cr_expect(ft_check_redirection("<<>>>>>>") == 0);
	cr_expect(ft_check_redirection("<< >>>>>>") == 0);
	cr_expect(ft_check_redirection("< >>") == 0);
	cr_expect(ft_check_redirection("> >>") == 0);
	cr_expect(ft_check_redirection("> > > >") == 0);
}

Test(Parser, test_check_file)
{
	t_elements *ele = malloc(sizeof(t_elements));
	t_elements *ele1 = malloc(sizeof(t_elements));
	ele->str = ft_strdup(">");
	ele->type = IN_CHEVRON;
	ele1->str = ft_strdup("file");
	ele1->type = WORD;
	ele->next = ele1;
	ele1->next = NULL;
	cr_expect(ft_check_file(ele) == 0);
}
