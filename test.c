#include <stdio.h>
#include "queue.h"
#include "stack_v0.h"

int main(){
	STACK *s = st_make_empty(6);
	st_push(s, 4);
	st_push(s, 2);
	st_push(s, 8);

	int a= st_pop(s);
	printf("pop %d\n", a);
	st_push(s,a);

	BOOL a= st_is_empty(s);
	while(a!=TRUE){
		printf("%d\n", st_pop(s));
		a= st_is_empty(s);
	}
}