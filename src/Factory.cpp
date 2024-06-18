#include "Factory.h"

namespace Galvanizer
{

Factory::Factory(std::string_view name, Factory* parent)
    : name(name), parent(parent), rootOwner(parent->rootOwner)
{
    parent->children.push_back(this);
}

Factory::Factory(std::string_view name, std::string* rootOwner)
    : name(name), parent(nullptr), rootOwner(rootOwner)
{}

Factory::~Factory()
{
    for (auto& ch: children)
        delete ch;
}


Factory* Factory::Find(std::string_view name)
{
    for (const auto& ch: children)
        if (ch->name == name)
            return ch;

    return nullptr;
}


bool FactoryRoot::operator==(std::string_view testOwner) const
{
    return this->ownerName == testOwner;
}
}
