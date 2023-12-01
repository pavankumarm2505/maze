#include <stdio.h>
#include <stdbool.h>

#define MAX_ROWS 100
#define MAX_COLS 100

// Structure to represent a vertex in the maze
typedef struct {
    int row;
    int col;
} Vertex;

// Function prototypes
bool solveMaze(char maze[MAX_ROWS][MAX_COLS], int rows, int cols);
bool DFS(Vertex current, Vertex target, char maze[MAX_ROWS][MAX_COLS], char path[MAX_ROWS * MAX_COLS][6], bool visited[MAX_ROWS][MAX_COLS], int rows, int cols);
bool canMove(Vertex current, Vertex neighbor, char maze[MAX_ROWS][MAX_COLS], int rows, int cols);
char* moveDirection(Vertex current, Vertex neighbor);
char getColor(Vertex vertex, char maze[MAX_ROWS][MAX_COLS]);
void writePathToFile(char path[MAX_ROWS * MAX_COLS][6], int pathLength);

int main() {
    char maze[MAX_ROWS][MAX_COLS];
    int rows, cols;

    // Read input from file
    FILE *inputFile = fopen("tiny.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    if (fscanf(inputFile, "%d %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0 || rows > MAX_ROWS || cols > MAX_COLS) {
        fprintf(stderr, "Invalid maze dimensions\n");
        fclose(inputFile);
        return 1;
    }

    // Read maze
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (fscanf(inputFile, " %c-%*c", &maze[i][j]) != 1 || (maze[i][j] != 'R' && maze[i][j] != 'B')) {
                fprintf(stderr, "Invalid arrow color at position (%d, %d)\n", i, j);
                fclose(inputFile);
                return 1;
            }
        }
    }

    fclose(inputFile);

    // Solve maze and write output to file
    if (solveMaze(maze, rows, cols)) {
        printf("Maze solved successfully!\n");
    } else {
        printf("No solution found.\n");
    }

    return 0;
}

bool solveMaze(char maze[MAX_ROWS][MAX_COLS], int rows, int cols) {
    if (rows <= 0 || cols <= 0 || rows > MAX_ROWS || cols > MAX_COLS) {
        fprintf(stderr, "Invalid maze dimensions\n");
        return false;
    }

    Vertex start = {0, 0};
    Vertex target = {rows - 1, cols - 1};
    char path[MAX_ROWS * MAX_COLS][6];  // Each move is represented as a string (e.g., "3NE")
    bool visited[MAX_ROWS][MAX_COLS] = {false};

    if (DFS(start, target, maze, path, visited, rows, cols)) {
        writePathToFile(path, rows * cols);
        return true;
    }

    return false;
}

bool DFS(Vertex current, Vertex target, char maze[MAX_ROWS][MAX_COLS], char path[MAX_ROWS * MAX_COLS][6], bool visited[MAX_ROWS][MAX_COLS], int rows, int cols) {
    if (current.row == target.row && current.col == target.col) {
        return true;  // Path found
    }

    visited[current.row][current.col] = true;

    // Define possible moves (North, East, South, West, Northeast, Southeast, Southwest, Northwest)
    int moveRow[] = {-1, 0, 1, 0, -1, 1, 1, -1};
    int moveCol[] = {0, 1, 0, -1, 1, 1, -1, -1};

    for (int i = 0; i < 8; ++i) {
        Vertex neighbor = {current.row + moveRow[i], current.col + moveCol[i]};

        if (neighbor.row >= 0 && neighbor.row < rows && neighbor.col >= 0 && neighbor.col < cols &&
            !visited[neighbor.row][neighbor.col] && canMove(current, neighbor, maze, rows, cols)) {
            sprintf(path[current.row * MAX_COLS + current.col], "%s", moveDirection(current, neighbor));

            if (DFS(neighbor, target, maze, path, visited, rows, cols)) {
                return true;  // Path found
            }

            path[current.row * MAX_COLS + current.col][0] = '\0';  // Backtrack
        }
    }
    visited[current.row][current.col] = false;
    return false;  // No path found
}

bool canMove(Vertex current, Vertex neighbor, char maze[MAX_ROWS][MAX_COLS], int rows, int cols) {
    if (neighbor.row >= 0 && neighbor.row < rows && neighbor.col >= 0 && neighbor.col < cols) {
        char currentColor = getColor(current, maze);
        char neighborColor = getColor(neighbor, maze);

        return currentColor != neighborColor;
    }

    return false;  // Neighbor is out of bounds
}

char* moveDirection(Vertex current, Vertex neighbor) {
    static char direction[6];

    int deltaRow = neighbor.row - current.row;
    int deltaCol = neighbor.col - current.col;

    if (deltaRow < 0) {
        sprintf(direction, "%dN", -deltaRow);
    } else if (deltaRow > 0) {
        sprintf(direction, "%dS", deltaRow);
    } else if (deltaCol < 0) {
        sprintf(direction, "%dW", -deltaCol);
    } else if (deltaCol > 0) {
        sprintf(direction, "%dE", deltaCol);
    } else if (deltaRow < 0 && deltaCol > 0) {
        sprintf(direction, "%dNE", -deltaRow);
    } else if (deltaRow > 0 && deltaCol > 0) {
        sprintf(direction, "%dSE", deltaRow);
    } else if (deltaRow > 0 && deltaCol < 0) {
        sprintf(direction, "%dSW", deltaRow);
    } else if (deltaRow < 0 && deltaCol < 0) {
        sprintf(direction, "%dNW", -deltaRow);
    }

    return direction;
}

char getColor(Vertex vertex, char maze[MAX_ROWS][MAX_COLS]) {
    return maze[vertex.row][vertex.col];
}

void writePathToFile(char path[MAX_ROWS * MAX_COLS][6], int pathLength) {
    FILE *outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return;
    }

    for (int i = 0; i < pathLength; ++i) {
        if (path[i][0] != '\0') {
            fprintf(outputFile, "%s ", path[i]);
        }
    }

    fclose(outputFile);
}