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
#include "database.h"

class AdminPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminPage(Database *db, QWidget *parent = nullptr);

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
    Database *database;

    // Widgets para cadastro de voo
    QLineEdit *codeInput;
    QLineEdit *originInput;
    QLineEdit *destinationInput;
    QDateEdit *dateInput;
    QTimeEdit *timeInput;
    QComboBox *aircraftCombo;
    QDoubleSpinBox *priceInput;
    QPushButton *addFlightButton;

    // Tabelas
    QTableWidget *flightsTable;
    QTableWidget *reservationsTable;

    void setupUI();
    void updateFlightsList();
    void updateReservationsList(int flightId);
};

#endif // ADMINPAGE_H
