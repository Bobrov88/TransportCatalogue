#include "json_builder.h"
#include <algorithm>

namespace json
{
    using namespace std::string_literals;

    Builder::Builder()
    {
        CheckCallMethod('B');
    }

    Builder &Builder::Value(json::Node value)
    {
        CheckCallMethod('V');
        nodes_.emplace(std::move(value));
        return *this;
    }

    Builder &Builder::Key(std::string key)
    {
        CheckCallMethod('K');
        nodes_.emplace(std::move(key));
        return *this;
    }

    Builder &Builder::StartDict()
    {
        CheckCallMethod('D');
        nodes_.emplace(Dict{});
        return *this;
    }

    Builder &Builder::EndDict()
    {
        CheckCallMethod('d');
        std::map<std::string, Node> tmp;
        while (!nodes_.top().IsMap())
        {
            Node node = std::move(nodes_.top());
            nodes_.pop();
            tmp[std::move(nodes_.top().AsString())] = std::move(node);
            nodes_.pop();
        }
        nodes_.pop();
        nodes_.push(std::move(tmp));
        return *this;
    }

    Builder &Builder::StartArray()
    {
        CheckCallMethod('A');
        nodes_.emplace(Array{});
        return *this;
    }

    Builder &Builder::EndArray()
    {
        CheckCallMethod('a');
        std::vector<Node> tmp;
        while (!nodes_.top().IsArray())
        {
            tmp.push_back(std::move(nodes_.top()));
            nodes_.pop();
        }
        nodes_.pop();
        std::reverse(tmp.begin(), tmp.end());
        nodes_.push(std::move(tmp));
        return *this;
    }

    Node Builder::Build()
    {
        CheckCallMethod('b');
        return nodes_.top();
    }

    void Builder::CheckCallMethod(char method)
    {
        /*
         * Обозначения:
         * B - Builder
         * b - build
         * D - StartDict
         * d - EndDict
         * A - StartArray
         * a - EndArray
         * K - Key
         * V - Value
         */

        switch (method)
        {
        case 'B':
            if (!call_stack_.empty())
            {
                throw std::logic_error("Invalid call sequence: Builder already initiated.");
            }
            call_stack_.push(method);
            break;

        case 'b':
            if (call_stack_.top() != 'B')
            {
                throw std::logic_error("Building incomplete object.");
            }
            call_stack_.pop(); // Удаляем 'B' при завершении
            break;

        case 'D':
            if (call_stack_.empty() || (call_stack_.top() != 'B' && call_stack_.top() != 'A' && call_stack_.top() != 'K' && call_stack_.top() != 'V'))
            {
                throw std::logic_error("StartDict can only be called after Builder, StartArray, Key, or Value.");
            }
            if (!call_stack_.empty() && call_stack_.top() == 'K')
            {
                call_stack_.pop();
            }
            call_stack_.push(method);
            break;

        case 'd':
            if (call_stack_.top() != 'D')
            {
                throw std::logic_error("EndDict not matched with StartDict.");
            }
            call_stack_.pop(); // Удаляем 'D' при завершении
            break;

        case 'A':
            if (call_stack_.empty() || (call_stack_.top() != 'B' && call_stack_.top() != 'A' && call_stack_.top() != 'K' && call_stack_.top() != 'V'))
            {
                throw std::logic_error("StartArray can only be called after Builder, StartArray, Key, or Value.");
            }
            if (!call_stack_.empty() && call_stack_.top() == 'K')
            {
                call_stack_.pop();
            }
            call_stack_.push(method);
            break;

        case 'a':
            if (call_stack_.top() != 'A')
            {
                throw std::logic_error("EndArray not matched with StartArray.");
            }
            call_stack_.pop(); // Удаляем 'A' при завершении
            break;

        case 'K':
            if (call_stack_.empty() || (call_stack_.top() != 'D' && call_stack_.top() != 'V'))
            {
                throw std::logic_error("Key must follow StartDict or Value.");
            }
            call_stack_.push(method);
            break;

        case 'V':
            if (call_stack_.empty() || (call_stack_.top() != 'K' && call_stack_.top() != 'A' && call_stack_.top() != 'B'))
            {
                throw std::logic_error("Value must follow Key, StartDict, StartArray or another Value.");
            }
            if (!call_stack_.empty() && call_stack_.top() == 'K')
            {
                call_stack_.pop();
                if (call_stack_.top() == 'B')
                {
                    throw std::logic_error("Value must follow Key, StartDict, StartArray or another Value.");
                }
            }
            break;

        default:
            throw std::logic_error("Invalid method call.");
        }
    }
}