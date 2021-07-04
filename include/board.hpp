#pragma once

#include <vector>
#include <windows.h>

class Board {
    public:
    	enum cellState {
			nought,
			blank,
			cross,
		};
		
		enum turnState {
			playerOne,
			playerTwo,
		};
    	
    	Board();
    	
        int getSize();
    	int getSizeW();
    	int getSizeH();
    	void setSize(int sizeW, int sizeH);
    	
    	cellState getCellState(int cell);
    	void setCellState(int cell, cellState state);
        
        int getSelectedCell();
        void setSelectedCell(int cell);
        
        void makeMove();
        
        void switchPlayers();
        
        cellState validate();
        
        int getNumberOfSymbolsForWin();
        void setNumberOfSymbolsForWin(int numberOfSymbolsForWin);
		
        bool isGameOver();
        void setGameOver(bool value);
    
    private:
        std::vector<cellState> state;
        
    	turnState turn;
    	bool gameOver;
    	
        int boardSizeW;
        int boardSizeH;
        int selectedCell;
        
    	int numberOfSymbolsForWin;
        bool checkPattern(std::vector<cellState> pattern);
};

