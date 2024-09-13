#include "treeitemmodel.h"
#include "sqlclient.h"

TreeItemModel::TreeItemModel() {}

Level TreeItemModel::getLevel() const
{
    return level;
}

QVariant TreeItemModel::getValue() const
{
    return value;
}

void TreeItemModel::setValue(const QVariant &newValue)
{
    value = newValue;
}

TreeItemModel::TreeItemModel(Level level, const QVariant &value)
    : level(level)
    , value(value)
{}

TreeItemModel::TreeItemModel(Level level)
    : level(level)
{}

QVariant TreeItemModel::toVariant() const {
    QVariantMap map;
    map["level"] = static_cast<int>(level); // 将枚举值转换为 int
    sqlClient client = value.value<sqlClient>();
    map["value"] = client.toVariant();
    return QVariant::fromValue(map);
}

TreeItemModel TreeItemModel::fromVariant(const QVariant &variant) {
    QVariantMap map = variant.toMap();
    Level level = static_cast<Level>(map.value("level").toInt()); // 将 int 转换回枚举值
    QVariant value = map.value("value");
    sqlClient client = sqlClient::fromVariant(value);
    return TreeItemModel(level, QVariant::fromValue(client));
}
