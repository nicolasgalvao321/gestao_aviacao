#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

signals:
    void loginSuccessful();
    void backToMenu();

private slots:
    void onLoginClicked();
    void onBackClicked();

private:
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    QPushButton *backButton;

    const QString ADMIN_PASSWORD = "admin123";
};

#endif // LOGINPAGE_H
