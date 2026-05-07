# Backend — AeroGestão

Backend em Python/Flask com integração ao módulo C para validação de assentos.

## Arquitetura

| Arquivo | Propósito |
|---------|-----------|
| `app.py` | Servidor Flask com rotas da API |
| `seat_rules.c` | Módulo C para validação de assentos |
| `libseat_rules.so` | Biblioteca compilada (gerada após compilação) |
| `setup_db.py` | Script para inicializar o banco SQLite |
| `requirements.txt` | Dependências Python |
| `compile_c.sh` | Script para compilar o módulo C |

## Instalação

### 1. Instalar dependências Python

```bash
pip install -r requirements.txt
```

### 2. Compilar o módulo C

```bash
bash compile_c.sh
```

Ou manualmente:

```bash
gcc -shared -o libseat_rules.so -fPIC seat_rules.c
```

### 3. Inicializar o banco de dados

```bash
python setup_db.py
```

Isso cria o arquivo `aviacao.db` com tabelas e dados de exemplo.

## Executar o servidor

```bash
python app.py
```

O servidor estará disponível em `http://localhost:5000`.

## Rotas da API

### Listar voos

```bash
GET /api/flights
```

Retorna lista de voos disponíveis.

**Resposta:**
```json
[
  {
    "id": 1,
    "code": "AV-1047",
    "origin": "São Paulo",
    "destination": "Rio de Janeiro",
    "date": "2026-05-18",
    "time": "08:35",
    "price": 428.0,
    "status": "Aberto"
  }
]
```

### Cadastrar voo (admin)

```bash
POST /api/flights
Content-Type: application/json

{
  "code": "AV-5000",
  "origin": "Salvador",
  "destination": "Fortaleza",
  "date": "2026-05-21",
  "time": "19:00",
  "aircraft_id": 1,
  "price": 350.00
}
```

### Obter mapa de assentos

```bash
GET /api/flights/<flight_id>/seats
```

Retorna lista de assentos disponíveis e reservados.

**Resposta:**
```json
{
  "flight_id": 1,
  "total_seats": 192,
  "available": 187,
  "all_seats": ["A1", "A2", ..., "F32"],
  "reserved_seats": ["A2", "B4", "C7"]
}
```

### Criar reserva

```bash
POST /api/reservations
Content-Type: application/json

{
  "flight_id": 1,
  "seat_code": "A5",
  "passenger_name": "João Silva",
  "passenger_document": "12345678900"
}
```

**Resposta (sucesso):**
```json
{
  "status": "confirmed",
  "seat": "A5"
}
```

**Resposta (erro):**
```json
{
  "error": "Assento indisponível ou inválido"
}
```

### Painel administrativo

```bash
GET /admin/dashboard
```

Retorna estatísticas gerais.

**Resposta:**
```json
{
  "flights": [...],
  "total_reservations": 5,
  "total_seats": 576
}
```

## Validação em C

O módulo C (`seat_rules.c`) implementa as seguintes funções:

- **`can_reserve_seat`** — Valida se um assento pode ser reservado
- **`seat_exists`** — Verifica se um assento existe na aeronave
- **`seat_is_reserved`** — Verifica se um assento já foi reservado
- **`count_available_seats`** — Conta assentos disponíveis

Essas funções são chamadas pelo Python via `ctypes` quando uma reserva é criada.

## Banco de dados

O SQLite armazena:

- **`aircraft`** — Modelos de aeronaves (A320, B737, etc.)
- **`flights`** — Voos cadastrados
- **`seats`** — Assentos disponíveis por aeronave
- **`reservations`** — Reservas feitas por clientes

## Segurança

- Todas as queries usam placeholders (`?`) para evitar injeção SQL
- Restrição UNIQUE em `reservations(flight_id, seat_code)` previne duplicação
- Validação em C garante regras críticas antes do insert

## Próximos passos

1. Conectar o frontend React à API
2. Adicionar autenticação (JWT ou OAuth)
3. Implementar painel administrativo real
4. Adicionar testes automatizados
5. Fazer deploy em produção (Gunicorn + Nginx)
