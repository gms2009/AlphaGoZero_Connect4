//
// Created by aherrera on 3/29/20.
//

#ifndef ALPHAZERO_CONNECT4_UTILS_H
#define ALPHAZERO_CONNECT4_UTILS_H

#include <torch/torch.h>
#include <Eigen/Core>
#include <type_traits>
#include <numeric>
#include <limits>

constexpr float inf_float = std::numeric_limits<float>::infinity();

enum Player {
    None,
    Player1,
    Player2
};

// -1 for white, 0 for none, 1 for black
typedef Eigen::Matrix<Player, 6, 7> State;
typedef Eigen::Array<int64_t, 7, 1> Vector7i;
typedef Eigen::Array<float, 7, 1> Vector7f;
typedef Eigen::Array<double, 7, 1> Vector7d;

bool operator<(const State &a, const State &b);

struct TensorPair {
    torch::Tensor tensor1;
    torch::Tensor tensor2;

    TensorPair() = default;

    TensorPair(const torch::Tensor &t1, const torch::Tensor &t2);
};

struct MoveResult {
    State state;
    bool done;

    MoveResult(State st, bool wn);
};

struct LeafResult {
    float value;
    State current_state;
    Player current_player;
    std::vector<State> states;
    std::vector<int64_t> actions;

    LeafResult(float val, State cur_state, Player pl, std::vector<State> st,
               std::vector<int64_t> acts);
};

Vector7f generate_dirichlet(const Vector7d &alpha);

template<typename T>
constexpr torch::ScalarType get_torch_type() {
    if (std::is_same<T, int8_t>::value)
        return torch::kInt8;
    else if (std::is_same<T, int16_t>::value)
        return torch::kInt16;
    else if (std::is_same<T, int32_t>::value)
        return torch::kInt32;
    else if (std::is_same<T, int64_t>::value)
        return torch::kInt64;
    else if (std::is_same<T, uint8_t>::value)
        return torch::kUInt8;
    else if (std::is_same<T, torch::Half>::value)
        return torch::kFloat16;
    else if (std::is_same<T, float>::value)
        return torch::kFloat32;
    else if (std::is_same<T, double>::value)
        return torch::kFloat64;
    else {
        std::cerr << "Error: type not supported";
        return torch::ScalarType::Undefined;
    }
}

template<typename Derived>
torch::Tensor toTensor(const Eigen::DenseBase<Derived> &b) {
    typedef typename Eigen::internal::traits<Derived>::Scalar scalar_type;

    std::vector<int64_t> shape = (b.cols() != 1) ? std::vector<int64_t>{b.rows(), b.cols()} : std::vector<int64_t>{
            b.rows()};
    int64_t size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
    torch::ScalarType dtype = get_torch_type<scalar_type>();
    auto options = torch::TensorOptions().dtype(dtype);
    torch::Tensor out_tensor = torch::zeros(shape, options);

    scalar_type *out_data = out_tensor.data_ptr<scalar_type>();
    const scalar_type *in_data = &b(0);
    if (shape.size() == 2)
        std::reverse_copy(in_data, in_data + size, out_data);
    else
        std::copy(in_data, in_data + size, out_data);
    return out_tensor;
}

template<typename T, int N, int M>
Eigen::Matrix<T, N, M> toEigenMatrix(const torch::Tensor &tensor) {
    assert(tensor.scalar_type() == get_torch_type<T>());
    assert(tensor.dim() <= 2);
    auto shape = tensor.sizes();
    assert(shape[0] == N || N == Eigen::Dynamic);
    assert(((shape.size() == 2) ? shape[1] == M : 1 == M) || M == Eigen::Dynamic);
    int64_t size = (shape.size() == 2) ? N * M : N;
    Eigen::Matrix<T, N, M> out_matrix = (shape.size() == 2) ? Eigen::Matrix<T, N, M>::Zero(shape[0], shape[1])
                                                            : Eigen::Matrix<T, N, M>::Zero(shape[0], 1);
    T *out_data = &out_matrix(0);
    const T *in_data = tensor.data_ptr<T>();
    if (shape.size() == 2)
        std::reverse_copy(in_data, in_data + size, out_data);
    else
        std::copy(in_data, in_data + size, out_data);
    return out_matrix;
}

template<typename T, int N, int M>
Eigen::Array<T, N, M> toEigenArray(const torch::Tensor &tensor) {
    assert(tensor.scalar_type() == get_torch_type<T>());
    assert(tensor.dim() <= 2);
    auto shape = tensor.sizes();
    assert(shape[0] == N || N == Eigen::Dynamic);
    assert(((shape.size() == 2) ? shape[1] == M : 1 == M) || M == Eigen::Dynamic);
    int64_t size = (shape.size() == 2) ? N * M : N;
    Eigen::Array<T, N, M> out_arr = (shape.size() == 2) ? Eigen::Array<T, N, M>::Zero(shape[0], shape[1])
                                                        : Eigen::Array<T, N, M>::Zero(shape[0], 1);
    T *out_data = &out_arr(0);
    const T *in_data = tensor.data_ptr<T>();
    if (shape.size() == 2)
        std::reverse_copy(in_data, in_data + size, out_data);
    else
        std::copy(in_data, in_data + size, out_data);
    return out_arr;
}

#endif //ALPHAZERO_CONNECT4_UTILS_H
