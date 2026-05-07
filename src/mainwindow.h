#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "database.h"
#include "clientpage.h"
#include "adminpage.h"
#include "loginpage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMenu();
    void showClientPage();
    void showLoginPage();
    void showAdminPage();

private:
    void setupUI();

    QStackedWidget *stackedWidget;
    Database *database;
    ClientPage *clientPage;
    AdminPage *adminPage;
    LoginPage *loginPage;
};

#endif // MAINWINDOW_H
