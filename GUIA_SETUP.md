# Guia de Setup — AeroGestão (Frontend + Backend)

Bem-vindo! Este guia orienta você a configurar e rodar o sistema completo de gestão de aviação.

## Estrutura do projeto

```
gestao_aviacao/
├── client/                    # Frontend React
│   ├── src/
│   │   ├── pages/            # Páginas (Home, Admin)
│   │   ├── components/       # Componentes React
│   │   └── index.css         # Estilos globais
│   └── package.json
├── backend/                   # Backend Python
│   ├── app.py                # Servidor Flask
│   ├── seat_rules.c          # Módulo C
│   ├── setup_db.py           # Inicializa banco
│   ├── requirements.txt      # Dependências Python
│   └── README.md             # Documentação backend
├── ARQUITETURA_PYTHON_C.md   # Arquitetura técnica
└── GUIA_SETUP.md             # Este arquivo
```

## Pré-requisitos

- **Node.js** 18+ (para frontend)
- **Python** 3.8+ (para backend)
- **pnpm** (gerenciador de pacotes Node)
- **gcc** (compilador C, opcional — só se quiser compilar o módulo C)

## Setup do Frontend

### 1. Instalar dependências

```bash
cd gestao_aviacao
pnpm install
```

### 2. Rodar em desenvolvimento

```bash
pnpm dev
```

Acesse `http://localhost:5173` no navegador.

### 3. Build para produção

```bash
pnpm build
```

## Setup do Backend

### 1. Instalar dependências Python

```bash
cd backend
pip install -r requirements.txt
```

### 2. Inicializar o banco de dados

```bash
python setup_db.py
```

Isso cria `aviacao.db` com tabelas e dados de exemplo.

### 3. Compilar o módulo C (opcional)

Se você tiver `gcc` instalado:

```bash
bash compile_c.sh
```

Ou manualmente:

```bash
gcc -shared -o libseat_rules.so -fPIC seat_rules.c
```

**Nota:** Se a compilação falhar, o backend continua funcionando normalmente. A validação de assentos será feita apenas em Python.

### 4. Rodar o servidor Flask

```bash
python app.py
```

O servidor estará em `http://localhost:5000`.

## Testando a API

### Listar voos

```bash
curl http://localhost:5000/api/flights
```

### Obter assentos de um voo

```bash
curl http://localhost:5000/api/flights/1/seats
```

### Criar uma reserva

```bash
curl -X POST http://localhost:5000/api/reservations \
  -H "Content-Type: application/json" \
  -d '{
    "flight_id": 1,
    "seat_code": "A10",
    "passenger_name": "João Silva",
    "passenger_document": "12345678900"
  }'
```

## Conectando Frontend ao Backend

No arquivo `client/src/pages/Home.tsx`, altere a URL da API de `http://localhost:3000` para `http://localhost:5000`:

```typescript
const response = await fetch("http://localhost:5000/api/flights");
```

## Estrutura de dados

### Aeronaves (aircraft)

| Campo | Tipo | Exemplo |
|-------|------|---------|
| id | INTEGER | 1 |
| model | TEXT | "A320 Neo" |
| seat_rows | INTEGER | 6 |
| seat_columns | INTEGER | 32 |

### Voos (flights)

| Campo | Tipo | Exemplo |
|-------|------|---------|
| id | INTEGER | 1 |
| code | TEXT | "AV-1047" |
| origin | TEXT | "São Paulo" |
| destination | TEXT | "Rio de Janeiro" |
| date | TEXT | "2026-05-18" |
| time | TEXT | "08:35" |
| price | REAL | 428.00 |
| status | TEXT | "Aberto" |

### Assentos (seats)

| Campo | Tipo | Exemplo |
|-------|------|---------|
| id | INTEGER | 1 |
| aircraft_id | INTEGER | 1 |
| code | TEXT | "A1" |
| row_label | TEXT | "A" |
| seat_number | INTEGER | 1 |

### Reservas (reservations)

| Campo | Tipo | Exemplo |
|-------|------|---------|
| id | INTEGER | 1 |
| flight_id | INTEGER | 1 |
| seat_code | TEXT | "A5" |
| passenger_name | TEXT | "João Silva" |
| passenger_document | TEXT | "12345678900" |

## Troubleshooting

### Erro: "gcc: command not found"

Instale o compilador C:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
Baixe MinGW: https://www.mingw-w64.org/
```

### Erro: "ModuleNotFoundError: No module named 'flask'"

Reinstale as dependências:

```bash
pip install --upgrade pip
pip install -r requirements.txt
```

### Erro: "Port 5000 already in use"

Use outra porta:

```bash
python app.py --port 5001
```

Ou mate o processo anterior:

```bash
lsof -ti:5000 | xargs kill -9
```

### Erro: "CORS error"

Certifique-se de que o backend está rodando em `http://localhost:5000` e que o frontend está acessando essa URL corretamente.

## Próximos passos

1. **Conectar frontend à API** — Altere as URLs de fetch nos componentes React
2. **Adicionar autenticação** — Implemente login com JWT
3. **Painel administrativo real** — Crie páginas para cadastro de voos
4. **Testes automatizados** — Escreva testes para as rotas Flask
5. **Deploy** — Coloque em produção (Vercel, Heroku, AWS, etc.)

## Documentação

- **Frontend**: Veja `client/src/pages/Home.tsx`
- **Backend**: Veja `backend/README.md`
- **Arquitetura**: Veja `ARQUITETURA_PYTHON_C.md`

## Suporte

Se tiver dúvidas, consulte a documentação técnica em `ARQUITETURA_PYTHON_C.md` ou entre em contato com o time de desenvolvimento.

---

**Boa sorte com o AeroGestão! 🚀**
