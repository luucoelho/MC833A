#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

// --- Constants for maximum values ----

// Buffer size
#define bufferSize 4096

// Max length for movie titles
#define maxTitleLenght 100

// Max length for genre names
#define maxGenreLenght 50

// Max length for director names
#define maxDirectorLenght 100

// Max number of genres per movie
#define maxGenres 10

// Max number of movies
#define maxMovies 100

// --- Response codes ---

/**
 * ResponseCode
 * These help to understand the response codes for the server responses.
 */
typedef enum {
    success = 0,           /**< Operation successful */
    error = 1,             /**< General error occurred */
    notFound = 2,   /**< Requested movie not found */
    invalidOperation = 3  /**< Invalid operation code */
} ResponseCode;

/**
 * Movie
 * Structure representing a movie in the database.
 */
typedef struct {
    uint32_t id; // movie identifier
    char title[maxTitleLenght]; // Movie title
    char genres[maxGenres][maxGenreLenght]; // Movie genres
    int numGenres; // number of genres assigned to the movie
    char director[maxDirectorLenght]; // Movie director
    uint16_t releaseYear; // Year of release for movie
} Movie;

// Request structure
typedef struct {
    int operationCode; // Operation to perform
    uint32_t movieID; // the id of the movie
    char title[maxTitleLenght]; // movie title
    char genre[maxGenreLenght]; // genre of the movie
    char director[maxDirectorLenght]; // director name
    uint16_t releaseYear; // movie year
} Request;

// Response structure
typedef struct {
    ResponseCode responseCode; // Response status code
    uint32_t numMovies; // Number of movies in the response
    Movie movies[maxMovies]; // Array of movies (max 100)
} Response;

// Serialize a request into a bufer
void serializeRequest(const Request* req, char* buffer);

// Deserialize a buffer into a request structure
void deserializeRequest(const char* buffer, Request* req);

// Serialize a response into a buffer
void serializeResponse(const Response* resp, char* buffer);

// Deserialize a buffer into a response structure
void deserializeResponse(const char* buffer, Response* resp);

#endif // COMMON_H 