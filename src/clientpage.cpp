#include "clientpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>
#include <QHeaderView>

ClientPage::ClientPage(Database *db, QWidget *parent)
    : QWidget(parent), database(db)
{
    setupUI();
    loadFlights();
}

void ClientPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Título
    QLabel *titleLabel = new QLabel("Reserva de Assentos");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Seção de seleção de voo
    QLabel *flightLabel = new QLabel("Selecione um voo:");
    mainLayout->addWidget(flightLabel);

    flightCombo = new QComboBox();
    flightCombo->setMinimumHeight(35);
    connect(flightCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientPage::onFlightSelected);
    mainLayout->addWidget(flightCombo);

    // Tabela de assentos
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
    for (int i = 0; i < 6; ++i) {
        seatsTable->setRowHeight(i, 50);
    }
    connect(seatsTable, &QTableWidget::itemClicked, this, &ClientPage::onSeatClicked);
    mainLayout->addWidget(seatsTable);

    // Assento selecionado
    selectedSeatLabel = new QLabel("Assento selecionado: Nenhum");
    selectedSeatLabel->setStyleSheet("color: #2196F3; font-weight: bold;");
    mainLayout->addWidget(selectedSeatLabel);

    // Dados do passageiro
    QLabel *passengerLabel = new QLabel("Dados do Passageiro:");
    mainLayout->addWidget(passengerLabel);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Nome:"));
    nameInput = new QLineEdit();
    nameInput->setMinimumHeight(35);
    nameLayout->addWidget(nameInput);
    mainLayout->addLayout(nameLayout);

    QHBoxLayout *docLayout = new QHBoxLayout();
    docLayout->addWidget(new QLabel("CPF:"));
    documentInput = new QLineEdit();
    documentInput->setMinimumHeight(35);
    documentInput->setPlaceholderText("000.000.000-00");
    docLayout->addWidget(documentInput);
    mainLayout->addLayout(docLayout);

    // Botões
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    reserveButton = new QPushButton("Confirmar Reserva");
    reserveButton->setMinimumHeight(40);
    reserveButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  font-weight: bold; "
        "  border-radius: 5px; "
        "} "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:pressed { background-color: #3d8b40; }"
    );
    connect(reserveButton, &QPushButton::clicked, this, &ClientPage::onReserveClicked);
    buttonLayout->addWidget(reserveButton);

    backButton = new QPushButton("Voltar");
    backButton->setMinimumHeight(40);
    backButton->setMinimumWidth(150);
    backButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  font-weight: bold; "
        "  border-radius: 5px; "
        "} "
        "QPushButton:hover { background-color: #da190b; } "
        "QPushButton:pressed { background-color: #ba0000; }"
    );
    connect(backButton, &QPushButton::clicked, this, &ClientPage::onBackClicked);
    buttonLayout->addWidget(backButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    setStyleSheet("QWidget { background-color: #f5f5f5; }");
}

void ClientPage::loadFlights()
{
    updateFlightsList();
}

void ClientPage::updateFlightsList()
{
    flightCombo->clear();
    QVector<Flight> flights = database->getAllFlights();

    for (const Flight &flight : flights) {
        QString displayText = QString("%1 - %2 → %3 (%4 às %5) - R$ %.2f")
                                  .arg(flight.code, flight.origin, flight.destination,
                                       flight.date, flight.time)
                                  .arg(flight.price);
        flightCombo->addItem(displayText, flight.id);
    }
}

void ClientPage::onFlightSelected(int index)
{
    if (index < 0) return;

    currentFlightId = flightCombo->itemData(index).toInt();
    selectedSeat = "";
    selectedSeatLabel->setText("Assento selecionado: Nenhum");
    displaySeats();
}

void ClientPage::displaySeats()
{
    if (currentFlightId < 0) return;

    QVector<Seat> seats = database->getFlightSeats(currentFlightId);

    // Limpar tabela
    for (int row = 0; row < seatsTable->rowCount(); ++row) {
        for (int col = 0; col < seatsTable->columnCount(); ++col) {
            seatsTable->setItem(row, col, nullptr);
        }
    }

    // Preencher assentos
    for (int i = 0; i < seats.size() && i < 48; ++i) {
        int row = i / 8;
        int col = i % 8;

        QTableWidgetItem *item = new QTableWidgetItem(seats[i].code);
        item->setTextAlignment(Qt::AlignCenter);

        if (seats[i].reserved) {
            item->setBackground(QColor("#f44336"));
            item->setForeground(QColor("white"));
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        } else {
            item->setBackground(QColor("#4CAF50"));
            item->setForeground(QColor("white"));
        }

        seatsTable->setItem(row, col, item);
    }
}

void ClientPage::onSeatClicked(QTableWidgetItem *item)
{
    if (item->background().color() == QColor("#f44336")) {
        QMessageBox::warning(this, "Aviso", "Este assento já está reservado!");
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

    if (nameInput->text().isEmpty()) {
        QMessageBox::warning(this, "Erro", "Digite seu nome!");
        return;
    }

    if (documentInput->text().isEmpty()) {
        QMessageBox::warning(this, "Erro", "Digite seu CPF!");
        return;
    }

    bool success = database->addReservation(currentFlightId, selectedSeat,
                                            nameInput->text(), documentInput->text());

    if (success) {
        QMessageBox::information(this, "Sucesso", 
                                QString("Reserva confirmada!\nAssento: %1").arg(selectedSeat));
        nameInput->clear();
        documentInput->clear();
        selectedSeat = "";
        selectedSeatLabel->setText("Assento selecionado: Nenhum");
        displaySeats();
    } else {
        QMessageBox::critical(this, "Erro", "Falha ao confirmar reserva. Tente novamente!");
    }
}

void ClientPage::onBackClicked()
{
    nameInput->clear();
    documentInput->clear();
    selectedSeat = "";
    emit backToMenu();
}
