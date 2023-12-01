#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLS 100

// Structure to represent an arrow in the maze
typedef struct {
    char color;
    char dir;
} Arrow;

// Structure to represent a vertex in the maze
typedef struct {
    int row;
    int col;
} Vertex;

// Function prototypes
bool solveMaze(Arrow maze[MAX_ROWS][MAX_COLS], int rows, int cols, char path[MAX_ROWS * MAX_COLS][3]);
bool DFS(Arrow maze[MAX_ROWS][MAX_COLS], Vertex current, Vertex target, bool visited[MAX_ROWS][MAX_COLS], int rows, int cols, int *pathIndex, char path[MAX_ROWS * MAX_COLS][3]);
bool canMove(Arrow currentArrow, Arrow nextArrow);
void writePathToFile(char path[MAX_ROWS * MAX_COLS][3], int pathIndex);

// Helper functions
int directionToIndex(char dir);
Vertex moveVertex(Vertex v, int dirIndex);

int main() {
    Arrow maze[MAX_ROWS][MAX_COLS];
    char path[MAX_ROWS * MAX_COLS][3];  // Each move is represented as a string (e.g., "E", "SE")
    int rows, cols;

    // Read input from file
    FILE *inputFile = fopen("tiny.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    if (fscanf(inputFile, "%d %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
        fprintf(stderr, "Invalid maze dimensions\n");
        fclose(inputFile);
        return 1;
    }

    // Read maze
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char color;
            
            char dir1, dir2;
            if(i == rows-1 && j==cols-1){
                 if (fscanf(inputFile, " %c", &dir1) != 1 || dir1 != 'O') {
                    fprintf(stderr, "Expected bull's-eye 'O' at the end of the maze\n");
                    fclose(inputFile);
                    return 1;
            }
            maze[i][j].dir = dir1;
            break;
            }
            int count = fscanf(inputFile, " %c-%c", &color, &dir1);
            
            if (count == 2) {
                dir2 = '\0'; // Null-terminate in case it's a single direction
                count += fscanf(inputFile, "%c", &dir2); // Try to read a second direction character
                printf("%d",count);
                if (count == 3 && dir2 != ' ' && dir2 != '\n') { // Check if a second direction character was actually read
                    if (dir1 == 'N' && dir2 == 'E') maze[i][j].dir = '1';
                    else if (dir1 == 'S' && dir2 == 'E') maze[i][j].dir = '2';
                    else if (dir1 == 'S' && dir2 == 'W') maze[i][j].dir = '3';
                    else if (dir1 == 'N' && dir2 == 'W') maze[i][j].dir = '4';
                    else {
                        fprintf(stderr, "Invalid direction at position (%d, %d)\n", i, j);
                        fclose(inputFile);
                        return 1;
                    }
                } else if (count == 3 && (dir2 == ' ' || dir2 == '\n')) { // Single direction character was read
                    maze[i][j].dir = dir1;
                } else {
                    fprintf(stderr, "Invalid arrow data at position (%d, %d)\n", i, j);
                    fclose(inputFile);
                    return 1;
                }
            } else {
                fprintf(stderr, "Invalid arrow color at position (%d, %d)\n", i, j);
                fclose(inputFile);
                return 1;
            }
            maze[i][j].color = color;
            
        }
    }

    fclose(inputFile);

    // Solve maze and write output to file
    if (solveMaze(maze, rows, cols, path)) {
        printf("Maze solved successfully!\n");
    } else {
        printf("No solution found.\n");
    }

    return 0;
}

bool solveMaze(Arrow maze[MAX_ROWS][MAX_COLS], int rows, int cols, char path[MAX_ROWS * MAX_COLS][3]) {
    bool visited[MAX_ROWS][MAX_COLS] = {false};
    int pathIndex = 0;
    Vertex start = {0, 0};
    Vertex target = {rows - 1, cols - 1};

    if (DFS(maze, start, target, visited, rows, cols, &pathIndex, path)) {
        writePathToFile(path, pathIndex);
        return true;
    }

    return false;
}

bool DFS(Arrow maze[MAX_ROWS][MAX_COLS], Vertex current, Vertex target, bool visited[MAX_ROWS][MAX_COLS], int rows, int cols, int *pathIndex, char path[MAX_ROWS * MAX_COLS][3]) {
    if (current.row == target.row && current.col == target.col) {
        return true;  // Reached the target
    }

    visited[current.row][current.col] = true;

    // Possible directions N, E, S, W, NE, SE, SW, NW
    const char* directions = "NESWNESENWSWNW";
    for (int i = 0; i < 8; ++i) {
        Vertex next = moveVertex(current, i);
        if (next.row >= 0 && next.row < rows && next.col >= 0 && next.col < cols &&
            !visited[next.row][next.col] && canMove(maze[current.row][current.col], maze[next.row][next.col])) {
            path[*pathIndex][0] = directions[i * 2];
            path[*pathIndex][1] = directions[i * 2 + 1];
            path[*pathIndex][2] = '\0';
            (*pathIndex)++;
            if (DFS(maze, next, target, visited, rows, cols, pathIndex, path)) {
                return true;
            }
            (*pathIndex)--;  // Backtrack
        }
    }

    visited[current.row][current.col] = false;
    return false;
}

bool canMove(Arrow currentArrow, Arrow nextArrow) {
    // Check if we can move from currentArrow to nextArrow based on their colors
    return currentArrow.color != nextArrow.color;
}

void writePathToFile(char path[MAX_ROWS * MAX_COLS][3], int pathIndex) {
    FILE *outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return;
    }

    for (int i = 0; i < pathIndex; ++i) {
        fprintf(outputFile, "%s ", path[i]);
    }

    fclose(outputFile);
}

int directionToIndex(char dir) {
    switch (dir) {
        case 'N': return 0;
        case 'E': return 1;
        case 'S': return 2;
        case 'W': return 3;
        case '1': return 4;
        case '2': return 5;
        case '3': return 6;
        case '4': return 7;
        default: return -1;
    }
}

Vertex moveVertex(Vertex v, int dirIndex) {
    // Direction offsets for N, E, S, W, NE, SE, SW, NW
    const int dirOffsets[8][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
    Vertex result = {v.row + dirOffsets[dirIndex][0], v.col + dirOffsets[dirIndex][1]};
    return result;
}
