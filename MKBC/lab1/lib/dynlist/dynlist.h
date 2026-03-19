#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct DynlistDescriptor DynlistDescriptor;
typedef struct DynlistNode DynlistNode;
typedef struct DynlistDescriptor *Dynlist;
typedef void (*dynlist_pval_func_t)(void *val);
typedef Dynlist istack_handle;   // дескриптор стека
__attribute__((warn_unused_result)) Dynlist dynlist_new(dynlist_pval_func_t clean_val_func);
__attribute__((warn_unused_result)) void* dynlist_poplast(Dynlist list);
void dynlist_free(Dynlist plist);
void dynlist_append(Dynlist list, void *value);
void dynlist_prepend(Dynlist list, void *value);
void dynlist_foreach(Dynlist list, dynlist_pval_func_t do_func);
struct DynlistNode *dynlist_begin(Dynlist list);
struct DynlistNode *dynlist_next(struct DynlistNode *node);
void *dynlist_value(struct DynlistNode *node);

istack_handle istack_new(void);
void istack_push(istack_handle handle, int value);
int istack_pop(istack_handle handle);
void istack_free(istack_handle handle);
#ifdef __cplusplus
}
#endif