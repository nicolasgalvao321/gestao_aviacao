#ifndef CLIENTPAGE_H
#define CLIENTPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QSqlDatabase>

class ClientPage : public QWidget
{
    Q_OBJECT

public:
    explicit ClientPage(QSqlDatabase *database, QWidget *parent = nullptr);

signals:
    void backToMenu();

private slots:
    void onFlightSelected(int index);
    void onSeatClicked(QTableWidgetItem *item);
    void onReserveClicked();
    void onBackClicked();
    void loadFlights();
    void displaySeats();

private:
    QSqlDatabase *db;
    QComboBox *flightCombo;
    QTableWidget *seatsTable;
    QLineEdit *nameInput;
    QLineEdit *documentInput;
    QLabel *selectedSeatLabel;
    QPushButton *reserveButton;
    QPushButton *backButton;

    int currentFlightId = -1;
    QString selectedSeat;

    void setupUI();
    void updateFlightsList();
};

#endif // CLIENTPAGE_H
