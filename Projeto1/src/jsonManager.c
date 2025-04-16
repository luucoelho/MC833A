#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jsonManager.h"

#define jsonFile "movies.json"
#define maxMovies 100
#define bufferSize 4096

Movie movies[maxMovies]; // we have a maximum of 100 movies
int movieCount = 0;

// this goes over whitespaces in the string
static void goOverWhitespaces(const char** str) {
    while (isspace(**str)) {
        (*str)++;
    }
}

// This function is a helper to get a string out of a json string
// so, for example: "title": "The Dark Knight", the function will return "The Dark Knight"
static int jsonToString(const char** str, char* dest, size_t max_len) {
    if (**str != '"') return -1;
    (*str)++;
    
    size_t i = 0;
    while (**str != '"' && **str != '\0' && i < max_len - 1) {
        if (**str == '\\') {
            (*str)++;
            switch (**str) {
                case 'n': dest[i++] = '\n'; break;
                case 't': dest[i++] = '\t'; break;
                case 'r': dest[i++] = '\r'; break;
                case 'b': dest[i++] = '\b'; break;
                case 'f': dest[i++] = '\f'; break;
                case '\\': dest[i++] = '\\'; break;
                case '/': dest[i++] = '/'; break;
                case '"': dest[i++] = '"'; break;
                default: return -1;
            }
        } else {
            dest[i++] = **str;
        }
        (*str)++;
    }
    
    if (**str != '"') return -1;
    (*str)++;
    dest[i] = '\0';
    return 0;
}

// This function gets a number from a json string
// example: "release_year": 2008, returns 2008
static int jsonToNumber(const char** str, int* value) {
    char* end;
    *value = strtol(*str, &end, 10);
    if (end == *str) return -1;
    *str = end;
    return 0;
}

// This function parses a single movie from a json string
// example: {"id": 1, "title": "The Dark Knight", "director": "Christopher Nolan", "release_year": 2008, "genres": ["Action"]}
static int parse_movie_object(const char** str, Movie* movie) {
    if (**str != '{') return -1;
    (*str)++;
    
    while (**str != '}') {
        goOverWhitespaces(str);
        
        // Parse key
        char key[20];
        if (jsonToString(str, key, sizeof(key)) != 0) return -1;
        
        goOverWhitespaces(str);
        if (**str != ':') return -1;
        (*str)++;
        goOverWhitespaces(str);
        
        // Parse value based on key
        if (strcmp(key, "id") == 0) {
            if (jsonToNumber(str, (int *)&movie->id) != 0)
                return -1;
        } else if (strcmp(key, "title") == 0) {
            if (jsonToString(str, movie->title, maxTitleLenght) != 0) return -1;
        } else if (strcmp(key, "director") == 0) {
            if (jsonToString(str, movie->director, maxDirectorLenght) != 0) {
                return -1;
            }
        } else if (strcmp(key, "release_year") == 0) {
            if (jsonToNumber(str, (int *)&movie->releaseYear) != 0) {
                return -1;
            }
        } else if (strcmp(key, "genres") == 0) {
            if (**str != '[') return -1;
            (*str)++;
            
            movie->numGenres = 0;
            while (**str != ']') {
                goOverWhitespaces(str);

                if (movie->numGenres >= maxGenres) {
                    return -1;
                }
                
                if (jsonToString(str, movie->genres[movie->numGenres], maxGenreLenght) != 0) {
                    return -1;
                }

                movie->numGenres++;
                
                goOverWhitespaces(str);
                if (**str == ',') (*str)++;
            }
            (*str)++;
        }
        
        goOverWhitespaces(str);
        if (**str == ',') (*str)++;
    }
    
    (*str)++;
    return 0;
}

// Parse a single movie from JSON string
int parseMovie(const char* json_str, Movie* movie) {
    const char* ptr = json_str;
    goOverWhitespaces(&ptr);
    return parse_movie_object(&ptr, movie);
}

// This function generates a json string for a movie
static void generate_movie_json(const Movie* movie, char* buffer, size_t* offset) {
    *offset += snprintf(buffer + *offset, bufferSize - *offset,
        "{\"id\":%u,\"title\":\"%s\",\"director\":\"%s\",\"release_year\":%d,\"genres\":[",
        movie->id, movie->title, movie->director, movie->releaseYear);
    
    for (int i = 0; i < movie->numGenres; i++) {
        if (i > 0) {
            *offset += snprintf(buffer + *offset, bufferSize - *offset, ",");
        }
        *offset += snprintf(buffer + *offset, bufferSize - *offset, "\"%s\"", movie->genres[i]);
    }
    
    *offset += snprintf(buffer + *offset, bufferSize - *offset, "]}");
}

