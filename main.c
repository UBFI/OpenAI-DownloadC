#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
  // Check the number of arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <URL> <output file>\n", argv[0]);
    return 1;
  }

  // Parse the URL
  char *url = argv[1];
  char *protocol = strtok(url, "://");
  char *host = strtok(NULL, "/");
  char *path = strtok(NULL, "?");

  // Check the protocol
  if (strcmp(protocol, "http") != 0) {
    fprintf(stderr, "Error: only HTTP is supported\n");
    return 1;
  }

  // Get the hostname and port number
  char *hostname = host;
  int port = 80;
  char *port_str = strchr(host, ':');
  if (port_str != NULL) {
    *port_str = '\0';
    port_str++;
    port = atoi(port_str);
  }

  // Get the host's IP address
  struct hostent *server = gethostbyname(hostname);
  if (server == NULL) {
    fprintf(stderr, "Error: invalid hostname\n");
    return 1;
  }
  struct in_addr **addresses = (struct in_addr **)server->h_addr_list;

  // Create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Error opening socket");
    return 1;
  }

  // Connect to the server
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  memcpy(&serv_addr.sin_addr, addresses[0], sizeof(struct in_addr));
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error connecting to server");
    return 1;
  }

  // Send the HTTP request
  char request[1024];
  snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, hostname);
  if (write(sockfd, request, strlen(request)) < 0) {
    perror("Error writing to socket");
    return 1;
  }

  // Receive the HTTP response
  char response[65536
