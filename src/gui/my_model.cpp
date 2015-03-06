#include "my_model.h"
#include <cassert>
#include <cstdio>
BinaryViewModel::BinaryViewModel(QObject *parent,int base_,BinaryTab* tab)
    :QAbstractTableModel(parent),bdata(tab),base(base_) {
    rowsNum=bdata->size/colsNum+1;
    assert(base==2 or base==4 or  base==16 or  base==10);
    strings=new QString[bdata->size];
    unsigned char* d= (unsigned char*)bdata->data;
    for(unsigned int j=0; j<bdata->size; j++) {
        strings[j]= CharToQString(d[j]);
    }
}

int BinaryViewModel::rowCount(const QModelIndex & /*parent*/) const {
    return rowsNum;
}

int BinaryViewModel::columnCount(const QModelIndex & /*parent*/) const {
    return colsNum;
}
void BinaryViewModel::setColumnCount(int num) {
    int dt=num-colsNum;
    if(dt>0) {
        insertColumns(0,dt);
    } else {
        removeColumns(0,-dt);
    }
    dt=bdata->size/num+1-rowsNum;
    if(dt>0) {
        insertRows(0,dt);
    } else {
        removeRows(0,-dt);
    }
    rowsNum=bdata->size/num+1;
    colsNum=num;
}
bool BinaryViewModel::removeRows ( int row, int count, const QModelIndex & parent ) {
    colsNum-=count;
    beginRemoveRows(parent,row,row+count-1);
    endRemoveRows();
    return true;
}
bool BinaryViewModel::insertRows ( int row, int count, const QModelIndex & parent  ) {
    colsNum+=count;
    beginInsertRows(parent,row,row+count-1);
    endInsertRows();
    return true;
}
bool BinaryViewModel::removeColumns ( int /*column*/, int count, const QModelIndex & parent ) {
    colsNum-=count;
    beginRemoveColumns(parent,0,count-1);
    endRemoveColumns();
    return true;
}
bool BinaryViewModel::insertColumns ( int /*column*/, int count, const QModelIndex & parent  ) {
    colsNum+=count;
    beginInsertColumns(parent,0,count-1);
    endInsertColumns();
    return true;
}
QVariant BinaryViewModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    unsigned int num=row*colsNum+col;
    if (role == Qt::DisplayRole) {
        if(bdata->size<=num)return QString();
        return strings[num];
    } else if (role == Qt::ForegroundRole ) {
        for(auto p: bdata->usedBlocks) {
            if(num>=p.first and num<p.second) {
                return QColor(10,130,10);
            }
        }
    }
    return QVariant();
}
Qt::ItemFlags BinaryViewModel::flags(const QModelIndex & index) const {
    unsigned int num=index.row()*colsNum+index.column();
    if(bdata->size<num)return 0;
    return   Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}
bool BinaryViewModel::setHeaderData(int section, Qt::Orientation orientation ,const QVariant& /*value*/,int /*role*/) {
    if(orientation==Qt::Horizontal and section>colsNum) {
        colsNum=section;
        rowsNum=bdata->size/colsNum;

        emit layoutChanged();
    }
    return true;
}
bool BinaryViewModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role == Qt::EditRole) {
        QString s=value.toString();
        int c = value.toInt();
        //printf("-----| %d\n",c%16);
        int num=index.row()*colsNum+index.column();
        bdata->data[num]=c;
        for(auto model:bdata->models) {
            model->UpdateString(num);
        }

    }
    return true;
}


