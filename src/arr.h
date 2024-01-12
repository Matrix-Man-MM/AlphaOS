#ifndef ARR_H
#define ARR_H

#include "sys.h"

typedef void* type_t;
typedef signed char (*less_than_predicate_t)(type_t, type_t);

typedef struct
{
	type_t* arr;
	uint32_t size;
	uint32_t max_size;
	less_than_predicate_t less_than;

} ordered_arr_t;

signed char std_less_than_predicate(type_t a, type_t b);

ordered_arr_t create_ordered_arr(uint32_t max_size, less_than_predicate_t less_than);
ordered_arr_t place_ordered_arr(void* addr, uint32_t max_size, less_than_predicate_t less_than);
void destroy_ordered_arr(ordered_arr_t* arr);
void insert_ordered_arr(type_t item, ordered_arr_t* arr);
type_t loopup_ordered_arr(uint32_t i, ordered_arr_t* arr);
void remove_ordered_arr(uint32_t i, ordered_arr_t* arr);

#endif /* ARR_H */
