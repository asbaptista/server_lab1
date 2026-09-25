#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int PORT = atoi(argv[1]);

  int sockfd;
  char buffer[BUFFER_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }


  for(;;){
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)&src_addr, &addrlen);

    if (n < 0) {
      perror("recvfrom error");
      continue;
    }

    buffer[n] = '\0'; // Null-terminate the received data

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(src_addr.sin_addr), client_ip, sizeof(client_ip));
    printf("recieved from %s:%d: %s\n", client_ip, ntohs(src_addr.sin_port), buffer);




    ssize_t m = sendto(sockfd, buffer, n, 0, (struct sockaddr *)&src_addr, addrlen);

    if (m < 0) {
      perror("sendto error");
      continue;
    }
  }
  close(sockfd);
  return 0;





}


