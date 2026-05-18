#!/usr/bin/env python3
"""
Servidor HTTP simples para servir o frontend
Roda na porta 3000 e serve o index.html
O servidor C deve estar rodando na porta 8080
"""

import http.server
import socketserver
import os
from pathlib import Path

PORT = 3000
FRONTEND_DIR = Path(__file__).parent / "frontend"

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(FRONTEND_DIR), **kwargs)
    
    def end_headers(self):
        # Adicionar headers para evitar cache
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

if __name__ == "__main__":
    os.chdir(FRONTEND_DIR)
    with socketserver.TCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
        print(f"✅ Servidor HTTP rodando em http://localhost:{PORT}")
        print(f"📁 Servindo arquivos de: {FRONTEND_DIR}")
        print(f"⚠️  Certifique-se de que o servidor C está rodando em http://localhost:8080")
        print(f"🛑 Pressione Ctrl+C para parar")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n✋ Servidor parado")
