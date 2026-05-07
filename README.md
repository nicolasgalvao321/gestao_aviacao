# AeroGestão — Aplicação Desktop em C/C++ com Qt

Sistema de gestão de aviação com interface gráfica em Qt, banco de dados SQLite integrado e duas áreas: cliente e administrador.

## Requisitos

- **Qt 6.x** (Core, Gui, Widgets, Sql)
- **CMake** 3.16+
- **Compilador C++** (GCC, Clang ou MSVC)
- **SQLite** (incluído no Qt)

## Instalação no CLion

### 1. Abrir o projeto

- Abra o CLion
- Clique em **File → Open**
- Navegue até a pasta `AeroGestao_Qt` e abra

### 2. Configurar o Kit

- Vá para **File → Settings → Project: AeroGestao → CMake**
- Certifique-se de que o **Toolchain** está configurado corretamente
- Se não encontrar Qt, clique em **CMake → Reload CMake Project**

### 3. Compilar

- Clique em **Build → Build Project** (Ctrl+F9)
- Ou use o botão de build na barra de ferramentas

### 4. Executar

- Clique em **Run → Run** (Shift+F10)
- Ou clique no botão verde de play

## Estrutura do Projeto

```
AeroGestao_Qt/
├── CMakeLists.txt          # Configuração CMake
├── src/
│   ├── main.cpp            # Ponto de entrada
│   ├── mainwindow.h/.cpp   # Janela principal
│   ├── clientpage.h/.cpp   # Página do cliente
│   ├── adminpage.h/.cpp    # Página do admin
│   ├── loginpage.h/.cpp    # Página de login
│   └── database.h/.cpp     # Gerenciador de banco de dados
└── README.md               # Este arquivo
```

## Funcionalidades

### Área do Cliente

- ✅ Listar voos disponíveis
- ✅ Selecionar voo
- ✅ Visualizar mapa de assentos (disponíveis e reservados)
- ✅ Selecionar assento
- ✅ Preencher dados do passageiro (nome e CPF)
- ✅ Confirmar reserva

### Área Administrativa

- ✅ Login com senha: **admin123**
- ✅ Cadastrar novos voos
- ✅ Visualizar lista de voos
- ✅ Visualizar reservas por voo
- ✅ Gerenciar aeronaves

## Banco de Dados

O banco SQLite é criado automaticamente em:

**Linux/Mac:** `~/.local/share/AeroGestao/aviacao.db`
**Windows:** `%APPDATA%/AeroGestao/aviacao.db`

### Tabelas

- **aircraft** — Modelos de aeronaves
- **flights** — Voos cadastrados
- **seats** — Assentos disponíveis
- **reservations** — Reservas feitas

## Dados de Exemplo

Na primeira execução, o sistema cria automaticamente:

- 3 aeronaves (A320 Neo, B737-800, E195-E2)
- 4 voos de exemplo
- Assentos gerados para cada aeronave
- 2 reservas de exemplo

## Senha de Admin

**Usuário:** admin
**Senha:** admin123

## Troubleshooting

### Erro: "Qt not found"

Instale Qt6:

```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-sql-dev

# macOS (com Homebrew)
brew install qt6

# Windows
Baixe em: https://www.qt.io/download
```

### Erro: "CMake not found"

Instale CMake:

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
Baixe em: https://cmake.org/download/
```

### Erro ao compilar: "cannot find -lQt6Sql"

Certifique-se de que o Qt está instalado corretamente e configure o CMake path no CLion.

## Desenvolvimento

### Adicionar nova funcionalidade

1. Crie os arquivos `.h` e `.cpp` em `src/`
2. Adicione ao `CMakeLists.txt` na lista `PROJECT_SOURCES`
3. Recarregue o projeto CMake
4. Compile e teste

### Modificar banco de dados

Edite `database.cpp` nas funções `createTables()` e `insertSampleData()`.

## Compilação em linha de comando

```bash
cd AeroGestao_Qt
mkdir build
cd build
cmake ..
make
./AeroGestao
```

## Próximos passos

- Adicionar autenticação de usuário
- Implementar relatórios de voos
- Adicionar integração com pagamento
- Melhorar interface com temas
- Adicionar busca avançada de voos

## Licença

MIT License - Livre para usar e modificar.

## Suporte

Para dúvidas ou problemas, consulte a documentação do Qt:
https://doc.qt.io/qt-6/
