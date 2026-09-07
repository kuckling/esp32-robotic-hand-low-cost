#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Definición de credenciales de la red Wi-Fi
const char* ssid = "ManoRobotica_ESP32";
const char* password = "manorobotica@2026";

// Creación del servidor web en el puerto 80
WebServer server(80);

// Declaración de los 5 Servomotores
Servo servoPulgar;
Servo servoIndice;
Servo servoMedio;
Servo servoAnular;
Servo servoMenique;

// Pines GPIO para cada servo
const int pinPulgar  = 13;
const int pinIndice  = 12;
const int pinMedio   = 14;
const int pinAnular  = 27;
const int pinMenique = 26;

// Variables para almacenar los ángulos de cada dedo (0 a 180)
int anguloPulgar  = 0;
int anguloIndice  = 0;
int anguloMedio   = 0;
int anguloAnular  = 0;
int anguloMenique = 0;

// Código HTML + CSS + JS de la Interfaz Web
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mano Robótica ESP32</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #1e1e2f; color: #ffffff; text-align: center; margin: 0; padding: 20px; }
        h1 { color: #00d2ff; margin-bottom: 20px; }
        .card { background-color: #2d2d44; max-width: 500px; margin: 0 auto; padding: 25px; border-radius: 15px; box-shadow: 0 10px 20px rgba(0,0,0,0.5); }
        .control-group { margin: 15px 0; text-align: left; }
        label { display: block; font-weight: bold; margin-bottom: 5px; color: #a0a0c0; }
        input[type=range] { width: 100%; height: 8px; border-radius: 5px; background: #4e4e6a; outline: none; -webkit-appearance: none; }
        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 22px; height: 22px; border-radius: 50%; background: #00d2ff; cursor: pointer; }
        .val { float: right; color: #00d2ff; }
        .grid-buttons { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 25px; }
        button { background-color: #3b3b58; border: 2px solid #00d2ff; color: white; padding: 12px; font-size: 15px; font-weight: bold; border-radius: 8px; cursor: pointer; transition: 0.3s; }
        button:hover { background-color: #00d2ff; color: #1e1e2f; }
        .full-btn { grid-column: span 2; background-color: #ff4757; border-color: #ff4757; }
        .full-btn:hover { background-color: #ff6b81; }
    </style>
</head>
<body>
    <div class="card">
        <h1>Mano Robótica 🦾</h1>
        
        <!-- Control Deslizante Individual para cada Dedo -->
        <div class="control-group">
            <label>👍 Pulgar <span id="vPulgar" class="val">0°</span></label>
            <input type="range" id="pulgar" min="0" max="180" value="0" oninput="moverDedo('pulgar', this.value)">
        </div>
        <div class="control-group">
            <label>☝️ Índice <span id="vIndice" class="val">0°</span></label>
            <input type="range" id="indice" min="0" max="180" value="0" oninput="moverDedo('indice', this.value)">
        </div>
        <div class="control-group">
            <label>🖕 Medio <span id="vMedio" class="val">0°</span></label>
            <input type="range" id="medio" min="0" max="180" value="0" oninput="moverDedo('medio', this.value)">
        </div>
        <div class="control-group">
            <label>💍 Anular <span id="vAnular" class="val">0°</span></label>
            <input type="range" id="anular" min="0" max="180" value="0" oninput="moverDedo('anular', this.value)">
        </div>
        <div class="control-group">
            <label>🤙 Meñique <span id="vMenique" class="val">0°</span></label>
            <input type="range" id="menique" min="0" max="180" value="0" oninput="moverDedo('menique', this.value)">
        </div>

        <hr style="border-color: #4e4e6a; margin: 20px 0;">

        <!-- Botones de Posturas Predefinidas -->
        <h3>Posturas Rápidas</h3>
        <div class="grid-buttons">
            <button onclick="ejecutarAccion('rock')">🤘 Seña de Rock</button>
            <button onclick="ejecutarAccion('pulgar_arriba')">👍 Pulgar Arriba</button>
            <button onclick="ejecutarAccion('abrir')">✋ Abrir Mano</button>
            <button onclick="ejecutarAccion('cerrar')">✊ Cerrar Mano</button>
            <button class="full-btn" onclick="ejecutarAccion('secuencia')">🔄 Abrir / Cerrar (5 seg)</button>
        </div>
    </div>

    <script>
        function moverDedo(dedo, valor) {
            document.getElementById('v' + dedo.charAt(0).toUpperCase() + dedo.slice(1)).innerText = valor + '°';
            fetch('/setDedo?dedo=' + dedo + '&val=' + valor);
        }

        function ejecutarAccion(accion) {
            fetch('/accion?tipo=' + accion)
            .then(response => response.json())
            .then(data => {
                // Actualizar sliders en pantalla si el servidor devuelve posiciones
                if(data.p) document.getElementById('pulgar').value = data.p;
                if(data.i) document.getElementById('indice').value = data.i;
                if(data.m) document.getElementById('medio').value = data.m;
                if(data.a) document.getElementById('anular').value = data.a;
                if(data.q) document.getElementById('menique').value = data.q;
                
                if(data.p) document.getElementById('vPulgar').innerText = data.p + '°';
                if(data.i) document.getElementById('vIndice').innerText = data.i + '°';
                if(data.m) document.getElementById('vMedio').innerText = data.m + '°';
                if(data.a) document.getElementById('vAnular').innerText = data.a + '°';
                if(data.q) document.getElementById('vMenique').innerText = data.q + '°';
            });
        }
    </script>
</body>
</html>
)rawliteral";

// Función para actualizar todos los servos de un solo paso
void moverMano(int p, int i, int m, int a, int q) {
  anguloPulgar = p;  servoPulgar.write(p);
  anguloIndice = i;  servoIndice.write(i);
  anguloMedio  = m;  servoMedio.write(m);
  anguloAnular = a;  servoAnular.write(a);
  anguloMenique= q;  servoMenique.write(q);
}

// Ruta principal: Entrega la interfaz Web
void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

// Ruta para controlar un dedo individual
void handleSetDedo() {
  if (server.hasArg("dedo") && server.hasArg("val")) {
    String dedo = server.arg("dedo");
    int val = server.arg("val").toInt();

    if (dedo == "pulgar")  { servoPulgar.write(val);  anguloPulgar = val; }
    if (dedo == "indice")  { servoIndice.write(val);  anguloIndice = val; }
    if (dedo == "medio")   { servoMedio.write(val);   anguloMedio = val; }
    if (dedo == "anular")  { servoAnular.write(val);  anguloAnular = val; }
    if (dedo == "menique") { servoMenique.write(val); anguloMenique = val; }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Faltan parametros");
  }
}

// Ruta para ejecutar poses y acciones predeterminadas
void handleAccion() {
  if (server.hasArg("tipo")) {
    String tipo = server.arg("tipo");
    
    // Asumimos: 0° = Abierto / Extendida, 180° = Cerrado / Doblad
    if (tipo == "abrir") {
      moverMano(0, 0, 0, 0, 0);
    } 
    else if (tipo == "cerrar") {
      moverMano(180, 180, 180, 180, 180);
    } 
    else if (tipo == "rock") {
      // Índice y Meñique abiertos (0°), los demás cerrados (180°)
      moverMano(0, 0, 180, 180, 0);
    } 
    else if (tipo == "pulgar_arriba") {
      // Pulgar abierto (0°), los demás cerrados (180°)
      moverMano(0, 180, 180, 180, 180);
    } 
    else if (tipo == "secuencia") {
      // Abre durante 5 segundos y luego cierra durante 5 segundos
      moverMano(0, 0, 0, 0, 0);       // Abrir mano
      delay(5000);                     // Espera 5 segundos
      moverMano(180, 180, 180, 180, 180); // Cerrar mano
      delay(5000);                     // Espera 5 segundos
      moverMano(0, 0, 0, 0, 0);       // Termina abierta
    }

    // Devuelve los valores actualizados en formato JSON para la web
    String json = "{\"p\":" + String(anguloPulgar) + ",\"i\":" + String(anguloIndice) + 
                  ",\"m\":" + String(anguloMedio) + ",\"a\":" + String(anguloAnular) + 
                  ",\"q\":" + String(anguloMenique) + "}";
    server.send(200, "application/json", json);
  } else {
    server.send(400, "text/plain", "Accion invalida");
  }
}

void setup() {
  Serial.begin(115200);

  // Asignar temporizadores de la ESP32 a la librería de servos
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Definir frecuencias estándar para servomotores (50Hz)
  servoPulgar.setPeriodHertz(50);
  servoIndice.setPeriodHertz(50);
  servoMedio.setPeriodHertz(50);
  servoAnular.setPeriodHertz(50);
  servoMenique.setPeriodHertz(50);

  // Adjuntar Servos a sus respectivos Pines (pulsos estándar 500us a 2400us)
  servoPulgar.attach(pinPulgar, 500, 2400);
  servoIndice.attach(pinIndice, 500, 2400);
  servoMedio.attach(pinMedio, 500, 2400);
  servoAnular.attach(pinAnular, 500, 2400);
  servoMenique.attach(pinMenique, 500, 2400);

  // Posición inicial: Mano totalmente abierta
  moverMano(0, 0, 0, 0, 0);

  // Configuración del Punto de Acceso Wi-Fi
  Serial.print("Iniciando Red Wi-Fi...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("Punto de acceso iniciado con éxito.");
  Serial.print("Direccion IP: ");
  Serial.println(myIP);

  // Configuración de rutas Web Server
  server.on("/", handleRoot);
  server.on("/setDedo", handleSetDedo);
  server.on("/accion", handleAccion);

  server.begin();
  Serial.println("Servidor Web iniciado correctamente.");
}

void loop() {
  server.handleClient(); // Atender peticiones de los usuarios en la web
}
