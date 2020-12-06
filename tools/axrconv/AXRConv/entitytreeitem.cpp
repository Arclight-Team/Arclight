#include "entitytreeitem.h"


EntityTreeItem::EntityTreeItem(const QVector<QVariant>& data, EntityTreeItem* parent) : itemData(data), parent(parent) {}



EntityTreeItem::~EntityTreeItem() {
    qDeleteAll(children);
}



void EntityTreeItem::appendChild(EntityTreeItem *item) {
    children.append(item);
}



void EntityTreeItem::removeChild(int row) {

    EntityTreeItem* item = child(row);

    if(item){

        delete item;
        children.remove(row);

    }

}




void EntityTreeItem::clearChildren() {

    for(int i = 0; i < childCount(); i++){

        EntityTreeItem* item = child(i);

        if(item) {
            delete item;
        }

    }

    children.clear();

}




EntityTreeItem* EntityTreeItem::child(int row) {

    if (row < 0 || row >= children.size()){
        return nullptr;
    }

    return children.at(row);

}



int EntityTreeItem::childCount() const {
    return children.count();
}



int EntityTreeItem::columnCount() const {
    return itemData.count();
}



QVariant EntityTreeItem::data(int column) const {

    if (column < 0 || column >= itemData.size()){
        return QVariant();
    }

    return itemData.at(column);

}



EntityTreeItem* EntityTreeItem::parentItem() {
    return parent;
}



int EntityTreeItem::row() const {

    if (parent){
        return parent->children.indexOf(const_cast<EntityTreeItem*>(this));
    }

    return 0;

}



bool EntityTreeItem::setData(int column, const QVariant& value) {

    if (column < 0 || column >= itemData.size()){
        return false;
    }

    itemData[column] = value;

    return true;

}
