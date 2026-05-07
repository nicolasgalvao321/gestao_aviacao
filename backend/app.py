"""
AeroGestão — Backend Flask
Sistema de gestão de aviação com reserva de assentos e validação em C.

Rotas:
- GET /api/flights — Lista voos disponíveis
- POST /api/flights — Cadastra novo voo (admin)
- GET /api/flights/<flight_id>/seats — Mapa de assentos do voo
- POST /api/reservations — Cria reserva de assento
- GET /admin/dashboard — Painel administrativo (dados)
"""

from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS
import sqlite3
import os
from datetime import datetime
from ctypes import CDLL, c_char_p, c_int, POINTER
import json

app = Flask(__name__)
CORS(app)

# Configuração
DATABASE = os.path.join(os.path.dirname(__file__), "aviacao.db")
STATIC_FOLDER = os.path.join(os.path.dirname(__file__), "..", "dist", "public")

# Carregar biblioteca C (se disponível)
try:
    lib = CDLL(os.path.join(os.path.dirname(__file__), "libseat_rules.so"))
    lib.can_reserve_seat.argtypes = [
        c_char_p,
        POINTER(c_char_p),
        c_int,
        POINTER(c_char_p),
        c_int,
    ]
    lib.can_reserve_seat.restype = c_int
    C_AVAILABLE = True
except Exception as e:
    print(f"Aviso: Biblioteca C não carregada. {e}")
    C_AVAILABLE = False


def get_connection():
    """Cria conexão com o banco SQLite."""
    con = sqlite3.connect(DATABASE)
    con.row_factory = sqlite3.Row
    return con


def init_db():
    """Inicializa o banco de dados com tabelas."""
    con = get_connection()
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

    con.commit()
    con.close()


def can_reserve_with_c(seat, valid_seats, reserved_seats):
    """Valida assento usando biblioteca C (se disponível)."""
    if not C_AVAILABLE:
        return True

    valid_array = (c_char_p * len(valid_seats))(*[s.encode() for s in valid_seats])
    reserved_array = (c_char_p * len(reserved_seats))(
        *[s.encode() for s in reserved_seats]
    )
    result = lib.can_reserve_seat(
        seat.encode(),
        valid_array,
        len(valid_seats),
        reserved_array,
        len(reserved_seats),
    )
    return bool(result)


@app.route("/api/flights", methods=["GET"])
def list_flights():
    """Lista todos os voos disponíveis."""
    con = get_connection()
    flights = con.execute(
        "SELECT id, code, origin, destination, date, time, price, status FROM flights ORDER BY date, time"
    ).fetchall()
    con.close()
    return jsonify([dict(row) for row in flights])


@app.route("/api/flights", methods=["POST"])
def create_flight():
    """Cadastra um novo voo (admin)."""
    data = request.get_json()

    con = get_connection()
    try:
        cur = con.cursor()
        cur.execute(
            """
            INSERT INTO flights (code, origin, destination, date, time, aircraft_id, price, status)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                data["code"],
                data["origin"],
                data["destination"],
                data["date"],
                data["time"],
                data.get("aircraft_id", 1),
                data["price"],
                "Aberto",
            ),
        )
        con.commit()
        flight_id = cur.lastrowid
        con.close()
        return jsonify({"id": flight_id, "code": data["code"]}), 201
    except sqlite3.IntegrityError:
        con.close()
        return jsonify({"error": "Código de voo já existe"}), 409
    except Exception as e:
        con.close()
        return jsonify({"error": str(e)}), 400


@app.route("/api/flights/<int:flight_id>/seats", methods=["GET"])
def get_flight_seats(flight_id):
    """Retorna o mapa de assentos de um voo."""
    con = get_connection()

    flight = con.execute("SELECT aircraft_id FROM flights WHERE id = ?", (flight_id,)).fetchone()
    if not flight:
        con.close()
        return jsonify({"error": "Voo não encontrado"}), 404

    aircraft_id = flight["aircraft_id"]

    # Obter todos os assentos possíveis
    all_seats = con.execute(
        "SELECT code FROM seats WHERE aircraft_id = ? ORDER BY code", (aircraft_id,)
    ).fetchall()

    # Obter assentos reservados
    reserved_seats = con.execute(
        "SELECT seat_code FROM reservations WHERE flight_id = ?", (flight_id,)
    ).fetchall()

    con.close()

    reserved_codes = [row["seat_code"] for row in reserved_seats]
    all_codes = [row["code"] for row in all_seats]

    return jsonify(
        {
            "flight_id": flight_id,
            "total_seats": len(all_codes),
            "available": len(all_codes) - len(reserved_codes),
            "all_seats": all_codes,
            "reserved_seats": reserved_codes,
        }
    )


@app.route("/api/reservations", methods=["POST"])
def create_reservation():
    """Cria uma reserva de assento."""
    data = request.get_json()
    flight_id = data["flight_id"]
    seat_code = data["seat_code"]
    passenger_name = data.get("passenger_name", "Passageiro")
    passenger_document = data.get("passenger_document", "")

    con = get_connection()

    # Obter todos os assentos válidos e reservados
    all_seats = con.execute(
        "SELECT code FROM seats WHERE aircraft_id = (SELECT aircraft_id FROM flights WHERE id = ?)",
        (flight_id,),
    ).fetchall()

    reserved = con.execute(
        "SELECT seat_code FROM reservations WHERE flight_id = ?", (flight_id,)
    ).fetchall()

    all_codes = [row["code"] for row in all_seats]
    reserved_codes = [row["seat_code"] for row in reserved]

    # Validar com C (ou apenas com Python se C não estiver disponível)
    if not can_reserve_with_c(seat_code, all_codes, reserved_codes):
        con.close()
        return jsonify({"error": "Assento indisponível ou inválido"}), 409

    # Inserir reserva
    try:
        con.execute(
            """
            INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document)
            VALUES (?, ?, ?, ?)
            """,
            (flight_id, seat_code, passenger_name, passenger_document),
        )
        con.commit()
        con.close()
        return jsonify({"status": "confirmed", "seat": seat_code}), 201
    except sqlite3.IntegrityError:
        con.close()
        return jsonify({"error": "Assento já foi reservado"}), 409
    except Exception as e:
        con.close()
        return jsonify({"error": str(e)}), 400


@app.route("/admin/dashboard", methods=["GET"])
def admin_dashboard():
    """Retorna dados do painel administrativo."""
    con = get_connection()

    flights = con.execute(
        "SELECT id, code, origin, destination, status FROM flights"
    ).fetchall()
    total_reservations = con.execute("SELECT COUNT(*) as count FROM reservations").fetchone()
    total_seats = con.execute(
        "SELECT COUNT(*) as count FROM seats"
    ).fetchone()

    con.close()

    return jsonify(
        {
            "flights": [dict(row) for row in flights],
            "total_reservations": total_reservations["count"],
            "total_seats": total_seats["count"],
        }
    )


@app.route("/", methods=["GET"])
@app.route("/<path:path>", methods=["GET"])
def serve_static(path=""):
    """Serve arquivos estáticos do frontend."""
    if path == "":
        path = "index.html"
    return send_from_directory(STATIC_FOLDER, path)


if __name__ == "__main__":
    init_db()
    app.run(debug=True, host="0.0.0.0", port=5000)
