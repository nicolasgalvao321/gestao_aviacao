#include "loginpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
{
    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    // Título
    QLabel *titleLabel = new QLabel("Acesso Administrativo");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Espaço
    mainLayout->addSpacing(30);

    // Label da senha
    QLabel *passwordLabel = new QLabel("Senha:");
    QFont labelFont = passwordLabel->font();
    labelFont.setPointSize(12);
    passwordLabel->setFont(labelFont);
    mainLayout->addWidget(passwordLabel);

    // Input da senha
    passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setMinimumHeight(40);
    passwordInput->setStyleSheet("QLineEdit { padding: 5px; font-size: 14px; }");
    mainLayout->addWidget(passwordInput);

    // Espaço
    mainLayout->addSpacing(20);

    // Botões
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    loginButton = new QPushButton("Entrar");
    loginButton->setMinimumHeight(40);
    loginButton->setMinimumWidth(150);
    loginButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  font-weight: bold; "
        "  font-size: 14px; "
        "  border-radius: 5px; "
        "  padding: 10px; "
        "} "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:pressed { background-color: #3d8b40; }"
    );
    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    buttonLayout->addWidget(loginButton);

    backButton = new QPushButton("Voltar");
    backButton->setMinimumHeight(40);
    backButton->setMinimumWidth(150);
    backButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  font-weight: bold; "
        "  font-size: 14px; "
        "  border-radius: 5px; "
        "  padding: 10px; "
        "} "
        "QPushButton:hover { background-color: #da190b; } "
        "QPushButton:pressed { background-color: #ba0000; }"
    );
    connect(backButton, &QPushButton::clicked, this, &LoginPage::onBackClicked);
    buttonLayout->addWidget(backButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    setStyleSheet("QWidget { background-color: #f5f5f5; }");
}

void LoginPage::onLoginClicked()
{
    QString password = passwordInput->text();

    if (password == ADMIN_PASSWORD) {
        passwordInput->clear();
        emit loginSuccessful();
    } else {
        QMessageBox::warning(this, "Erro", "Senha incorreta!");
        passwordInput->clear();
        passwordInput->setFocus();
    }
}

void LoginPage::onBackClicked()
{
    passwordInput->clear();
    emit backToMenu();
}
