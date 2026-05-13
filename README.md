# AeroGestão - Sistema de Gestão de Aviação

Aplicação web para gestão de voos e reserva de assentos com backend em C++ e frontend em HTML/CSS/JavaScript.

## Arquitetura

- **Frontend**: HTML/CSS/JavaScript (roda no navegador)
- **Backend**: C++ com Crow (servidor HTTP)
- **Banco de dados**: SQLite3
- **API**: RESTful em JSON

## Estrutura do Projeto

```
gestao_aviacao/
├── frontend/
│   ├── index.html       # Interface web
│   ├── styles.css       # Estilos
│   └── app.js          # Lógica JavaScript
├── backend/
│   ├── src/
│   │   ├── main.cpp    # Servidor HTTP
│   │   ├── database.h  # Header do banco
│   │   └── database.cpp # Implementação do banco
│   └── include/        # Bibliotecas externas (Crow, nlohmann/json)
├── CMakeLists.txt      # Configuração CMake
└── README.md           # Este arquivo
```

## Requisitos

- **CMake** 3.16+
- **C++17** ou superior
- **SQLite3**
- **Crow** (header-only, já incluído)
- **nlohmann/json** (header-only, já incluído)

### Windows (MinGW com CLion)

CLion já vem com MinGW, CMake e Ninja. Apenas certifique-se de ter SQLite3 instalado.

### Linux

```bash
sudo apt-get install cmake g++ sqlite3 libsqlite3-dev
```

### macOS

```bash
brew install cmake sqlite3
```

## Compilação

### No CLion

1. Abra o projeto em CLion
2. Clique em **Build → Build Project** (Ctrl+F9)
3. A aplicação será compilada em `cmake-build-debug/`

### Via Terminal

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Execução

### Backend (Servidor C++)

```bash
# No CLion: Shift+F10 ou Run → Run
# Via terminal:
./cmake-build-debug/aerogestao-server
```

O servidor iniciará em `http://localhost:8080`

### Frontend

Abra o arquivo `frontend/index.html` no navegador ou sirva com um servidor local:

```bash
# Com Python 3
python -m http.server 8000 --directory frontend

# Com Node.js
npx http-server frontend
```

Acesse `http://localhost:8000`

## API Endpoints

### Voos

- `GET /api/flights` - Listar todos os voos
- `GET /api/flights/<id>` - Obter voo específico
- `POST /api/flights` - Criar novo voo

**Exemplo POST:**
```json
{
  "code": "AV-1003",
  "origin": "São Paulo",
  "destination": "Salvador",
  "date": "2026-05-20",
  "time": "14:00",
  "price": 450.00
}
```

### Reservas

- `GET /api/reservations` - Listar todas as reservas
- `GET /api/reservations/flight/<id>` - Reservas de um voo específico
- `POST /api/reservations` - Criar nova reserva
- `DELETE /api/reservations/<id>` - Cancelar reserva

**Exemplo POST:**
```json
{
  "flight_id": 1,
  "seat_code": "1A",
  "passenger_name": "João Silva",
  "passenger_document": "12345678900"
}
```

## Funcionalidades

### Página do Cliente
- ✅ Listar voos disponíveis
- ✅ Visualizar mapa de assentos
- ✅ Selecionar assento
- ✅ Preencher dados do passageiro
- ✅ Confirmar reserva

### Página do Admin (Senha: admin123)
- ✅ Cadastrar novo voo
- ✅ Visualizar lista de voos
- ✅ Ver reservas por voo
- ✅ Cancelar reservas

## Banco de Dados

O banco SQLite é criado automaticamente na primeira execução com as seguintes tabelas:

- **aircraft**: Modelos de aeronaves
- **flights**: Voos cadastrados
- **seats**: Assentos dos voos
- **reservations**: Reservas de passageiros

## Troubleshooting

### Erro: "SQLite3 não encontrado"
Instale SQLite3 dev:
```bash
# Linux
sudo apt-get install libsqlite3-dev

# macOS
brew install sqlite3
```

### Porta 8080 já em uso
Modifique em `backend/src/main.cpp` a linha:
```cpp
app.port(8080).multithreaded().run();
```

### Frontend não conecta ao backend
Certifique-se de que:
1. O backend está rodando em `http://localhost:8080`
2. O frontend está em `http://localhost:8000` (ou outro servidor local)
3. O CORS está habilitado (já está no código)

## Desenvolvimento

Para adicionar novas funcionalidades:

1. Adicione métodos em `backend/src/database.h` e `database.cpp`
2. Crie novas rotas em `backend/src/main.cpp`
3. Atualize o frontend em `frontend/app.js`

## Licença

MIT
