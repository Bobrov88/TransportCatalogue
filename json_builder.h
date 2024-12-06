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

        void PushStack(std::string method);

    private:
        std::stack<Node> nodes_;
        std::stack<std::string> call_stack_;
    };

}