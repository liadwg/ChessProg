//**************** Memory allocation and standard functions monitoring ******************//

/* During the program's run time we collect all the allocated pointers in a statically allocated array,
if one of the standard functions fail, we free all pointers before aborting the program.
After we ran some tests we came to a conclusion that the maximum number of pointers allocated in a specific moment does not exceed 150-200,
So we gave a very big buffer and used a fail safe so that if the array would fill up it wouldn't interfere with the program's functionality. */

#include "ChessProg.h"

void* mem_list[1000];
int mem_count = 0;
int fail_safe = 1;

// Pointer list management
void add_to_list(void* mem){
	mem_list[mem_count] = mem;
	mem_count++;
	if (mem_count > 950) {
		printf("WARNING - Memory allocation close to bounds, turning off pointer monitoring.");
		fail_safe = 0;
	}
}

void remove_from_list(void* mem){
	for (int i = 0; i < mem_count; i++)
		if (mem_list[i] == mem) {
		mem_count--;
		mem_list[i] = mem_list[mem_count];
		break;
		}
}

// safe_funcs verifies that that the original functions succeeded
void * safe_malloc(size_t size){
	void *res = malloc(size);
	if (!res && size != 0){
		perror_message("malloc");
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]);
		abort();
	}
	else{
		if (fail_safe) add_to_list(res);
		return res;
	}
}
#define malloc(x) safe_malloc(x)

void * safe_realloc(void *old_pointer, size_t size){
	void *res = realloc(old_pointer, size);
	if (!old_pointer && size != 0){
		free(old_pointer);
		perror_message("realloc");
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]);
		abort();
	}
	else{
		if (fail_safe){
			remove_from_list(old_pointer);
			add_to_list(res);
		}
		return res;
	}
}
#define realloc(x, y) safe_realloc((x), (y))

int safe_fgetc(FILE *stream){
	int res = fgetc(stream);
	if (res == EOF){
		perror_message("fgetc");
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]);
		abort();
	}
	else return res;
}
#define fgetc(x) safe_fgetc(x)

#define printf(...) \
	if (printf(__VA_ARGS__) < 0){ \
		perror_message("printf"); \
		if (fail_safe) for (int i = 0; i < mem_count; i++) free(mem_list[i]); \
		abort();} \
								else (void)0

void safe_free(void * mem){
	if (fail_safe) remove_from_list(mem);
	free(mem);
}
#define free(x) safe_free(x)
