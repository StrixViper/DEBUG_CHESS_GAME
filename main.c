#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_MOVES 150 // a macro for the past moves the player made

#define BOARD_SIZE 8

typedef struct // struct for the Piece of every soldier
{
    char type;
    char color;
    int x,y;
    bool hasMoved;
} Piece;

typedef struct // strcut for the player details
{
    char color;
    bool isInCheck;
    bool isLost;
    time_t timeLeft; //store the time IN SECONDS (600 seconds max ----- >0 min)
    time_t startTime; //store the time the player has started playing
    int kingX;
    int kingY;

} Player;

typedef struct // struct for the chess board
{
    Piece* board[8][8];
} Board;

typedef struct // struct for every move
{
    int startX, startY; // Starting coordinates of the piece
    int endX, endY;     // Ending coordinates of the piece
    char pieceMoved;    // The type of piece that was moved (e.g., 'K' for King)
    char pieceCaptured; // The type of piece that was captured, if any (e.g., 'P' for Pawn)
    bool isCastlingMove; // To indicate if this move was a castling move
    bool isEnPassant;    // To indicate if this move was an en passant
    bool isPromotion;    // To indicate if this move was a pawn promotion
    int playerWhoMadeTheMove;
    char move[100];
    struct Move *next;
} Move;

typedef struct
{
    Board* board;
    Player* players[2];
    int currentPlayer; // 0 for Player 1, 1 for Player 2
    int moveCount;
    Move moveHistory[MAX_MOVES];
} Game;



bool isGameOver = false; // a variable that hold if the game is over or not


//##########################-----INITIALIZATION FUNCTIONS---------############################

void InitializeBoard(Board *board); //Set up the initial positions of pieces on the board.

void InitializePlayers(Player *player1, Player *player2); //Initialize player-specific settings.

void placePiece(int x, int y, char type, char color);

void UpdateTimeLeft(Player *player); //function to update how much time the player has left

//##########################-----END OF INITIALIZATION FUNCTIONS---------############################



//##########################-----MOVEMENT AND RULES FUNCTIONS---------############################

bool IsInCheck(Board *board, Player *player); //Determines if a player's king is in check.

bool IsCheckmate(Board *board,Player *player); //Determines if a player is in checkmate.

bool IsPlayerWin(Board *board, Player *player1 , Player *player2); //Determines if a player is won

bool IsGameOver(Board *board, Player *player1 , Player *player2); //Determines if a player is won

bool PerformMove(Game* game, const char* moveInput); // Function to perform a move based on user input (e.g., "E2-E4")

bool IsLegalMove(Board *board, int startX, int startY, int endX, int endY, Player *player); //function that check if the move the player want to perform is vsalid

bool IsKingCaptured(Board *board, Player *player);


// Function to handle the movement of the King
// The King can move one square in any direction (horizontally, vertically, or diagonally)
// The function should check if the move is valid, ensure the destination square is not occupied by a piece of the same color,
// and handle special rules like castling.
bool MoveKing(Board *board, int startX, int startY, int endX, int endY, Player *player);

// Function to handle the movement of the Queen
// The Queen can move any number of squares along a rank, file, or diagonal
// The function should ensure there are no pieces blocking the path and that the destination square is valid.
bool MoveQueen(Board *board, int startX, int startY, int endX, int endY, Player *player);

// Function to handle the movement of the Rook
// The Rook can move any number of squares along a rank or file
// The function should ensure the path is clear and validate the destination square.
// It should also account for castling rights.
bool MoveRook(Board *board, int startX, int startY, int endX, int endY, Player *player);

// Function to handle the movement of the Bishop
// The Bishop can move any number of squares diagonally
// The function should check if the path is clear and validate the destination square.
bool MoveBishop(Board *board, int startX, int startY, int endX, int endY, Player *player);

// Function to handle the movement of the Knight
// The Knight moves in an "L" shape: two squares in one direction and then one square perpendicular, or vice versa
// The function should validate the unique movement pattern and check if the destination square is valid.
bool MoveKnight(Board *board, int startX, int startY, int endX, int endY, Player *player);

