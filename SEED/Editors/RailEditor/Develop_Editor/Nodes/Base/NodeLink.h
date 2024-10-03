#pragma once
#include <cstdint>

// ノード間を接続するリンクの情報を格納する構造体
struct NodeLink{

    NodeLink() = default;

    static int32_t nextID;
    int32_t id;
    int32_t start_attr;  // 出力ピンのID
    int32_t end_attr;    // 入力ピンのID
};