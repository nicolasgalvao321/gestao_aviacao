const API_URL = 'http://localhost:8080/api';

class AeroApp {
    constructor() {
        this.currentPage = 'menu';
        this.selectedFlight = null;
        this.selectedSeats = [];
        this.flights = [];
        this.reservations = [];
        this.init();
    }

    init() {
        this.render();
        this.attachEventListeners();
        this.loadFlights();
    }

    render() {
        const app = document.getElementById('app');
        
        if (this.currentPage === 'menu') {
            app.innerHTML = this.renderMenu();
        } else if (this.currentPage === 'client') {
            app.innerHTML = this.renderClientPage();
        } else if (this.currentPage === 'admin-login') {
            app.innerHTML = this.renderAdminLogin();
        } else if (this.currentPage === 'admin') {
            app.innerHTML = this.renderAdminPage();
        }
        
        this.attachEventListeners();
    }

    renderMenu() {
        return `
            <div class="container">
                <div class="header">
                    <h1>✈️ AeroGestão</h1>
                    <p>Sistema de Gestão de Aviação</p>
                </div>
                <div class="menu" style="flex-direction: column; gap: 20px;">
                    <button class="btn btn-primary" id="btn-client" style="width: 100%; padding: 20px; font-size: 18px;">
                        👤 Área do Cliente
                    </button>
                    <button class="btn btn-info" id="btn-admin" style="width: 100%; padding: 20px; font-size: 18px;">
                        🔐 Área Administrativa
                    </button>
                </div>
            </div>
        `;
    }

    renderClientPage() {
        return `
            <div class="container">
                <div class="header">
                    <h1>Reserva de Voos</h1>
                    <p>Escolha um voo e reserve seu assento</p>
                </div>
                <button class="btn btn-secondary" id="btn-back">← Voltar</button>
                
                <h2 style="margin-top: 30px; margin-bottom: 20px;">Voos Disponíveis</h2>
                <div id="flights-list"></div>
                
                <div id="reservation-form" style="display: none; margin-top: 30px; border-top: 2px solid #ddd; padding-top: 30px;">
                    <h2>Reservar Assento</h2>
                    <div class="form-group">
                        <label>Voo: <span id="selected-flight-info"></span></label>
                    </div>
                    
                    <h3>Mapa de Assentos</h3>
                    <div id="seat-map" class="seat-map"></div>
                    
                    <div class="form-group">
                        <label>Assento Selecionado: <strong id="selected-seat">Nenhum</strong></label>
                    </div>
                    
                    <div class="form-row">
                        <div class="form-group">
                            <label>Nome Completo:</label>
                            <input type="text" id="passenger-name" placeholder="Seu nome">
                        </div>
                        <div class="form-group">
                            <label>CPF:</label>
                            <input type="text" id="passenger-cpf" placeholder="000.000.000-00">
                        </div>
                    </div>
                    
                    <button class="btn btn-primary" id="btn-confirm-reservation" style="width: 100%; padding: 15px;">
                        Confirmar Reserva
                    </button>
                </div>
            </div>
        `;
    }

    renderAdminLogin() {
        return `
            <div class="login-container">
                <h2>Acesso Administrativo</h2>
                <div class="form-group">
                    <label>Senha:</label>
                    <input type="password" id="admin-password" placeholder="Digite a senha">
                </div>
                <button class="btn btn-primary" id="btn-login" style="width: 100%; padding: 12px; margin-bottom: 10px;">
                    Entrar
                </button>
                <button class="btn btn-secondary" id="btn-back-login" style="width: 100%; padding: 12px;">
                    Voltar
                </button>
            </div>
        `;
    }