// Function to handle the movement of the Pawn
// Pawns move forward one square, but capture diagonally. They can also move two squares forward from their starting position
// The function should validate normal moves, captures, en passant, and promotion.
bool MovePawn(Board *board, int startX, int startY, int endX, int endY, Player *player);


//##########################-----END OF MOVEMENT AND RULES FUNCTIONS---------############################



//##########################-----GAME FLOW FUNCTIONS---------############################

void PrintBoard(Board *board , Game *game); //Print the current state of the board for debugging or gameplay.

void SwitchPlayer(Game *game); //Switch the current player after a successful move.

void HandleCastling(Board *board, Player *player);//Handle castling rules and logic.

void HandlePawnPromotion(Board *board, int x, int y); //Handle the promotion of pawns when they reach the opposite side of the board.

void RedoMove(Board *board, Move *move, Player *player, Player *opponent); // Redo a move

void UndoMove(Board *board, Move *move, Player *player, Player *opponent); // Undo a move

void StoreMove(Game *game, Move *move); // function to save the past moves and print on the scrint

void DisplayMoveHistory(Game *game, int n); // function to display the last n moves for the history

void saveGameHistory(Game *game); //function to save all the moves that was on the game

void uploadGame(FILE *gamefile, char filename); //function that you could upload a game you had and then see it later

//##########################-----END OF GAME FLOW FUNCTIONS---------############################



//##########################-----UTILITY FUNCTIONS---------############################

const char* GetPieceSymbol(Piece *piece); //Return a character symbol for the piece (e.g., 'K' for king, 'Q' for queen).

bool IsMoveWithinBounds(int startX, int startY,int endX,int endY); //Check if the given coordinates are within the board limits.

void ConvertAlgebraicToIndices(const char* position, int* x, int* y); // Function to convert algebraic notation (e.g., "E2") to board indices (e.g., (6, 4))

//##########################-----END OF UTILITY FUNCTIONS---------############################


void ClearConsole() {
    #if defined(_WIN32)
        system("cls"); // For Windows
    #else
        system("clear"); // For Linux and macOS
    #endif
}


//##########################-----MEMORY MANAGMENT FUNCTIONS---------############################

void FreeBoard(Board *board); //Free any dynamically allocated memory for the board.

//##########################-----END OF MEMORY MANAGMENT FUNCTIONS---------############################


int main() {
    setlocale(LC_CTYPE, "");

    Board board;
    Player player1, player2;
    Game game;

    InitializeBoard(&board);
    InitializePlayers(&player1, &player2);

    game.board = &board;
    game.players[0] = &player1;
    game.players[1] = &player2;
    game.currentPlayer = 0; // Assuming White starts
    game.moveCount = 0;     // Initialize moveCount

    // Print the initial board state
    PrintBoard(&board, &game);

    // Main game loop
    while (!isGameOver) {
        Player *currentPlayer = game.players[game.currentPlayer];
        Player *opponentPlayer = game.players[1 - game.currentPlayer]; // Get the opponent player
        char moveInput[6];

        // Record the start time for the current player
        currentPlayer->startTime = time(NULL);

        printf("\n\n\nPlayer %d, enter your move (e.g., E2-E4): ", game.currentPlayer + 1);
        scanf("%s", moveInput);

        if (PerformMove(&game, moveInput)) {
            // Update time for the current player
            UpdateTimeLeft(currentPlayer);


            // Check if the current player's move has put the opponent in check
            if (IsInCheck(&board, opponentPlayer)) {
                opponentPlayer->isInCheck = true;
                printf("Player %d is in check!\n", 1 - game.currentPlayer + 1);

                // Check for checkmate
                if (IsCheckmate(&board, opponentPlayer)) {
                    printf("Checkmate! Player %d wins!\n", game.currentPlayer + 1);
                    isGameOver = true;
                }
            }
            else {
                opponentPlayer->isInCheck = false;
            }




            if(IsKingCaptured(&board,opponentPlayer))
            {
                printf("Player: %d has Win the game due to capture the opponent king",currentPlayer->color);
            }


            // Check if game is over due to time constraints or other conditions
            if (currentPlayer->isLost) {
                printf("Player %d has run out of time. Player %d wins!\n", game.currentPlayer + 1, 1 - game.currentPlayer + 1);
                isGameOver = true;
            }

            PrintBoard(&board, &game);

            if (!isGameOver) {
                // Switch the player for the next turn
                SwitchPlayer(&game);
            }
        }
    }

    return 0;
}




