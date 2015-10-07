#ifndef CONNCOMMANDS_H_
#define CONNCOMMANDS_H_

	void schedulerStartProcess(socket_connection * connection, char ** args);

	void memoryStartProcessOk(socket_connection * connection, char ** args);
	void memoryNoFrames(socket_connection * connection, char ** args);
	void memoryNoSpace(socket_connection * connection, char ** args);
	void memoryFrameData(socket_connection * connection, char ** args);
	void memoryWriteOk(socket_connection * connection, char ** args);

#endif