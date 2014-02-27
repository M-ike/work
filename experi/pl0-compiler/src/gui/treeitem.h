#ifndef TREEITEM_H
#define TREEITEM_H

#include <QTreeWidgetItem>
#include <QStringList>

template <class T>
class TreeItem: public QTreeWidgetItem
{
 public:
     TreeItem(QTreeWidgetItem *parent, const QStringList &strings)
     : QTreeWidgetItem(parent, strings) {}
     T *getReference() { return reference; }
     void setReference(T *reference) { this->reference = reference; }

 private:
     T *reference;
};


#endif