void ConvertAlgebraicToIndices(const char* position, int* x, int* y) {
    *y = toupper(position[0]) - 'A'; // Column (A-H) to index (0-7)
    *x = 8 - (position[1] - '0'); // Row (1-8) to index (0-7)
}

void PrintBoard(Board *board, Game *game) {
    ClearConsole();

    // Print the column labels and the board as before
    printf("      A     B     C     D     E     F     G     H\n");
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");

    for (int i = 0; i < 8; i++) {
        printf("%d |", 8 - i); // Print the row label correctly

        for (int j = 0; j < 8; j++) {
            if (board->board[i][j] == NULL) {
                printf("  .  |"); // Empty square
            } else {
                Piece *piece = board->board[i][j];
                const char* symbol = GetPieceSymbol(piece);
                printf(" %s |", symbol); // Print piece symbol with color
            }
        }
        printf(" %d\n", 8 - i); // Print the row label correctly

        // Print the separator line
        printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    }

    printf("      A     B     C     D     E     F     G     H\n");

    printf("\n\n");

    // Show the move history
    DisplayMoveHistory(game, game->moveCount);

    printf("\n");
    // Print the time left for each player
    printf("Player 1 (White) time left: %02d:%02d\n", (int)(game->players[0]->timeLeft / 60), (int)(game->players[0]->timeLeft % 60));
    printf("Player 2 (Black) time left: %02d:%02d\n", (int)(game->players[1]->timeLeft / 60), (int)(game->players[1]->timeLeft % 60));
}

void DisplayMoveHistory(Game *game, int n) {
    // Display the header
    printf("     White Past Moves     |   Black Past Moves\n");
    printf(" -------------------------|------------------------\n");

    // Iterate through the first n moves
    for (int i = 0; i < n; i++) {
        // Print White's move if available
        if (i < game->moveCount && game->moveHistory[i].playerWhoMadeTheMove == 0) {
            printf(" %d: %c from (%d,%d) to (%d,%d) |",
                   i + 1,
                   game->moveHistory[i].pieceMoved,
                   game->moveHistory[i].startX, game->moveHistory[i].startY,
                   game->moveHistory[i].endX, game->moveHistory[i].endY);
        } else {
            printf("                          "); // Space for White's move if none
        }

        // Print Black's move if available
        if (i < game->moveCount && game->moveHistory[i].playerWhoMadeTheMove == 1) {
            printf("| %d: %c from (%d,%d) to (%d,%d)\n",
                   i + 1,
                   game->moveHistory[i].pieceMoved,
                   game->moveHistory[i].startX, game->moveHistory[i].startY,
                   game->moveHistory[i].endX, game->moveHistory[i].endY);
        } else {
            printf("\n"); // Newline if no Black's move
        }
    }
    printf(" _________________________|________________________\n");
}

void InitializeBoard(Board *board) {
    // Define the initial piece placement for rows 0 and 1 (white pieces)
    char initialRow0[8] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    char initialRow1[8] = {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'};

    // Define the initial piece placement for rows 6 and 7 (black pieces)
    char initialRow6[8] = {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'};
    char initialRow7[8] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};

    // Clear the board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board->board[i][j] = NULL;
        }
    }

    // Function to create and place a piece on the board
    void placePiece(int x, int y, char type, char color) {
        Piece *piece = (Piece*)malloc(sizeof(Piece));
        piece->type = type;
        piece->color = color;
        piece->x = x;
        piece->y = y;
        piece->hasMoved = false;
        board->board[x][y] = piece;
    }

    // Place white pieces
    for (int j = 0; j < 8; j++) {
        placePiece(0, j, initialRow7[j], 'W'); // Row 0 for white pieces
        placePiece(1, j, initialRow1[j], 'W'); // Row 1 for white pawns
    }

    // Place black pieces
    for (int j = 0; j < 8; j++) {
        placePiece(6, j, initialRow6[j], 'B'); // Row 6 for black pawns
        placePiece(7, j, initialRow0[j], 'B'); // Row 7 for black pieces
    }
}

