#ifndef JSON_MANAGER_H
#define JSON_MANAGER_H

#include "common.h"

#define jsonFile "movies.json"
#define maxMovies 100
#define bufferSize 4096

extern int movieCount; // Make movieCount accessible from other files
extern Movie movies[maxMovies]; // Make movies array accessible from other files

// Initialize the JSON file
int initJsonFile();

// Add a new movie to the database
int addMovie(const char* title, const char* director, int release_year);

// Add a genre to an existing movie
int addGenreToMovie(uint32_t movie_id, const char* genre);

// Remove a movie from the database
int removeMovie(uint32_t movie_id);

// Get all movies in the database
int getAllMovies(char* buffer);

// Get all movie titles with their IDs
int getMovieTitles(char* buffer);

// Get a specific movie by ID
int getMovieById(uint32_t movie_id, Movie* movie);

// Get all movies of a specific genre
int getMoviesByGenre(const char* genre, Movie* movies, int* count);

// Parse a single movie from JSON string
int parseMovie(const char* json_str, Movie* movie);

// Save all movies to the JSON file
int saveMoviesToFile();

#endif 