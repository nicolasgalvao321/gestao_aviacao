#include "adminpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>
#include <QTabWidget>
#include <QHeaderView>
#include <QDate>
#include <QTime>

AdminPage::AdminPage(Database *db, QWidget *parent)
    : QWidget(parent), database(db)
{
    setupUI();
    loadFlights();
}

void AdminPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Título
    QLabel *titleLabel = new QLabel("Painel Administrativo");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Abas
    QTabWidget *tabWidget = new QTabWidget();

    // Aba 1: Cadastro de voos
    QWidget *flightTab = new QWidget();
    QVBoxLayout *flightLayout = new QVBoxLayout(flightTab);
    flightLayout->setSpacing(10);

    QLabel *addFlightLabel = new QLabel("Cadastrar Novo Voo");
    QFont labelFont = addFlightLabel->font();
    labelFont.setPointSize(14);
    labelFont.setBold(true);
    addFlightLabel->setFont(labelFont);
    flightLayout->addWidget(addFlightLabel);

    // Formulário
    QHBoxLayout *formLayout1 = new QHBoxLayout();
    formLayout1->addWidget(new QLabel("Código:"));
    codeInput = new QLineEdit();
    codeInput->setPlaceholderText("Ex: AV-5000");
    formLayout1->addWidget(codeInput);
    flightLayout->addLayout(formLayout1);

    QHBoxLayout *formLayout2 = new QHBoxLayout();
    formLayout2->addWidget(new QLabel("Origem:"));
    originInput = new QLineEdit();
    formLayout2->addWidget(originInput);
    flightLayout->addLayout(formLayout2);

    QHBoxLayout *formLayout3 = new QHBoxLayout();
    formLayout3->addWidget(new QLabel("Destino:"));
    destinationInput = new QLineEdit();
    formLayout3->addWidget(destinationInput);
    flightLayout->addLayout(formLayout3);

    QHBoxLayout *formLayout4 = new QHBoxLayout();
    formLayout4->addWidget(new QLabel("Data:"));
    dateInput = new QDateEdit();
    dateInput->setDate(QDate::currentDate().addDays(1));
    dateInput->setCalendarPopup(true);
    formLayout4->addWidget(dateInput);
    flightLayout->addLayout(formLayout4);

    QHBoxLayout *formLayout5 = new QHBoxLayout();
    formLayout5->addWidget(new QLabel("Hora:"));
    timeInput = new QTimeEdit();
    timeInput->setTime(QTime(10, 0));
    formLayout5->addWidget(timeInput);
    flightLayout->addLayout(formLayout5);

    QHBoxLayout *formLayout6 = new QHBoxLayout();
    formLayout6->addWidget(new QLabel("Aeronave:"));
    aircraftCombo = new QComboBox();
    QVector<Aircraft> aircraft = database->getAllAircraft();
    for (const Aircraft &ac : aircraft) {
        aircraftCombo->addItem(ac.model, ac.id);
    }
    formLayout6->addWidget(aircraftCombo);
    flightLayout->addLayout(formLayout6);

    QHBoxLayout *formLayout7 = new QHBoxLayout();
    formLayout7->addWidget(new QLabel("Preço (R$):"));
    priceInput = new QDoubleSpinBox();
    priceInput->setMinimum(0);
    priceInput->setMaximum(10000);
    priceInput->setValue(500);
    formLayout7->addWidget(priceInput);
    flightLayout->addLayout(formLayout7);

    addFlightButton = new QPushButton("Adicionar Voo");
    addFlightButton->setMinimumHeight(40);
    addFlightButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  font-weight: bold; "
        "  border-radius: 5px; "
        "} "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:pressed { background-color: #3d8b40; }"
    );
    connect(addFlightButton, &QPushButton::clicked, this, &AdminPage::onAddFlightClicked);
    flightLayout->addWidget(addFlightButton);

    flightLayout->addSpacing(20);

    // Tabela de voos
    QLabel *flightListLabel = new QLabel("Voos Cadastrados:");
    QFont flightListFont = flightListLabel->font();
    flightListFont.setPointSize(12);
    flightListFont.setBold(true);
    flightListLabel->setFont(flightListFont);
    flightLayout->addWidget(flightListLabel);

    flightsTable = new QTableWidget();
    flightsTable->setColumnCount(8);
    flightsTable->setHorizontalHeaderLabels({"ID", "Código", "Origem", "Destino", "Data", "Hora", "Preço", "Status"});
    flightsTable->horizontalHeader()->setStretchLastSection(true);
    flightsTable->setMinimumHeight(250);
    connect(flightsTable, &QTableWidget::cellClicked, this, &AdminPage::onFlightSelectionChanged);
    flightLayout->addWidget(flightsTable);

    tabWidget->addTab(flightTab, "Cadastro de Voos");

    // Aba 2: Reservas
    QWidget *reservationTab = new QWidget();
    QVBoxLayout *reservationLayout = new QVBoxLayout(reservationTab);

    QLabel *reservationLabel = new QLabel("Reservas por Voo");
    QFont reservationFont = reservationLabel->font();
    reservationFont.setPointSize(14);
    reservationFont.setBold(true);
    reservationLabel->setFont(reservationFont);
    reservationLayout->addWidget(reservationLabel);

    reservationsTable = new QTableWidget();
    reservationsTable->setColumnCount(5);
    reservationsTable->setHorizontalHeaderLabels({"ID", "Assento", "Passageiro", "CPF", "Data"});
    reservationsTable->horizontalHeader()->setStretchLastSection(true);
    reservationLayout->addWidget(reservationsTable);

    tabWidget->addTab(reservationTab, "Reservas");

    mainLayout->addWidget(tabWidget);

    // Botão de logout
    QHBoxLayout *logoutLayout = new QHBoxLayout();
    logoutLayout->addStretch();

    QPushButton *logoutButton = new QPushButton("Sair");
    logoutButton->setMinimumHeight(40);
    logoutButton->setMinimumWidth(150);
    logoutButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  font-weight: bold; "
        "  border-radius: 5px; "
        "} "
        "QPushButton:hover { background-color: #da190b; } "
        "QPushButton:pressed { background-color: #ba0000; }"
    );
    connect(logoutButton, &QPushButton::clicked, this, &AdminPage::onLogoutClicked);
    logoutLayout->addWidget(logoutButton);

    mainLayout->addLayout(logoutLayout);

    setStyleSheet("QWidget { background-color: #f5f5f5; }");
}

