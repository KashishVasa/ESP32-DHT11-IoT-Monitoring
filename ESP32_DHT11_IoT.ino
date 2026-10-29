
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

const char* ssid = "ESP32_Test";
const char* password = "kashish1234";

float temperature = 0;
float humidity = 0;
bool sensorOK = false;

unsigned long lastRead = 0;

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Live Environment Dashboard</title>

<style>
body {
  font-family: Arial, sans-serif;
  background: #eef4fa;
  text-align: center;
  margin: 0;
  padding: 15px;
  color: #16324f;
}
h1 { margin-bottom: 5px; }
.subtitle { color: #607080; }

.card {
  background: white;
  padding: 20px;
  margin: 18px auto;
  max-width: 650px;
  border-radius: 16px;
  box-shadow: 0 3px 12px #ccd5df;
}
.value {
  font-size: 36px;
  font-weight: bold;
  color: #1769aa;
}
.humidity { color: #159b83; }
canvas {
  width: 100%;
  height: 240px;
  display: block;
}
.status {
  font-size: 14px;
  color: #607080;
}
</style>
</head>

<body>
<h1>Environment Monitor</h1>
<p class="subtitle">ESP32 + DHT11 Live Dashboard</p>

<div class="card">
  <h2>Temperature</h2>
  <div class="value" id="temp">-- °C</div>
  <canvas id="tempChart"></canvas>
  <p class="status">Temperature history (°C)</p>
</div>

<div class="card">
  <h2>Humidity</h2>
  <div class="value humidity" id="hum">-- %</div>
  <canvas id="humChart"></canvas>
  <p class="status">Humidity history (%)</p>
</div>

<p id="status">Connecting to ESP32...</p>
<p class="status">Graphs update every 2.5 seconds</p>

<script>
const maxPoints = 20;
const tempHistory = [];
const humHistory = [];

function drawGraph(canvasId, values, lineColor, unit) {
  const canvas = document.getElementById(canvasId);
  const ctx = canvas.getContext('2d');

  const width = canvas.clientWidth;
  const height = 240;
  const dpr = window.devicePixelRatio || 1;

  canvas.width = width * dpr;
  canvas.height = height * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

  ctx.clearRect(0, 0, width, height);

  const left = 42;
  const right = 12;
  const top = 20;
  const bottom = 30;
  const graphWidth = width - left - right;
  const graphHeight = height - top - bottom;

  let min = 0;
  let max = 100;

  if (values.length > 0) {
    const low = Math.min(...values);
    const high = Math.max(...values);
    const padding = Math.max((high - low) * 0.25, 2);
    min = Math.max(0, low - padding);
    max = high + padding;
    if (max - min < 5) max = min + 5;
  }

  // Horizontal grid and labels
  ctx.font = '11px Arial';
  ctx.textAlign = 'right';
  ctx.textBaseline = 'middle';

  for (let i = 0; i <= 4; i++) {
    const y = top + graphHeight * i / 4;
    const val = max - (max - min) * i / 4;

    ctx.beginPath();
    ctx.strokeStyle = '#e4eaf0';
    ctx.lineWidth = 1;
    ctx.moveTo(left, y);
    ctx.lineTo(width - right, y);
    ctx.stroke();

    ctx.fillStyle = '#607080';
    ctx.fillText(val.toFixed(1), left - 6, y);
  }

  if (values.length === 0) {
    ctx.fillStyle = '#607080';
    ctx.textAlign = 'center';
    ctx.fillText('Waiting for readings...', width / 2, height / 2);
    return;
  }

  const xAt = i => values.length === 1
    ? left + graphWidth / 2
    : left + graphWidth * i / (maxPoints - 1);

  const yAt = v => top + graphHeight -
    ((v - min) / (max - min)) * graphHeight;

  // Draw graph line
  ctx.beginPath();
  values.forEach((v, i) => {
    if (i === 0) ctx.moveTo(xAt(i), yAt(v));
    else ctx.lineTo(xAt(i), yAt(v));
  });

  ctx.strokeStyle = lineColor;
  ctx.lineWidth = 2.5;
  ctx.lineJoin = 'round';
  ctx.stroke();

  // Draw reading points
  values.forEach((v, i) => {
    ctx.beginPath();
    ctx.arc(xAt(i), yAt(v), 3, 0, Math.PI * 2);
    ctx.fillStyle = lineColor;
    ctx.fill();
  });

  // X-axis labels
  ctx.fillStyle = '#607080';
  ctx.textAlign = 'center';
  ctx.textBaseline = 'top';
  ctx.fillText('Older', left, height - 20);
  ctx.fillText('Latest', width - right, height - 20);
}

async function updateData() {
  try {
    const response = await fetch('/data');

    if (!response.ok) throw new Error('Request failed');

    const d = await response.json();

    if (!d.ok) {
      document.getElementById('status').textContent =
        'Check DHT11 sensor wiring';
      return;
    }

    document.getElementById('temp').textContent =
      Number(d.temperature).toFixed(1) + ' °C';

    document.getElementById('hum').textContent =
      Number(d.humidity).toFixed(1) + ' %';

    tempHistory.push(Number(d.temperature));
    humHistory.push(Number(d.humidity));

    if (tempHistory.length > maxPoints) tempHistory.shift();
    if (humHistory.length > maxPoints) humHistory.shift();

    drawGraph('tempChart', tempHistory, '#1769aa', '°C');
    drawGraph('humChart', humHistory, '#159b83', '%');

    document.getElementById('status').textContent =
      'Live readings connected';
  } catch (error) {
    document.getElementById('status').textContent =
      'Connection lost. Retrying...';
  }
}

window.addEventListener('resize', () => {
  drawGraph('tempChart', tempHistory, '#1769aa', '°C');
  drawGraph('humChart', humHistory, '#159b83', '%');
});

updateData();
setInterval(updateData, 2500);
</script>
</body>
</html>
)rawliteral";

void handleHome() {
  server.send_P(200, "text/html", webpage);
}

void handleData() {
  String json = "{\"ok\":";
  json += sensorOK ? "true" : "false";
  json += ",\"temperature\":";
  json += String(temperature, 1);
  json += ",\"humidity\":";
  json += String(humidity, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("Open dashboard: http://");
    Serial.println(WiFi.localIP());

    server.on("/", handleHome);
    server.on("/data", handleData);
    server.begin();

    Serial.println("Web server started");
  } else {
    Serial.println("\nWi-Fi failed. Sensor mode only.");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }

  if (millis() - lastRead >= 2500) {
    lastRead = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      temperature = t;
      humidity = h;
      sensorOK = true;

      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" C | Humidity: ");
      Serial.print(h);
      Serial.println(" %");
    } else {
      sensorOK = false;
      Serial.println("DHT11 read failed. Check wiring.");
    }
  }
}
