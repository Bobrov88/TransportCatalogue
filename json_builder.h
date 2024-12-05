#pragma once
#include "json.h"
#include <stack>
#include <functional>

namespace json
{
    class Builder
    {
    public:
        Builder();

        Builder &Value(json::Node value);
        Builder &Key(std::string key);
        Builder &StartDict();
        Builder &EndDict();
        Builder &StartArray();
        Builder &EndArray();

        json::Node Build();

        void PushStack(std::string method);

    private:
        std::stack<std::string> call_stack_;
        Node root_;
    };

}