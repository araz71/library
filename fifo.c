#include <fifo.h>

#define FIFO_MAX_AVAL	4

#pragma pack(push)
#pragma pack(1)
struct Fifo_st {
	int front;
	int rear;
	uint8_t *ptr;
	uint16_t size;
	uint16_t max_elem_aval;
} Fifo[FIFO_MAX_AVAL];
#pragma pack(pop)

uint8_t FifoCntr = 0;

uint8_t fifo_req(uint8_t *_ptr, uint16_t _size, uint16_t _max_elem_aval) {
	if (FifoCntr == FIFO_MAX_AVAL)
		return 0xFF;
	Fifo[FifoCntr].front = 0;
	Fifo[FifoCntr].rear = 0;
	Fifo[FifoCntr].ptr = _ptr;
	Fifo[FifoCntr].size = _size;
	Fifo[FifoCntr].max_elem_aval = _max_elem_aval;

	return FifoCntr++;
}

uint8_t fifo_push(uint8_t _fifo_id, uint8_t *_data) {
	if (fifo_full(_fifo_id)) {
		return 0;
	}
	Fifo[_fifo_id].rear = (Fifo[_fifo_id].rear + 1)
			% Fifo[_fifo_id].max_elem_aval;
	memcpy(Fifo[_fifo_id].ptr + (Fifo[_fifo_id].rear * Fifo[_fifo_id].size),
			_data, Fifo[_fifo_id].size);
	return 1;
}
uint8_t fifo_full(uint8_t _fifo_id) {
	if (Fifo[_fifo_id].front
			== ((Fifo[_fifo_id].rear + 1) % Fifo[_fifo_id].max_elem_aval))
		return 1;
	return 0;
}
uint8_t fifo_empty(uint8_t _fifo_id) {
	if (Fifo[_fifo_id].front == Fifo[_fifo_id].rear)
		return 1;
	return 0;
}
uint8_t *fifo_pop(uint8_t _fifo_id) {
	if (fifo_empty(_fifo_id))
		return 0;
	Fifo[_fifo_id].front = (Fifo[_fifo_id].front + 1) % Fifo[_fifo_id].max_elem_aval;
	return (Fifo[_fifo_id].ptr + (Fifo[_fifo_id].front * Fifo[_fifo_id].size));
}

uint8_t *fifo_pop_stay(uint8_t _fifo_id) {
	if (fifo_empty(_fifo_id))
		return 0;
	uint16_t front = Fifo[_fifo_id].front;
	front = (front + 1) % Fifo[_fifo_id].max_elem_aval;
	return (Fifo[_fifo_id].ptr + (front * Fifo[_fifo_id].size));
}

void fifo_free_elem(uint8_t _fifo_id) {
	fifo_pop(_fifo_id);
}

uint16_t fifo_size(uint8_t _fifo_id) {
	if (Fifo[_fifo_id].rear < Fifo[_fifo_id].front)
		return (Fifo[_fifo_id].max_elem_aval - Fifo[_fifo_id].front
				+ Fifo[_fifo_id].rear);
	else
		return (Fifo[_fifo_id].rear - Fifo[_fifo_id].front);
}

void fifo_clear(uint8_t _fifo_id) {
	Fifo[_fifo_id].front = Fifo[_fifo_id].rear = 0;
}

