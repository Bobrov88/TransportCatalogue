#pragma once
#include "json.h"
#include <stack>

namespace json
{
    class Builder
    {
    public:
        Builder();
        ~Builder();
        Builder &Value(Node value);
        Builder &Key(std::string key);
        Builder &StartDict();
        Builder &EndDict();
        Builder &StartArray();
        Builder &EndArray();

        json::Node Build();

        void CheckCallMethod(char method);

    private:
        std::stack<Node> nodes_;
        std::stack<char> call_stack_;
    };
}