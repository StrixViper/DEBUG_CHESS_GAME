#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "chess.h"


void ClearConsole() {
    #if defined(_WIN32)
        system("cls"); // For Windows
    #else
        system("clear"); // For Linux and macOS
    #endif
}

// Define min and max functions
int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}


//##########################-----MEMORY MANAGMENT FUNCTIONS---------############################

void FreeBoard(Board *board); //Free any dynamically allocated memory for the board.

//##########################-----END OF MEMORY MANAGMENT FUNCTIONS---------############################


int main() {
    setlocale(LC_CTYPE, "");

    // Initialize game components
    Board board;
    Player player1, player2;
    Game game;
    bool isGameOver = false;

    // Initialize the board and players
    InitializeBoard(&board);
    InitializePlayers(&player1, &player2);

    game.board = &board;
    game.players[0] = &player1;
    game.players[1] = &player2;
    game.currentPlayer = 0; // Player 1 (White) starts
    game.moveCount = 0;     // Initialize move count

    // Print the initial board state
    PrintBoard(&board, &game);

    // Main game loop
    while (!isGameOver) {
        Player *currentPlayer = game.players[game.currentPlayer];
        Player *opponentPlayer = game.players[1 - game.currentPlayer]; // Determine opponent player
        char moveInput[6];

        // Record the start time for the current player
        currentPlayer->startTime = time(NULL);

        // Get player move input
        printf("\n\nPlayer %d (%c), enter your move (e.g., E2-E4): ", game.currentPlayer + 1, currentPlayer->color);
        scanf("%s", moveInput);

        // Perform the move
        if (PerformMove(&game, moveInput)) {
            // Update time left for the current player
            UpdateTimeLeft(currentPlayer);

            // Check if the opponent is in check after the move
            if (isCheck(game.board, opponentPlayer))
                    printf("Player %d is in check!\n", 1 - game.currentPlayer + 1);

                // Check if the opponent is in checkmate
                if (isCheckmate(game.board, opponentPlayer))
                {
                    printf("Player %d is in checkmate. Player %d wins!\n", 1 - game.currentPlayer + 1, game.currentPlayer + 1);
                    isGameOver = true;
                }


            // Check if the game is over due to time constraints
            if (currentPlayer->isLost) {
                printf("Player %d has run out of time. Player %d wins!\n", game.currentPlayer + 1, 1 - game.currentPlayer + 1);
                isGameOver = true;
            }

            // If no one has won yet, switch to the other player
            if (!isGameOver) {
                SwitchPlayer(&game); // Switch turns
                PrintBoard(&board, &game); // Print updated board after the move
            }
        } else {
            printf("Invalid move, please try again.\n");
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
    player1->hasMovedKing = false;
    player1->hasMovedRook = false;
    player1->timeLeft = 600; // 10 minutes
    player1->startTime = time(NULL); // Set the start time
    player1->kingX = 0;  // White king starts at row 0
    player1->kingY = 4;  // White king starts at column 4

    player2->color = 'B'; // Black
    player2->isInCheck = false;
    player2->isLost = false;
    player2->hasMovedKing = false;
    player2->hasMovedRook = false;
    player2->timeLeft = 600; // 10 minutes
    player2->startTime = time(NULL); // Set the start time
    player2->kingX = 7;  // Black king starts at row 7
    player2->kingY = 4;  // Black king starts at column 4
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

    // Handle normal king move
    if (dx <= 1 && dy <= 1) {
        Piece *destination = board->board[endX][endY];
        if (destination == NULL || destination->color != player->color) {
            // Update king's position in the Player struct
            player->kingX = endX;
            player->kingY = endY;

            // Set the hasMovedKing flag
            player->hasMovedKing = true;

            return true;
        }
    }
    // Handle castling move
    else if (startX == endX && (endY == startY + 2 || endY == startY - 2)) {
        // Castling is only allowed if the king and rook have not moved before
        if (!player->hasMovedKing && !player->hasMovedRook &&
            startY == 4 && (endY == 2 || endY == 6)) {
            int rookX = startX;
            int rookY = (endY == 2) ? 0 : 7; // rook's starting position

            // Check if there are no pieces between the king and the rook
            for (int y = min(startY, endY) + 1; y < max(startY, endY); ++y) {
                if (board->board[startX][y] != NULL) {
                    return false;
                }
            }

            // Update player states
            player->hasMovedKing = true;
            player->hasMovedRook = true;

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
            player->hasMovedRook = true;
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
            isValidMove =  MoveKing(board, startX, startY, endX, endY, currentPlayer);
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

bool IsMoveWithinBounds(int startX, int startY, int endX, int endY) {
    // Check if the starting and ending positions are within bounds of the chessboard
    return (startX >= 0 && startX < BOARD_SIZE) &&
           (startY >= 0 && startY < BOARD_SIZE) &&
           (endX >= 0 && endX < BOARD_SIZE) &&
           (endY >= 0 && endY < BOARD_SIZE);
}

void HandleCastling(Board *board, Player *player, bool isKingside) {
    int row = (player->color == 'W') ? 0 : 7;
    int kingStartCol = (isKingside) ? 4 : 4;
    int kingEndCol = (isKingside) ? 6 : 2;
    int rookStartCol = (isKingside) ? 7 : 0;
    int rookEndCol = (isKingside) ? 5 : 3;

    // Move the king
    Piece *king = board->board[row][kingStartCol];
    board->board[row][kingEndCol] = king;
    board->board[row][kingStartCol] = NULL;
    king->x = row;
    king->y = kingEndCol;
    king->hasMoved = true;

    // Move the rook
    Piece *rook = board->board[row][rookStartCol];
    board->board[row][rookEndCol] = rook;
    board->board[row][rookStartCol] = NULL;
    rook->x = row;
    rook->y = rookEndCol;
    rook->hasMoved = true;

    // Update player king position
    player->kingX = row;
    player->kingY = kingEndCol;
}

bool isCheck(Board *board, Player *currentPlayer) {
    int kingX = currentPlayer->kingX;
    int kingY = currentPlayer->kingY;
    char opponentColor = (currentPlayer->color == 'W') ? 'B' : 'W';

    // Iterate over the board to find the opponent's pieces
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Piece *piece = board->board[i][j];
            if (piece != NULL && piece->color == opponentColor) {
                // Check if the opponent's piece can attack the king
                if (CanPieceAttack(piece, kingX, kingY, board, currentPlayer)) {
                    return true; // King is in check
                }
            }
        }
    }
    return false; // King is not in check
}

bool isCheckmate(Board *board, Player *currentPlayer) {

    int kingX = currentPlayer->kingX;
    int kingY = currentPlayer->kingY;

    // Directions the king can move: up, down, left, right, and diagonals
    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    // Check if the king can escape by moving to a safe position
    for (int i = 0; i < 8; i++) {
        int newX = kingX + directions[i][0];
        int newY = kingY + directions[i][1];

        // Check if the move is within bounds and not blocked by a same-color piece
        if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE) {
            Piece *destination = board->board[newX][newY];
            if (destination == NULL || destination->color != currentPlayer->color) {
                // Simulate the move
                int oldX = kingX;
                int oldY = kingY;
                currentPlayer->kingX = newX;
                currentPlayer->kingY = newY;

                if (!isCheck(board, currentPlayer)) {
                    // King can escape the check, so it's not checkmate
                    currentPlayer->kingX = oldX;
                    currentPlayer->kingY = oldY;
                    return false;
                }

                // Undo the move
                currentPlayer->kingX = oldX;
                currentPlayer->kingY = oldY;
            }
        }
    }

    // Check if any other piece can block the check or capture the attacking piece
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            Piece *piece = board->board[i][j];
            if (piece != NULL && piece->color == currentPlayer->color && piece->type != 'K') {  // Exclude the king
                // Try moving the piece to all possible squares on the board
                for (int x = 0; x < BOARD_SIZE; x++) {
                    for (int y = 0; y < BOARD_SIZE; y++) {
                        Piece *destination = board->board[x][y];
                        // Check if the piece can move to the destination and is not blocked by a same-color piece
                        if (destination == NULL || destination->color != currentPlayer->color) {
                            if (CanPieceAttack(piece, x, y, board,currentPlayer)) {
                                // Simulate the move
                                int originalX = piece->x;
                                int originalY = piece->y;
                                board->board[originalX][originalY] = NULL;
                                board->board[x][y] = piece;
                                piece->x = x;
                                piece->y = y;

                                if (!isCheck(board, currentPlayer)) {
                                    // The check can be blocked or the attacking piece can be captured
                                    board->board[originalX][originalY] = piece;
                                    board->board[x][y] = destination;
                                    piece->x = originalX;
                                    piece->y = originalY;
                                    return false;
                                }

                                // Undo the move
                                board->board[originalX][originalY] = piece;
                                board->board[x][y] = destination;
                                piece->x = originalX;
                                piece->y = originalY;
                            }
                        }
                    }
                }
            }
        }
    }

    // If we reach here, the king is in check and no other piece can block the check or capture the attacking piece
    return true;
}

bool CanPieceAttack(Piece *piece, int targetX, int targetY, Board *board, Player *currentPlayer) {
    // Implement movement rules for each piece type (King, Queen, Bishop, Knight, Rook, Pawn)
    // For example, check if the piece can move to (targetX, targetY)
    // Return true if the piece can attack the given coordinates

    bool isValidMove = false;

    // Ensure we're using the piece->type for switch case
    switch (toupper(piece->type)) {
        case 'K':
            isValidMove = MoveKing(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        case 'Q':
            isValidMove = MoveQueen(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        case 'R':
            isValidMove = MoveRook(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        case 'B':
            isValidMove = MoveBishop(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        case 'N':
            isValidMove = MoveKnight(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        case 'P':
            isValidMove = MovePawn(board, piece->x, piece->y, targetX, targetY, currentPlayer);
            break;
        default:
            // Handle unexpected piece types, possibly an error
            break;
    }

    return isValidMove;
}



