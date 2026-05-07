/*
Design reminder — Precision Brutalism Aeronáutico:
Esta página combina experiência do cliente e administração em um cockpit operacional.
Usar blocos retangulares, linhas diagonais, dados visíveis, contraste grafite/gelo e amarelo de pista.
Pergunta guia: esta escolha reforça ou dilui a sensação de controle aeroportuário?
*/

import { useMemo, useState } from "react";
import {
  Activity,
  Armchair,
  CalendarDays,
  Check,
  Clock3,
  Gauge,
  LayoutDashboard,
  MapPinned,
  Plane,
  Plus,
  Radar,
  Route,
  ShieldCheck,
  Users,
} from "lucide-react";
import { toast } from "sonner";

const heroImage =
  "https://d2xsxph8kpxj0f.cloudfront.net/310519663636496724/nJWQRuG3ncnUWzCxb68dUk/aviation-command-hero-h6b5VqBvwok2JAcj5YTn8H.webp";
const seatMapImage =
  "https://d2xsxph8kpxj0f.cloudfront.net/310519663636496724/nJWQRuG3ncnUWzCxb68dUk/seat-map-technical-TaGkmdsgmrZuFErXyzncUz.webp";
const routeImage =
  "https://d2xsxph8kpxj0f.cloudfront.net/310519663636496724/nJWQRuG3ncnUWzCxb68dUk/route-network-radar-nHfJJ5eT4cL2Zzjwd4HTq6.webp";
const adminImage =
  "https://d2xsxph8kpxj0f.cloudfront.net/310519663636496724/nJWQRuG3ncnUWzCxb68dUk/admin-panel-cockpit-JphjsdPJyja9UMjDD6tZTd.webp";

type Flight = {
  id: string;
  origin: string;
  destination: string;
  date: string;
  time: string;
  aircraft: string;
  gate: string;
  price: number;
  available: number;
  status: "Aberto" | "Embarque" | "Fechado";
};

const initialFlights: Flight[] = [
  {
    id: "AV-1047",
    origin: "São Paulo",
    destination: "Rio de Janeiro",
    date: "2026-05-18",
    time: "08:35",
    aircraft: "A320 Neo",
    gate: "B12",
    price: 428,
    available: 42,
    status: "Aberto",
  },
  {
    id: "AV-2190",
    origin: "Brasília",
    destination: "Recife",
    date: "2026-05-18",
    time: "11:10",
    aircraft: "B737-800",
    gate: "C04",
    price: 612,
    available: 28,
    status: "Embarque",
  },
  {
    id: "AV-3382",
    origin: "Curitiba",
    destination: "Salvador",
    date: "2026-05-19",
    time: "17:45",
    aircraft: "E195-E2",
    gate: "A07",
    price: 735,
    available: 36,
    status: "Aberto",
  },
];

const rows = ["A", "B", "C", "D", "E", "F"];
const reservedSeats = new Set(["A2", "B4", "C7", "D3", "E8", "F5", "A9", "D10"]);

function formatCurrency(value: number) {
  return new Intl.NumberFormat("pt-BR", {
    style: "currency",
    currency: "BRL",
  }).format(value);
}

function StatCard({ icon: Icon, label, value, detail }: { icon: typeof Plane; label: string; value: string; detail: string }) {
  return (
    <div className="command-card group relative overflow-hidden p-5 transition duration-300 hover:-translate-y-1 hover:border-runway/70">
      <div className="absolute inset-x-0 top-0 h-px bg-gradient-to-r from-transparent via-runway/70 to-transparent" />
      <div className="flex items-start justify-between gap-4">
        <div>
          <p className="technical-label">{label}</p>
          <strong className="mt-3 block font-display text-3xl tracking-tight text-ice">{value}</strong>
          <span className="mt-2 block text-sm text-steel">{detail}</span>
        </div>
        <span className="grid h-12 w-12 place-items-center border border-ice/10 bg-ice/5 text-runway transition group-hover:bg-runway group-hover:text-graphite">
          <Icon size={22} />
        </span>
      </div>
    </div>
  );
}

