/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_line2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyopark <gyopark@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/29 14:19:35 by youngski          #+#    #+#             */
/*   Updated: 2023/01/31 23:05:58 by youngski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	wait_all(pid_t last_pid)
{
	pid_t	pid;
	int		temp;
	int		signo;
	int		status;

	pid = 1;
	while (pid != -1)
	{
		pid = wait(&temp);
		if (WIFSIGNALED(temp))
		{
			signo = WTERMSIG(temp);
			if (signo == SIGINT)
				ft_putstr_fd("^C\n", STDERR_FILENO);
			else if (signo == SIGQUIT)
				ft_putstr_fd("^\\Quit: 3\n", STDERR_FILENO);
			g_exit_code = 128 + signo;
		}
		else
		{
			if (pid == last_pid)
			{
				status = temp;
				g_exit_code = WEXITSTATUS(status);
			}
		}
	}
	return (g_exit_code);
}

void	free_orders(char **t)
{
	int	i;

	i = -1;
	while (t[++i])
		free(t[i]);
}

void	copy_make_orders(char ***t, t_token head)
{
	char	**t_temp;
	char	**ret;
	int		i;
	int		k;
	int		len;

	i = 0;
	k = 0;
	t_temp = *t;
	while (t_temp[i])
		i++;
	printf("asdf\n\n");
	ret[0] = (char *)malloc(sizeof(char *) * i + 1);
	ret[i] = 0;
	while (k < i)
	{
		if (!t_temp[k])
		{
			ret[i] = 0;
			break;
		}
		len = ft_strlen(t_temp[k]);
		ret[k] = (char *)malloc(sizeof(char) * len);
		ft_memcpy(ret[k], t_temp[k], len);
		k++;
	}
	*t = ret;
}

int	keep_execve(t_data data, t_token **head, char ***t, int *flag)
{
	int		i;
	char	***k;
	k = t;
	//첫번째 명령어를 쓸 때는 주소값 찾아서 넣어줘야된다 todo
	i = 0;
	//명령어 체킹 있는지 없는지(*head)->str
	if (check_command(data.path, (*head)->str) != 0 && *flag == 0)
		return (-1);
	copy_make_orders(t, **head);
	t[0][i] = (*head)->str;
	*flag = 1;
	(*head) = (*head)->next;
	return (0);
}

void	forked_child_work(t_data *data, t_token **head, int *pipes,
		int *heredoc_count)
{
	char	**t;
	int		flag;
	int		output_fd;
	int		input_fd;

	t = (char **)malloc(sizeof(char *));
	t[0] = 0;
	flag = 0; // 함수 내부에서 플래그 값 1로 바꿔주고 활용할것
	(*head) = (*head)->next;
	while ((*head) && ft_strncmp((*head)->str, "|", 1) != 0)
	{
		if (ft_strncmp((*head)->str, "<", 1) == 0) // 문장이 < 일때
			input_fd = input_redirection(input_fd, head);
		else if (ft_strncmp((*head)->str, ">", 1) == 0) // 문장이 >일때
			output_fd = output_redirection(output_fd, head);
		else if (ft_strncmp((*head)->str, ">>", 2) == 0)
			input_fd = append_redirection(output_fd, head);
		else if (ft_strncmp((*head)->str, "<<", 2 == 0))
			input_fd = heredoc_redirection(input_fd, head, data, heredoc_count);
		else
		{
			if (keep_execve(*data, head, &t, &flag) == -1)
				return ;
		}
	}
	printf("\n\nfinally : %s\n\n",t[0]);
	printf("\n\nfinally : %s\n\n",t[1]);
	dup_pipes(head, pipes, input_fd, output_fd);
	(*head) = (*head)->next;
	execve(t[0], t, data->envp);
}
// 만일 함수 내부에서 fd 로 묶었는데 모든 파일에 변경사항이 저장되는 현상이 발생할 경우
	// 각각의 fd 값을 클로즈 해주기 위해서 fd 배열값을 가지고 가야된다.
