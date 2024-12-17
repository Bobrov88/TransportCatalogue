#pragma once
#include "json.h"
#include <stack>

namespace json
{
    class Builder
    {
        class BuilderContext;
        class KeyContext;
        class ArrayContext;
        class DictContext;
        class FinalContext;

    public:
        Builder();
        ~Builder();
        FinalContext Value(Node value);
        DictContext StartDict();
        ArrayContext StartArray();
        void KeyInternal(std::string key);
        void ValueInternal(Node value);
        void StartDictInternal();
        void StartArrayInternal();
        void EndDictInternal();
        void EndArrayInternal();
        const Node &GetRoot() const;

    private:
        void CheckCallMethod(char method) const;
        std::stack<Node> nodes_;
        mutable std::stack<char> call_stack_;

        class BuilderContext
        {
        public:
            Builder &builder_;

            BuilderContext(Builder &builder) : builder_(builder) {}
            DictContext StartDict()
            {
                builder_.StartDictInternal();
                return DictContext(builder_);
            }
            KeyContext Key(std::string value)
            {
                builder_.KeyInternal(std::move(value));
                return KeyContext(builder_);
            }
            ArrayContext StartArray()
            {
                builder_.StartArray();
                return ArrayContext(builder_);
            }
            BuilderContext Value(Node value)
            {
                builder_.ValueInternal(std::move(value));
                return BuilderContext(builder_);
            }
            BuilderContext EndDict()
            {
                builder_.EndDictInternal();
                return BuilderContext(builder_);
            }
            BuilderContext EndArray()
            {
                builder_.EndArrayInternal();
                return BuilderContext(builder_);
            }
            Node Build()
            {
                return builder_.GetRoot();
            }
        };

        class KeyContext final : public BuilderContext
        {
        public:
            KeyContext(Builder &builder) : BuilderContext(builder) {}
            DictContext Key(std::string value) = delete;
            BuilderContext EndDict() = delete;
            BuilderContext EndArray() = delete;
            DictContext Value(Node value)
            {
                builder_.ValueInternal(std::move(value));
                return DictContext(builder_);
            }
            Node Build() = delete;
        };

        class ArrayContext final : public BuilderContext
        {
        public:
            ArrayContext(Builder &builder) : BuilderContext(builder) {}
            DictContext Key(std::string value) = delete;
            BuilderContext EndDict() = delete;
            ArrayContext Value(Node value)
            {
                builder_.ValueInternal(std::move(value));
                return ArrayContext(builder_);
            }
            Node Build() = delete;
        };

        class DictContext final : public BuilderContext
        {
        public:
            DictContext(Builder &builder) : BuilderContext(builder) {}
            ArrayContext StartArray() = delete;
            DictContext StartDict() = delete;
            BuilderContext EndArray() = delete;
            BuilderContext Value(Node value) = delete;
            Node Build() = delete;
        };

        class FinalContext final
        {
            Builder &builder_;

        public:
            FinalContext(Builder &builder) : builder_(builder) {}
            const Node &Build()
            {
                return builder_.GetRoot();
            }
        };
    };
}