function FlightCard({ flight, active, onSelect }: { flight: Flight; active: boolean; onSelect: () => void }) {
  return (
    <button
      onClick={onSelect}
      className={`command-card w-full p-5 text-left transition duration-300 ${
        active ? "border-runway bg-runway/10 shadow-[0_0_0_1px_rgba(245,178,42,.45)]" : "hover:border-ice/25"
      }`}
    >
      <div className="flex flex-wrap items-start justify-between gap-4">
        <div>
          <span className="technical-label">{flight.id}</span>
          <h3 className="mt-2 font-display text-2xl text-ice">
            {flight.origin} <span className="text-runway">→</span> {flight.destination}
          </h3>
        </div>
        <span className={`status-chip ${flight.status === "Aberto" ? "status-open" : "status-boarding"}`}>{flight.status}</span>
      </div>
      <div className="mt-5 grid grid-cols-2 gap-3 text-sm text-fog sm:grid-cols-4">
        <span className="flex items-center gap-2"><CalendarDays size={16} /> {flight.date}</span>
        <span className="flex items-center gap-2"><Clock3 size={16} /> {flight.time}</span>
        <span className="flex items-center gap-2"><Plane size={16} /> {flight.aircraft}</span>
        <span className="flex items-center gap-2"><Armchair size={16} /> {flight.available} lugares</span>
      </div>
      <div className="mt-5 flex items-center justify-between border-t border-ice/10 pt-4">
        <span className="text-sm text-steel">Portão {flight.gate}</span>
        <strong className="font-mono text-lg text-ice">{formatCurrency(flight.price)}</strong>
      </div>
    </button>
  );
}

function SeatButton({ code, selected, reserved, onClick }: { code: string; selected: boolean; reserved: boolean; onClick: () => void }) {
  return (
    <button
      disabled={reserved}
      onClick={onClick}
      className={`seat ${reserved ? "seat-reserved" : selected ? "seat-selected" : "seat-free"}`}
      aria-label={`Assento ${code}`}
      title={reserved ? `${code} reservado` : `${code} disponível`}
    >
      {code}
    </button>
  );
}