void AdminPage::loadFlights()
{
    updateFlightsList();
}

void AdminPage::updateFlightsList()
{
    flightsTable->setRowCount(0);
    QVector<Flight> flights = database->getAllFlights();

    for (int i = 0; i < flights.size(); ++i) {
        flightsTable->insertRow(i);

        flightsTable->setItem(i, 0, new QTableWidgetItem(QString::number(flights[i].id)));
        flightsTable->setItem(i, 1, new QTableWidgetItem(flights[i].code));
        flightsTable->setItem(i, 2, new QTableWidgetItem(flights[i].origin));
        flightsTable->setItem(i, 3, new QTableWidgetItem(flights[i].destination));
        flightsTable->setItem(i, 4, new QTableWidgetItem(flights[i].date));
        flightsTable->setItem(i, 5, new QTableWidgetItem(flights[i].time));
        flightsTable->setItem(i, 6, new QTableWidgetItem(QString::number(flights[i].price, 'f', 2)));
        flightsTable->setItem(i, 7, new QTableWidgetItem(flights[i].status));
    }
}

void AdminPage::updateReservationsList(int flightId)
{
    reservationsTable->setRowCount(0);
    QVector<Reservation> reservations = database->getFlightReservations(flightId);

    for (int i = 0; i < reservations.size(); ++i) {
        reservationsTable->insertRow(i);

        reservationsTable->setItem(i, 0, new QTableWidgetItem(QString::number(reservations[i].id)));
        reservationsTable->setItem(i, 1, new QTableWidgetItem(reservations[i].seatCode));
        reservationsTable->setItem(i, 2, new QTableWidgetItem(reservations[i].passengerName));
        reservationsTable->setItem(i, 3, new QTableWidgetItem(reservations[i].passengerDocument));
        reservationsTable->setItem(i, 4, new QTableWidgetItem(""));
    }
}

void AdminPage::onAddFlightClicked()
{
    if (codeInput->text().isEmpty() || originInput->text().isEmpty() ||
        destinationInput->text().isEmpty()) {
        QMessageBox::warning(this, "Erro", "Preencha todos os campos!");
        return;
    }

    Flight flight;
    flight.code = codeInput->text();
    flight.origin = originInput->text();
    flight.destination = destinationInput->text();
    flight.date = dateInput->date().toString("yyyy-MM-dd");
    flight.time = timeInput->time().toString("HH:mm");
    flight.aircraftId = aircraftCombo->currentData().toInt();
    flight.price = priceInput->value();
    flight.status = "Aberto";

    if (database->addFlight(flight)) {
        QMessageBox::information(this, "Sucesso", "Voo adicionado com sucesso!");
        codeInput->clear();
        originInput->clear();
        destinationInput->clear();
        priceInput->setValue(500);
        updateFlightsList();
    } else {
        QMessageBox::critical(this, "Erro", "Falha ao adicionar voo!");
    }
}

void AdminPage::onFlightSelectionChanged()
{
    int row = flightsTable->currentRow();
    if (row < 0) return;

    int flightId = flightsTable->item(row, 0)->text().toInt();
    updateReservationsList(flightId);
}

void AdminPage::onLogoutClicked()
{
    emit backToMenu();
}

void AdminPage::refreshData()
{
    updateFlightsList();
}
