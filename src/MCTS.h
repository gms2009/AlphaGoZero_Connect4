//
// Created by aherrera on 3/27/20.
//

#ifndef ALPHAZERO_CONNECT4_MCTS_H
#define ALPHAZERO_CONNECT4_MCTS_H

#include <map>
#include <cstdint>
#include "utils.h"

class MCTS {
    float _c_puct;
    std::map<State, Vector7i> _visit_count;
    std::map<State, Vector7f> _value;
    std::map<State, Vector7f> _value_avg;
    std::map<State, Vector7f> _probs;

public:
    explicit MCTS(float c_punct = 1.0f);
    void clear();
    uint64_t size();
    void find_leaf(const State &state, Player player);
    bool is_leaf(const State &state);
};


#endif //ALPHAZERO_CONNECT4_MCTS_H
