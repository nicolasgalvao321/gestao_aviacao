"""
setup_db.py — Script para inicializar o banco de dados com dados de exemplo.

Uso:
    python setup_db.py
"""

import sqlite3
import os

DATABASE = os.path.join(os.path.dirname(__file__), "aviacao.db")


def setup_database():
    """Cria tabelas e insere dados de exemplo."""
    con = sqlite3.connect(DATABASE)
    cur = con.cursor()

    # Tabela de aeronaves
    cur.execute("""
        CREATE TABLE IF NOT EXISTS aircraft (
            id INTEGER PRIMARY KEY,
            model TEXT NOT NULL,
            seat_rows INTEGER NOT NULL,
            seat_columns INTEGER NOT NULL
        )
    """)

    # Tabela de voos
    cur.execute("""
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
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (aircraft_id) REFERENCES aircraft(id)
        )
    """)

    # Tabela de assentos
    cur.execute("""
        CREATE TABLE IF NOT EXISTS seats (
            id INTEGER PRIMARY KEY,
            aircraft_id INTEGER NOT NULL,
            code TEXT NOT NULL,
            row_label TEXT NOT NULL,
            seat_number INTEGER NOT NULL,
            FOREIGN KEY (aircraft_id) REFERENCES aircraft(id),
            UNIQUE(aircraft_id, code)
        )
    """)

    # Tabela de reservas
    cur.execute("""
        CREATE TABLE IF NOT EXISTS reservations (
            id INTEGER PRIMARY KEY,
            flight_id INTEGER NOT NULL,
            seat_code TEXT NOT NULL,
            passenger_name TEXT NOT NULL,
            passenger_document TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (flight_id) REFERENCES flights(id),
            UNIQUE(flight_id, seat_code)
        )
    """)

    # Inserir aeronaves de exemplo
    aircraft_data = [
        ("A320 Neo", 6, 32),
        ("B737-800", 6, 35),
        ("E195-E2", 5, 40),
    ]

    cur.executemany(
        "INSERT OR IGNORE INTO aircraft (model, seat_rows, seat_columns) VALUES (?, ?, ?)",
        aircraft_data,
    )

    # Obter IDs das aeronaves
    aircraft = cur.execute("SELECT id, seat_rows, seat_columns FROM aircraft").fetchall()

    # Gerar assentos para cada aeronave
    for aircraft_id, rows, cols in aircraft:
        for row in range(rows):
            row_label = chr(65 + row)  # A, B, C, D, E, F
            for col in range(1, cols + 1):
                seat_code = f"{row_label}{col}"
                cur.execute(
                    "INSERT OR IGNORE INTO seats (aircraft_id, code, row_label, seat_number) VALUES (?, ?, ?, ?)",
                    (aircraft_id, seat_code, row_label, col),
                )

    # Inserir voos de exemplo
    flights_data = [
        ("AV-1047", "São Paulo", "Rio de Janeiro", "2026-05-18", "08:35", 1, 428.00, "Aberto"),
        ("AV-2190", "Brasília", "Recife", "2026-05-18", "11:10", 2, 612.00, "Embarque"),
        ("AV-3382", "Curitiba", "Salvador", "2026-05-19", "17:45", 3, 735.00, "Aberto"),
        ("AV-4521", "Manaus", "Belém", "2026-05-20", "14:25", 1, 540.00, "Aberto"),
    ]

    cur.executemany(
        "INSERT OR IGNORE INTO flights (code, origin, destination, date, time, aircraft_id, price, status) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
        flights_data,
    )

    # Inserir algumas reservas de exemplo
    reservations_data = [
        (1, "A2", "João Silva", "12345678900"),
        (1, "B4", "Maria Santos", "98765432100"),
        (1, "C7", "Pedro Oliveira", "11122233344"),
        (2, "A1", "Ana Costa", "55566677788"),
        (2, "D3", "Carlos Ferreira", "99988877766"),
    ]

    cur.executemany(
        "INSERT OR IGNORE INTO reservations (flight_id, seat_code, passenger_name, passenger_document) VALUES (?, ?, ?, ?)",
        reservations_data,
    )

    con.commit()
    con.close()

    print(f"✓ Banco de dados inicializado: {DATABASE}")
    print("✓ Aeronaves: 3 modelos")
    print("✓ Voos: 4 rotas de exemplo")
    print("✓ Assentos: gerados automaticamente")
    print("✓ Reservas: 5 exemplos inseridos")


if __name__ == "__main__":
    setup_database()
