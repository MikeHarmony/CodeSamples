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


struct headers {
	char value[256];
};

/* 
RX[269][HTTP/1.1 200 OK
Date: Mon, 16 Aug 2021 17:21:34 GMT
Server: Hydra/0.1.8
Accept-Ranges: bytes
Connection: Keep-Alive
Keep-Alive: timeout=10, max=100
Content-Length: 8028
Last-Modified: Tue, 01 Oct 2019 08:26:15 GMT
ETag: "8028-18375"
Content-Type: text/html

]
*/
void handle_response(char *data, int data_len)
{
	char *tok = NULL, delimiter[] = "\r\n";
	
	struct headers resp_headers[8];
	
	printf("RX[%d][%s]\n", data_len, data);
	
	tok = strtok(data, delimiter);
	
	int count = 0;
	while (tok) {
		snprintf(resp_headers[count].value, sizeof(resp_headers[count].value), "%s", tok);
		
		tok = strtok(NULL, delimiter);
		count++;
	}
}

int main(void)
{
	int my_sock, len = 512;
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
	snprintf(msg, len, "HEAD / HTTP/1.1\r\nHost: %s:%d\r\nUser-Agent: MyApp/1.0.0\r\nAccept: */*\r\n\r\n", config.addr, config.port);

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
	if ((bytes = read(my_sock, recv, len)) < 0) {
		printf("Unable to read from socket\n");
		return -1;
	}
	recv[bytes] = '\0';
	
	handle_response(recv, bytes);
	
	free(msg);
	free(recv);

	close(my_sock);

	return 0;
}

