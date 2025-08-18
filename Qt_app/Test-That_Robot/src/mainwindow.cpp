#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionadd_box, &QAction::triggered, ui->opengl_widget, &OpenGl::addingBox  );
}

MainWindow::~MainWindow()
{
    delete ui;
}
