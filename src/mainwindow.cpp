#include "mainwindow.h"
#include "clientpage.h"
#include "adminpage.h"
#include "loginpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (!initializeDatabase()) {
        qWarning() << "Falha ao inicializar banco de dados";
    }

    setupUI();
}

MainWindow::~MainWindow()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool MainWindow::initializeDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    
    QString dbPath = dataPath + "/aviacao.db";
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "Erro ao abrir banco:" << db.lastError().text();
        return false;
    }

    qDebug() << "Banco aberto em:" << dbPath;

    // Verificar se precisa executar o schema
    QSqlQuery checkQuery("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='flights'");
    bool needsSchema = true;
    
    if (checkQuery.next()) {
        needsSchema = checkQuery.value(0).toInt() == 0;
    }

    if (needsSchema) {
        QFile schemaFile(":/schema.sql");
        if (schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString schema = QString::fromUtf8(schemaFile.readAll());
            schemaFile.close();

            QStringList commands = schema.split(";", Qt::SkipEmptyParts);
            for (const QString& command : commands) {
                QString trimmedCommand = command.trimmed();
                if (!trimmedCommand.isEmpty()) {
                    QSqlQuery execQuery;
                    if (!execQuery.exec(trimmedCommand)) {
                        qWarning() << "Erro ao executar SQL:" << execQuery.lastError().text();
                    }
                }
            }
            qDebug() << "Schema executado com sucesso";
        }
    }

    return true;
}

void MainWindow::setupUI()
{
    stackedWidget = new QStackedWidget();
    setCentralWidget(stackedWidget);

    // Página de Menu
    QWidget *menuPage = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuPage);
    menuLayout->setSpacing(20);
    menuLayout->setContentsMargins(50, 50, 50, 50);

    QLabel *titleLabel = new QLabel("AeroGestão");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Sistema de Gestão de Aviação");
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(16);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #666;");
    menuLayout->addWidget(subtitleLabel);

    menuLayout->addSpacing(40);

    QPushButton *clientButton = new QPushButton("Área do Cliente");
    clientButton->setMinimumHeight(60);
    clientButton->setMinimumWidth(300);
    clientButton->setFont(QFont("Arial", 14, QFont::Bold));
    clientButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #2196F3; "
        "  color: white; "
        "  border-radius: 10px; "
        "  padding: 15px; "
        "} "
        "QPushButton:hover { background-color: #0b7dda; } "
        "QPushButton:pressed { background-color: #0056b3; }"
    );
    connect(clientButton, &QPushButton::clicked, this, &MainWindow::showClientPage);
    menuLayout->addWidget(clientButton, 0, Qt::AlignCenter);

    QPushButton *adminButton = new QPushButton("Área Administrativa");
    adminButton->setMinimumHeight(60);
    adminButton->setMinimumWidth(300);
    adminButton->setFont(QFont("Arial", 14, QFont::Bold));
    adminButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #FF9800; "
        "  color: white; "
        "  border-radius: 10px; "
        "  padding: 15px; "
        "} "
        "QPushButton:hover { background-color: #e68900; } "
        "QPushButton:pressed { background-color: #cc7000; }"
    );
    connect(adminButton, &QPushButton::clicked, this, &MainWindow::showLoginPage);
    menuLayout->addWidget(adminButton, 0, Qt::AlignCenter);

    menuLayout->addStretch();

    QLabel *footerLabel = new QLabel("© 2026 AeroGestão - Todos os direitos reservados");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet("color: #999; font-size: 10px;");
    menuLayout->addWidget(footerLabel);

    menuPage->setStyleSheet("QWidget { background-color: #f5f5f5; }");
    stackedWidget->addWidget(menuPage);

    // Página do Cliente
    clientPage = new ClientPage(&db);
    connect(clientPage, &ClientPage::backToMenu, this, &MainWindow::showMenu);
    stackedWidget->addWidget(clientPage);

    // Página de Login
    loginPage = new LoginPage();
    connect(loginPage, &LoginPage::loginSuccessful, this, &MainWindow::showAdminPage);
    connect(loginPage, &LoginPage::backToMenu, this, &MainWindow::showMenu);
    stackedWidget->addWidget(loginPage);

    // Página Admin
    adminPage = new AdminPage(&db);
    connect(adminPage, &AdminPage::backToMenu, this, &MainWindow::showMenu);
    stackedWidget->addWidget(adminPage);

    stackedWidget->setCurrentIndex(0);
    setWindowTitle("AeroGestão");
    setGeometry(100, 100, 900, 600);
}

void MainWindow::showMenu()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showClientPage()
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showLoginPage()
{
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::showAdminPage()
{
    stackedWidget->setCurrentIndex(3);
}
