#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>

struct Flight {
    int id;
    std::string code;
    std::string origin;
    std::string destination;
    std::string date;
    std::string time;
    double price;
    std::vector<std::string> reserved_seats;
};

struct Reservation {
    int id;
    int flight_id;
    std::string seat_code;
    std::string passenger_name;
    std::string passenger_document;
};

class Database {
public:
    Database(const std::string& db_path = "aviacao.db");
    ~Database();

    bool init();
    
    // Flights
    std::vector<Flight> getFlights();
    Flight getFlightById(int id);
    bool addFlight(const std::string& code, const std::string& origin, 
                   const std::string& destination, const std::string& date, 
                   const std::string& time, double price);
    
    // Reservations
    std::vector<Reservation> getReservations();
    std::vector<Reservation> getReservationsByFlight(int flight_id);
    bool addReservation(int flight_id, const std::string& seat_code,
                       const std::string& passenger_name, 
                       const std::string& passenger_document);
    bool cancelReservation(int reservation_id);
    
    // Seats
    std::vector<std::string> getReservedSeats(int flight_id);

private:
    sqlite3* db;
    std::string db_path;
    
    bool createTables();
    bool insertSampleData();
};

#endif // DATABASE_H
