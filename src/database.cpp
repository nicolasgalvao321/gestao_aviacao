#include "database.h"
#include <QSqlDriver>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database::Database()
{
}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::initialize()
{
    if (!openDatabase()) {
        return false;
    }

    if (!createTables()) {
        return false;
    }

    // Verificar se já tem dados
    QSqlQuery query("SELECT COUNT(*) FROM flights");
    if (query.next() && query.value(0).toInt() == 0) {
        insertSampleData();
    }

    return true;
}

bool Database::openDatabase()
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
    return true;
}

bool Database::createTables()
{
    QSqlQuery query;

    // ===== TABELA: AIRCRAFT =====
    if (!query.exec("CREATE TABLE IF NOT EXISTS aircraft ("
                    "  id INTEGER PRIMARY KEY,"
                    "  model TEXT NOT NULL,"
                    "  seat_rows INTEGER NOT NULL,"
                    "  seat_columns INTEGER NOT NULL"
                    ")")) {
        qWarning() << "Erro ao criar tabela aircraft:" << query.lastError().text();
        return false;
    }

    // ===== TABELA: FLIGHTS =====
    if (!query.exec("CREATE TABLE IF NOT EXISTS flights ("
                    "  id INTEGER PRIMARY KEY,"
                    "  code TEXT UNIQUE NOT NULL,"
                    "  origin TEXT NOT NULL,"
                    "  destination TEXT NOT NULL,"
                    "  date TEXT NOT NULL,"
                    "  time TEXT NOT NULL,"
                    "  aircraft_id INTEGER NOT NULL,"
                    "  status TEXT DEFAULT 'Aberto',"
                    "  price REAL NOT NULL,"
                    "  FOREIGN KEY (aircraft_id) REFERENCES aircraft(id)"
                    ")")) {
        qWarning() << "Erro ao criar tabela flights:" << query.lastError().text();
        return false;
    }

    // ===== TABELA: SEATS =====
    if (!query.exec("CREATE TABLE IF NOT EXISTS seats ("
                    "  id INTEGER PRIMARY KEY,"
                    "  aircraft_id INTEGER NOT NULL,"
                    "  code TEXT NOT NULL,"
                    "  row_label TEXT NOT NULL,"
                    "  seat_number INTEGER NOT NULL,"
                    "  FOREIGN KEY (aircraft_id) REFERENCES aircraft(id),"
                    "  UNIQUE(aircraft_id, code)"
                    ")")) {
        qWarning() << "Erro ao criar tabela seats:" << query.lastError().text();
        return false;
    }

    // ===== TABELA: RESERVATIONS =====
    if (!query.exec("CREATE TABLE IF NOT EXISTS reservations ("
                    "  id INTEGER PRIMARY KEY,"
                    "  flight_id INTEGER NOT NULL,"
                    "  seat_code TEXT NOT NULL,"
                    "  passenger_name TEXT NOT NULL,"
                    "  passenger_document TEXT NOT NULL,"
                    "  FOREIGN KEY (flight_id) REFERENCES flights(id),"
                    "  UNIQUE(flight_id, seat_code)"
                    ")")) {
        qWarning() << "Erro ao criar tabela reservations:" << query.lastError().text();
        return false;
    }

    qDebug() << "Tabelas criadas com sucesso";
    return true;
}

bool Database::insertSampleData()
{
    QSqlQuery query;

    // ===== INSERIR AERONAVES =====
    query.exec("INSERT INTO aircraft (model, seat_rows, seat_columns) VALUES ('A320 Neo', 6, 32)");
    query.exec("INSERT INTO aircraft (model, seat_rows, seat_columns) VALUES ('B737-800', 6, 35)");
    query.exec("INSERT INTO aircraft (model, seat_rows, seat_columns) VALUES ('E195-E2', 5, 40)");

    // ===== GERAR ASSENTOS =====
    query.exec("SELECT id, seat_rows, seat_columns FROM aircraft");
    while (query.next()) {
        int aircraftId = query.value(0).toInt();
        int rows = query.value(1).toInt();
        int cols = query.value(2).toInt();
        generateSeatsForAircraft(aircraftId, rows, cols);
    }

    // ===== INSERIR VOOS =====
    query.exec("INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status) "
               "VALUES ('AV-1047', 'São Paulo', 'Rio de Janeiro', '2026-05-18', '08:35', 1, 428.00, 'Aberto')");
    query.exec("INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status) "
               "VALUES ('AV-2190', 'Brasília', 'Recife', '2026-05-18', '11:10', 2, 612.00, 'Embarque')");
    query.exec("INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status) "
               "VALUES ('AV-3382', 'Curitiba', 'Salvador', '2026-05-19', '17:45', 3, 735.00, 'Aberto')");

    // ===== INSERIR RESERVAS DE EXEMPLO =====
    query.exec("INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) "
               "VALUES (1, 'A2', 'João Silva', '12345678900')");
    query.exec("INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) "
               "VALUES (1, 'B4', 'Maria Santos', '98765432100')");

    qDebug() << "Dados de exemplo inseridos com sucesso";
    return true;
}

