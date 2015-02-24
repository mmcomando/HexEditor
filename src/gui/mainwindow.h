#pragma once

#include <memory>

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>

#include "../parser/parser.h"
#include "../parser/moduledata.h"
#include "tabs.h"

class BinaryViewModel;
class MainWindow;
namespace Ui {
class MainWindow;
}
/** Class MainWindow represents main window of the aplication
 * and implements main logic of the program
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ///Creates tree of variables loaded by interpreter
    void SetMenu(QTreeView *variables,QStandardItemModel* model,ModuleData& data);

private slots:
    void Info();
    void on_ActionOpen_triggered();
    void on_pushButton_clicked();
    void on_actionZamknij_triggered();
    void on_OpenParser_clicked();
    void on_OpenBinary_clicked();
    void on_ColsSpin_changed(int);
    void on_ChangeTab_changed(int);
    void on_CloseTab_request(int tabNum);
    void on_CloseTabEdit_request(int tabNum);
    void on_ChangeTabEdit_changed(int tabNum);
    void SaveBinary();

private:
    void RefreshBinary();
    void AddBinaryTab();
    void AddEditTab();

    QMessageBox msgBox;

    Parser::ParserData parserData;
    ModuleData moduleData;

    std::vector<std::unique_ptr<EditTab>> tabEditData;
    std::vector<std::unique_ptr<BinaryTab>> binaryEditData;

    Ui::MainWindow *ui;

    QFont font;
    bool tabDeleted=false;
    bool tabDeletedEdit=false;
};

