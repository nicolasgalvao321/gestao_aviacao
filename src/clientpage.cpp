#include "clientpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ClientPage::ClientPage(QSqlDatabase *database, QWidget *parent)
    : QWidget(parent), db(database)
{
    setupUI();
    loadFlights();
}

void ClientPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("Reserva de Assentos");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel *flightLabel = new QLabel("Selecione um voo:");
    mainLayout->addWidget(flightLabel);

    flightCombo = new QComboBox();
    flightCombo->setMinimumHeight(35);
    connect(flightCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientPage::onFlightSelected);
    mainLayout->addWidget(flightCombo);

    QLabel *seatsLabel = new QLabel("Mapa de Assentos:");
    mainLayout->addWidget(seatsLabel);

    seatsTable = new QTableWidget();
    seatsTable->setColumnCount(8);
    seatsTable->setRowCount(6);
    seatsTable->setMinimumHeight(300);
    seatsTable->horizontalHeader()->setStretchLastSection(false);
    seatsTable->verticalHeader()->setStretchLastSection(false);
    for (int i = 0; i < 8; ++i) {
        seatsTable->setColumnWidth(i, 50);
    }
    connect(seatsTable, &QTableWidget::itemClicked, this, &ClientPage::onSeatClicked);
    mainLayout->addWidget(seatsTable);

    QHBoxLayout *reservationLayout = new QHBoxLayout();
    
    QLabel *nameLabel = new QLabel("Nome:");
    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Digite seu nome");
    reservationLayout->addWidget(nameLabel);
    reservationLayout->addWidget(nameInput);

    QLabel *docLabel = new QLabel("CPF:");
    documentInput = new QLineEdit();
    documentInput->setPlaceholderText("Digite seu CPF");
    reservationLayout->addWidget(docLabel);
    reservationLayout->addWidget(documentInput);

    mainLayout->addLayout(reservationLayout);

    selectedSeatLabel = new QLabel("Assento selecionado: Nenhum");
    selectedSeatLabel->setStyleSheet("color: #2196F3; font-weight: bold;");
    mainLayout->addWidget(selectedSeatLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    reserveButton = new QPushButton("Confirmar Reserva");
    reserveButton->setMinimumHeight(40);
    reserveButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  border-radius: 5px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:pressed { background-color: #3d8b40; }"
    );
    connect(reserveButton, &QPushButton::clicked, this, &ClientPage::onReserveClicked);
    buttonLayout->addWidget(reserveButton);

    backButton = new QPushButton("Voltar");
    backButton->setMinimumHeight(40);
    backButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  border-radius: 5px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: #da190b; } "
        "QPushButton:pressed { background-color: #ba0000; }"
    );
    connect(backButton, &QPushButton::clicked, this, &ClientPage::onBackClicked);
    buttonLayout->addWidget(backButton);

    mainLayout->addLayout(buttonLayout);
}

void ClientPage::loadFlights()
{
    flightCombo->clear();
    
    QSqlQuery query("SELECT id, code, origin, destination, date, time, price FROM flights ORDER BY date, time");
    
    while (query.next()) {
        int id = query.value(0).toInt();
        QString code = query.value(1).toString();
        QString origin = query.value(2).toString();
        QString destination = query.value(3).toString();
        QString date = query.value(4).toString();
        QString time = query.value(5).toString();
        double price = query.value(6).toDouble();
        
        QString displayText = QString("%1 - %2 → %3 (%4 às %5) - R$ %.2f")
            .arg(code, origin, destination, date, time).arg(price);
        
        flightCombo->addItem(displayText, id);
    }
}

void ClientPage::onFlightSelected(int index)
{
    if (index < 0) return;
    
    currentFlightId = flightCombo->currentData().toInt();
    selectedSeat = "";
    selectedSeatLabel->setText("Assento selecionado: Nenhum");
    displaySeats();
}

void ClientPage::displaySeats()
{
    seatsTable->clearContents();
    
    QSqlQuery query;
    query.prepare("SELECT code FROM seats WHERE aircraft_id = (SELECT aircraft_id FROM flights WHERE id = ?) ORDER BY code");
    query.addBindValue(currentFlightId);
    
    QStringList reservedSeats;
    QSqlQuery reservedQuery;
    reservedQuery.prepare("SELECT seat_code FROM reservations WHERE flight_id = ?");
    reservedQuery.addBindValue(currentFlightId);
    
    if (reservedQuery.exec()) {
        while (reservedQuery.next()) {
            reservedSeats.append(reservedQuery.value(0).toString());
        }
    }
    
    if (query.exec()) {
        int row = 0, col = 0;
        while (query.next()) {
            QString seatCode = query.value(0).toString();
            QTableWidgetItem *item = new QTableWidgetItem(seatCode);
            
            if (reservedSeats.contains(seatCode)) {
                item->setBackground(QColor("#f44336"));
                item->setForeground(QColor("white"));
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            } else {
                item->setBackground(QColor("#4CAF50"));
                item->setForeground(QColor("white"));
            }
            
            seatsTable->setItem(row, col, item);
            
            col++;
            if (col >= 8) {
                col = 0;
                row++;
            }
        }
    }
}

void ClientPage::onSeatClicked(QTableWidgetItem *item)
{
    if (item->background().color() == QColor("#f44336")) {
        QMessageBox::warning(this, "Assento Indisponível", "Este assento já está reservado!");
        return;
    }
    
    selectedSeat = item->text();
    selectedSeatLabel->setText(QString("Assento selecionado: %1").arg(selectedSeat));
}

void ClientPage::onReserveClicked()
{
    if (currentFlightId < 0) {
        QMessageBox::warning(this, "Erro", "Selecione um voo!");
        return;
    }
    
    if (selectedSeat.isEmpty()) {
        QMessageBox::warning(this, "Erro", "Selecione um assento!");
        return;
    }
    
    QString name = nameInput->text().trimmed();
    QString document = documentInput->text().trimmed();
    
    if (name.isEmpty() || document.isEmpty()) {
        QMessageBox::warning(this, "Erro", "Preencha nome e CPF!");
        return;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) VALUES (?, ?, ?, ?)");
    query.addBindValue(currentFlightId);
    query.addBindValue(selectedSeat);
    query.addBindValue(name);
    query.addBindValue(document);
    
    if (query.exec()) {
        QMessageBox::information(this, "Sucesso", "Reserva confirmada!");
        nameInput->clear();
        documentInput->clear();
        selectedSeat = "";
        displaySeats();
    } else {
        QMessageBox::critical(this, "Erro", "Falha ao fazer reserva: " + query.lastError().text());
    }
}

void ClientPage::onBackClicked()
{
    emit backToMenu();
}
