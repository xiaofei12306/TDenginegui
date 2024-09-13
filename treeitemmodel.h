#ifndef TREEITEMMODEL_H
#define TREEITEMMODEL_H

#include <QVariant>

enum Level {
    CLIENT,
    DATABASE,
    TABLE,
    SUBTABLE,
};

class TreeItemModel
{
private:
    Level level;
    QVariant value;

public:
    TreeItemModel();
    TreeItemModel(Level level);
    TreeItemModel(Level level, const QVariant &value);

    Level getLevel() const;
    QVariant getValue() const;
    void setValue(const QVariant &newValue);
    void setLevel(Level newLevel);
    QVariant toVariant() const;
    static TreeItemModel fromVariant(const QVariant &variant);
};

#endif // TREEITEMMODEL_H
