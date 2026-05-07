# Arquitetura técnica — AeroGestão com Python, C e SQLite

**Autor:** Manus AI  
**Projeto:** Gestão de Aviação  
**Objetivo:** orientar a evolução do protótipo web para um sistema funcional com backend em **Python/Flask**, banco **SQLite** e módulo em **C** para validações críticas de assentos.

## Visão geral

O protótipo entregue possui uma interface navegável com **área do cliente** e **área administrativa**. A próxima etapa técnica recomendada é conectar essa interface a um backend em Python. O Flask é adequado para iniciar esse tipo de aplicação porque permite definir rotas HTTP com decoradores como `@app.route`, retornar HTML ou JSON e organizar páginas com templates durante o desenvolvimento.[^1]

> A arquitetura recomendada separa o sistema em três camadas: a interface web cuida da experiência do usuário, o Python coordena regras de negócio e persistência, e o C executa validações pequenas, rápidas e críticas, como verificar disponibilidade e limites de assentos.

| Camada | Tecnologia recomendada | Responsabilidade | Observação prática |
|---|---|---|---|
| Cliente | HTML, CSS e JavaScript/React | Escolha de voo, seleção de assento e painel visual | O protótipo atual já representa essa camada. |
| Backend | Python com Flask | Rotas, autenticação futura, reservas, consultas e respostas JSON | Flask permite criar rotas para `GET` e `POST`, o que serve para listar voos e registrar reservas.[^1] |
| Banco de dados | SQLite no início | Persistir voos, aeronaves, assentos e reservas | O módulo `sqlite3` cria conexões com arquivos de banco e permite executar SQL diretamente em Python.[^3] |
| Validação nativa | C compilado como biblioteca compartilhada | Verificar assentos, limites de aeronave e colisões de reserva | Python pode carregar bibliotecas C por `ctypes`, que chama funções em DLLs ou bibliotecas compartilhadas.[^2] |

## Modelo de dados inicial

O banco deve começar simples, mas já preparado para crescer. Para um MVP, quatro tabelas bastam: `flights`, `aircraft`, `seats` e `reservations`. Essa divisão evita misturar cadastro operacional com transações de cliente.

| Tabela | Campos principais | Finalidade |
|---|---|---|
| `aircraft` | `id`, `model`, `seat_rows`, `seat_columns` | Define o tipo de aeronave e sua capacidade. |
| `flights` | `id`, `code`, `origin`, `destination`, `date`, `time`, `aircraft_id`, `status`, `price` | Representa cada voo cadastrado pelo administrador. |
| `seats` | `id`, `aircraft_id`, `code`, `row_label`, `seat_number` | Guarda os assentos possíveis por aeronave. |
| `reservations` | `id`, `flight_id`, `seat_code`, `passenger_name`, `passenger_document`, `created_at` | Guarda a reserva feita pelo cliente. |

O SQLite é uma boa escolha inicial porque funciona como banco em arquivo, sem exigir um servidor separado, e pode ser usado para prototipagem antes de migrar para PostgreSQL ou outro banco maior.[^3]

## Rotas Flask recomendadas

As rotas abaixo conectariam a interface atual ao backend real. Em produção, as rotas administrativas devem exigir login, mas no primeiro MVP elas podem ser protegidas por uma senha simples ou por sessão.

| Método | Rota | Uso | Retorno esperado |
|---|---|---|---|
| `GET` | `/api/flights` | Listar voos disponíveis para clientes | JSON com voos ativos. |
| `POST` | `/api/flights` | Cadastrar voo pelo admin | JSON do voo criado. |
| `GET` | `/api/flights/<flight_id>/seats` | Ver mapa de assentos do voo | JSON com livres, reservados e bloqueados. |
| `POST` | `/api/reservations` | Criar reserva de assento | Confirmação ou erro de assento indisponível. |
| `GET` | `/admin` | Painel administrativo | Página HTML ou frontend separado. |

O Flask diferencia métodos HTTP com o argumento `methods` no decorador de rota, permitindo que uma mesma aplicação exponha consultas e cadastros.[^1]

## Exemplo de módulo C para validação

O módulo C deve ser pequeno e determinístico. Ele não precisa acessar o banco; o Python coleta os dados, monta os vetores de assentos ocupados e chama o C apenas para decidir se a seleção é válida.

```c
// seat_rules.c
#include <string.h>

int seat_exists(const char *seat, const char *valid_seats[], int valid_count) {
    for (int i = 0; i < valid_count; i++) {
        if (strcmp(seat, valid_seats[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int seat_is_reserved(const char *seat, const char *reserved_seats[], int reserved_count) {
    for (int i = 0; i < reserved_count; i++) {
        if (strcmp(seat, reserved_seats[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int can_reserve_seat(const char *seat, const char *valid_seats[], int valid_count, const char *reserved_seats[], int reserved_count) {
    if (!seat_exists(seat, valid_seats, valid_count)) {
        return 0;
    }
    if (seat_is_reserved(seat, reserved_seats, reserved_count)) {
        return 0;
    }
    return 1;
}
```

No Linux, o arquivo pode ser compilado como biblioteca compartilhada com o comando abaixo.

