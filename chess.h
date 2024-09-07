#ifndef CHESS_H_INCLUDED
#define CHESS_H_INCLUDED

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
    bool hasMovedKing;
    bool hasMovedRook;
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
bool CanPieceAttack(Piece *piece, int targetX, int targetY, Board *board, Player *currentPlayer);// function that check if specific piece can attack specific target

bool isCheck(Board *board, Player *currentPlayer); //Determines if a player's king is in check.

bool isCheckmate(Board *board,Player *currentPlayer); //Determines if a player is in checkmate.

bool IsPlayerWin(Board *board, Player *player1 , Player *player2); //Determines if a player is won

bool IsGameOver(Board *board, Player *player1 , Player *player2); //Determines if a player is won

bool PerformMove(Game* game, const char* moveInput); // Function to perform a move based on user input (e.g., "E2-E4")

bool IsLegalMove(Board *board, int startX, int startY, int endX, int endY, Player *player); //function that check if the move the player want to perform is vsalid

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

void HandleCastling(Board *board, Player *player, bool isKingside); //Handle castling rules and logic.

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

#endif // CHESS_H_INCLUDED
