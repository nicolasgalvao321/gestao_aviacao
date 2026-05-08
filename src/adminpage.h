#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDoubleSpinBox>
#include <QSqlDatabase>

class AdminPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminPage(QSqlDatabase *database, QWidget *parent = nullptr);

signals:
    void backToMenu();

private slots:
    void onAddFlightClicked();
    void onFlightSelectionChanged();
    void onLogoutClicked();
    void loadFlights();
    void loadReservations();
    void refreshData();

private:
    QSqlDatabase *db;

    QLineEdit *codeInput;
    QLineEdit *originInput;
    QLineEdit *destinationInput;
    QDateEdit *dateInput;
    QTimeEdit *timeInput;
    QComboBox *aircraftCombo;
    QDoubleSpinBox *priceInput;
    QPushButton *addFlightButton;

    QTableWidget *flightsTable;
    QTableWidget *reservationsTable;

    void setupUI();
    void updateFlightsList();
    void updateReservationsList(int flightId);
};

#endif // ADMINPAGE_H
