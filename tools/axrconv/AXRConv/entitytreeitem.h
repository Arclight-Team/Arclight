#ifndef ENTITYTREEITEM_H
#define ENTITYTREEITEM_H

#include <QVariant>
#include <QVector>


enum class EntityType {
    Node,
    Mesh
};


class EntityTreeItem {

public:
    explicit EntityTreeItem(const QVector<QVariant>& data, EntityTreeItem* parent = nullptr);
    ~EntityTreeItem();

    void appendChild(EntityTreeItem* child);
    void removeChild(int row);
    void clearChildren();

    EntityTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    EntityTreeItem* parentItem();
    bool setData(int column, const QVariant& value);

private:
    QVector<EntityTreeItem*> children;
    QVector<QVariant> itemData;
    EntityTreeItem* parent;

};


#endif // ENTITYTREEITEM_H
