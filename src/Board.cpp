#include "../include/Board.hpp"

#include <sstream>
#include <vector>

Board::Board::Board() : squares{new uint16_t[64]} {
	init_board();
}

Board::Board::~Board() {
    delete[] squares;
}

void Board::Board::init_board() {
	uint16_t file = 0;
	uint16_t rank = 7;

	std::vector<std::string> splitted_fen;
	std::stringstream stream {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
	std::string tmp;

	while(std::getline(stream, tmp, ' ')) {
		splitted_fen.push_back(tmp);
	}

	for(auto& sym : splitted_fen.front()) {
		if(sym == '/') {
			file = 0;
			rank--;
		} else {
			if(isdigit(sym)) {
				file += sym - '0';
			} else {
				Piece color = isupper(sym) ? Piece::WHITE : Piece::BLACK;
				Piece type = Board::fen_to_piece.at(std::tolower(sym));

				squares[rank * 8 + file] = color | type;
				file++;
			}
		}
	}
}
