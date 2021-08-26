#include "hblk_cli.h"
/* free */
#include <stdlib.h>


/**
 * freeSTList - frees a syntax token list
 *
 * @st_list: syntax token list to free
 */
void freeSTList(st_list_t *st_list)
{
	st_list_t *curr, *next;

	curr = st_list;
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}
