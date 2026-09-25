#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main( int argc, char *argv[] ){
  if (argc < 4){
    fprintf(stderr, "Usage: %s <server_ip> <server_port> <message>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *server_ip = argv[1];
  int server_port = atoi(argv[2]);
  char *message = argv[3];

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);

  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    close(sockfd);
    exit(EXIT_FAILURE);

  }

  printf("Sending message to %s:%d: %s\n", server_ip, server_port, message);

  ssize_t sent = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (sent < 0) {
    perror("sendto error");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];
  struct sockaddr_in src_addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)&src_addr, &addrlen);

  if (n < 0) {
    perror("recvfrom error");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  buffer[n] = '\0';
  char sender_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(src_addr.sin_addr), sender_ip, sizeof(sender_ip));
  printf("Received message from %s:%d: %s\n", sender_ip, ntohs(src_addr.sin_port), buffer);

  close(sockfd);
  return 0;


}