void InitializePlayers(Player *player1, Player *player2) {
    player1->color = 'W'; // White
    player1->isInCheck = false;
    player1->isLost = false;
    player1->timeLeft = 600; // 10 minutes
    player1->startTime = time(NULL); // Set the start time

    player2->color = 'B'; // Black
    player2->isInCheck = false;
    player2->isLost = false;
    player2->timeLeft = 600; // 10 minutes
    player2->startTime = time(NULL); // Set the start time
}

void UpdateTimeLeft(Player *player) {
    time_t currentTime = time(NULL);
    double elapsedTime = difftime(currentTime, player->startTime);
    player->timeLeft -= (int)elapsedTime;

    if (player->timeLeft <= 0) {
        player->timeLeft = 0;    // Ensure timeLeft doesn't go negative
        player->isLost = true;  // Set the player as lost the game
    }

    player->startTime = currentTime; // Reset startTime for the next move
}

const char* GetPieceSymbol(Piece *piece) {
    if (piece == NULL) {
        return " .  ";
    }
    static char symbol[5]; // Allocate enough space for 'B_W' + null terminator
    char colorIndicator = (piece->color == 'W') ? 'W' : 'B';

    snprintf(symbol, sizeof(symbol), "%c_%c", piece->type, colorIndicator);

    return symbol;
}

bool MoveKing(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    if (dx <= 1 && dy <= 1) { // King can move one square in any direction
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            // Update king's position in the Player struct
            player->kingX = endX;
            player->kingY = endY;

            // Move the piece on the board
            board->board[endX][endY] = board->board[startX][startY];
            board->board[startX][startY] = NULL;
            board->board[endX][endY]->x = endX;
            board->board[endX][endY]->y = endY;

            return true;
        }
    }
    return false;
}

bool MoveQueen(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    if (dx == dy || startX == endX || startY == endY) { // Moves like Rook or Bishop
        // Check if the path is clear
        int xDir = (endX - startX) == 0 ? 0 : (endX - startX) / dx;
        int yDir = (endY - startY) == 0 ? 0 : (endY - startY) / dy;
        for (int i = 1; i < (dx > dy ? dx : dy); i++) {
            if (board->board[startX + i * xDir][startY + i * yDir] != NULL) {
                return false;
            }
        }
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            return true;
        }
    }
    return false;
}

bool MoveRook(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    if (startX == endX || startY == endY) { // Rook moves in a straight line
        // Check if the path is clear
        int dx = (endX - startX) == 0 ? 0 : (endX - startX) / abs(endX - startX);
        int dy = (endY - startY) == 0 ? 0 : (endY - startY) / abs(endY - startY);
        for (int i = 1; i < (abs(endX - startX) + abs(endY - startY)); i++) {
            if (board->board[startX + i * dx][startY + i * dy] != NULL) {
                return false;
            }
        }
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            return true;
        }
    }
    return false;
}

bool MoveBishop(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    if (dx == dy) { // Bishop moves diagonally
        // Check if the path is clear
        int xDir = (endX - startX) / dx;
        int yDir = (endY - startY) / dy;
        for (int i = 1; i < dx; i++) {
            if (board->board[startX + i * xDir][startY + i * yDir] != NULL) {
                return false;
            }
        }
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            return true;
        }
    }
    return false;
}

bool MoveKnight(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) { // Knight moves in an L shape
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            return true;
        }
    }
    return false;
}

