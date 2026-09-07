[README.md](https://github.com/user-attachments/files/31929414/README.md)
# 🦾 Mano Robótica con ESP32 — Control por WiFi sin apps

Proyecto ganador del **primer lugar** en el concurso de robótica (categoría básico) de un instituto en Gualán, Zacapa, Guatemala. Creado por una estudiante de tercero básico, con asesoría técnica y electrónica de su papá, ingeniero.

Este repositorio existe para que cualquier estudiante, maestro o padrino de un Club de Ciencias en Guatemala (o donde sea) pueda replicarlo, aprenderlo y mejorarlo — **gratis y sin restricciones**.

---

## ✨ ¿Qué hace?

- Controla **5 servomotores** (uno por dedo) desde una página web propia.
- El ESP32 crea su propia red WiFi (modo *Access Point*) y sirve la interfaz — **no requiere instalar ninguna app**, solo abrir el navegador del celular.
- Incluye **posturas rápidas** predefinidas: seña de rock 🤘, pulgar arriba 👍, abrir/cerrar mano, y una secuencia automática de abrir-cerrar cada 5 segundos.
- Cada dedo también se puede mover de forma individual y precisa con un control deslizante (slider).

## 🧠 ¿Por qué es diferente?

La mayoría de proyectos similares controlan el robot con una app de terceros o un mando por Bluetooth. Aquí el propio ESP32 genera la página web — **cualquier celular con navegador puede controlar la mano al instante**, sin descargar nada.

## 🧰 Materiales

La mayoría fueron reciclados o reutilizados de otros proyectos.

| Material | Uso |
|---|---|
| Cartón | Estructura de la palma y los dedos |
| Ligas de hule (de billetes) | Retorno de cada dedo a su posición |
| Paletas de madera | Refuerzo y soporte de las falanges |
| Hilo / tanza | Tendones que mueven cada dedo |
| Mini protoboard | Conexiones ordenadas del ESP32 |
| 5 servomotores SG90 | Motores que doblan cada dedo |
| 2 baterías recargables 18650 | Fuente de energía portátil |
| Módulo de carga 18650 | Recarga segura, salida de 5V |
| 1 ESP32 (DevKit) | Microcontrolador — el "cerebro" |

### ¿Dónde comprar los componentes en Guatemala?

- **Steren** — [www.steren.com.gt](https://www.steren.com.gt/) — tienda física y en línea, servos, protoboards, cables.
- **Electrónica DIY** — [electronicadiy.com/es](https://www.electronicadiy.com/es) — especializada en proyectos maker/IoT.
- **Oxdea** — [oxdea.gt](https://oxdea.gt/) — componentes electrónicos y desarrollo.

## 🔌 Conexiones (pines)

| Servo | Pin GPIO del ESP32 |
|---|---|
| Pulgar | 13 |
| Índice | 12 |
| Medio | 14 |
| Anular | 27 |
| Meñique | 26 |

Los servos se alimentan desde las baterías 18650 (vía el módulo de carga), **no directamente del pin 5V/3.3V del ESP32** — con 5 servos moviéndose a la vez, el ESP32 solo no da abasto y se puede reiniciar. Todos los GND (servos, ESP32, batería) deben estar unidos.

## 💻 Software necesario

1. [Arduino IDE](https://www.arduino.cc/en/software)
2. Soporte de placas **ESP32** instalado en Arduino IDE (Board Manager)
3. Librería **ESP32Servo** (Sketch → Include Library → Manage Libraries → buscar "ESP32Servo")

## 🚀 Cómo usarlo

1. Abre `mano_robotica.ino` en Arduino IDE (o copia el código de este archivo a un sketch nuevo).
2. Selecciona tu placa ESP32 y el puerto correcto.
3. Sube el código.
4. Desde tu celular, conéctate a la red WiFi `ManoRobotica_ESP32` (contraseña: `manorobotica@2026`).
5. Abre un navegador y entra a `192.168.4.1`.
6. ¡Listo! Ya puedes mover los dedos desde ahí.

> 💡 Puedes cambiar el nombre de red y contraseña editando las variables `ssid` y `password` al inicio del código.

## 🤖 El "prompt mágico": cómo se generó este código con IA

No se escribió desde cero. Se usó una IA como asistente de programación con esta instrucción:

> *"Dame el código para controlar los dedos de los 5 servomotores desde una interfaz web, que tenga la contraseña de WiFi manorobotica@2026. Que aparezca una interfaz web con un botón para cada dedo, además un botón predeterminado para la seña de rock, otro botón de dedo gordo arriba, y otro botón para abrir y cerrar la mano durante 5 segundos."*

El código resultante se pegó y ajustó en Arduino IDE. Si tu prompt es claro y detallado, el código que obtienes es mucho mejor — esa es la lección más valiosa de todo el proyecto.

## 📜 Código completo

```cpp
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
    
    // Asumimos: 0° = Abierto / Extendida, 180° = Cerrado / Doblada
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
```

## 🏆 Resultado

Primer lugar en el concurso de robótica del instituto (categoría básico). El jurado destacó la originalidad de controlar la mano por WiFi sin usar ninguna aplicación.

## 🙋 Créditos

- **Creadora del proyecto:** mi hija, estudiante de tercero básico A.M.A:A.
- **Asesoría técnica y electrónica:** Ing. Marlon Estuardo Alvarado Kuckling — Padrino, Club de Ciencias Senacyt, Región Oriente (Zacapa).
- Código generado con asistencia de IA a partir del prompt documentado arriba.


## 📄 Licencia

Este proyecto se publica bajo la licencia **Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**.

- **Uso Didáctico y No Comercial:** Libre para ser copiado, compartido y adaptado con fines educativos y en clubes de ciencia. Prohibida su comercialización o venta por parte de terceros.
- **Atribución:** Se debe dar crédito a los autores originales (Ing. Marlon Estuardo Alvarado Kuckling y la estudiante A.M.A.A.
- **Compartir Igual:** Cualquier proyecto derivado debe publicarse bajo esta misma licencia.

Consulta el archivo `LICENSE` en la raíz del repositorio para ver el código legal completo.

*¿Tienes dudas o mejoras? Este proyecto nació para inspirar a más niños y jóvenes de Guatemala a meterse en robótica — siéntete libre de compartir tu propia versión.*
