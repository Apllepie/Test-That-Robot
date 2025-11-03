#include "mainwindow.h"
#include "luasyntaxhighlighter.h"
#include "ui_mainwindow.h"
#include "scripting/luasyntaxhighlighter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionadd_box, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingBox  );
    QObject::connect(ui->actionrun, &QAction::triggered, this, &MainWindow::onRunScriptClicked  );
    new LuaSyntaxHighlighter(ui->codeEditor->document());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRunScriptClicked()
{
    QString scriptCode = ui->codeEditor->toPlainText();

    if(!scriptCode.isEmpty()){
         ui->opengl_widget->runScript(scriptCode);
    }
}

