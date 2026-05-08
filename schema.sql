-- ============================================================================
-- AeroGestão - Schema do Banco de Dados SQLite
-- ============================================================================
-- Este arquivo contém toda a estrutura e dados iniciais do banco de dados
-- para o sistema de gestão de aviação.
-- ============================================================================

-- ===== TABELA: AIRCRAFT =====
-- Armazena os modelos de aeronaves disponíveis
CREATE TABLE IF NOT EXISTS aircraft (
    id INTEGER PRIMARY KEY,
    model TEXT NOT NULL,
    seat_rows INTEGER NOT NULL,
    seat_columns INTEGER NOT NULL
);

-- ===== TABELA: FLIGHTS =====
-- Armazena os voos cadastrados no sistema
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

-- ===== TABELA: SEATS =====
-- Armazena todos os assentos disponíveis em cada aeronave
CREATE TABLE IF NOT EXISTS seats (
    id INTEGER PRIMARY KEY,
    aircraft_id INTEGER NOT NULL,
    code TEXT NOT NULL,
    row_label TEXT NOT NULL,
    seat_number INTEGER NOT NULL,
    FOREIGN KEY (aircraft_id) REFERENCES aircraft(id),
    UNIQUE(aircraft_id, code)
);

-- ===== TABELA: RESERVATIONS =====
-- Armazena as reservas feitas pelos passageiros
CREATE TABLE IF NOT EXISTS reservations (
    id INTEGER PRIMARY KEY,
    flight_id INTEGER NOT NULL,
    seat_code TEXT NOT NULL,
    passenger_name TEXT NOT NULL,
    passenger_document TEXT NOT NULL,
    FOREIGN KEY (flight_id) REFERENCES flights(id),
    UNIQUE(flight_id, seat_code)
);

-- ============================================================================
-- DADOS DE EXEMPLO
-- ============================================================================

-- Inserir aeronaves
INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) 
VALUES (1, 'A320 Neo', 6, 32);

INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) 
VALUES (2, 'B737-800', 6, 35);

INSERT OR IGNORE INTO aircraft (id, model, seat_rows, seat_columns) 
VALUES (3, 'E195-E2', 5, 40);

-- Inserir voos
INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-1047', 'São Paulo', 'Rio de Janeiro', '2026-05-18', '08:35', 1, 428.00, 'Aberto');

INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-2190', 'Brasília', 'Recife', '2026-05-18', '11:10', 2, 612.00, 'Embarque');

INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) 
VALUES ('AV-3382', 'Curitiba', 'Salvador', '2026-05-19', '17:45', 3, 735.00, 'Aberto');

-- Gerar assentos para A320 Neo (6 linhas x 32 colunas)
INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'A' || col, 'A', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'B' || col, 'B', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'C' || col, 'C', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'D' || col, 'D', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'E' || col, 'E', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 1, 'F' || col, 'F', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 32
    ) SELECT col FROM cnt
);

-- Gerar assentos para B737-800 (6 linhas x 35 colunas)
INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'A' || col, 'A', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'B' || col, 'B', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'C' || col, 'C', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'D' || col, 'D', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'E' || col, 'E', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 2, 'F' || col, 'F', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 35
    ) SELECT col FROM cnt
);

-- Gerar assentos para E195-E2 (5 linhas x 40 colunas)
INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 3, 'A' || col, 'A', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 40
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 3, 'B' || col, 'B', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 40
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 3, 'C' || col, 'C', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 40
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 3, 'D' || col, 'D', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 40
    ) SELECT col FROM cnt
);

INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) 
SELECT 3, 'E' || col, 'E', col FROM (
    WITH RECURSIVE cnt(col) AS (
        SELECT 1 UNION ALL SELECT col+1 FROM cnt WHERE col < 40
    ) SELECT col FROM cnt
);

-- Inserir reservas de exemplo
INSERT OR IGNORE INTO reservations (flight_id, seat_code, passenger_name, passenger_document) 
VALUES (1, 'A2', 'João Silva', '12345678900');

INSERT OR IGNORE INTO reservations (flight_id, seat_code, passenger_name, passenger_document) 
VALUES (1, 'B4', 'Maria Santos', '98765432100');
