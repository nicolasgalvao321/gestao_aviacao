#include "crow_all.h"
#include "database.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

Database db;

int main() {
    crow::SimpleApp app;

    // Initialize database
    if (!db.init()) {
        std::cerr << "Falha ao inicializar banco de dados!" << std::endl;
        return 1;
    }

    std::cout << "Banco de dados inicializado com sucesso!" << std::endl;

    // ==================== FLIGHTS ====================
    
    // GET /api/flights - Listar todos os voos
    CROW_ROUTE(app, "/api/flights")
    .methods("GET"_method)
    ([](const crow::request&) {
        auto flights = db.getFlights();
        json response = json::array();
        
        for (const auto& flight : flights) {
            json f;
            f["id"] = flight.id;
            f["code"] = flight.code;
            f["origin"] = flight.origin;
            f["destination"] = flight.destination;
            f["date"] = flight.date;
            f["time"] = flight.time;
            f["price"] = flight.price;
            f["reserved_seats"] = flight.reserved_seats;
            response.push_back(f);
        }
        
        auto res = crow::response{response.dump()};
        res.add_header("Content-Type", "application/json");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // GET /api/flights/<id> - Obter voo específico
    CROW_ROUTE(app, "/api/flights/<int>")
    .methods("GET"_method)
    ([](int id) {
        auto flight = db.getFlightById(id);
        
        if (flight.id == 0) {
            return crow::response(404, "Voo não encontrado");
        }
        
        json f;
        f["id"] = flight.id;
        f["code"] = flight.code;
        f["origin"] = flight.origin;
        f["destination"] = flight.destination;
        f["date"] = flight.date;
        f["time"] = flight.time;
        f["price"] = flight.price;
        f["reserved_seats"] = flight.reserved_seats;
        
        auto res = crow::response{f.dump()};
        res.add_header("Content-Type", "application/json");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // POST /api/flights - Adicionar novo voo
    CROW_ROUTE(app, "/api/flights")
    .methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        
        if (!body || !body.has("code") || !body.has("origin") || 
            !body.has("destination") || !body.has("date") || 
            !body.has("time") || !body.has("price")) {
            return crow::response(400, "Campos obrigatórios faltando");
        }
        
        bool success = db.addFlight(
            body["code"].s(),
            body["origin"].s(),
            body["destination"].s(),
            body["date"].s(),
            body["time"].s(),
            body["price"].d()
        );
        
        if (success) {
            json response;
            response["success"] = true;
            response["message"] = "Voo adicionado com sucesso";
            
            auto res = crow::response{response.dump()};
            res.add_header("Content-Type", "application/json");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        } else {
            return crow::response(400, "Erro ao adicionar voo");
        }
    });

    // ==================== RESERVATIONS ====================
    
    // GET /api/reservations - Listar todas as reservas
    CROW_ROUTE(app, "/api/reservations")
    .methods("GET"_method)
    ([](const crow::request&) {
        auto reservations = db.getReservations();
        json response = json::array();
        
        for (const auto& res : reservations) {
            json r;
            r["id"] = res.id;
            r["flight_id"] = res.flight_id;
            r["seat_code"] = res.seat_code;
            r["passenger_name"] = res.passenger_name;
            r["passenger_document"] = res.passenger_document;
            response.push_back(r);
        }
        
        auto res = crow::response{response.dump()};
        res.add_header("Content-Type", "application/json");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // GET /api/reservations/flight/<id> - Obter reservas de um voo
    CROW_ROUTE(app, "/api/reservations/flight/<int>")
    .methods("GET"_method)
    ([](int flight_id) {
        auto reservations = db.getReservationsByFlight(flight_id);
        json response = json::array();
        
        for (const auto& res : reservations) {
            json r;
            r["id"] = res.id;
            r["flight_id"] = res.flight_id;
            r["seat_code"] = res.seat_code;
            r["passenger_name"] = res.passenger_name;
            r["passenger_document"] = res.passenger_document;
            response.push_back(r);
        }
        
        auto res = crow::response{response.dump()};
        res.add_header("Content-Type", "application/json");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // POST /api/reservations - Criar nova reserva
    CROW_ROUTE(app, "/api/reservations")
    .methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        
        if (!body || !body.has("flight_id") || !body.has("seat_code") || 
            !body.has("passenger_name") || !body.has("passenger_document")) {
            return crow::response(400, "Campos obrigatórios faltando");
        }
        
        bool success = db.addReservation(
            body["flight_id"].i(),
            body["seat_code"].s(),
            body["passenger_name"].s(),
            body["passenger_document"].s()
        );
        
        if (success) {
            json response;
            response["success"] = true;
            response["message"] = "Reserva criada com sucesso";
            
            auto res = crow::response{response.dump()};
            res.add_header("Content-Type", "application/json");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        } else {
            return crow::response(400, "Erro ao criar reserva");
        }
    });

    // DELETE /api/reservations/<id> - Cancelar reserva
    CROW_ROUTE(app, "/api/reservations/<int>")
    .methods("DELETE"_method)
    ([](int id) {
        bool success = db.cancelReservation(id);
        
        if (success) {
            json response;
            response["success"] = true;
            response["message"] = "Reserva cancelada com sucesso";
            
            auto res = crow::response{response.dump()};
            res.add_header("Content-Type", "application/json");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        } else {
            return crow::response(400, "Erro ao cancelar reserva");
        }
    });

    // ==================== CORS ====================
    
    // OPTIONS para CORS
    CROW_ROUTE(app, "/api/<path>")
    .methods("OPTIONS"_method)
    ([](const crow::request&, const std::string&) {
        auto res = crow::response(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        return res;
    });

    std::cout << "Servidor iniciado em http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();

    return 0;
}
