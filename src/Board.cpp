#include "../include/Board.hpp"

#include <sstream>
#include <vector>

Board::Board::Board() : squares{new uint16_t[64]} {
	init_board();
}

Board::Board::~Board() {
    delete[] squares;
}

std::map<char, Piece> Board::Board::fen_to_piece {
	{'p', Piece::PAWN},
	{'n', Piece::KNIGHT},
	{'b', Piece::BISHOP},
	{'r', Piece::ROOK},
	{'q', Piece::QUEEN},
	{'k', Piece::KING}
};

void Board::Board::set_squares(int pos, uint16_t value) {
	squares[pos] = value;
}

void Board::Board::init_board() {
	uint16_t file = 0;
	uint16_t rank = 0;

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
				Piece type = Board::fen_to_piece[static_cast<char>(tolower(sym))];

				set_squares(rank * 8 + file, color | type);
				file++;
			}
		}
	}
}
