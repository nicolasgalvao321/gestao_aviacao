#include "adminpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AdminPage::AdminPage(QSqlDatabase *database, QWidget *parent)
    : QWidget(parent), db(database)
{
    setupUI();
    loadFlights();
}

void AdminPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("Painel Administrativo");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel *addFlightLabel = new QLabel("Cadastrar Novo Voo:");
    QFont sectionFont = addFlightLabel->font();
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);
    addFlightLabel->setFont(sectionFont);
    mainLayout->addWidget(addFlightLabel);

    QHBoxLayout *formLayout = new QHBoxLayout();
    
    formLayout->addWidget(new QLabel("Código:"));
    codeInput = new QLineEdit();
    codeInput->setPlaceholderText("Ex: AV-1000");
    formLayout->addWidget(codeInput);

    formLayout->addWidget(new QLabel("Origem:"));
    originInput = new QLineEdit();
    originInput->setPlaceholderText("Ex: São Paulo");
    formLayout->addWidget(originInput);

    formLayout->addWidget(new QLabel("Destino:"));
    destinationInput = new QLineEdit();
    destinationInput->setPlaceholderText("Ex: Rio de Janeiro");
    formLayout->addWidget(destinationInput);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *formLayout2 = new QHBoxLayout();
    
    formLayout2->addWidget(new QLabel("Data:"));
    dateInput = new QDateEdit();
    dateInput->setDate(QDate::currentDate());
    formLayout2->addWidget(dateInput);

    formLayout2->addWidget(new QLabel("Hora:"));
    timeInput = new QTimeEdit();
    timeInput->setTime(QTime(12, 0));
    formLayout2->addWidget(timeInput);

    formLayout2->addWidget(new QLabel("Aeronave:"));
    aircraftCombo = new QComboBox();
    formLayout2->addWidget(aircraftCombo);

    formLayout2->addWidget(new QLabel("Preço:"));
    priceInput = new QDoubleSpinBox();
    priceInput->setMinimum(0);
    priceInput->setMaximum(10000);
    priceInput->setValue(500);
    formLayout2->addWidget(priceInput);

    mainLayout->addLayout(formLayout2);

    QSqlQuery query("SELECT id, model FROM aircraft");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString model = query.value(1).toString();
        aircraftCombo->addItem(model, id);
    }

    addFlightButton = new QPushButton("Adicionar Voo");
    addFlightButton->setMinimumHeight(40);
    addFlightButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  border-radius: 5px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:pressed { background-color: #3d8b40; }"
    );
    connect(addFlightButton, &QPushButton::clicked, this, &AdminPage::onAddFlightClicked);
    mainLayout->addWidget(addFlightButton);

    QLabel *flightsLabel = new QLabel("Voos Cadastrados:");
    flightsLabel->setFont(sectionFont);
    mainLayout->addWidget(flightsLabel);

    flightsTable = new QTableWidget();
    flightsTable->setColumnCount(7);
    flightsTable->setHorizontalHeaderLabels({"ID", "Código", "Origem", "Destino", "Data", "Hora", "Preço"});
    flightsTable->setMinimumHeight(200);
    connect(flightsTable, &QTableWidget::itemSelectionChanged, this, &AdminPage::onFlightSelectionChanged);
    mainLayout->addWidget(flightsTable);

    QLabel *reservationsLabel = new QLabel("Reservas do Voo Selecionado:");
    reservationsLabel->setFont(sectionFont);
    mainLayout->addWidget(reservationsLabel);

    reservationsTable = new QTableWidget();
    reservationsTable->setColumnCount(5);
    reservationsTable->setHorizontalHeaderLabels({"ID", "Assento", "Passageiro", "CPF", "Voo"});
    reservationsTable->setMinimumHeight(150);
    mainLayout->addWidget(reservationsTable);

    QPushButton *logoutButton = new QPushButton("Logout");
    logoutButton->setMinimumHeight(40);
    logoutButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  border-radius: 5px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #da190b; } "
        "QPushButton:pressed { background-color: #ba0000; }"
    );
    connect(logoutButton, &QPushButton::clicked, this, &AdminPage::onLogoutClicked);
    mainLayout->addWidget(logoutButton);
}

void AdminPage::onAddFlightClicked()
{
    QString code = codeInput->text().trimmed();
    QString origin = originInput->text().trimmed();
    QString destination = destinationInput->text().trimmed();
    QString date = dateInput->date().toString("yyyy-MM-dd");
    QString time = timeInput->time().toString("HH:mm");
    int aircraftId = aircraftCombo->currentData().toInt();
    double price = priceInput->value();

    if (code.isEmpty() || origin.isEmpty() || destination.isEmpty()) {
        QMessageBox::warning(this, "Erro", "Preencha todos os campos!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status) VALUES (?, ?, ?, ?, ?, ?, ?, 'Aberto')");
    query.addBindValue(code);
    query.addBindValue(origin);
    query.addBindValue(destination);
    query.addBindValue(date);
    query.addBindValue(time);
    query.addBindValue(aircraftId);
    query.addBindValue(price);

    if (query.exec()) {
        QMessageBox::information(this, "Sucesso", "Voo adicionado com sucesso!");
        codeInput->clear();
        originInput->clear();
        destinationInput->clear();
        priceInput->setValue(500);
        loadFlights();
    } else {
        QMessageBox::critical(this, "Erro", "Falha ao adicionar voo: " + query.lastError().text());
    }
}

void AdminPage::onFlightSelectionChanged()
{
    if (flightsTable->selectedItems().isEmpty()) {
        reservationsTable->clearContents();
        return;
    }

    int row = flightsTable->currentRow();
    int flightId = flightsTable->item(row, 0)->text().toInt();
    updateReservationsList(flightId);
}

void AdminPage::loadFlights()
{
    flightsTable->setRowCount(0);
    
    QSqlQuery query("SELECT id, code, origin, destination, date, time, price FROM flights ORDER BY date, time");
    int row = 0;

    while (query.next()) {
        flightsTable->insertRow(row);
        flightsTable->setItem(row, 0, new QTableWidgetItem(QString::number(query.value(0).toInt())));
        flightsTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        flightsTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        flightsTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        flightsTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        flightsTable->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        flightsTable->setItem(row, 6, new QTableWidgetItem(QString::number(query.value(6).toDouble(), 'f', 2)));
        row++;
    }
}

void AdminPage::updateReservationsList(int flightId)
{
    reservationsTable->setRowCount(0);
    
    QSqlQuery query;
    query.prepare("SELECT id, seat_code, passenger_name, passenger_document, flight_id FROM reservations WHERE flight_id = ?");
    query.addBindValue(flightId);
    
    int row = 0;
    if (query.exec()) {
        while (query.next()) {
            reservationsTable->insertRow(row);
            reservationsTable->setItem(row, 0, new QTableWidgetItem(QString::number(query.value(0).toInt())));
            reservationsTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            reservationsTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            reservationsTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            reservationsTable->setItem(row, 4, new QTableWidgetItem(QString::number(query.value(4).toInt())));
            row++;
        }
    }
}

void AdminPage::onLogoutClicked()
{
    emit backToMenu();
}

void AdminPage::refreshData()
{
    loadFlights();
}