bool MovePawn(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    Piece *pawn = board->board[startX][startY];
    int direction = (pawn->color == 'W') ? 1 : -1;

    // Normal move
    if (startY == endY && board->board[endX][endY] == NULL) {
        if (endX == startX + direction) { // Single step forward
            return true;
        }
        if (!pawn->hasMoved && endX == startX + 2 * direction && board->board[startX + direction][startY] == NULL) { // Double step forward
            return true;
        }
    }

    // Capture move
    if (abs(startY - endY) == 1 && endX == startX + direction && board->board[endX][endY] != NULL && board->board[endX][endY]->color != pawn->color) {
        return true;
    }

    // En Passant (requires additional logic to handle)
    // if (/* conditions for en passant */) {
    //     return true;
    // }

    return false;
}

void StoreMove(Game *game, Move *move) {
    if (game->moveCount < MAX_MOVES) {
        game->moveHistory[game->moveCount] = *move;
        game->moveCount++;
    } else {
        printf("Move history is full!\n");
    }
}

void SwitchPlayer(Game *game) {
    game->currentPlayer = 1 - game->currentPlayer;
}

bool PerformMove(Game* game, const char* moveInput) {
    int startX, startY, endX, endY;
    ConvertAlgebraicToIndices(&moveInput[0], &startX, &startY);
    ConvertAlgebraicToIndices(&moveInput[3], &endX, &endY);

    Board* board = game->board;
    Player* currentPlayer = game->players[game->currentPlayer];
    Player* opponentPlayer = game->players[1 - game->currentPlayer];
    Piece* movingPiece = board->board[startX][startY];

    if (movingPiece == NULL || movingPiece->color != currentPlayer->color) {
        printf("Invalid move: No piece to move or moving opponent's piece.\n");
        return false;
    }

    bool isValidMove = false;
    switch (toupper(movingPiece->type)) {
        case 'K':
            isValidMove = MoveKing(board, startX, startY, endX, endY, currentPlayer);
            if (isValidMove) {
                currentPlayer->kingX = endX; // Update king position
                currentPlayer->kingY = endY;
            }
            break;
        case 'Q':
            isValidMove = MoveQueen(board, startX, startY, endX, endY, currentPlayer);
            break;
        case 'R':
            isValidMove = MoveRook(board, startX, startY, endX, endY, currentPlayer);
            break;
        case 'B':
            isValidMove = MoveBishop(board, startX, startY, endX, endY, currentPlayer);
            break;
        case 'N':
            isValidMove = MoveKnight(board, startX, startY, endX, endY, currentPlayer);
            break;
        case 'P':
            isValidMove = MovePawn(board, startX, startY, endX, endY, currentPlayer);
            break;
    }

    if (!isValidMove) {
        printf("Invalid move for the selected piece.\n");
        return false;
    }

    Piece* capturedPiece = board->board[endX][endY];
    if (capturedPiece != NULL) {
        if (capturedPiece->color == opponentPlayer->color) {
            printf("Piece captured: %c at (%d,%d)\n", capturedPiece->type, endX, endY);
            free(capturedPiece); // Free the memory allocated for the captured piece
            board->board[endX][endY] = NULL;
        } else {
            printf("Invalid move: Cannot capture your own piece.\n");
            return false;
        }
    }

    // Move the piece
    board->board[endX][endY] = movingPiece;
    board->board[startX][startY] = NULL;
    movingPiece->x = endX;
    movingPiece->y = endY;
    movingPiece->hasMoved = true;

    // Store the move
    Move move = {startX, startY, endX, endY, movingPiece->type, capturedPiece ? capturedPiece->type : 0, false, false, false, game->currentPlayer};
    StoreMove(game, &move);

    return true;
}

void saveGameHistory(Game *game) {
    // Prompt user for file name
    char fileName[256];
    printf("Enter the file name to save the game history (e.g., game_history.txt): ");
    fgets(fileName, sizeof(fileName), stdin);
    // Remove newline character if present
    fileName[strcspn(fileName, "\n")] = '\0';

    // Open file for writing
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Write game history to file
    Move *current = game->moveHistory;
    while (current != NULL) {
        fprintf(file, "%s\n", current->move);
        current = current->next;
    }

    // Close the file
    fclose(file);
    printf("Game history saved to %s\n", fileName);
}

