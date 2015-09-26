#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

	void initConnections(t_config* memoryConfig);

	void cpuDisconnected(socket_connection* socketInfo);

	void cpuNew(socket_connection* socketInfo);

	void listenStart();

	void swDisconnected(socket_connection* socketInfo);

	void connectSwap();

#endif
