#include <string>

#include <QtGui/QHeaderView>

#include "mainwindow.h"
//uic -o src/gui/ui_mainwindow.h src/gui/mainwindow.ui
//moc src/gui/mainwindow.h -o src/gui/moc_mainwindow.cppp

static QList<QStandardItem*> GetItem(const std::string& varName,Var* var) {
    QList<QStandardItem*> items;
    QStandardItem* name=new QStandardItem(varName.c_str());
    items<<name;
    if(Float* ff=dynamic_cast<Float*>(var)) {
        QString str=QString::number(ff->num);
        items.append(new QStandardItem(str) );
    }else if(Integer* ff=dynamic_cast<Integer*>(var)) {
        QString str=QString::number(ff->num);
        items.append(new QStandardItem(str) );
    }else if(String* ff=dynamic_cast<String*>(var)) {
        items.append( new QStandardItem(ff->mStr.c_str()) );
    } else if(Custom* ff=dynamic_cast<Custom*>(var)) {
        QStandardItem* custom=new QStandardItem("custom");
        items.append(custom);
        for(auto zvar:ff->vars) {
            Var* var=zvar.second.mVar.get();
            auto items=GetItem(zvar.first,var);

            name->appendRow(items);
        }

    }
    return items;
}
void MainWindow::SetMenu(QTreeView *variables,QStandardItemModel* model,ModuleData& data) {
    QStandardItem* root= model->invisibleRootItem();
    if(root->hasChildren()) {
        root->removeRows(0, model->rowCount());
    }
    for(auto zvar:data.globalNameSpace.variables) {
        Var* var=zvar.second.mVar.get();
        auto items=GetItem(zvar.first,var);
        root->appendRow(items);
    }
    variables->collapseAll();
}
