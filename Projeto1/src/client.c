#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "common.h"

void printUserMenu() {
    printf("\nBanco de Dados de Filmes\n");
    printf("1. Cadastrar novo filme\n");
    printf("2. Adicionar gênero a um filme\n");
    printf("3. Remover filme por id\n");
    printf("4. Listar todos os filmes\n");
    printf("5. Pesquisar filme por id\n");
    printf("6. Listar filmes por gênero\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

void listAllMovies(int socket) {
    Request req;
    Response resp;
    char buffer[bufferSize];

    req.operationCode = 4; // Lista todos os filmes
    serializeRequest(&req, buffer);
    send(socket, buffer, bufferSize, 0);

    recv(socket, buffer, bufferSize, 0);
    deserializeResponse(buffer, &resp);

    if (resp.responseCode == success) {
        printf("\n--- Lista de Filmes ---\n\n");
        for (uint32_t i = 0; i < resp.numMovies; i++) {
            Movie* movie = &resp.movies[i];
            printf("ID: %u\n", movie->id);
            printf("Título: %s\n", movie->title);
            printf("Diretor: %s\n", movie->director);
            printf("Ano: %hu\n", movie->releaseYear);
            if (movie->numGenres > 0) {
                printf("Gêneros: ");
                for (int j = 0; j < movie->numGenres; j++) {
                    printf("%s%s", movie->genres[j], j < movie->numGenres - 1 ? ", " : "\n");
                }
            } else {
                printf("Nenhum gênero cadastrado\n");
            }
            printf("\n");
        }
    } else {
        printf("Erro ao listar filmes.\n");
    }
}

void listMoviesByGenre(int socket)
{
    Request req;
    req.operationCode = 6; // Listar filmes por gênero
    printf("Digite o gênero: ");
    scanf(" %[^\n]", req.genre);

    char buffer[bufferSize];
    serializeRequest(&req, buffer);
    send(socket, buffer, bufferSize, 0);

    Response resp;
    recv(socket, buffer, bufferSize, 0);
    deserializeResponse(buffer, &resp);

    if (resp.responseCode == success)
    {
        printf("\n--- Filmes do gênero %s ---\n", req.genre);
        for (uint32_t i = 0; i < resp.numMovies; i++)
        {
            Movie *movie = &resp.movies[i];
            printf("\nID: %u\n", movie->id);
            printf("Título: %s\n", movie->title);
            printf("Diretor: %s\n", movie->director);
            printf("Ano: %hu\n", movie->releaseYear);
        }
    }
    else
    {
        printf("Nenhum filme encontrado com este gênero.\n");
    }
}

void listMovieById(int socket) {
    Request req;
    req.operationCode = 5; // Listar filmes por Id
    printf("Digite o ID do filme: ");
    scanf("%u", &req.movieID);

    Response resp;
    char buffer[bufferSize];
    serializeRequest(&req, buffer);
    send(socket, buffer, bufferSize, 0);

    recv(socket, buffer, bufferSize, 0);
    deserializeResponse(buffer, &resp);

    if (resp.responseCode == success && resp.numMovies > 0) {
        Movie* movie = &resp.movies[0];
        printf("\nID: %u\n", movie->id);
        printf("Título: %s\n", movie->title);
        printf("Diretor: %s\n", movie->director);
        printf("Ano: %hu\n", movie->releaseYear);
        printf("Gêneros: ");
        for (int j = 0; j < movie->numGenres; j++) {
            printf("%s%s", movie->genres[j], j < movie->numGenres - 1 ? ", " : "\n");
        }
    } else if (resp.responseCode == notFound) {
        printf("Filme não encontrado.\n");
    } else {
        printf("Erro ao buscar filme.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    printf("Iniciando cliente...\n");
    printf("Conectando ao server %s:%s\n", argv[1], argv[2]);

    // Criando o socket
    printf("Criando socket...\n");
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Erro ao criar o socket: %s\n", strerror(errno));
        return 1;
    }
    printf("Socket criado (fd: %d)!!!\n", clientSocket);

    printf("Configurando endereço do servidor...\n");
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    
    printf("Convertendo endereço IP...\n");
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        printf("Endereço inválido: %s\n", strerror(errno));
        close(clientSocket);
        return 1;
    }
    printf("Endereço do servidor configurado!\n");

    printf("Conectando ao servidor...\n");
    int connect_result = connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (connect_result < 0) {
        printf("Erro ao conectar: %s\n", strerror(errno));
        printf("Código do erro: %d\n", errno);
        close(clientSocket);
        return 1;
    }

    printf("Conectado ao servidor %s:%s\n", argv[1], argv[2]);

    int choice;
    do {
        printUserMenu();
        scanf("%d", &choice);

        switch (choice) {
        case 1: { // cadastrar novo filme
            Request req;
            req.operationCode = 1;

            Response resp;

            printf("Digite o título do filme: ");
            scanf(" %[^\n]", req.title);

            printf("Digite o nome do diretor: ");
            scanf(" %[^\n]", req.director);

            printf("Digite o ano de lançamento: ");
            scanf("%hu", &req.releaseYear);

            printf("Digite o gênero do filme: ");
            scanf(" %[^\n]", req.genre);

            char buffer[bufferSize];

            // send request
            serializeRequest(&req, buffer);
            send(clientSocket, buffer, bufferSize, 0);

            // response
            recv(clientSocket, buffer, bufferSize, 0);
            deserializeResponse(buffer, &resp);
            if (resp.responseCode == success) {
                printf("Filme cadastrado com sucesso!\n");
            } else {
                printf("Erro ao cadastrar filme.\n");
            }
            break;
        }

        case 2: { // adicionar genero a um filme
            Request req;
            req.operationCode = 2;

            printf("Digite o ID do filme: ");
            scanf("%u", &req.movieID);

            printf("Digite o gênero a ser adicionado: ");
            scanf(" %[^\n]", req.genre);

            char buffer[bufferSize];

            serializeRequest(&req, buffer);
            send(clientSocket, buffer, bufferSize, 0);

            // response handling
            Response resp;

            recv(clientSocket, buffer, bufferSize, 0);
            deserializeResponse(buffer, &resp);
            if (resp.responseCode == success) {
                printf("Gênero adicionado com sucesso!\n");
            } else if (resp.responseCode == notFound) {
                printf("Filme não encontrado.\n");
            } else {
                printf("Erro ao adicionar gênero.\n");
            }
            break;
        }

        case 3: { // remover filme pelo id
            Request req;
            req.operationCode = 3;

            printf("Digite o ID do filme a ser removido: ");
            scanf("%u", &req.movieID);

            char buffer[bufferSize];
            serializeRequest(&req, buffer);
            send(clientSocket, buffer, bufferSize, 0);

            // response
            Response resp;

            recv(clientSocket, buffer, bufferSize, 0);
            deserializeResponse(buffer, &resp);
            if (resp.responseCode == success) {
                printf("Filme removido com sucesso!\n");
            } else if (resp.responseCode == notFound) {
                printf("Filme não encontrado.\n");
            } else {
                printf("Erro ao remover filme.\n");
            }
            break;
        }

        case 4: // listar todos os filmes
            listAllMovies(clientSocket);
            break;

        case 5: // listar filme pelo id
            listMovieById(clientSocket);
            break;

        case 6: // listar filmes por genero 
            listMoviesByGenre(clientSocket);
            break;

        case 0:
            printf("Saindo...\n");
            break;

        default:
            printf("Opção inválida. Tente novamente.\n");
            break;
        }
    } while (choice != 0);

    close(clientSocket);
    return 0;
}