export default function Home() {
  const [mode, setMode] = useState<"cliente" | "admin">("cliente");
  const [flights, setFlights] = useState<Flight[]>(initialFlights);
  const [selectedFlightId, setSelectedFlightId] = useState(initialFlights[0].id);
  const [selectedSeats, setSelectedSeats] = useState<string[]>(["C4"]);
  const [form, setForm] = useState({ origin: "Belo Horizonte", destination: "Fortaleza", date: "2026-05-20", time: "14:25", price: "540" });

  const selectedFlight = useMemo(
    () => flights.find((flight) => flight.id === selectedFlightId) ?? flights[0],
    [flights, selectedFlightId],
  );

  const total = selectedSeats.length * selectedFlight.price;

  function toggleSeat(code: string) {
    if (reservedSeats.has(code)) return;
    setSelectedSeats((current) => (current.includes(code) ? current.filter((seat) => seat !== code) : [...current, code]));
  }

  function confirmReservation() {
    if (!selectedSeats.length) {
      toast.error("Escolha pelo menos um assento antes de confirmar.");
      return;
    }
    toast.success(`Reserva criada para ${selectedFlight.id}: ${selectedSeats.join(", ")}.`);
  }

  function addFlight() {
    const nextFlight: Flight = {
      id: `AV-${Math.floor(4000 + Math.random() * 4999)}`,
      origin: form.origin,
      destination: form.destination,
      date: form.date,
      time: form.time,
      aircraft: "A321 LR",
      gate: "D02",
      price: Number(form.price || 0),
      available: 54,
      status: "Aberto",
    };
    setFlights((current) => [nextFlight, ...current]);
    setSelectedFlightId(nextFlight.id);
    toast.success(`Voo ${nextFlight.id} cadastrado no painel.`);
  }

  return (
    <div className="min-h-screen overflow-hidden bg-graphite text-ice">
      <div className="fixed inset-0 -z-10 bg-[radial-gradient(circle_at_18%_20%,rgba(245,178,42,.16),transparent_28%),radial-gradient(circle_at_80%_8%,rgba(73,117,150,.22),transparent_34%),linear-gradient(135deg,#07090b_0%,#111820_45%,#07090b_100%)]" />
      <div className="fixed inset-0 -z-10 opacity-[0.13] [background-image:linear-gradient(rgba(255,255,255,.12)_1px,transparent_1px),linear-gradient(90deg,rgba(255,255,255,.12)_1px,transparent_1px)] [background-size:48px_48px]" />

      <header className="relative border-b border-ice/10 bg-graphite/80 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-5 py-5 lg:px-8">
          <button onClick={() => setMode("cliente")} className="flex items-center gap-3 text-left">
            <span className="grid h-11 w-11 place-items-center border border-runway bg-runway text-graphite shadow-[0_0_30px_rgba(245,178,42,.25)]">
              <Plane size={24} />
            </span>
            <span>
              <span className="block font-display text-xl uppercase tracking-[.08em]">AeroGestão</span>
              <span className="block font-mono text-xs uppercase tracking-[.24em] text-steel">Reserva · Assentos · Operação</span>
            </span>
          </button>
          <nav className="hidden items-center gap-2 md:flex">
            <button className={`nav-tab ${mode === "cliente" ? "nav-tab-active" : ""}`} onClick={() => setMode("cliente")}>Área do cliente</button>
            <button className={`nav-tab ${mode === "admin" ? "nav-tab-active" : ""}`} onClick={() => setMode("admin")}>Administração</button>
          </nav>
          <button onClick={() => setMode(mode === "cliente" ? "admin" : "cliente")} className="control-button">
            <LayoutDashboard size={18} /> Alternar painel
          </button>
        </div>
      </header>

      <main>
        <section className="relative isolate min-h-[720px] overflow-hidden border-b border-ice/10">
          <img src={heroImage} alt="Painel técnico de gestão de aviação" className="absolute inset-0 -z-10 h-full w-full object-cover opacity-75" />
          <div className="absolute inset-0 -z-10 bg-gradient-to-r from-graphite via-graphite/82 to-graphite/20" />
          <div className="mx-auto grid max-w-7xl gap-10 px-5 py-20 lg:grid-cols-[1.02fr_.98fr] lg:px-8 lg:py-28">
            <div className="max-w-2xl">
              <p className="technical-label text-runway">Sistema de gestão de aviação</p>
              <h1 className="mt-5 font-display text-5xl uppercase leading-[.95] tracking-[-.04em] text-ice md:text-7xl">
                Controle voos, assentos e reservas em um cockpit digital.
              </h1>
              <p className="mt-7 max-w-xl text-lg leading-8 text-fog">
                Protótipo navegável com uma jornada para o passageiro escolher voo e assento, e um painel administrativo para cadastrar voos, acompanhar ocupação e preparar a futura integração com Python e C.
              </p>
              <div className="mt-9 flex flex-col gap-3 sm:flex-row">
                <button onClick={() => setMode("cliente")} className="primary-action"><Plane size={18} /> Reservar assento</button>
                <button onClick={() => setMode("admin")} className="secondary-action"><ShieldCheck size={18} /> Abrir admin</button>
              </div>
            </div>

            <div className="command-card relative self-end overflow-hidden p-5">
              <img src={routeImage} alt="Rede técnica de rotas aéreas" className="h-64 w-full border border-ice/10 object-cover opacity-90" />
              <div className="mt-5 grid grid-cols-3 gap-3">
                <StatCard icon={Plane} label="Voos ativos" value={String(flights.length)} detail="rotas cadastradas" />
                <StatCard icon={Armchair} label="Assentos" value="106" detail="disponíveis" />
                <StatCard icon={Activity} label="Ocupação" value="73%" detail="média do dia" />
              </div>
            </div>
          </div>
        </section>

        {mode === "cliente" ? (
          <section className="mx-auto grid max-w-7xl gap-8 px-5 py-16 lg:grid-cols-[.9fr_1.1fr] lg:px-8">
            <aside className="space-y-4">
              <div>
                <p className="technical-label text-runway">Etapa 01 · escolha de voo</p>
                <h2 className="mt-3 font-display text-4xl uppercase tracking-tight">Rotas disponíveis</h2>
              </div>
              {flights.map((flight) => (
                <FlightCard key={flight.id} flight={flight} active={flight.id === selectedFlightId} onSelect={() => setSelectedFlightId(flight.id)} />
              ))}
            </aside>

            <section className="space-y-6">
              <div className="command-card overflow-hidden">
                <div className="grid gap-6 p-5 lg:grid-cols-[1fr_.9fr]">
                  <div>
                    <p className="technical-label text-runway">Etapa 02 · mapa de assentos</p>
                    <h2 className="mt-3 font-display text-4xl uppercase tracking-tight">Selecione seu lugar</h2>
                    <p className="mt-4 text-fog">Os assentos livres podem ser alternados abaixo. Em uma implementação final, o Python chamaria o módulo em C para validar disponibilidade antes de gravar a reserva.</p>
                    <div className="mt-6 grid grid-cols-[repeat(10,minmax(0,1fr))] gap-2">
                      {rows.flatMap((row) =>
                        Array.from({ length: 10 }, (_, index) => {
                          const code = `${row}${index + 1}`;
                          return (
                            <SeatButton
                              key={code}
                              code={code}
                              reserved={reservedSeats.has(code)}
                              selected={selectedSeats.includes(code)}
                              onClick={() => toggleSeat(code)}
                            />
                          );
                        }),
                      )}
                    </div>
                  </div>
                  <img src={seatMapImage} alt="Mapa técnico de assentos" className="h-full min-h-80 w-full border border-ice/10 object-cover" />
                </div>
              </div>

              <div className="command-card p-6">
                <div className="flex flex-wrap items-start justify-between gap-6">
                  <div>
                    <p className="technical-label text-runway">Resumo de reserva</p>
                    <h3 className="mt-2 font-display text-3xl uppercase">{selectedFlight.id}</h3>
                    <p className="mt-2 text-fog">{selectedFlight.origin} para {selectedFlight.destination} · {selectedFlight.date} às {selectedFlight.time}</p>
                  </div>
                  <div className="text-right">
                    <span className="block font-mono text-sm uppercase tracking-[.2em] text-steel">Total</span>
                    <strong className="font-display text-4xl text-runway">{formatCurrency(total)}</strong>
                  </div>
                </div>
                <div className="mt-6 flex flex-wrap items-center justify-between gap-4 border-t border-ice/10 pt-5">
                  <span className="text-sm text-fog">Assentos: <strong className="text-ice">{selectedSeats.length ? selectedSeats.join(", ") : "nenhum selecionado"}</strong></span>
                  <button onClick={confirmReservation} className="primary-action"><Check size={18} /> Confirmar reserva</button>
                </div>
              </div>
            </section>
          </section>
        ) : (
          <section className="mx-auto grid max-w-7xl gap-8 px-5 py-16 lg:grid-cols-[.95fr_1.05fr] lg:px-8">
            <aside className="space-y-6">
              <div className="command-card overflow-hidden">
                <img src={adminImage} alt="Painel administrativo de aviação" className="h-72 w-full object-cover opacity-90" />
                <div className="p-6">
                  <p className="technical-label text-runway">Painel administrativo</p>
                  <h2 className="mt-3 font-display text-4xl uppercase tracking-tight">Cadastre voos e monitore assentos</h2>
                  <p className="mt-4 text-fog">Este painel simula as rotinas que futuramente serão persistidas no backend Python, com validações críticas delegadas ao módulo C.</p>
                </div>
              </div>

              <div className="command-card p-6">
                <p className="technical-label text-runway">Novo voo</p>
                <div className="mt-5 grid gap-4 sm:grid-cols-2">
                  <input className="field" value={form.origin} onChange={(event) => setForm({ ...form, origin: event.target.value })} placeholder="Origem" />
                  <input className="field" value={form.destination} onChange={(event) => setForm({ ...form, destination: event.target.value })} placeholder="Destino" />
                  <input className="field" type="date" value={form.date} onChange={(event) => setForm({ ...form, date: event.target.value })} />
                  <input className="field" type="time" value={form.time} onChange={(event) => setForm({ ...form, time: event.target.value })} />
                  <input className="field sm:col-span-2" value={form.price} onChange={(event) => setForm({ ...form, price: event.target.value })} placeholder="Preço" />
                </div>
                <button onClick={addFlight} className="primary-action mt-5 w-full justify-center"><Plus size={18} /> Cadastrar voo</button>
              </div>
            </aside>

            <section className="space-y-6">
              <div className="grid gap-4 sm:grid-cols-2 xl:grid-cols-4">
                <StatCard icon={Route} label="Rotas" value={String(flights.length)} detail="em operação" />
                <StatCard icon={Users} label="Reservas" value="184" detail="simuladas" />
                <StatCard icon={Gauge} label="Lotação" value="73%" detail="média" />
                <StatCard icon={Radar} label="Alertas" value="02" detail="assentos bloqueados" />
              </div>

              <div className="command-card p-6">
                <div className="flex flex-wrap items-center justify-between gap-4">
                  <div>
                    <p className="technical-label text-runway">Malha cadastrada</p>
                    <h3 className="mt-2 font-display text-3xl uppercase">Voos do dia</h3>
                  </div>
                  <span className="status-chip status-open">Operacional</span>
                </div>
                <div className="mt-6 overflow-x-auto">
                  <table className="w-full min-w-[720px] border-collapse text-left text-sm">
                    <thead>
                      <tr className="border-b border-ice/10 text-steel">
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Código</th>
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Rota</th>
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Horário</th>
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Aeronave</th>
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Assentos</th>
                        <th className="py-3 font-mono uppercase tracking-[.16em]">Status</th>
                      </tr>
                    </thead>
                    <tbody>
                      {flights.map((flight) => (
                        <tr key={flight.id} className="border-b border-ice/5 text-fog transition hover:bg-ice/[.04]">
                          <td className="py-4 font-mono text-runway">{flight.id}</td>
                          <td className="py-4 text-ice">{flight.origin} → {flight.destination}</td>
                          <td className="py-4">{flight.date} · {flight.time}</td>
                          <td className="py-4">{flight.aircraft}</td>
                          <td className="py-4">{flight.available}</td>
                          <td className="py-4"><span className={`status-chip ${flight.status === "Aberto" ? "status-open" : "status-boarding"}`}>{flight.status}</span></td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              </div>

              <div className="grid gap-6 lg:grid-cols-2">
                <div className="command-card p-6">
                  <MapPinned className="text-runway" />
                  <h3 className="mt-4 font-display text-2xl uppercase">Fluxo previsto</h3>
                  <p className="mt-3 text-fog">Cliente escolhe voo, seleciona assento, Python valida com C, SQLite grava reserva e admin acompanha a ocupação.</p>
                </div>
                <div className="command-card p-6">
                  <ShieldCheck className="text-runway" />
                  <h3 className="mt-4 font-display text-2xl uppercase">Regras críticas</h3>
                  <p className="mt-3 text-fog">O módulo em C deve impedir assento duplicado, validar limites da aeronave e responder rapidamente ao backend Python.</p>
                </div>
              </div>
            </section>
          </section>
        )}
      </main>
    </div>
  );
}
