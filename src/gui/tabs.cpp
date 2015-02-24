#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QHeaderView>

#include <string>
#include <fstream>
using namespace std;

#include "tabs.h"
#include "my_model.h"

static void LoadFileToString(const string& fileName,string& output) {
    ifstream file;
    file.exceptions(ifstream::failbit | ifstream::badbit);
    file.open(fileName.c_str(), std::ios::in);
    file.seekg(0, std::ios::end);
    output.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&output[0], output.size());
    file.close();
}
static void LoadFileToBytes(const string& fileName,char*& output,unsigned int& size) {
    ifstream file;
    file.exceptions(ifstream::failbit | ifstream::badbit);
    file.open(fileName.c_str(), std::ios::in|std::ios::binary);
    file.seekg(0, file.end);
    size=file.tellg();
    file.seekg(0, file.beg);
    output= new char [size];
    file.read(output, size);
    file.close();
}

EditTab::EditTab(QWidget* parent) {
    tab = new QWidget(parent);

    textEdit = new QTextEdit(tab);
    variables = new QTreeView(tab);
    model=new QStandardItemModel(0);

    tab->setObjectName(QString::fromUtf8("tab"));
    QVBoxLayout* hl = new QVBoxLayout(tab);
    ///////////
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(0);

    loadButton = new QPushButton(tab);
    loadButton->setText("LoadFile");
    horizontalLayout_2->addWidget(loadButton);

    saveButton = new QPushButton(tab);
    saveButton->setText("SaveFile");
    horizontalLayout_2->addWidget(saveButton);

    interpretButton = new QPushButton(tab);
    interpretButton->setText("Interpret");
    horizontalLayout_2->addWidget(interpretButton);

    hl->addLayout(horizontalLayout_2);

    ////////////
    QSplitter* splitter = new QSplitter(tab);
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(variables);
    splitter->addWidget(textEdit);
    hl->setSpacing(2);
    hl->setContentsMargins(2, 2, 2, 2);
    hl->addWidget(splitter);

    variables->setSizeIncrement(QSize(0, 0));
    variables->setBaseSize(QSize(0, 0));
    variables->setAutoScrollMargin(16);

    QList<QString> header;
    header<<"name"<<"content";
    model->setHorizontalHeaderLabels(header);
    variables->header()->setResizeMode(QHeaderView::Interactive);
    variables->setModel(model);

    textEdit->setBaseSize(QSize(100, 0));

}

void EditTab::Init(std::string path) {
    this->path=path;
    LoadFileToString(path,text);
    textEdit->setText(text.c_str());
}

QWidget* EditTab::GetWidget() {
    return tab;
}

//----------------------- BinaryTab

BinaryTab::BinaryTab(QWidget* parent) {
    tab = new QWidget();
    views.push_back(new QTableView(parent));
    views.push_back(new QTableView(parent));
    views.push_back(new QTableView(parent));
    views.push_back(new QTableView(parent));

    QVBoxLayout* boxLayout = new QVBoxLayout(tab);
    boxLayout->setSpacing(0);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(0);

    colsSpin = new QSpinBox(tab);
    colsSpin->setValue(10);
    horizontalLayout_2->addWidget(colsSpin);

    loadButton = new QPushButton(tab);
    loadButton->setText("LoadFile");
    horizontalLayout_2->addWidget(loadButton);

    saveButton = new QPushButton(tab);
    saveButton->setText("SaveFile");
    horizontalLayout_2->addWidget(saveButton);


    boxLayout->addLayout(horizontalLayout_2);

    QTabWidget* tabWidget = new QTabWidget(tab);
    tabWidget->setTabPosition(QTabWidget::South);

    QWidget* widget = new QWidget();
    QHBoxLayout* hl = new QHBoxLayout(widget);
    hl->setSpacing(2);
    hl->setContentsMargins(2, 2, 2, 2);
    hl->addWidget(views[0]);

    tabWidget->addTab(widget,"binary");

    widget = new QWidget();
    hl = new QHBoxLayout(widget);
    hl->setSpacing(2);
    hl->setContentsMargins(2, 2, 2, 2);
    hl->addWidget(views[1]);
    tabWidget->addTab(widget,"quat");

    widget = new QWidget();
    hl = new QHBoxLayout(widget);
    hl->setSpacing(2);
    hl->setContentsMargins(2, 2, 2, 2);
    hl->addWidget(views[2]);
    tabWidget->addTab(widget,"dec");

    widget = new QWidget();
    hl = new QHBoxLayout(widget);
    hl->setSpacing(2);
    hl->setContentsMargins(2, 2, 2, 2);
    hl->addWidget(views[3]);
    tabWidget->addTab(widget,"hex");

    boxLayout->addWidget(tabWidget);
}
void BinaryTab::Init(std::string path) {
    this->path=path;
    LoadFileToBytes(path,data,size);
    models.push_back(new BinaryViewModel(0,2,this));
    models.push_back(new BinaryViewModel(0,4,this));
    models.push_back(new BinaryViewModel(0,10,this));
    models.push_back(new BinaryViewModel(0,16,this));
    views[0]->setModel(models[0]);
    views[1]->setModel(models[1]);
    views[2]->setModel(models[2]);
    views[3]->setModel(models[3]);

}
void BinaryTab::Clear(){
    for(auto model:models){
        delete model;
    }
    views[0]->setModel(0);
    views[1]->setModel(0);
    views[2]->setModel(0);
    views[3]->setModel(0);
    models.clear();
    delete [] data;
}
QWidget* BinaryTab::GetWidget() {
    return tab;
}
