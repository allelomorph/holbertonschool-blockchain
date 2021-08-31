#include "hblk_cli.h"
/* fprintf */
#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* strtok */
#include <string.h>


/**
 * lineToSTList - simple lexing of a CLI line into a singly linked list of
 *   tokens
 *
 * @line: line of CLI input
 * @cli_state: pointer to struct containing information about the cli and
 *   blockchain in use
 *
 * Return: head of syntax token list, or NULL on failure
 */
st_list_t *lineToSTList(char *line, cli_state_t *cli_state)
{
	st_list_t *head, *temp;
	char *next_token;

	if (!line || !cli_state)
	{
		fprintf(stderr, "lineToSTList: NULL paramter(s)\n");
		return (NULL);
	}

	head = malloc(sizeof(st_list_t));
	if (!head)
	{
		fprintf(stderr, "lineToSTList: malloc failure\n");
		return (NULL);
	}
	head->token = strtok(line, WHITESPACE);

	for (temp = head; temp; temp = temp->next)
	{
		next_token = strtok(NULL, WHITESPACE);
		if (next_token)
		{
			temp->next = malloc(sizeof(st_list_t));
			if (!temp->next)
			{
				fprintf(stderr,
					"lineToSTList: malloc failure\n");
				freeSTList(head);
				return (NULL);
			}
			temp->next->token = next_token;
		}
		else
			temp->next = NULL;
	}

	return (head);
}
