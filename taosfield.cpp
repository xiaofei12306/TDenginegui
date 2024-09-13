#include "taosfield.h"
#include <algorithm>

TaosField::TaosField() {}

int8_t TaosField::getType() const
{
    return type;
}

void TaosField::setType(int8_t newType)
{
    type = newType;
}

int32_t TaosField::getBytes() const
{
    return bytes;
}

void TaosField::setBytes(int32_t newBytes)
{
    bytes = newBytes;
}

QString TaosField::getName() const
{
    return name;
}

void TaosField::setName(const QString &newName)
{
    name = newName;
}

TaosField::TaosField(int8_t type, int32_t bytes) : type(std::move(type)),
    bytes(std::move(bytes))
{}

TaosField::TaosField(int8_t type, int32_t bytes, const QString &name) : type(std::move(type)),
    bytes(std::move(bytes)),
    name(name)
{}
