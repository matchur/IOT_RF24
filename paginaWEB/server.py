import serial
import threading
from flask import Flask, jsonify, render_template_string

app = Flask(__name__)
umidade = 0

# Configura a porta serial
try:
    ser = serial.Serial('COM5', 115200)
    print("Porta serial aberta com sucesso.")
except serial.SerialException as e:
    print(f"Erro ao abrir a porta serial: {e}")
    ser = None

def ler_serial():
    global umidade 
    umidade = 0 
    if ser:
        while True:
            if ser.in_waiting > 0:
                leitura = ser.readline().decode('utf-8').strip()
                print(f"Leitura da porta serial: {leitura}")
                umidade = leitura

                print(f"Umidade atualizada: {umidade}")

serial_thread = threading.Thread(target=ler_serial, daemon=True)
serial_thread.start()

@app.route('/')
def index():
    return render_template_string('''
        <html>
            <head>
                <title>Monitor de Umidade</title>
                <script>
                    function atualizarUmidade() {
                        fetch('/umidade')
                            .then(response => response.json())
                            .then(data => {
                             document.getElementById('umidade').innerText = `${data.umidade}%`;
                            
                            })
                            .catch(error => console.error('Erro ao buscar a umidade:', error));
                    }
                    setInterval(atualizarUmidade, 2000); 
                </script>
            </head>
            <body>
                <h1>Umidade Atual</h1>
                <p id="umidade">Carregando...</p>
               
            </body>
        </html>
    ''')

@app.route('/umidade')
def umidade_api():
    return jsonify({'umidade': umidade})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