    renderAdminPage() {
        return `
            <div class="container">
                <div class="header">
                    <h1>Painel Administrativo</h1>
                    <p>Gerenciar voos e reservas</p>
                </div>
                <button class="btn btn-secondary" id="btn-logout">Logout</button>
                
                <h2 style="margin-top: 30px; margin-bottom: 20px;">Cadastrar Novo Voo</h2>
                <div class="form-row">
                    <div class="form-group">
                        <label>Código do Voo:</label>
                        <input type="text" id="flight-code" placeholder="Ex: AV-1000">
                    </div>
                    <div class="form-group">
                        <label>Origem:</label>
                        <input type="text" id="flight-origin" placeholder="Ex: São Paulo">
                    </div>
                </div>
                <div class="form-row">
                    <div class="form-group">
                        <label>Destino:</label>
                        <input type="text" id="flight-destination" placeholder="Ex: Rio de Janeiro">
                    </div>
                    <div class="form-group">
                        <label>Data:</label>
                        <input type="date" id="flight-date">
                    </div>
                </div>
                <div class="form-row">
                    <div class="form-group">
                        <label>Hora:</label>
                        <input type="time" id="flight-time">
                    </div>
                    <div class="form-group">
                        <label>Preço:</label>
                        <input type="number" id="flight-price" placeholder="0.00" step="0.01">
                    </div>
                </div>
                <button class="btn btn-primary" id="btn-add-flight" style="width: 100%; padding: 12px;">
                    Adicionar Voo
                </button>
                
                <h2 style="margin-top: 30px; margin-bottom: 20px;">Voos Cadastrados</h2>
                <table class="table">
                    <thead>
                        <tr>
                            <th>Código</th>
                            <th>Origem</th>
                            <th>Destino</th>
                            <th>Data</th>
                            <th>Hora</th>
                            <th>Preço</th>
                        </tr>
                    </thead>
                    <tbody id="admin-flights-list"></tbody>
                </table>
                
                <h2 style="margin-top: 30px; margin-bottom: 20px;">Reservas</h2>
                <table class="table">
                    <thead>
                        <tr>
                            <th>Voo</th>
                            <th>Assento</th>
                            <th>Passageiro</th>
                            <th>CPF</th>
                        </tr>
                    </thead>
                    <tbody id="admin-reservations-list"></tbody>
                </table>
            </div>
        `;
    }