QString BinaryViewModel::CharToQString(unsigned char c)  {
    if(base==2) {
        QString str;
        const char chars[2]= {'0','1'};
        for(int i=0; i<8; i++) {
            str.push_front(chars[0b00000001 & c]);
            c=c>>1;
        }
        return str;
    } else if(base==4) {
        QString str;
        for(int i=0; i<4; i++) {
            str.push_front('0'+c%4);
            c=c>>2;
        }
        return str;
    } else if(base==16) {
        QString str;
        for(int i=0; i<2; i++) {
            int a=c%16;
            if(a<10) {
                str.push_front('0'+a);
            } else {
                str.push_front('A'-10+a);
            }
            c=c>>4;
        }
        return str;
    }
    return QString::number( c,base);
}
void BinaryViewModel::UpdateString(unsigned int stringNum)  {
    if(bdata->size<stringNum)return;
    strings[stringNum]=CharToQString(bdata->data[stringNum]);
}
void BinaryViewModel::UpdateData(void* data,unsigned int start,unsigned int size)  {
    if(bdata->size<start+size)return;
    memcpy(bdata->data+start,data,size);
}
///////////////////
TreeItem::TreeItem( TreeItem *parent) {
    parentItem = parent;
}
TreeItem::~TreeItem() {
    qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item) {
    childItems.append(item);
}
TreeItem *TreeItem::child(int row) {
    return childItems.value(row);
}
int TreeItem::childCount() const {
    return childItems.count();
}
int TreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
int TreeItem::columnCount() const {
    return 2;
}
TreeItem *TreeItem::parent() {
    return parentItem;
}
//////////////
static TreeItem* GetItem(const std::string& varName,Var* var,TreeItem* parent) {
    TreeItem* item=new TreeItem(parent);
    item->setName(QString(varName.c_str()));
    /* if(Float* ff=dynamic_cast<Float*>(var)) {
         item->appendData(QString::number(ff->num));
     } else if(Integer* ff=dynamic_cast<Integer*>(var)) {
         item->appendData(QString::number(ff->num));
     } else if(String* ff=dynamic_cast<String*>(var)) {
         item->appendData(QString(ff->mStr.c_str()));
     } else */if(Custom* ff=dynamic_cast<Custom*>(var)) {
        item->setData(new String("custom"));
        for(auto zvar:ff->vars) {
            Var* var=zvar.second.mVar.get();
            auto itemss=GetItem(zvar.first,var,item);
            item->appendChild(itemss);
        }

    } else {
        item->setData(var);
    }
    return item;
}
////////

TreeModel::TreeModel(ModuleData& data,BinaryTab* bdata_, QObject *parent)
    : QAbstractItemModel(parent),bdata(bdata_) {
    rootItem = new TreeItem();
    rootItem->setName(QString("nameee"));
    rootItem->setData(new String("conttt"));
    for(auto varPair:data.globalNameSpace.variables) {
        Var* var=varPair.second.mVar.get();
        auto items=GetItem(varPair.first,var,rootItem);
        rootItem->appendChild(items);
    }
}
TreeModel::~TreeModel() {
    delete rootItem;
}
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
int TreeModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
int TreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if(index.column()==0) {
        return item->getName();
    } else {
        return item->getData()->toQString();
    }
}
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;
    if(index.column()==0) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    } else {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if(section==0) {
            return rootItem->getName();
        } else {
            return rootItem->getData()->toQString();
        }
    }


    return QVariant();
}
bool TreeModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role == Qt::EditRole) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        QString s=value.toString();
        Var* var=item->getData();
        var->Set(s);
        int size=var->GetSize();
        if(size>0) {
            if(Float* ff=dynamic_cast<Float*>(var)) {
                *((float*)&bdata->data[ff->start])=(float)ff->num;
            } else if(Integer* ff=dynamic_cast<Integer*>(var)) {
                if(size==1)*((char*)&bdata->data[ff->start])=(char)ff->num;
                else if(size==2)*((short*)&bdata->data[ff->start])=(short)ff->num;
                else if(size==4)*((int*)&bdata->data[ff->start])=(int)ff->num;
            } else if(String* ff=dynamic_cast<String*>(var)) {
                memcpy(bdata->data+ff->start,&ff->str[0],size);
            }
            for(auto model:bdata->models) {
                for(int i=0; i<size; i++)
                    model->UpdateString(var->start+i);
            }
        }




    }
    return true;
}
