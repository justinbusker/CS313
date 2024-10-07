#include <threading.h>

void t_init()
{
        // TODO
	for(int i =0; i < NUM_CTX; i++){
		contexts[i].state = INVALID;
	}

	getcontext(&contexts[0].context);
	contexts[0].state = VALID;

	current_context_idx = 0;
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // TODO
	volatile int i;
	for (i = 1; i < NUM_CTX; i++){
		if(contexts[i].state == INVALID){
			getcontext(&contexts[i].context);
			contexts[i].context.uc_stack.ss_sp = malloc(STK_SZ);
			if(!contexts[i].context.uc_stack.ss_sp){
				return 1;
			}
			contexts[i].context.uc_stack.ss_size = STK_SZ;
			contexts[i].context.uc_link = &contexts[0].context;

			makecontext(&contexts[i].context, (ctx_ptr)foo, 2, arg1, arg2);

			contexts[i].state = VALID;

			return 0;
		}
	}
	return 1;
}

int32_t t_yield()
{
        // TODO
	int valid_count = 0;
    int next_context = -1;

    for (int i = 1; i < NUM_CTX; i++) {
        if (contexts[i].state == VALID) {
            valid_count++;
            if (i != current_context_idx && next_context == -1) {
                next_context = i;
            }
        }
    }

    if (next_context == -1) {
        return -1;
    }

    int previous_context = current_context_idx;
    current_context_idx = (uint8_t)next_context;
    swapcontext(&contexts[previous_context].context, &contexts[next_context].context);

    return valid_count; 
}

void t_finish()
{
	int idx = current_context_idx;
    if (contexts[idx].state == VALID) {
        contexts[idx].state = DONE;

		if(contexts[idx].context.uc_stack.ss_sp){
			free(contexts[idx].context.uc_stack.ss_sp);
			contexts[idx].context.uc_stack.ss_sp = NULL;
		}
        current_context_idx = 0;
        setcontext(&contexts[0].context);
    }
}
