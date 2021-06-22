#pragma once

#include "../include/Piece.hpp"

#include <cstdint>

namespace Board {
    class Board {
    private:
        bool turn = 0;
        uint16_t* squares;
    public:
        Board();
        ~Board();
    };
};