bool IsLegalMove(Board *board, int startX, int startY, int endX, int endY, Player *player) {
    // Check if the move is within bounds
    if (!IsMoveWithinBounds(startX, startY,endX,endY)) {
        return false;
    }

    // Check if there is a piece at the start position
    Piece *movingPiece = board->board[startX][startY];
    if (movingPiece == NULL) {
        return false; // No piece to move
    }

    // Check if the piece belongs to the player
    if (movingPiece->color != player->color) {
        return false; // The player can only move their own pieces
    }

    // Check if the destination square contains a piece of the same color
    Piece *destinationPiece = board->board[endX][endY];
    if (destinationPiece != NULL && destinationPiece->color == player->color) {
        return false; // Can't capture own piece
    }

    // Call the appropriate movement function based on the type of the piece
    switch (toupper(movingPiece->type)) {
        case 'K': // King
            return MoveKing(board, startX, startY, endX, endY, player);
        case 'Q': // Queen
            return MoveQueen(board, startX, startY, endX, endY, player);
        case 'R': // Rook
            return MoveRook(board, startX, startY, endX, endY, player);
        case 'B': // Bishop
            return MoveBishop(board, startX, startY, endX, endY, player);
        case 'N': // Knight
            return MoveKnight(board, startX, startY, endX, endY, player);
        case 'P': // Pawn
            return MovePawn(board, startX, startY, endX, endY, player);
        default:
            return false; // Invalid piece type
    }

    return false; // Default case, should never reach here
}

bool IsInCheck(Board *board, Player *player) {
    // Assuming we have a function to check if a specific position is under attack
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Piece *piece = board->board[i][j];
            if (piece != NULL && piece->color != player->color) {
                // Check if this piece can attack the player's king
                if (IsLegalMove(board, i, j, player->kingX, player->kingY, player)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsCheckmate(Board *board, Player *player) {
    if (!IsInCheck(board, player)) {
        return false;
    }
    // Iterate over all possible moves to see if any move can escape the check
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            // Try moving each piece to see if it can escape check
            Piece *piece = board->board[x][y];
            if (piece != NULL && piece->color == player->color) {
                // Generate all possible moves for this piece
                // This is a simplified version; you need to generate all valid moves
                // and see if any move would get the king out of check
                for (int newX = 0; newX < BOARD_SIZE; newX++) {
                    for (int newY = 0; newY < BOARD_SIZE; newY++) {
                        if (IsLegalMove(board, x, y, newX, newY, player)) {
                            // Temporarily make the move and check if it resolves the check
                            Piece *originalPiece = board->board[newX][newY];
                            board->board[newX][newY] = board->board[x][y];
                            board->board[x][y] = NULL;
                            if (!IsInCheck(board, player)) {
                                // Undo the move
                                board->board[x][y] = board->board[newX][newY];
                                board->board[newX][newY] = originalPiece;
                                return false;
                            }
                            // Undo the move
                            board->board[x][y] = board->board[newX][newY];
                            board->board[newX][newY] = originalPiece;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool IsMoveWithinBounds(int startX, int startY, int endX, int endY) {
    // Check if the starting and ending positions are within bounds of the chessboard
    return (startX >= 0 && startX < BOARD_SIZE) &&
           (startY >= 0 && startY < BOARD_SIZE) &&
           (endX >= 0 && endX < BOARD_SIZE) &&
           (endY >= 0 && endY < BOARD_SIZE);
}

bool IsKingCaptured(Board *board, Player *player) {
    // Check if the player's king is still on the board
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Piece *piece = board->board[i][j];
            if (piece != NULL && piece->type == 'K' && piece->color == player->color) {
                // Found the king on the board
                return false;
            }
        }
    }
    // King's piece not found, hence captured
    return true;
}


