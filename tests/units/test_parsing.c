#include "../../includes/minishell.h"
#include "../../includes/testing.h"
#include "../../lib/criterion/include/criterion/criterion.h"
#include "../../lib/criterion/include/criterion/internal/assert.h"
#include <stdlib.h>

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

Test(Parser, test_check_redirection) {
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

t_token *create_mock_ele(char *rd, t_types t, char *file) {
  t_token *ele = malloc(sizeof(t_token));
  t_token *ele1 = malloc(sizeof(t_token));

  ele->str = ft_strdup(rd);
  ele->type = t;
  ele1->str = ft_strdup(file);
  ele1->type = R_FILE;
  ele->next = ele1;
  ele1->next = NULL;
  return (ele);
}

void free_ele(t_token *ele) {
  t_token *tmp;

  if (!ele)
    return;
  while (ele) {
    tmp = ele->next;
    free(ele);
    ele = tmp;
  }
}

Test(Parser, test_check_file) {
  // BASE CASE
  t_token *ele = create_mock_ele(">", OUT_CHEVRON, "infile");
  cr_expect(ft_check_file_of_redirection(ele) == 1);
  free_ele(ele);
  t_token *ele1 = create_mock_ele("<", IN_CHEVRON, "infile");
  cr_expect(ft_check_file_of_redirection(ele1) == 1);
  free_ele(ele);

  t_token *ele3 = create_mock_ele(">>", OUT_DCHEVRON, "infile");
  cr_expect(ft_check_file_of_redirection(ele3) == 1);
  free_ele(ele3);

  t_token *ele4 = create_mock_ele("<<", IN_DCHEVRON, "eof");
  cr_expect(ft_check_file_of_redirection(ele4) == 1);
  free_ele(ele4);

  // file not exist
  t_token *ele2 = create_mock_ele("<", IN_CHEVRON, "foo");
  cr_expect(ft_check_file_of_redirection(ele2) == 0);
  free_ele(ele2);

  t_token *ele5 = create_mock_ele(">", OUT_CHEVRON, "foo");
  cr_expect(ft_check_file_of_redirection(ele5) == 1);
  free_ele(ele5);

  t_token *ele6 = create_mock_ele(">>", OUT_DCHEVRON, "foo");
  cr_expect(ft_check_file_of_redirection(ele6) == 1);
  free_ele(ele6);

  t_token *ele7 = create_mock_ele("<<", OUT_CHEVRON, "eof");
  cr_expect(ft_check_file_of_redirection(ele7) == 1);
  free_ele(ele7);

  // no endfile
  t_token *ele8 = create_mock_ele("<<", OUT_CHEVRON, "");
  cr_expect(ft_check_file_of_redirection(ele8) == 1);
  free_ele(ele8);
  t_token *ele9 = create_mock_ele("<<", OUT_CHEVRON, " ");
  cr_expect(ft_check_file_of_redirection(ele9) == 1);
  free_ele(ele9);
  t_token *ele10 = create_mock_ele("<<", OUT_CHEVRON, "   ");
  cr_expect(ft_check_file_of_redirection(ele10) == 1);
  free_ele(ele10);
}

Test(Parser, test_check_cmd) {
  t_minishell *mini = malloc(sizeof(t_minishell));
  if (!mini)
    return;
  ft_bzero(mini, sizeof(t_minishell));
  t_token *token = malloc(sizeof(t_token));
  if (!token)
    return;
  ft_bzero(token, sizeof(t_token));

  token->str = ft_strdup("ls");
  cr_expect(ft_check_cmd(mini, token) == 1);

  free(token->str);
  token->str = ft_strdup("/bin/ls");
  cr_expect(ft_check_cmd(mini, token) == 1);

  free(token->str);
  token->str = ft_strdup("/foo/bin/ls");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup("foo");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup("");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup(" ");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup("  ");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup("/");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  token->str = ft_strdup("/bin");
  cr_expect(ft_check_cmd(mini, token) == 0);

  free(token->str);
  free(mini);
  free_ele(token);
}

Test(Parser, test_check_pipe) {
  t_minishell *mini = malloc(sizeof(t_minishell));
  ft_bzero(mini, sizeof(t_minishell));

  cr_expect(ft_check_pipe("|") == 1);
  cr_expect(ft_check_pipe("") == 0);
  cr_expect(ft_check_pipe(" ") == 0);
}
