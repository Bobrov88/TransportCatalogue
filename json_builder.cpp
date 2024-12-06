#include "json_builder.h"

namespace json
{
    using namespace std::string_literals;

    Builder::Builder()
    {
    //    PushStack("Builder"s);
    }

    Builder::~Builder()
    {
    }

    Builder &Builder::Value(json::Node value)
    {
    //    PushStack("Value"s);
        nodes_.emplace(std::move(value));
        return *this;
    }

    Builder &Builder::Key(std::string key)
    {
     //   PushStack("Key"s);
        nodes_.emplace(std::move(key));
        return *this;
    }

    Builder &Builder::StartDict()
    {
      //  PushStack("StartDict"s);
        nodes_.emplace(Dict{});
        return *this;
    }

    Builder &Builder::EndDict()
    {
    //    PushStack("EndDict"s);
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
    //    PushStack("StartArray"s);
        nodes_.emplace(Array{});
        return *this;
    }

    Builder &Builder::EndArray()
    {
   //     PushStack("EndArray"s);
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
     //   PushStack("Builder"s);
        return nodes_.top();
    }

    void Builder::PushStack(std::string method)
    {
        if (method == "Builder"s)
        {
            if (!call_stack_.empty())
            {
                if (call_stack_.top() != "Builder"s)
                    throw std::logic_error("Incorrect 'Builder' call");
                else
                {
                    call_stack_.pop();
                    return;
                }
            }
        }
        else if (method == "Key"s)
        {
            if (call_stack_.top() != "StartDict"s)
                throw std::logic_error("Incorrect 'key' call");
        }
        else if (method == "Value"s ||
                 method == "StartDict"s ||
                 method == "StartArray"s)
        {
            if (call_stack_.top() != "Builder"s &&
                call_stack_.top() != "Key"s &&
                call_stack_.top() != "Value"s)
                throw std::logic_error("Incorrect '" + method + "' call");

            if (call_stack_.top() == "Key"s || call_stack_.top() == "Value"s)
                call_stack_.pop();
        }
        else if (method == "EndDict"s)
        {
            if (call_stack_.top() == "Value"s)
                call_stack_.pop();
            if (call_stack_.top() != "StartDict"s)
                throw std::logic_error("Incorrect '" + method + "' call");
            call_stack_.pop();
            return;
        }
        else if (method == "EndArray"s)
        {
            if (call_stack_.top() == "Value"s)
                call_stack_.pop();
            if (call_stack_.top() != "StartArray"s)
                throw std::logic_error("Incorrect '" + method + "' call");
            call_stack_.pop();
            return;
        }
        call_stack_.push(std::move(method));
    }
}