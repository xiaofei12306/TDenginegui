#ifndef TAOSFIELD_H
#define TAOSFIELD_H

#include <QString>
#include <cstdint>
#include <taos.h>
class TaosField
{
public:
    TaosField();
    TaosField(int8_t type, int32_t bytes);
    TaosField(int8_t type, int32_t bytes, const QString &name);
    int8_t getType() const;
    void setType(int8_t newType);

    int32_t getBytes() const;
    void setBytes(int32_t newBytes);

    QString getName() const;
    void setName(const QString &newName);

private:
    int8_t  type;
    int32_t bytes;
    QString name;
};

#endif // TAOSFIELD_H
