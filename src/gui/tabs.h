#pragma once

#include <string>
#include <vector>

#include <QtGui/QStandardItemModel>
#include <QtGui/QTreeView>
#include <QtGui/QTextEdit>
#include <QtGui/QTableView>
#include <QtGui/QSpinBox>
#include <QtGui/QPushButton>

class MainWindow;
class BinaryViewModel;


struct EditTab {
    EditTab(QWidget* parent);
    void Init(std::string path);

    QWidget* GetWidget();

    std::string path;
    std::string text;
    QTreeView* variables=nullptr;
    QTextEdit* textEdit=nullptr;
    QStandardItemModel* model=nullptr;

    QWidget* tab=nullptr;
    QPushButton * loadButton=nullptr;
    QPushButton * saveButton=nullptr;
    QPushButton * interpretButton=nullptr;
};

struct BinaryTab {
    BinaryTab(QWidget* parent);
    void Init(std::string path);

    ~BinaryTab(){Clear();}
    void Clear();
    QWidget* GetWidget();

    std::string path;
    std::vector<std::pair<unsigned int,unsigned int>> usedBlocks;
    std::vector<BinaryViewModel*> models;
    std::vector<QTableView*> views;

    QWidget* tab=nullptr;
    QSpinBox* colsSpin=nullptr;
    QPushButton * loadButton=nullptr;
    QPushButton * saveButton=nullptr;

    char* data=nullptr;
    unsigned int size=0;
};
