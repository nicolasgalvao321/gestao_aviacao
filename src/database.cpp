#include "database.h"
#include <QSqlDriver>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QFile>

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

    // Verificar se já tem dados
    QSqlQuery query("SELECT COUNT(*) FROM flights");
    if (query.next() && query.value(0).toInt() == 0) {
        // Se não tem dados, executar o schema.sql
        QFile schemaFile(":/schema.sql");
        if (schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString schema = QString::fromUtf8(schemaFile.readAll());
            schemaFile.close();

            // Executar cada comando SQL
            QStringList commands = schema.split(";", Qt::SkipEmptyParts);
            for (const QString& command : commands) {
                QString trimmedCommand = command.trimmed();
                if (!trimmedCommand.isEmpty()) {
                    QSqlQuery execQuery;
                    if (!execQuery.exec(trimmedCommand)) {
                        qWarning() << "Erro ao executar SQL:" << execQuery.lastError().text();
                    }
                }
            }
            qDebug() << "Schema.sql executado com sucesso";
        }
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
