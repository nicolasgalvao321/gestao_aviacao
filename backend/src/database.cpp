#include "database.h"
#include <iostream>
#include <cstring>

Database::Database(const std::string& db_path) : db(nullptr), db_path(db_path) {}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::init() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir banco de dados: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (!createTables()) {
        return false;
    }

    // Check if sample data exists
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM flights", -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_step(stmt);
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        if (count == 0) {
            insertSampleData();
        }
    }

    return true;
}

bool Database::createTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS aircraft (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            model TEXT NOT NULL,
            seats_count INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS flights (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT UNIQUE NOT NULL,
            origin TEXT NOT NULL,
            destination TEXT NOT NULL,
            date TEXT NOT NULL,
            time TEXT NOT NULL,
            aircraft_id INTEGER,
            price REAL NOT NULL,
            status TEXT DEFAULT 'Aberto',
            FOREIGN KEY(aircraft_id) REFERENCES aircraft(id)
        );

        CREATE TABLE IF NOT EXISTS seats (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            flight_id INTEGER NOT NULL,
            seat_code TEXT NOT NULL,
            is_reserved INTEGER DEFAULT 0,
            FOREIGN KEY(flight_id) REFERENCES flights(id)
        );

        CREATE TABLE IF NOT EXISTS reservations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            flight_id INTEGER NOT NULL,
            seat_code TEXT NOT NULL,
            passenger_name TEXT NOT NULL,
            passenger_document TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(flight_id) REFERENCES flights(id)
        );
    )";

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar tabelas: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

bool Database::insertSampleData() {
    // Insert aircraft
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO aircraft (model, seats_count) VALUES (?, ?)";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "Boeing 737", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, 36);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "Airbus A320", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, 36);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Insert sample flights
    sql = "INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price) VALUES (?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "AV-1001", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, "São Paulo", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Rio de Janeiro", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, "2026-05-15", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, "08:00", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, 1);
    sqlite3_bind_double(stmt, 7, 350.00);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, "AV-1002", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, "São Paulo", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Brasília", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, "2026-05-15", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, "10:30", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, 2);
    sqlite3_bind_double(stmt, 7, 280.00);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return true;
}

std::vector<Flight> Database::getFlights() {
    std::vector<Flight> flights;
    const char* sql = "SELECT id, code, origin, destination, date, time, price FROM flights";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Flight flight;
            flight.id = sqlite3_column_int(stmt, 0);
            flight.code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            flight.origin = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            flight.destination = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            flight.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            flight.time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            flight.price = sqlite3_column_double(stmt, 6);
            flight.reserved_seats = getReservedSeats(flight.id);
            
            flights.push_back(flight);
        }
    }
    sqlite3_finalize(stmt);
    
    return flights;
}

Flight Database::getFlightById(int id) {
    Flight flight = {0, "", "", "", "", "", 0.0, {}};
    const char* sql = "SELECT id, code, origin, destination, date, time, price FROM flights WHERE id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            flight.id = sqlite3_column_int(stmt, 0);
            flight.code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            flight.origin = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            flight.destination = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            flight.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            flight.time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            flight.price = sqlite3_column_double(stmt, 6);
            flight.reserved_seats = getReservedSeats(id);
        }
    }
    sqlite3_finalize(stmt);
    
    return flight;
}

bool Database::addFlight(const std::string& code, const std::string& origin,
                        const std::string& destination, const std::string& date,
                        const std::string& time, double price) {
    const char* sql = "INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price) VALUES (?, ?, ?, ?, ?, 1, ?)";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, origin.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, destination.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, time.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 6, price);
        
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return res == SQLITE_DONE;
    }
    
    return false;
}

std::vector<Reservation> Database::getReservations() {
    std::vector<Reservation> reservations;
    const char* sql = "SELECT id, flight_id, seat_code, passenger_name, passenger_document FROM reservations";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Reservation res;
            res.id = sqlite3_column_int(stmt, 0);
            res.flight_id = sqlite3_column_int(stmt, 1);
            res.seat_code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            res.passenger_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            res.passenger_document = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            
            reservations.push_back(res);
        }
    }
    sqlite3_finalize(stmt);
    
    return reservations;
}

std::vector<Reservation> Database::getReservationsByFlight(int flight_id) {
    std::vector<Reservation> reservations;
    const char* sql = "SELECT id, flight_id, seat_code, passenger_name, passenger_document FROM reservations WHERE flight_id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, flight_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Reservation res;
            res.id = sqlite3_column_int(stmt, 0);
            res.flight_id = sqlite3_column_int(stmt, 1);
            res.seat_code = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            res.passenger_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            res.passenger_document = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            
            reservations.push_back(res);
        }
    }
    sqlite3_finalize(stmt);
    
    return reservations;
}

bool Database::addReservation(int flight_id, const std::string& seat_code,
                             const std::string& passenger_name,
                             const std::string& passenger_document) {
    const char* sql = "INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) VALUES (?, ?, ?, ?)";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, flight_id);
        sqlite3_bind_text(stmt, 2, seat_code.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, passenger_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, passenger_document.c_str(), -1, SQLITE_TRANSIENT);
        
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return res == SQLITE_DONE;
    }
    
    return false;
}

bool Database::cancelReservation(int reservation_id) {
    const char* sql = "DELETE FROM reservations WHERE id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, reservation_id);
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return res == SQLITE_DONE;
    }
    
    return false;
}

std::vector<std::string> Database::getReservedSeats(int flight_id) {
    std::vector<std::string> seats;
    const char* sql = "SELECT seat_code FROM reservations WHERE flight_id = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, flight_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            seats.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    
    return seats;
}
