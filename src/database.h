#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QMap>

struct Aircraft {
    int id;
    QString model;
    int seatRows;
    int seatColumns;
};

struct Flight {
    int id;
    QString code;
    QString origin;
    QString destination;
    QString date;
    QString time;
    int aircraftId;
    QString status;
    double price;
};

struct Reservation {
    int id;
    int flightId;
    QString seatCode;
    QString passengerName;
    QString passengerDocument;
};

struct Seat {
    QString code;
    bool reserved;
};

class Database
{
public:
    Database();
    ~Database();

    bool initialize();
    bool createTables();
    bool insertSampleData();

    // Voos
    QVector<Flight> getAllFlights();
    bool addFlight(const Flight& flight);
    bool updateFlightStatus(int flightId, const QString& status);

    // Assentos
    QVector<Seat> getFlightSeats(int flightId);
    QVector<QString> getReservedSeats(int flightId);

    // Reservas
    bool addReservation(int flightId, const QString& seatCode, 
                       const QString& passengerName, const QString& passengerDocument);
    QVector<Reservation> getFlightReservations(int flightId);
    bool canReserveSeat(int flightId, const QString& seatCode);

    // Aeronaves
    QVector<Aircraft> getAllAircraft();
    bool addAircraft(const Aircraft& aircraft);

private:
    QSqlDatabase db;
    bool openDatabase();
    void generateSeatsForAircraft(int aircraftId, int rows, int columns);
};

#endif // DATABASE_H