    attachEventListeners() {
        // Menu
        const btnClient = document.getElementById('btn-client');
        const btnAdmin = document.getElementById('btn-admin');
        if (btnClient) btnClient.addEventListener('click', () => this.goToClient());
        if (btnAdmin) btnAdmin.addEventListener('click', () => this.goToAdminLogin());

        // Client Page
        const btnBack = document.getElementById('btn-back');
        if (btnBack) btnBack.addEventListener('click', () => this.goToMenu());

        // Admin Login
        const btnLogin = document.getElementById('btn-login');
        const btnBackLogin = document.getElementById('btn-back-login');
        const adminPassword = document.getElementById('admin-password');
        if (btnLogin) btnLogin.addEventListener('click', () => this.loginAdmin());
        if (btnBackLogin) btnBackLogin.addEventListener('click', () => this.goToMenu());
        if (adminPassword) adminPassword.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.loginAdmin();
        });

        // Admin Page
        const btnLogout = document.getElementById('btn-logout');
        const btnAddFlight = document.getElementById('btn-add-flight');
        if (btnLogout) btnLogout.addEventListener('click', () => this.goToMenu());
        if (btnAddFlight) btnAddFlight.addEventListener('click', () => this.addFlight());

        // Reservation
        const btnConfirmReservation = document.getElementById('btn-confirm-reservation');
        if (btnConfirmReservation) btnConfirmReservation.addEventListener('click', () => this.confirmReservation());
    }

    loadFlights() {
        fetch(`${API_URL}/flights`)
            .then(res => res.json())
            .then(data => {
                this.flights = data;
                if (this.currentPage === 'client') this.renderFlightsList();
                if (this.currentPage === 'admin') this.renderAdminFlightsList();
            })
            .catch(err => console.error('Erro ao carregar voos:', err));
    }

    renderFlightsList() {
        const list = document.getElementById('flights-list');
        if (!list) return;

        list.innerHTML = this.flights.map(flight => `
            <div style="border: 1px solid #ddd; padding: 15px; margin-bottom: 10px; border-radius: 5px; cursor: pointer;" 
                 onclick="app.selectFlight(${flight.id})">
                <strong>${flight.code}</strong> - ${flight.origin} → ${flight.destination}
                <br>
                <small>${flight.date} às ${flight.time} - R$ ${flight.price.toFixed(2)}</small>
            </div>
        `).join('');
    }

    renderAdminFlightsList() {
        const list = document.getElementById('admin-flights-list');
        if (!list) return;

        list.innerHTML = this.flights.map(flight => `
            <tr>
                <td>${flight.code}</td>
                <td>${flight.origin}</td>
                <td>${flight.destination}</td>
                <td>${flight.date}</td>
                <td>${flight.time}</td>
                <td>R$ ${flight.price.toFixed(2)}</td>
            </tr>
        `).join('');
    }

    selectFlight(flightId) {
        this.selectedFlight = this.flights.find(f => f.id === flightId);
        this.selectedSeats = [];
        this.renderSeatMap();
        
        const form = document.getElementById('reservation-form');
        const info = document.getElementById('selected-flight-info');
        if (form && info) {
            info.textContent = `${this.selectedFlight.code} - ${this.selectedFlight.origin} → ${this.selectedFlight.destination}`;
            form.style.display = 'block';
            form.scrollIntoView({ behavior: 'smooth' });
        }
    }

    renderSeatMap() {
        const seatMap = document.getElementById('seat-map');
        if (!seatMap) return;

        const seats = [];
        for (let i = 1; i <= 36; i++) {
            const row = String.fromCharCode(65 + Math.floor((i - 1) / 6));
            const col = ((i - 1) % 6) + 1;
            const seatCode = `${row}${col}`;
            
            const isReserved = this.selectedFlight.reserved_seats && 
                             this.selectedFlight.reserved_seats.includes(seatCode);
            const isSelected = this.selectedSeats.includes(seatCode);
            
            const seatClass = isReserved ? 'reserved' : (isSelected ? 'selected' : 'available');
            
            seats.push(`
                <div class="seat ${seatClass}" 
                     onclick="app.toggleSeat('${seatCode}')"
                     ${isReserved ? 'style="cursor: not-allowed;"' : ''}>
                    ${seatCode}
                </div>
            `);
        }
        
        seatMap.innerHTML = seats.join('');
    }

    toggleSeat(seatCode) {
        const idx = this.selectedSeats.indexOf(seatCode);
        if (idx > -1) {
            this.selectedSeats.splice(idx, 1);
        } else {
            this.selectedSeats = [seatCode]; // Apenas um assento por vez
        }
        
        const selected = document.getElementById('selected-seat');
        if (selected) {
            selected.textContent = this.selectedSeats.length > 0 ? this.selectedSeats[0] : 'Nenhum';
        }
        
        this.renderSeatMap();
    }

    confirmReservation() {
        const name = document.getElementById('passenger-name').value.trim();
        const cpf = document.getElementById('passenger-cpf').value.trim();
        
        if (!name || !cpf || this.selectedSeats.length === 0) {
            alert('Preencha todos os campos e selecione um assento!');
            return;
        }

        const reservation = {
            flight_id: this.selectedFlight.id,
            seat_code: this.selectedSeats[0],
            passenger_name: name,
            passenger_document: cpf
        };

        fetch(`${API_URL}/reservations`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(reservation)
        })
        .then(res => res.json())
        .then(data => {
            alert('Reserva confirmada com sucesso!');
            this.goToMenu();
        })
        .catch(err => alert('Erro ao confirmar reserva: ' + err));
    }

    addFlight() {
        const code = document.getElementById('flight-code').value.trim();
        const origin = document.getElementById('flight-origin').value.trim();
        const destination = document.getElementById('flight-destination').value.trim();
        const date = document.getElementById('flight-date').value;
        const time = document.getElementById('flight-time').value;
        const price = parseFloat(document.getElementById('flight-price').value);

        if (!code || !origin || !destination || !date || !time || !price) {
            alert('Preencha todos os campos!');
            return;
        }

        const flight = { code, origin, destination, date, time, price };

        fetch(`${API_URL}/flights`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(flight)
        })
        .then(res => res.json())
        .then(data => {
            alert('Voo adicionado com sucesso!');
            document.getElementById('flight-code').value = '';
            document.getElementById('flight-origin').value = '';
            document.getElementById('flight-destination').value = '';
            document.getElementById('flight-date').value = '';
            document.getElementById('flight-time').value = '';
            document.getElementById('flight-price').value = '';
            this.loadFlights();
        })
        .catch(err => alert('Erro ao adicionar voo: ' + err));
    }

    loginAdmin() {
        const password = document.getElementById('admin-password').value;
        if (password === 'admin123') {
            this.currentPage = 'admin';
            this.loadFlights();
            this.render();
        } else {
            alert('Senha incorreta!');
        }
    }

    goToMenu() {
        this.currentPage = 'menu';
        this.selectedFlight = null;
        this.selectedSeats = [];
        this.render();
    }

    goToClient() {
        this.currentPage = 'client';
        this.loadFlights();
        this.render();
    }

    goToAdminLogin() {
        this.currentPage = 'admin-login';
        this.render();
    }
}

// Iniciar aplicação quando o DOM estiver pronto
document.addEventListener('DOMContentLoaded', () => {
    window.app = new AeroApp();
});
