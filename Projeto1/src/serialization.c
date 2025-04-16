#include <string.h>
#include "common.h"

void serializeRequest(const Request* req, char* buffer) {
    int offset = 0;
    
    // Serialize operation code
    memcpy(buffer + offset, &req->operationCode, sizeof(int));
    offset += sizeof(int);
    
    // Serialize movie ID
    memcpy(buffer + offset, &req->movieID, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // Serialize title
    strncpy(buffer + offset, req->title, maxTitleLenght);
    offset += maxTitleLenght;
    
    // Serialize genre
    strncpy(buffer + offset, req->genre, maxGenreLenght);
    offset += maxGenreLenght;
    
    // Serialize director
    strncpy(buffer + offset, req->director, maxDirectorLenght);
    offset += maxDirectorLenght;
    
    // Serialize release year
    memcpy(buffer + offset, &req->releaseYear, sizeof(uint16_t));
}

void deserializeRequest(const char* buffer, Request* req) {
    int offset = 0;
    
    // Deserialize operation code
    memcpy(&req->operationCode, buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Deserialize movie ID
    memcpy(&req->movieID, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // Deserialize title
    strncpy(req->title, buffer + offset, maxTitleLenght);
    offset += maxTitleLenght;
    
    // Deserialize genre
    strncpy(req->genre, buffer + offset, maxGenreLenght);
    offset += maxGenreLenght;
    
    // Deserialize director
    strncpy(req->director, buffer + offset, maxDirectorLenght);
    offset += maxDirectorLenght;
    
    // Deserialize release year
    memcpy(&req->releaseYear, buffer + offset, sizeof(uint16_t));
}

void serializeResponse(const Response* resp, char* buffer) {
    int offset = 0;
    
    // Serialize response code
    memcpy(buffer + offset, &resp->responseCode, sizeof(ResponseCode));
    offset += sizeof(ResponseCode);
    
    // Serialize number of movies
    memcpy(buffer + offset, &resp->numMovies, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // Serialize each movie
    for (uint32_t i = 0; i < resp->numMovies; i++) {
        const Movie* movie = &resp->movies[i];
        
        // Serialize movie ID
        memcpy(buffer + offset, &movie->id, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Serialize title
        strncpy(buffer + offset, movie->title, maxTitleLenght);
        offset += maxTitleLenght;
        
        // Serialize number of genres
        memcpy(buffer + offset, &movie->numGenres, sizeof(int));
        offset += sizeof(int);
        
        // Serialize genres
        for (int j = 0; j < movie->numGenres; j++) {
            strncpy(buffer + offset, movie->genres[j], maxGenreLenght);
            offset += maxGenreLenght;
        }
        
        // Fill remaining genre slots with null terminators
        for (int j = movie->numGenres; j < maxGenres; j++) {
            memset(buffer + offset, 0, maxGenreLenght);
            offset += maxGenreLenght;
        }
        
        // Serialize director
        strncpy(buffer + offset, movie->director, maxDirectorLenght);
        offset += maxDirectorLenght;
        
        // Serialize release year
        memcpy(buffer + offset, &movie->releaseYear, sizeof(uint16_t));
        offset += sizeof(uint16_t);
    }
}

void deserializeResponse(const char* buffer, Response* resp) {
    int offset = 0;
    
    // Deserialize response code
    memcpy(&resp->responseCode, buffer + offset, sizeof(ResponseCode));
    offset += sizeof(ResponseCode);
    
    // Deserialize number of movies
    memcpy(&resp->numMovies, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // Deserialize each movie
    for (uint32_t i = 0; i < resp->numMovies; i++) {
        Movie* movie = &resp->movies[i];
        
        // Deserialize movie ID
        memcpy(&movie->id, buffer + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        // Deserialize title
        strncpy(movie->title, buffer + offset, maxTitleLenght);
        offset += maxTitleLenght;
        
        // Deserialize number of genres
        memcpy(&movie->numGenres, buffer + offset, sizeof(int));
        offset += sizeof(int);
        
        // Deserialize genres
        for (int j = 0; j < movie->numGenres; j++) {
            strncpy(movie->genres[j], buffer + offset, maxGenreLenght);
            offset += maxGenreLenght;
        }
        
        // Skip remaining genre slots
        offset += (maxGenres - movie->numGenres) * maxGenreLenght;
        
        // Deserialize director
        strncpy(movie->director, buffer + offset, maxDirectorLenght);
        offset += maxDirectorLenght;
        
        // Deserialize release year
        memcpy(&movie->releaseYear, buffer + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
    }
} 