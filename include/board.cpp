#include "board.hpp"
#include <string>

Board::Board() {
	this->turn = Board::playerOne;
	this->gameOver = false;
	this->numberOfSymbolsForWin = 4;
    this->selectedCell = 0;
    this->boardSizeW = 9;
    this->boardSizeH = 9;
    this->state.assign(this->boardSizeW * this->boardSizeH, Board::blank);
}

int Board::getSizeW() {
	return this->boardSizeW;
}

int Board::getSizeH() {
	return this->boardSizeH;
}

int Board::getSize() {
	return this->state.size();
}

void Board::setSize(int sizeW, int sizeH) {
	this->boardSizeW = sizeW;
	this->boardSizeH = sizeH;
	this->state.clear();
	this->state.assign(this->boardSizeW * this->boardSizeH, Board::blank);
}

Board::cellState Board::getCellState(int cell) {
	return this->state.at(cell);
}
void Board::setCellState(int cell, Board::cellState state) {
	this->state.at(cell) = state;
}

int Board::getSelectedCell() {
	return this->selectedCell;
}

void Board::setSelectedCell(int cell) {
	this->selectedCell = cell;
}

void Board::makeMove() {
	if (this->gameOver) return;
	
	cellState state = this->state.at(this->selectedCell);
	if (state == blank) {
		this->state.at(this->selectedCell) = (this->turn == Board::playerOne) ? Board::cross : Board::nought;
	}
}

void Board::switchPlayers() {
	this->turn = (this->turn == Board::playerOne) ? Board::playerTwo : Board::playerOne;
}

int Board::getNumberOfSymbolsForWin() {
	return this->numberOfSymbolsForWin;
}
void Board::setNumberOfSymbolsForWin(int numberOfSymbolsForWin) {
	this->numberOfSymbolsForWin = numberOfSymbolsForWin;
}

bool Board::isGameOver() {
	return this->gameOver;
}
void Board::setGameOver(bool value) {
	this->gameOver = value;
}

bool Board::checkPattern(std::vector<Board::cellState> pattern) {
	Board::cellState symbolToCheck = (this->turn == Board::playerOne) ? Board::cross : Board::nought;
	int count;
	
	for (Board::cellState state : pattern) {
		if (state == symbolToCheck) {
			count++;
			if (count >= this->numberOfSymbolsForWin)
				return true;
		} else {
			count = 0;
		}
	}
	
	return false;
}

Board::cellState Board::validate() {
	struct {
		int left, top, right, bottom;
	} distandeFromEdge = {
		left: getSelectedCell() % boardSizeW,
		top: selectedCell / boardSizeW,
		right: boardSizeW - selectedCell % boardSizeW - 1,
		bottom: boardSizeH - selectedCell / boardSizeW - 1,
	};
	bool resultRow, resultColumn, resultDiagonal, resultAntiDiagonal;
	int firstCell, lastCell, step;
	std::vector<Board::cellState> pattern;
	
	// Check the row last move was made in
	firstCell = this->selectedCell - this->selectedCell % this->boardSizeW;
	lastCell = firstCell + this->boardSizeW;
	for (int i = firstCell; i < lastCell; i++) {
		pattern.push_back(this->state.at(i));
	}
	
	resultRow = this->checkPattern(pattern);
	pattern.clear();
	
	// Check the column last move was made in
	int column = this->selectedCell % this->boardSizeW;
	for (int i = 0; i < this->state.size(); i++) {
		if (i % this->boardSizeW == column)
			pattern.push_back(this->state.at(i));
	}
	
	resultColumn = this->checkPattern(pattern);
	pattern.clear();
	
	// Check the diagonal last move was made in
	step = this->boardSizeW + 1;
	firstCell = this->selectedCell - std::min(distandeFromEdge.left, distandeFromEdge.top) * step;
	lastCell = this->selectedCell + std::min(distandeFromEdge.right, distandeFromEdge.bottom) * step;
	for (int i = firstCell; i <= lastCell; i+= step)
		pattern.push_back(this->state.at(i));
	
	resultDiagonal = this->checkPattern(pattern);
	pattern.clear();
	
	// Check the anti-diagonal last move was made in
	step = this->boardSizeW - 1;
	firstCell = this->selectedCell - std::min(distandeFromEdge.top, distandeFromEdge.right) * step;
	lastCell = this->selectedCell + std::min(distandeFromEdge.left, distandeFromEdge.bottom) * step;
	for (int i = firstCell; i <= lastCell; i+= step)
		pattern.push_back(this->state.at(i));

	resultAntiDiagonal = this->checkPattern(pattern);
	pattern.clear();
	
	if (resultRow || resultColumn || resultDiagonal || resultAntiDiagonal) {
		this->gameOver = true;
		MessageBox(NULL, (this->turn == Board::playerOne) ? "Crosses win the game!" : "Noughts win the game!", "Congratulations!", MB_ICONINFORMATION);
	}
}
