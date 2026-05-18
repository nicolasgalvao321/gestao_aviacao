# AeroGestão - Sistema de Gerenciamento de Aviação

Sistema simples de gerenciamento de voos e reservas de assentos desenvolvido em **C puro** com frontend HTML/CSS/JavaScript.

## 📋 Requisitos

### Windows (Recomendado: VS Code)
- **Visual Studio Code** (https://code.visualstudio.com/)
- **C/C++ Extension Pack** (Microsoft) - instale via VS Code
- **MinGW-w64** (compilador GCC para Windows) - instalado automaticamente
- **CMake** - instalado automaticamente

### macOS
- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake**: `brew install cmake`

### Linux
- **build-essential**: `sudo apt-get install build-essential cmake`

## 🚀 Como Compilar e Executar

### Opção 1: VS Code (Windows) ⭐ Recomendado

#### Primeira Vez: Configuração Inicial

1. **Abra o projeto no VS Code**
   - File → Open Folder → selecione a pasta `gestao_aviacao`

2. **Instale a extensão C/C++**
   - Ctrl + Shift + X (abrir extensões)
   - Procure por: `C/C++ Extension Pack` (Microsoft)
   - Clique em "Install"
   - Aguarde a instalação

3. **Recarregue o VS Code**
   - Ctrl + Shift + P
   - Digite: `Developer: Reload Window`
   - Pressione Enter

4. **Configure o CMake**
   - Ctrl + Shift + P
   - Digite: `CMake: Configure`
   - Escolha o compilador: **GCC** ou **MinGW**
   - Aguarde a configuração (pode demorar alguns minutos)

#### Compilar e Executar

1. **Compile o projeto**
   - Ctrl + Shift + P
   - Digite: `CMake: Build`
   - Ou pressione: **Ctrl + Shift + B**

2. **Execute o servidor**
   - Ctrl + Shift + P
   - Digite: `CMake: Run`
   - Ou clique no botão ▶ (Play) no canto superior direito

3. **Abra no navegador**
   - Acesse: **http://localhost:8080**
   - Pronto! 🎉

### Opção 2: Terminal (Qualquer SO)

```bash
# Navegue até a pasta do projeto
cd gestao_aviacao

# Crie a pasta de build
mkdir -p build
cd build

# Configure o CMake
cmake ..

# Compile
make

# Execute
./aerogestao-server
```

Depois acesse: **http://localhost:8080**

## 🎯 Funcionalidades

- ✅ **Listagem de Voos**: Visualize todos os voos disponíveis
- ✅ **Adicionar Voos**: Admin pode cadastrar novos voos
- ✅ **Reservar Assentos**: Clientes podem reservar assentos
- ✅ **Visualizar Reservas**: Veja todas as reservas realizadas
- ✅ **API REST**: Endpoints para integração

## 📡 API Endpoints

### GET
- `GET /` - Retorna o frontend (HTML)
- `GET /styles.css` - Retorna o CSS
- `GET /app.js` - Retorna o JavaScript
- `GET /api/flights` - Lista todos os voos (JSON)
- `GET /api/reservations` - Lista todas as reservas (JSON)

### POST
- `POST /api/flights` - Adiciona um novo voo
  ```json
  {
    "code": "BR001",
    "origin": "São Paulo",
    "destination": "Rio de Janeiro",
    "date": "2026-05-20",
    "time": "10:00",
    "price": 250.00
  }
  ```

- `POST /api/reservations` - Faz uma reserva
  ```json
  {
    "flight_id": 1,
    "seat_code": "1A",
    "passenger_name": "João Silva",
    "passenger_document": "12345678900"
  }
  ```

## 🔐 Admin

- **Senha**: `admin123`
- Acesse a seção Admin para adicionar voos

## 📁 Estrutura do Projeto

```
gestao_aviacao/
├── backend/
│   └── src/
│       ├── main.c          # Servidor HTTP em C
│       ├── database.c      # Lógica de dados em memória
│       ├── database.h      # Headers
│       └── ...
├── frontend/
│   ├── index.html          # Interface web
│   ├── styles.css          # Estilos
│   ├── app.js              # Lógica do frontend
│   └── ...
├── build/                  # Pasta de compilação (gerada automaticamente)
├── CMakeLists.txt          # Configuração do CMake
└── README.md               # Este arquivo
```

## 🛠️ Troubleshooting

### Erro: "cmake.configure not found" (VS Code)
**Solução:**
1. Ctrl + Shift + X (abrir extensões)
2. Procure por: `CMake Tools`
3. Instale a extensão (Microsoft)
4. Reinicie o VS Code (Ctrl + Shift + P → "Developer: Reload Window")

### Erro: "GCC not found" (VS Code)
**Solução:**
1. Instale o **MinGW-w64** de: https://www.mingw-w64.org/
2. Ou deixe o VS Code instalar automaticamente quando configurar o CMake
3. Reinicie o VS Code

### Porta 8080 já está em uso
**Solução:**
1. Abra `backend/src/main.c`
2. Procure pela linha: `bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));`
3. Mude a porta (8080) para outra (ex: 8081)
4. Recompile

### Frontend não carrega
**Solução:**
- Certifique-se de que está acessando `http://localhost:8080` (não `file://`)
- Verifique se o servidor está rodando (deve aparecer "Servidor iniciado em..." no terminal)

## 📝 Notas Importantes

- O servidor roda na **porta 8080**
- Os dados são armazenados em **memória** (não persistem após reiniciar)
- Sem dependências externas além de C puro
- Compatível com Windows, macOS e Linux
- Tudo em um único executável! 🚀

## 👨‍💼 Para Apresentar ao Professor

1. **Compile** o projeto (Ctrl + Shift + B no VS Code)
2. **Execute** o servidor (Ctrl + Shift + P → "CMake: Run")
3. **Abra** `http://localhost:8080` no navegador
4. **Demonstre:**
   - Listagem de voos
   - Adição de novo voo (Admin → Senha: admin123)
   - Reserva de assento
   - Visualização de reservas

Tudo funciona em um único executável! 🎉

## 📚 Tecnologias Utilizadas

- **Backend**: C puro (sem frameworks)
- **Frontend**: HTML5, CSS3, JavaScript vanilla
- **Servidor HTTP**: Socket programming (BSD sockets)
- **Dados**: Memória (arrays em C)
- **Build**: CMake

---

**Desenvolvido para projeto escolar de Gestão de Aviação**