```bash
gcc -shared -o libseat_rules.so -fPIC seat_rules.c
```

## Exemplo de chamada Python com `ctypes`

O Python pode carregar a biblioteca C com `ctypes.CDLL`. A documentação oficial descreve `ctypes` como uma biblioteca de função estrangeira que oferece tipos compatíveis com C e permite chamar funções em bibliotecas compartilhadas.[^2]

```python
# c_rules.py
from ctypes import CDLL, c_char_p, c_int, POINTER

lib = CDLL("./libseat_rules.so")
lib.can_reserve_seat.argtypes = [
    c_char_p,
    POINTER(c_char_p),
    c_int,
    POINTER(c_char_p),
    c_int,
]
lib.can_reserve_seat.restype = c_int


def can_reserve(seat: str, valid_seats: list[str], reserved_seats: list[str]) -> bool:
    valid_array = (c_char_p * len(valid_seats))(*[s.encode() for s in valid_seats])
    reserved_array = (c_char_p * len(reserved_seats))(*[s.encode() for s in reserved_seats])
    result = lib.can_reserve_seat(
        seat.encode(),
        valid_array,
        len(valid_seats),
        reserved_array,
        len(reserved_seats),
    )
    return bool(result)
```

## Exemplo de backend Flask

O backend abaixo mostra o esqueleto de uma reserva. Ele não substitui uma implementação completa, mas demonstra onde o módulo C entraria no fluxo.

```python
# app.py
from flask import Flask, jsonify, request
import sqlite3
from c_rules import can_reserve

app = Flask(__name__)
DATABASE = "aviacao.db"


def get_connection():
    con = sqlite3.connect(DATABASE)
    con.row_factory = sqlite3.Row
    return con


@app.get("/api/flights")
def list_flights():
    con = get_connection()
    flights = con.execute("SELECT * FROM flights ORDER BY date, time").fetchall()
    con.close()
    return jsonify([dict(row) for row in flights])


@app.post("/api/reservations")
def create_reservation():
    data = request.get_json()
    flight_id = data["flight_id"]
    seat_code = data["seat_code"]

    con = get_connection()
    valid_seats = [row["code"] for row in con.execute("SELECT code FROM seats").fetchall()]
    reserved = [
        row["seat_code"]
        for row in con.execute(
            "SELECT seat_code FROM reservations WHERE flight_id = ?",
            (flight_id,),
        ).fetchall()
    ]

    if not can_reserve(seat_code, valid_seats, reserved):
        con.close()
        return jsonify({"error": "Assento indisponível ou inválido"}), 409

    con.execute(
        "INSERT INTO reservations (flight_id, seat_code, passenger_name, passenger_document) VALUES (?, ?, ?, ?)",
        (flight_id, seat_code, data["passenger_name"], data["passenger_document"]),
    )
    con.commit()
    con.close()
    return jsonify({"status": "confirmed", "seat": seat_code})
```

## Cuidados de segurança e produção

O servidor de desenvolvimento do Flask é útil para testes locais, mas a própria documentação alerta que o modo de debug não deve ser usado em produção, pois o depurador interativo representa risco de segurança.[^1] Além disso, as queries SQL devem usar placeholders, como `?`, em vez de interpolação de strings, porque a documentação do `sqlite3` recomenda placeholders para vincular valores e evitar ataques de injeção SQL.[^3]

| Risco | Medida recomendada |
|---|---|
| Assento duplicado | Criar índice único em `reservations(flight_id, seat_code)` e validar no C antes do insert. |
| Injeção SQL | Usar placeholders `?` em todos os comandos parametrizados. |
| Admin sem proteção | Adicionar autenticação antes de liberar cadastro de voos. |
| Debug em produção | Rodar Flask por WSGI, sem debug ativo. |
| Concorrência de reserva | Usar transação e restrição única no banco, mesmo com validação em C. |

## Próximos passos recomendados

A evolução natural é transformar o protótipo em uma aplicação conectada ao backend. Primeiro, crie o banco SQLite e as tabelas. Depois, implemente as rotas Flask e conecte os botões da interface aos endpoints via `fetch`. Por fim, compile a biblioteca C e use `ctypes` somente nas regras que realmente precisam de validação nativa.

| Etapa | Resultado esperado |
|---|---|
| 1. Criar banco | Arquivo `aviacao.db` com tabelas de voos, assentos e reservas. |
| 2. Implementar Flask | Endpoints reais para listar voos e criar reservas. |
| 3. Compilar C | Arquivo `libseat_rules.so` disponível para o Python. |
| 4. Conectar frontend | A tela deixa de usar dados simulados e passa a consumir a API. |
| 5. Testar concorrência | Duas reservas para o mesmo assento devem falhar de forma segura. |

## References

[^1]: [Flask Documentation — Quickstart](https://flask.palletsprojects.com/en/stable/quickstart/)
[^2]: [Python Documentation — ctypes, a foreign function library for Python](https://docs.python.org/3/library/ctypes.html)
[^3]: [Python Documentation — sqlite3, DB-API 2.0 interface for SQLite databases](https://docs.python.org/3/library/sqlite3.html)
