#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../parser/exceptions.h"
#include "my_model.h"

#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <cstdio>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->editTab->removeTab(0);
    ui->editTab->removeTab(0);
    ui->binaryTab->removeTab(0);
    ui->editTab->setStyleSheet("QTabBar::tab { height: 25px; }");
    ui->binaryTab->setStyleSheet("QTabBar::tab { height: 25px; }");

    font.setPointSize(8);

    AddBinaryTab();
    AddEditTab();

    connect(ui->binaryTab, SIGNAL(currentChanged(int)), this, SLOT(on_ChangeTab_changed(int)));
    connect(ui->binaryTab, SIGNAL(tabCloseRequested(int)), this, SLOT(on_CloseTab_request(int)));
    connect(ui->editTab, SIGNAL(currentChanged(int)), this, SLOT(on_ChangeTabEdit_changed(int)));
    connect(ui->editTab, SIGNAL(tabCloseRequested(int)), this, SLOT(on_CloseTabEdit_request(int)));
    connect(ui->actionInfo, SIGNAL(triggered()), this, SLOT(Info()));
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::Info() {
    QMessageBox::about(this,
                       tr(""),
                       QString::fromUtf8(
                           "Author: Michał Masiukiewicz<br><br>"
                           "Program interprets binary data in left tab by code in right tab.<br>"
                           "The middle panel shows interpreted data.<br><br>"
                           "<b>Functions:</b><br>"
                           "uint1,uint2,uint4 -<i> unsigned numbers of size 1,2,4 bytes</i><br>"
                           "int1,int2,int4 -   <i>signed numbers of size 1,2,4 bytes</i><br>"
                           "add(num1,num2)       - <i>returns sum of two numbers</i><br>"
                           "addto(num,numToAdd)  - <i>adds number to number</i><br>"
                           "string(begin,count)  - <i>returns ascii null terminated string, begin - start of string, count - count of characters</i><br>"
                       )
                      );
}
void MainWindow::AddBinaryTab() {
    binaryEditData.push_back(unique_ptr<BinaryTab>(new BinaryTab(this)));
    auto& bData=binaryEditData.back();
    ui->binaryTab->addTab(bData->GetWidget(), "+" );
    connect(bData->loadButton, SIGNAL(pressed()), this, SLOT(on_OpenBinary_clicked()));
    connect(bData->saveButton, SIGNAL(pressed()), this, SLOT(SaveBinary()));
    connect(bData->colsSpin, SIGNAL(valueChanged(int)), this, SLOT(on_ColsSpin_changed(int)));
}
void MainWindow::AddEditTab() {
    tabEditData.push_back( unique_ptr<EditTab>( new EditTab(this) ) );
    auto& tData=tabEditData.back();
    ui->editTab->addTab(tData->GetWidget(), "+" );
    connect(tData->loadButton, SIGNAL(pressed()), this, SLOT(on_OpenParser_clicked()));
    connect(tData->interpretButton, SIGNAL(pressed()), this, SLOT(on_pushButton_clicked()));
}
void MainWindow::on_ChangeTab_changed(int tabNum) {
    if(tabDeleted==false and tabNum==ui->binaryTab->count()-1) {
        AddBinaryTab();
    }
    tabDeleted=false;
}
void MainWindow::on_ChangeTabEdit_changed(int tabNum) {
    if(tabDeletedEdit==false and tabNum==ui->editTab->count()-1) {
        AddEditTab();
    }
    tabDeletedEdit=false;
}
void MainWindow::on_CloseTab_request(int tabNum) {
    if(tabNum==ui->binaryTab->count()-1)return;
    ui->binaryTab->removeTab(tabNum);
    binaryEditData.erase(binaryEditData.begin()+tabNum);
    tabDeleted=true;

}

void MainWindow::on_CloseTabEdit_request(int tabNum) {
    if(tabNum==ui->editTab->count()-1)return;
    ui->editTab->removeTab(tabNum);
    tabEditData.erase(tabEditData.begin()+tabNum);
    tabDeletedEdit=true;
}

void MainWindow::on_ColsSpin_changed(int) {
    RefreshBinary();
}

void MainWindow::on_OpenParser_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.*)"));
    auto& tData=tabEditData[ui->editTab->currentIndex()];
    tData->Init(fileName.toStdString());

    ui->editTab->setTabText(ui->editTab->currentIndex(),fileName.mid( fileName.lastIndexOf('/') +1 ));
    if(ui->editTab->currentIndex()==ui->editTab->count()-1) {
        AddEditTab();
        tabDeletedEdit=false;
    }
}
void MainWindow::on_OpenBinary_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.*)"));
    auto& bData=binaryEditData[ui->binaryTab->currentIndex()];
    ui->binaryTab->setTabText(ui->binaryTab->currentIndex(),fileName.mid( fileName.lastIndexOf('/') +1 ));
    try {
        bData->Clear();
        bData->Init(fileName.toStdString());

        RefreshBinary();
        if(ui->binaryTab->currentIndex()==ui->binaryTab->count()-1) {
            AddBinaryTab();
            tabDeleted=false;
        }
    } catch(...) {
        msgBox.setText("Can not open the file.");
    }

}

void MainWindow::RefreshBinary() {
    for(auto& bData:binaryEditData) {
        if(bData->data!=nullptr) {
            int colsNum=bData->colsSpin->value();
            for(auto model:bData->models)model->setColumnCount(colsNum);
            //for(auto view:bData->views)view->resizeColumnsToContents();
            for(int i=0; i<colsNum; i++)bData->views[0]->setColumnWidth(i,80);
            for(int i=0; i<colsNum; i++)bData->views[1]->setColumnWidth(i,40);
            for(int i=0; i<colsNum; i++)bData->views[2]->setColumnWidth(i,30);
            for(int i=0; i<colsNum; i++)bData->views[3]->setColumnWidth(i,25);
        }
    }

}

void MainWindow::SaveBinary() {
    auto& currBinaryTab=binaryEditData[ui->binaryTab->currentIndex()];
    ofstream file(currBinaryTab->path.c_str());
    file.write(currBinaryTab->data,currBinaryTab->size);
}
void MainWindow::on_pushButton_clicked() {
    moduleData.Clear();
    parserData.Clear();
    auto& currEditTab=tabEditData[ui->editTab->currentIndex()];
    auto& currBinaryTab=binaryEditData[ui->binaryTab->currentIndex()];
    try {
        if(binaryEditData.empty() or !currBinaryTab->data)throw ParseException {"Nie ma danych do edytowania"};

        parserData.Init(currEditTab->textEdit->document()->toPlainText().toStdString());
        parserData.Parse();
        //parserData.Print();

        moduleData.data=currBinaryTab->data;
        moduleData.dataSize=currBinaryTab->size;
        moduleData.ComputeAll(parserData,moduleData.globalNameSpace);
        currBinaryTab->usedBlocks.swap(moduleData.usedBlocks);
        RefreshBinary();

        //moduleData.mGlobalNameSpace.PrintVariables();
    } catch(ParseException& e) {
        QString txt("Exception: ");
        txt+=e.mMessage.c_str();
        msgBox.setText(txt);
        msgBox.exec();
    }  catch(string& str) {
        QString txt("Exception: ");
        txt+=str.c_str();
        msgBox.setText(txt);
        msgBox.exec();
    } catch(...) {
        msgBox.setText("Unexpected Exception.");
        msgBox.exec();
    }
        SetMenu(currEditTab->variables,currEditTab->model,moduleData);

}


void MainWindow::on_ActionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.*)"));
}

void MainWindow::on_actionZamknij_triggered() {
    close();
}