// Initialize JSON file if it doesn't exist
int initJsonFile() {
    FILE* file = fopen(jsonFile, "r");
    if (file) {
        char buffer[bufferSize];
        size_t len = fread(buffer, 1, bufferSize - 1, file);
        buffer[len] = '\0';
        fclose(file);
        
        const char* ptr = buffer;
        goOverWhitespaces(&ptr);
        
        if (*ptr != '{') return -1;
        ptr++;
        
        goOverWhitespaces(&ptr);
        if (strncmp(ptr, "\"movies\":[", 10) != 0) return -1;
        ptr += 10;
        
        movieCount = 0;
        while (*ptr != ']' && movieCount < maxMovies) {
            goOverWhitespaces(&ptr);
            if (parse_movie_object(&ptr, &movies[movieCount]) == 0) {
                movieCount++;
            }
            goOverWhitespaces(&ptr);
            if (*ptr == ',') ptr++;
        }
        
        return 0;
    }
    
    file = fopen(jsonFile, "w");
    if (!file) {
        perror("Failed to create JSON file");
        return -1;
    }
    
    fprintf(file, "{\"movies\":[]}");
    fclose(file);
    return 0;
}

// Save movies to JSON file
int saveMoviesToFile() {
    FILE* file = fopen(jsonFile, "w");
    if (!file) {
        perror("Failed to open JSON file for writing");
        return -1;
    }
    
    char buffer[bufferSize];
    size_t offset = 0;
    
    offset += snprintf(buffer + offset, bufferSize - offset, "{\"movies\":[");
    
    for (int i = 0; i < movieCount; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, bufferSize - offset, ",");
        }
        generate_movie_json(&movies[i], buffer, &offset);
    }
    
    offset += snprintf(buffer + offset, bufferSize - offset, "]}");
    
    fwrite(buffer, 1, offset, file);
    fclose(file);
    return 0;
}

// Add a new movie to the JSON file
int addMovie(const char* title, const char* director, int releaseYear) {
    if (movieCount >= maxMovies) {
        return -1;
    }
    
    movies[movieCount].id = movieCount + 1;
    strncpy(movies[movieCount].title, title, maxTitleLenght - 1);
    movies[movieCount].title[maxTitleLenght - 1] = '\0';
    
    strncpy(movies[movieCount].director, director, maxDirectorLenght - 1);
    movies[movieCount].director[maxDirectorLenght - 1] = '\0';
    
    movies[movieCount].releaseYear = releaseYear;
    movies[movieCount].numGenres = 0;
    
    movieCount++;
    return saveMoviesToFile();
}

// Add a genre to an existing movie
int addGenreToMovie(uint32_t movie_id, const char* genre) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i].id == movie_id) {
            if (movies[i].numGenres >= maxGenres) {
                return -1;
            }
            
            for (int j = 0; j < movies[i].numGenres; j++) {
                if (strcmp(movies[i].genres[j], genre) == 0) {
                    return 0;
                }
            }
            
            strncpy(movies[i].genres[movies[i].numGenres], genre, maxGenreLenght - 1);
            movies[i].genres[movies[i].numGenres][maxGenreLenght - 1] = '\0';
            movies[i].numGenres++;
            
            return saveMoviesToFile();
        }
    }
    return -1;
}

// Remove a movie by ID
int removeMovie(uint32_t movie_id) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i].id == movie_id) {
            for (int j = i; j < movieCount - 1; j++) {
                movies[j] = movies[j + 1];
            }
            movieCount--;
            return saveMoviesToFile();
        }
    }
    return -1;
}

// Get all movies
int getAllMovies(char* buffer) {
    size_t offset = 0;
    offset += snprintf(buffer + offset, bufferSize - offset, "{\"movies\":[");
    
    for (int i = 0; i < movieCount; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, bufferSize - offset, ",");
        }
        generate_movie_json(&movies[i], buffer, &offset);
    }
    
    offset += snprintf(buffer + offset, bufferSize - offset, "]}");
    return 0;
}

// Get a movie by ID
int getMovieById(uint32_t movie_id, Movie* movie) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i].id == movie_id) {
            *movie = movies[i];
            return 0;
        }
    }
    return -1;
}

// Get movies by genre
int getMoviesByGenre(const char* genre, Movie* resultMovies, int* count) {
    *count = 0;
    for (int i = 0; i < movieCount; i++) {
        for (int j = 0; j < movies[i].numGenres; j++) {
            if (strcmp(movies[i].genres[j], genre) == 0) {
                resultMovies[*count] = movies[i];
                (*count)++;
                break;
            }
        }
    }
    return 0;
}

// Get all movie titles with IDs
int getMovieTitles(char* buffer) {
    size_t offset = 0;
    offset += snprintf(buffer + offset, bufferSize - offset, "{\"movies\":[");
    
    for (int i = 0; i < movieCount; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, bufferSize - offset, ",");
        }
        offset += snprintf(buffer + offset, bufferSize - offset,
            "{\"id\":%u,\"title\":\"%s\"}", movies[i].id, movies[i].title);
    }
    
    offset += snprintf(buffer + offset, bufferSize - offset, "]}");
    return 0;
} 