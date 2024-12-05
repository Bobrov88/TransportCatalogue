#include "json_builder.h"

namespace json
{
    using namespace std::string_literals;

    Builder::Builder()
    {
        call_stack_.push("Builder"s);
    }

    Builder &Builder::Value(json::Node value)
    {
        PushStack("Value"s);

        return *this;
    }

    Builder &Builder::Key(std::string key)
    {
        PushStack("Key"s);

        return *this;
    }

    Builder &Builder::StartDict()
    {
        PushStack("StartDict"s);

        return *this;
    }

    Builder &Builder::EndDict()
    {
        PushStack("EndDict"s);

        return *this;
    }

    Builder &Builder::StartArray()
    {
        PushStack("StartArray"s);

        return *this;
    }

    Builder &Builder::EndArray()
    {
        PushStack("EndArray"s);

        return *this;
    }

    Node Builder::Build()
    {
        PushStack("Builder"s);
        
        return Node();
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