#pragma once

#include <QtCore/QAbstractTableModel>

#include "mainwindow.h"


class BinaryViewModel : public QAbstractTableModel {
    Q_OBJECT
    BinaryTab* bdata;
    QString* strings;
    int base;
    int rowsNum;
    int colsNum=8;
public:
    BinaryViewModel(QObject *parent,int base,BinaryTab* tab);
    ~BinaryViewModel() {
        delete[] strings;
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeColumns ( int column, int count, const QModelIndex & parent = QModelIndex() )override;
    bool insertColumns ( int column, int count, const QModelIndex & parent = QModelIndex() )override;
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() )override;
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() )override;
    bool setHeaderData(int section, Qt::Orientation orientatiob ,const QVariant& value,int role=0) override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    Qt::ItemFlags flags(const QModelIndex & /*index*/) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setColumnCount(int num);
    QString CharToQString(unsigned char c);
    void UpdateString(unsigned int stringNum);
};

///////////////////////////////////////////////
class TreeItem {
public:
    TreeItem( TreeItem *parent = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void appendData(QVariant var);
    int row() const;
    TreeItem *parent();

private:
    QList<TreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItem *parentItem;
};

 class TreeModel : public QAbstractItemModel
 {
     Q_OBJECT

 public:
     TreeModel(ModuleData& data, QObject *parent = 0);
     ~TreeModel();

     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

 private:
     TreeItem *rootItem;
 };
