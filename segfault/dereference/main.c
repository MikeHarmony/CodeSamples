#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

struct conf {
	char addr[128];
	int port;
} config = { .addr = "127.0.0.1", .port = 8080};


int main(void)
{
	int my_sock, len = 32;
	struct sockaddr_in addr;
	char *msg = NULL, *recv = NULL;

	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0){
		printf("Unable to create socket\n");
		return -1;
	}
	printf("Created\n");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(config.port);
	addr.sin_addr.s_addr = inet_addr(config.addr);

	if (connect(my_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		printf("Unable to connect to socket\n");
		return -1;
	}
	printf("Connected\n");
	
	msg = calloc(len, sizeof(msg));
	if (msg == NULL) {
		printf("Unable to allocate memory\n");
		return -1;
	}
	snprintf(msg, len, "Hello World");

	printf("TX[%s]\n", msg);
	if (write(my_sock, msg, strlen(msg)) < 0) {
		printf("Unable to write to socket\n");
		return -1;
	}
	
	recv = calloc(len, sizeof(recv));
	if (recv == NULL) {
		printf("Unable to allocate memory\n");
		return -1;
	}

	int bytes = 0;
	if ((bytes = read(my_sock, &recv, len)) < 0) {
		printf("Unable to read from socket\n");
		return -1;
	}
	recv[bytes] = '\0';
	printf("RX[%d][%s]\n", bytes, recv);
	
	free(msg);
	free(recv);

	close(my_sock);

	return 0;
}

