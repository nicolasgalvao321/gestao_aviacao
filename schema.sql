-- AeroGestão - Schema do Banco de Dados SQLite
-- Este arquivo define a estrutura completa do banco de dados

-- Tabela de Aeronaves
CREATE TABLE IF NOT EXISTS aircraft (
    id INTEGER PRIMARY KEY,
    model TEXT NOT NULL,
    seat_rows INTEGER NOT NULL,
    seat_columns INTEGER NOT NULL
);

-- Tabela de Voos
CREATE TABLE IF NOT EXISTS flights (
    id INTEGER PRIMARY KEY,
    code TEXT UNIQUE NOT NULL,
    origin TEXT NOT NULL,
    destination TEXT NOT NULL,
    date TEXT NOT NULL,
    time TEXT NOT NULL,
    aircraft_id INTEGER NOT NULL,
    status TEXT DEFAULT 'Aberto',
    price REAL NOT NULL,
    FOREIGN KEY (aircraft_id) REFERENCES aircraft(id)
);

-- Tabela de Assentos
CREATE TABLE IF NOT EXISTS seats (
    id INTEGER PRIMARY KEY,
    aircraft_id INTEGER NOT NULL,
    code TEXT NOT NULL,
    row_label TEXT NOT NULL,
    seat_number INTEGER NOT NULL,
    FOREIGN KEY (aircraft_id) REFERENCES aircraft(id),
    UNIQUE(aircraft_id, code)
);

-- Tabela de Reservas
CREATE TABLE IF NOT EXISTS reservations (
    id INTEGER PRIMARY KEY,
    flight_id INTEGER NOT NULL,
    seat_code TEXT NOT NULL,
    passenger_name TEXT NOT NULL,
    passenger_document TEXT NOT NULL,
    FOREIGN KEY (flight_id) REFERENCES flights(id),
    UNIQUE(flight_id, seat_code)
);

-- Dados de Exemplo
-- Inserir aeronaves
INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) VALUES (1, 'A320 Neo', 6, 32);
INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) VALUES (2, 'B737-800', 6, 35);
INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) VALUES (3, 'E195-E2', 5, 40);

-- Inserir voos de exemplo
INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-1047', 'São Paulo', 'Rio de Janeiro', '2026-05-18', '08:35', 1, 428.00, 'Aberto');

INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-2190', 'Brasília', 'Recife', '2026-05-18', '11:10', 2, 612.00, 'Embarque');

INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-3382', 'Curitiba', 'Salvador', '2026-05-19', '17:45', 3, 735.00, 'Aberto');

-- Inserir algumas reservas de exemplo
INSERT OR IGNORE INTO reservations (flight_id, seat_code, passenger_name, passenger_document) 
VALUES (1, 'A2', 'João Silva', '12345678900');

INSERT OR IGNORE INTO reservations (flight_id, seat_code, passenger_name, passenger_document) 
VALUES (1, 'B4', 'Maria Santos', '98765432100');
