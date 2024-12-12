#pragma once
#include "json.h"
#include <stack>

namespace json
{
    class Builder
    {
    public:
        Builder();
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
        void CheckCallMethod(char method);

    public:
        Builder() : BaseMethods(*this)
        {
            CheckCallMethod('B');
        }

        const Node &GetRoot()
        {
            CheckCallMethod('B');
            return nodes_.top();
        }

        BaseMethods EndDict() = delete;
        BaseMethods EndArray() = delete;
        KeyContext Key(std::string key) = delete;
        const Node &Build() = delete;

        void StartDictInternal();
        void StartArrayInternal();
        void EndDictInternal();
        void EndArrayInternal();
        void KeyInternal(std::string key);
        void ValueInternal(Node value);
    };

    class KeyContext final : public BaseMethods
    {
    public:
        KeyContext(Builder &builder) : BaseMethods(builder) {}
        // DictContext Value(Node value)
        // {
        //     builder_.ValueInternal(std::move(value));
        //     return DictContext(builder_);
        // }
        BaseMethods EndDict() = delete;
        BaseMethods EndArray() = delete;
        KeyContext Key(std::string key) = delete;
        const Node &Build() = delete;
    };

    class DictContext final : public BaseMethods
    {
    public:
        DictContext(Builder &builder) : BaseMethods(builder) {}
        DictContext StartDict() = delete;
        ArrayContext StartArray() = delete;
        BaseMethods EndArray() = delete;
        const Node &Build() = delete;
    };

    class ArrayContext final : public BaseMethods
    {
    public:
        ArrayContext(Builder &builder) : BaseMethods(builder) {}
        // ArrayContext Value(Node value)
        // {
        //     builder_.ValueInternal(std::move(value));
        //     return ArrayContext(builder_);
        // }
        // BaseMethods EndDict() = delete;
        KeyContext Key(std::string key) = delete;
        const Node &Build() = delete;
    };

    class BaseMethods
    {
    public:
        Builder &builder_;
        BaseMethods(Builder &builder) : builder_(builder) {}

        DictContext StartDict()
        {
            builder_.StartDictInternal();
            return DictContext(builder_);
        }
        ArrayContext StartArray()
        {
            builder_.StartArrayInternal();
            return ArrayContext(builder_);
        }
        BaseMethods EndDict()
        {
            builder_.EndDictInternal();
            return BaseMethods(builder_);
        }
        BaseMethods EndArray()
        {
            builder_.EndArrayInternal();
            return BaseMethods(builder_);
        }
        KeyContext Key(std::string key)
        {
            builder_.KeyInternal(std::move(key));
            return KeyContext(builder_);
        }
        BaseMethods Value(Node value)
        {
            builder_.ValueInternal(std::move(value));
            return BaseMethods(builder_);
        }
        const Node &Build()
        {
            return builder_.GetRoot();
        }
    };
}