void Database::generateSeatsForAircraft(int aircraftId, int rows, int columns)
{
    QSqlQuery query;
    for (int row = 0; row < rows; ++row) {
        char rowLabel = 'A' + row;
        for (int col = 1; col <= columns; ++col) {
            QString seatCode = QString("%1%2").arg(rowLabel).arg(col);
            query.prepare("INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) "
                         "VALUES (?, ?, ?, ?)");
            query.addBindValue(aircraftId);
            query.addBindValue(seatCode);
            query.addBindValue(QString(rowLabel));
            query.addBindValue(col);
            query.exec();
        }
    }
}

// ===== OPERAÇÕES DE VOOS =====

QVector<Flight> Database::getAllFlights()
{
    QVector<Flight> flights;
    QSqlQuery query("SELECT id, code, origin, destination, date, time, aircraft_id, status, price FROM flights ORDER BY date, time");

    while (query.next()) {
        Flight flight;
        flight.id = query.value(0).toInt();
        flight.code = query.value(1).toString();
        flight.origin = query.value(2).toString();
        flight.destination = query.value(3).toString();
        flight.date = query.value(4).toString();
        flight.time = query.value(5).toString();
        flight.aircraftId = query.value(6).toInt();
        flight.status = query.value(7).toString();
        flight.price = query.value(8).toDouble();
        flights.append(flight);
    }

    return flights;
}

bool Database::addFlight(const Flight& flight)
{
    QSqlQuery query;
    query.prepare("INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(flight.code);
    query.addBindValue(flight.origin);
    query.addBindValue(flight.destination);
    query.addBindValue(flight.date);
    query.addBindValue(flight.time);
    query.addBindValue(flight.aircraftId);
    query.addBindValue(flight.price);
    query.addBindValue("Aberto");

    return query.exec();
}

bool Database::updateFlightStatus(int flightId, const QString& status)
{
    QSqlQuery query;
    query.prepare("UPDATE flights SET status = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(flightId);
    return query.exec();
}

// ===== OPERAÇÕES DE ASSENTOS =====

QVector<Seat> Database::getFlightSeats(int flightId)
{
    QVector<Seat> seats;
    QVector<QString> reserved = getReservedSeats(flightId);

    QSqlQuery query;
    query.prepare("SELECT code FROM seats WHERE aircraft_id = (SELECT aircraft_id FROM flights WHERE id = ?) ORDER BY code");
    query.addBindValue(flightId);

    if (query.exec()) {
        while (query.next()) {
            Seat seat;
            seat.code = query.value(0).toString();
            seat.reserved = reserved.contains(seat.code);
            seats.append(seat);
        }
    }

    return seats;
}

QVector<QString> Database::getReservedSeats(int flightId)
{
    QVector<QString> reserved;
    QSqlQuery query;
    query.prepare("SELECT seat_code FROM reservations WHERE flight_id = ?");
    query.addBindValue(flightId);

    if (query.exec()) {
        while (query.next()) {
            reserved.append(query.value(0).toString());
        }
    }

    return reserved;
}

// ===== OPERAÇÕES DE RESERVAS =====

bool Database::addReservation(int flightId, const QString& seatCode,
                              const QString& passengerName, const QString& passengerDocument)
{
    if (!canReserveSeat(flightId, seatCode)) {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) "
                  "VALUES (?, ?, ?, ?)");
    query.addBindValue(flightId);
    query.addBindValue(seatCode);
    query.addBindValue(passengerName);
    query.addBindValue(passengerDocument);

    return query.exec();
}

QVector<Reservation> Database::getFlightReservations(int flightId)
{
    QVector<Reservation> reservations;
    QSqlQuery query;
    query.prepare("SELECT id, flight_id, seat_code, passenger_name, passenger_document FROM reservations WHERE flight_id = ?");
    query.addBindValue(flightId);

    if (query.exec()) {
        while (query.next()) {
            Reservation res;
            res.id = query.value(0).toInt();
            res.flightId = query.value(1).toInt();
            res.seatCode = query.value(2).toString();
            res.passengerName = query.value(3).toString();
            res.passengerDocument = query.value(4).toString();
            reservations.append(res);
        }
    }

    return reservations;
}

bool Database::canReserveSeat(int flightId, const QString& seatCode)
{
    QVector<QString> reserved = getReservedSeats(flightId);
    return !reserved.contains(seatCode);
}

// ===== OPERAÇÕES DE AERONAVES =====

QVector<Aircraft> Database::getAllAircraft()
{
    QVector<Aircraft> aircraft;
    QSqlQuery query("SELECT id, model, seat_rows, seat_columns FROM aircraft");

    while (query.next()) {
        Aircraft ac;
        ac.id = query.value(0).toInt();
        ac.model = query.value(1).toString();
        ac.seatRows = query.value(2).toInt();
        ac.seatColumns = query.value(3).toInt();
        aircraft.append(ac);
    }

    return aircraft;
}

bool Database::addAircraft(const Aircraft& aircraft)
{
    QSqlQuery query;
    query.prepare("INSERT INTO aircraft (model, seat_rows, seat_columns) VALUES (?, ?, ?)");
    query.addBindValue(aircraft.model);
    query.addBindValue(aircraft.seatRows);
    query.addBindValue(aircraft.seatColumns);

    if (query.exec()) {
        int aircraftId = query.lastInsertId().toInt();
        generateSeatsForAircraft(aircraftId, aircraft.seatRows, aircraft.seatColumns);
        return true;
    }

    return false;
}
