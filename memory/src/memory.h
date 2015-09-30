#ifndef MEMORY_H_
#define MEMORY_H_

	#include <socket.h>

	void initMemory(t_config* memoryConfig);

	typedef struct
	{
		int id;
		t_list * pages;

	} t_process;

	typedef struct
	{
		int num;
		bool present;
		bool modified;
		int frame;
	} t_page;

	typedef struct
	{
		int pid;
		int page;
		char * data;
		bool arriving;
		socket_connection * connection;

	} t_write_petition;

	typedef struct
	{
		int pid;
		int page;
		bool arriving;
		socket_connection * connection;
	} t_read_petition;

	typedef struct
	{
		int pid;
		int page;
		int frame;
	} t_translation;

#endif
