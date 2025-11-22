#include "mainwindow.h"
#include "luasyntaxhighlighter.h"
#include "ui_mainwindow.h"
#include "scripting/luasyntaxhighlighter.h"

#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionadd_box, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingBox  );
    QObject::connect(ui->actionadd_wall, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingP  );
    QObject::connect(ui->actionadd_circle, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingC  );
    QObject::connect(ui->actionadd_triangle, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingT  );
    QObject::connect(ui->actionadd_robot, &QAction::triggered, ui->opengl_widget, &OpenGLWidget::addingRobot);
    QObject::connect(ui->actionrun, &QAction::triggered, this, &MainWindow::onRunScriptClicked  );
    
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::onSaveAs);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::onOpen);
    
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

void MainWindow::onSave()
{
    if (m_currentFilePath.isEmpty()) {
        onSaveAs(); // Если путь не известен, вызываем "Сохранить как..."
    } else {
        saveProject(m_currentFilePath);
    }
}

void MainWindow::onSaveAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Simulation", "", "Simulation Files (*.sim);;All Files (*)");
    if (!filePath.isEmpty()) {
        m_currentFilePath = filePath;
        saveProject(filePath);
    }
}

void MainWindow::onOpen()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Simulation", "", "Simulation Files (*.sim);;All Files (*)");
    if (!filePath.isEmpty()) {
        m_currentFilePath = filePath;
        loadProject(filePath);
    }
}

void MainWindow::saveProject(const QString &filePath)
{
    QJsonObject projectJson;

    // 1. Получаем состояние мира из OpenGLWidget -> World
    projectJson["world"] = ui->opengl_widget->getWorldState();

    // 2. Получаем скрипт из редактора
    projectJson["script"] = ui->codeEditor->toPlainText();

    // 3. Сохраняем все в файл
    QJsonDocument saveDoc(projectJson);
    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Save Error", "Could not open file for writing.");
        return;
    }
    saveFile.write(saveDoc.toJson());
    saveFile.close();
}

void MainWindow::loadProject(const QString &filePath)
{
    QFile loadFile(filePath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Load Error", "Could not open file for reading.");
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    
    if(loadDoc.isNull()){
        QMessageBox::warning(this, "Load Error", "Failed to parse the project file. It might be corrupted.");
        return;
    }

    QJsonObject projectJson = loadDoc.object();

    // 1. Загружаем состояние мира
    if (projectJson.contains("world") && projectJson["world"].isObject()) {
        ui->opengl_widget->loadWorldState(projectJson["world"].toObject());
    }

    // 2. Загружаем скрипт
    if (projectJson.contains("script") && projectJson["script"].isString()) {
        ui->codeEditor->setPlainText(projectJson["script"].toString());
    }
}
