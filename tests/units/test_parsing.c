#include "../../includes/minishell.h"
#include "../../includes/testing.h"
#include "../../lib/criterion/include/criterion/criterion.h"
#include "../../lib/criterion/include/criterion/internal/assert.h"

// t_elements *create_mocks_element1();
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

t_elements *create_mock_ele(char *rd, t_types t,  char *file)
{
	t_elements *ele = malloc(sizeof(t_elements));
	t_elements *ele1 = malloc(sizeof(t_elements));

	ele->str = ft_strdup(rd);
	ele->type = t;
	ele1->str = ft_strdup(file);
	ele1->type = WORD;
	ele->next = ele1;
	ele1->next = NULL;
	return (ele);
}

void free_ele(t_elements *ele)
{
	t_elements	*tmp;

	if (!ele)
		return ;
	while (ele)
	{
		tmp = ele->next;
		free(ele);
		ele = tmp;
	}
}

Test(Parser, test_check_file)
{
	// BASE CASE
	t_elements *ele = create_mock_ele(">", OUT_CHEVRON, "infile");
	cr_expect(ft_check_file(ele) == 1);
	free_ele(ele);
	t_elements *ele1 = create_mock_ele("<", IN_CHEVRON, "infile");
	cr_expect(ft_check_file(ele1) == 1);
	free_ele(ele);

	t_elements *ele3 = create_mock_ele(">>",OUT_DCHEVRON, "infile");
	cr_expect(ft_check_file(ele3) == 1);
	free_ele(ele3);

	t_elements *ele4 = create_mock_ele("<<", IN_DCHEVRON, "infile");
	cr_expect(ft_check_file(ele4) == 1);
	free_ele(ele4);

	//file not exist
	t_elements *ele2 = create_mock_ele("<", IN_CHEVRON, "foo"); 
	cr_expect(ft_check_file(ele2) == 0); 
	free_ele(ele2);

	t_elements *ele5 = create_mock_ele(">", OUT_CHEVRON, "foo"); 
	cr_expect(ft_check_file(ele5) == 1);
	free_ele(ele5);
}
