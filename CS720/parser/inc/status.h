#ifndef _STATUS_H_
#define _STATUS_H_

typedef enum
{
	SUCCESS = 0,
	OUT_OF_MEM,
	UNEXPECTED_CHAR,
	CMD_ARGS,
	FILE_OPEN_ERROR,
} status_t;

#endif
