#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "common.h"
#include "jsonManager.h"

// port num and max number of clients
#define portNumber 8080

pthread_mutex_t jsonMutex = PTHREAD_MUTEX_INITIALIZER;

// This function handles the client requests
// it gets the requests, acts on it, and then sends the 
// response back to client
void* handle_client(void* arg) {
    int clientSocket = *(int*)arg;
    char buffer[bufferSize];
    Request req;
    Response resp;

    while (1) {
        // Receive request
        ssize_t receivedBytes = recv(clientSocket, buffer, bufferSize, 0);
        if (receivedBytes <= 0)
        {
            break;
        }

        // Deserialize request
        deserializeRequest(buffer, &req);

        // Process request based on operation code
        pthread_mutex_lock(&jsonMutex);
        switch (req.operationCode) {
            case 1: { // Cadastrar novo filme
                if (addMovie(req.title, req.director, req.releaseYear) == 0) {
                    if (addGenreToMovie(movieCount, req.genre) == 0) {
                        resp.responseCode = success;
                    } else {
                        resp.responseCode = error;
                    }
                } else {
                    resp.responseCode = error;
                }
                break;
            }
            case 2: { // Adicionar gênero a um filme
                if (addGenreToMovie(req.movieID, req.genre) == 0) {
                    resp.responseCode = success;
                } else {
                    resp.responseCode = notFound;
                }
                break;
            }
            case 3: { // Remover filme
                if (removeMovie(req.movieID) == 0) {
                    resp.responseCode = success;
                } else {
                    resp.responseCode = notFound;
                }
                break;
            }
            case 4: { // Listar todos os filmes
                resp.responseCode = success;
                resp.numMovies = movieCount;
                
                // Copy all movies directly from the global movies array
                for (int i = 0; i < movieCount; i++) {
                    resp.movies[i] = movies[i];
                }
                break;
            }
            case 5: { // Mostrar filme por ID
                Movie movie;
                if (getMovieById(req.movieID, &movie) == 0) {
                    resp.responseCode = success;
                    resp.numMovies = 1;
                    resp.movies[0] = movie;

                } else {
                    resp.responseCode = notFound;
                }
                break;
            }
            case 6: { // Listar filmes por gênero
                int count;
                if (getMoviesByGenre(req.genre, resp.movies, &count) == 0) {
                    resp.responseCode = success;
                    resp.numMovies = count;

                } else {
                    resp.responseCode = error;
                }

                break;
            }
            default:
                resp.responseCode = invalidOperation;
                break;
        }

        // unlock mutex
        pthread_mutex_unlock(&jsonMutex);

        // Serialize and send response
        serializeResponse(&resp, buffer);
        send(clientSocket, buffer, bufferSize, 0);
    }

    close(clientSocket);
    free(arg);
    return NULL;
}

int main() {
    printf("Starting server...\n");
    
    // Initializing JSON file
    printf("Initializing JSON file...\n");
    if (initJsonFile() != 0) {
        printf("JSON file initialization has failed :(\n");
        return 1;
    }
    printf("JSON file initialized successfully!\n");

    // Create socket
    printf("Creating server socket...\n");
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed :(");
        return 1;
    }
    printf("Server socket created successfully!\n");

    // Configure socket
    printf("Configuring server socket...\n");
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNumber);

    // Bind socket
    printf("Binding server socket to port %d...\n", portNumber);
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed :(");
        return 1;
    }
    printf("Server socket bound successfully!\n");

    // Retrieve and display the actual server IP address
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("Error getting hostname");
        return 1;
    }

    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("Error getting server IP address");
        return 1;
    }

    char ip[INET_ADDRSTRLEN];
    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
        printf("Server IP: %s\n", ip);
        break; // Use the first valid IP address
    }

    freeaddrinfo(res);

    // Listen for connections
    printf("Starting to listen for connections...\n");
    if (listen(serverSocket, 10) < 0) { // 10 to be the max number of clients
        perror("Listen failed :(");
        return 1;
    }

    printf("Server is now listening on port %d!\n", portNumber);

    // Accept connections
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int* clientSocket = malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (*clientSocket < 0) {
            perror("Accept failed");
            free(clientSocket);
            continue;
        }
        
        printf("New connection from %s:%d\n", 
               inet_ntoa(clientAddr.sin_addr), 
               ntohs(clientAddr.sin_port));
        
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, clientSocket) != 0) {
            perror("Thread creation failed");
            close(*clientSocket);
            free(clientSocket);
        }
        
        pthread_detach(thread);
    }
    
    close(serverSocket);
    return 0;
} 