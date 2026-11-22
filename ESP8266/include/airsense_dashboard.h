#pragma once
#include <Arduino.h>
const char PAGE_HTML[] PROGMEM = R"airsenseHtml(<!doctype html>
<html lang="fr" data-mode="esp8266">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#153d34">
<title>AirSense · Surveillance du laboratoire</title>
<style>
:root{
  color-scheme:light;--bg:#f5f6f2;--paper:#fff;--ink:#18342d;--muted:#52645e;--line:#e0e7e1;
  --green:#176552;--mint:#e6f4eb;--warm:#9a4d17;--peach:#f9eee4;--blue:#2b5681;--pale-blue:#eaf1f8;
  --red:#9c3340;--pale-red:#fae8eb;--mono:"IBM Plex Mono",ui-monospace,monospace;--sans:"DM Sans",system-ui,sans-serif
}
*{box-sizing:border-box}
[hidden]{display:none!important}
body{margin:0;background:var(--bg);color:var(--ink);font:15px/1.5 var(--sans)}
h1,h2,h3,p{margin:0}
button,select{font:inherit;color:inherit}
button,select{border:1px solid #cbd8ce;border-radius:8px;background:var(--paper);min-height:39px;padding:5px 12px}
button{cursor:pointer}
button:hover,select:hover{border-color:var(--green)}
:focus-visible{outline:3px solid #23876d;outline-offset:3px}
.header{background:var(--paper);border-bottom:1px solid var(--line)}
.header-inner,main{width:min(1170px,100% - 56px);margin:auto}
.header-inner{height:64px;display:flex;align-items:center;justify-content:space-between;gap:12px}
.brand{display:flex;align-items:center;gap:10px;font-weight:750;font-size:20px;letter-spacing:-.035em}
.brand-icon{display:grid;place-items:center;width:33px;height:33px;border-radius:10px;background:#163e34;color:#c7efd6}
.brand-icon i{filter:drop-shadow(0 0 4px #9adbb180);font-style:normal;font-size:19px}
.source,.flag{display:inline-flex;align-items:center;gap:7px;width:fit-content;border-radius:30px;font-size:12px;font-weight:700;padding:5px 10px;background:#edf1ed;color:#415950}
.source::before,.flag::before{content:"";width:7px;height:7px;border-radius:100%;background:currentColor;flex:none}
main{padding:34px 0 24px}
.intro{display:flex;justify-content:space-between;align-items:end;gap:16px;margin-bottom:22px}
h1{font-size:clamp(29px,5vw,46px);line-height:1.08;letter-spacing:-.045em;font-weight:750}
.intro p{color:var(--muted);margin-top:8px}
.clock{color:var(--muted);white-space:nowrap;font:12px var(--mono);padding-bottom:4px}
.hero{background:#153d34;color:white;border-radius:19px;padding:23px 26px;display:flex;align-items:center;gap:17px;justify-content:space-between;min-height:114px;transition:background-color .18s}
.hero[data-tone="warning"]{background:#623c1d}
.hero[data-tone="critical"]{background:#602a33}
.hero[data-tone="muted"]{background:#344940}
.hero-main{display:flex;align-items:center;gap:16px}
.hero-icon{display:grid;place-items:center;width:52px;height:52px;border:1px solid #ffffff38;border-radius:16px;background:#ffffff13;font-size:25px;color:#dcf7e6;flex:none}
.hero-label{color:#e0ebe4;font-size:11px;letter-spacing:.11em;text-transform:uppercase;font-weight:700}
.hero h2{font-size:27px;line-height:1.18;letter-spacing:-.025em;margin:2px 0 4px}
.hero p{color:#f0f4f1;font-size:13px}
.hero-side{border-left:1px solid #ffffff38;padding-left:23px;min-width:154px}
.hero-side strong{display:block;margin-top:4px;font-size:15px}
.demo{display:flex;align-items:center;gap:14px;margin:15px 0 21px;padding:11px 13px;background:#ecf1ea;border:1px solid #dce7dc;border-radius:12px}
.demo label{font-weight:700;white-space:nowrap}
.demo select{max-width:100%}
.demo p{margin-left:auto;color:var(--muted);font-size:12px}
.demo button{white-space:nowrap}
.metrics{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:15px;margin:20px 0}
.card{background:var(--paper);border:1px solid var(--line);border-radius:16px;min-width:0}
.metric{padding:17px 19px 15px}
.metric-head{display:flex;justify-content:space-between;align-items:center;gap:8px}
.metric h2{font-weight:700;font-size:15px}
.sensor{border-radius:6px;font:11px var(--mono);background:#f0f3ef;color:#445952;padding:4px 7px;white-space:nowrap}
.sensor.temp{background:var(--peach);color:var(--warm)}
.sensor.hum{background:var(--pale-blue);color:var(--blue)}
.reading{font:700 clamp(27px,5vw,40px)/1.15 var(--sans);letter-spacing:-.045em;margin:10px 0 5px;font-variant-numeric:tabular-nums}
.unit{color:var(--muted);font-size:.45em;font-weight:600;margin-left:4px;letter-spacing:0}
.metric p,.panel p{font-size:13px;color:var(--muted)}
.gas{display:flex;align-items:center;min-height:46px;margin:10px 0 5px}
.gas .flag{font-size:14px;padding:5px 10px}
.flag.good{background:var(--mint);color:var(--green)}
.flag.danger{background:var(--pale-red);color:var(--red)}
.panels{display:grid;grid-template-columns:minmax(0,1.55fr) minmax(0,1fr);gap:15px;align-items:start}
.panel{padding:18px 19px}
.panel h2{font-size:18px;letter-spacing:-.023em}
.panel-top{display:flex;align-items:start;justify-content:space-between;gap:9px}
.panel-top p{margin-top:2px}
.tabs{display:flex;gap:2px;background:#eef2ee;border-radius:9px;padding:3px;flex:none}
.tabs button{font-size:12px;font-weight:700;min-height:30px;padding:3px 8px;color:var(--muted);border-color:transparent;background:transparent}
.tabs button[aria-pressed="true"]{background:var(--paper);border-color:#e2e9e3;color:var(--ink)}
.chart{width:100%;height:181px;display:block;margin:10px 0 2px}
.chart-meta{display:flex;justify-content:space-between;gap:8px;font:11px var(--mono);color:var(--muted)}
.thresholds{margin:10px 0 0;padding:0}
.rule{display:grid;grid-template-columns:minmax(0,1fr) auto;align-items:start;gap:10px;padding:9px 0;border-top:1px solid var(--line)}
.rule dt{font-size:13px;font-weight:650}
.rule dd{margin:0;font:12px/1.6 var(--mono);text-align:right}
.rule .warn{color:var(--warm)}
.rule .critical{color:var(--red)}
.threshold-note{border-top:1px solid var(--line);padding-top:9px}
.events{margin-top:15px}
.events ul{padding:0;margin:7px 0 0;list-style:none}
.events li{border-top:1px solid var(--line);display:flex;gap:13px;align-items:baseline;padding:6px 0;font-size:13px}
.events li:last-child{padding-bottom:0}
.events time{color:var(--muted);font:11px var(--mono)}
footer{display:flex;justify-content:space-between;gap:8px;flex-wrap:wrap;color:var(--muted);font-size:12px;margin-top:19px}
@media(max-width:880px){
  .demo{flex-wrap:wrap}.demo p{margin-left:0;width:100%}
  .panels{grid-template-columns:1fr}
}
@media(max-width:620px){
  .header-inner,main{width:calc(100% - 28px)}main{padding-top:24px}
  .intro{align-items:start;flex-direction:column;gap:6px}
  .hero{align-items:start;flex-direction:column;padding:17px}
  .hero-side{width:100%;border-left:0;border-top:1px solid #ffffff38;padding:10px 0 0;margin-top:0}
  .metrics{grid-template-columns:1fr;gap:9px}.metric{padding:13px 15px}
  .panel{padding:14px}.panel-top{flex-wrap:wrap}
  .demo{gap:8px}.demo label{width:100%}.demo select{min-width:0;flex:1}
}
@media(prefers-reduced-motion:reduce){*,*::before,*::after{transition:none!important}}

/* Deuxieme noeud : tableau de bord commun */
.node-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:15px;margin:14px 0 20px}
.node-card{padding:17px 19px}.node-value{font-size:24px;font-weight:750;margin:7px 0}.node-caption{font-size:12px;color:var(--muted)}
.node-title{display:flex;justify-content:space-between;align-items:center;gap:12px;margin-top:25px}
@media(max-width:750px){.node-grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<header class="header">
  <div class="header-inner">
    <div class="brand"><span class="brand-icon" aria-hidden="true"><i>∿</i></span> AirSense</div>
    <span class="source" id="source">Source en attente</span>
  </div>
</header>
<main>
  <div class="intro">
    <div>
      <h1>Surveillance du laboratoire</h1>
      <p>Température, humidité et seuil de détection du gaz</p>
    </div>
    <span class="clock" id="received">Dernière réponse · —</span>
  </div>
  <section class="hero" id="hero" data-tone="muted" aria-labelledby="state">
    <div class="hero-main">
      <span class="hero-icon" id="state-icon" aria-hidden="true">·</span>
      <div role="status">
        <div class="hero-label">État de la surveillance</div>
        <h2 id="state">En attente</h2>
        <p id="reason">Récupération du premier relevé.</p>
      </div>
    </div>
    <div class="hero-side"><div class="hero-label">Interrupteur</div><strong id="switch">—</strong></div>
  </section>
  <div class="demo" id="demo" hidden>
    <label for="scenario">Scénario simulé</label>
    <select id="scenario">
      <option value="normal">Conditions normales</option>
      <option value="warm">Avertissement température</option>
      <option value="critical">Température critique</option>
      <option value="humid">Humidité hors plage</option>
      <option value="mq">Alerte gaz (MQ)</option>
      <option value="sensor">DHT11 indisponible</option>
      <option value="off">Interrupteur OFF</option>
      <option value="offline">Perte de connexion</option>
    </select>
    <button type="button" id="pause" aria-pressed="false">Pause</button>
    <p>Le DHT11 simulé produit un relevé toutes les 3 s.</p>
  </div>
  <div class="metrics" aria-label="Mesures">
    <section class="card metric">
      <div class="metric-head"><h2>Température</h2><span class="sensor temp">DHT11</span></div>
      <div class="reading"><span id="temperature">—</span><span class="unit">°C</span></div>
      <p id="temp-detail">En attente d’un relevé</p>
    </section>
    <section class="card metric">
      <div class="metric-head"><h2>Humidité relative</h2><span class="sensor hum">DHT11</span></div>
      <div class="reading"><span id="humidity">—</span><span class="unit">%</span></div>
      <p id="hum-detail">En attente d’un relevé</p>
    </section>
    <section class="card metric">
      <div class="metric-head"><h2>Détection de gaz</h2><span class="sensor">MQ · DO</span></div>
      <div class="gas"><span class="flag" id="gas">En attente</span></div>
      <p id="gas-detail">Sortie numérique du comparateur</p>
    </section>
  </div>
  <div class="panels">
    <section class="card panel">
      <div class="panel-top">
        <div><h2>Évolution des mesures</h2><p>40 derniers relevés DHT11 au maximum</p></div>
        <div class="tabs" role="group" aria-label="Mesure affichée">
          <button type="button" id="chart-temp" aria-pressed="true">Température</button>
          <button type="button" id="chart-hum" aria-pressed="false">Humidité</button>
        </div>
      </div>
      <canvas class="chart" id="chart" role="img" aria-label="Historique DHT11">Historique des mesures DHT11.</canvas>
      <div class="chart-meta"><span id="chart-range">—</span><span id="point-count">0 relevé</span></div>
    </section>
    <section class="card panel">
      <h2>Seuils de surveillance</h2>
      <p>Valeurs de démonstration du firmware actuel</p>
      <dl class="thresholds">
        <div class="rule"><dt>Température · avertissement</dt><dd class="warn">30 °C ≤ T &lt; 35 °C</dd></div>
        <div class="rule"><dt>Température · critique</dt><dd class="critical">T ≥ 35 °C</dd></div>
        <div class="rule"><dt>Humidité · avertissement</dt><dd class="warn">H &lt; 30 % ou H &gt; 70 %</dd></div>
        <div class="rule"><dt>Gaz · critique</dt><dd class="critical">DO = LOW</dd></div>
      </dl>
      <p class="threshold-note">Le potentiomètre du MQ règle le seuil. Sa concentration équivalente en ppm est inconnue.</p>
    </section>
  </div>

  <section aria-labelledby="esp32-title">
    <div class="node-title"><h2 id="esp32-title">Nœud ESP32 · Détection et alarme</h2><span class="source" id="esp32-link">En attente MQTT</span></div>
    <div class="node-grid">
      <article class="card node-card"><h3>Détection de mouvement</h3><div class="node-value" id="esp32-pir">—</div><p class="node-caption">PIR · GPIO27</p></article>
      <article class="card node-card"><h3>Luminosité</h3><div class="node-value" id="esp32-ldr">—</div><p class="node-caption">LDR · GPIO26</p></article>
      <article class="card node-card"><h3>Alarme sonore</h3><div class="node-value" id="esp32-buzzer">—</div><p class="node-caption">Buzzer · GPIO25</p></article>
    </div>
    <p class="node-caption" id="esp32-age">Aucune donnée reçue de l'ESP32.</p>
  </section>
  <section class="card panel events">
    <h2>Événements récents</h2>
    <ul id="events"><li id="placeholder">En attente de données</li></ul>
  </section>
  <footer><span>ESP8266 · DHT11 · MQ avec comparateur LM393</span><span id="cadence">Interrogation · 1 s après chaque réponse</span></footer>
</main>
<script>
"use strict";
const demo = document.documentElement.dataset.mode === "simulation";
const $ = id => document.getElementById(id);
const states = {
  NORMAL: ["Normal", "Conditions dans les seuils configurés.", "good", "✓"],
  WARNING: ["Avertissement", "Température ou humidité hors plage normale.", "warning", "!"],
  CRITICAL: ["Alerte critique", "Un seuil critique est atteint.", "critical", "!"],
  SENSOR_ERROR: ["DHT11 indisponible", "Premier relevé en attente ou lecture invalide.", "warning", "!"],
  OFF: ["Surveillance arrêtée", "L’interrupteur physique est en position OFF.", "muted", "○"]
};
const statesWithData = new Set(["NORMAL", "WARNING", "CRITICAL"]);
const number = new Intl.NumberFormat("fr-FR", {minimumFractionDigits:1, maximumFractionDigits:1});
const clock = new Intl.DateTimeFormat("fr-FR", {hour:"2-digit", minute:"2-digit", second:"2-digit"});
const chart = $("chart");
const context = chart.getContext("2d");
const points = [];
let chartField = "temperature";
let chartFrame = 0;
let latest = null;
let lastEvent = "";
let lastSample = null;
let sampleReceived = null;
let transport = "waiting";
let paused = false;
let timer = 0;
let active = null;
let failures = 0;
let wake = false;
let pendingGap = false;

function setText(id, value) {
  const element = $(id);
  if (element.textContent !== value) element.textContent = value;
}

function addEvent(key, label) {
  if (key === lastEvent) return;
  lastEvent = key;
  $("placeholder")?.remove();
  const row = document.createElement("li");
  const time = document.createElement("time");
  const now = new Date();
  time.dateTime = now.toISOString();
  time.textContent = clock.format(now);
  row.append(time, document.createTextNode(label));
  const list = $("events");
  list.prepend(row);
  if (list.children.length > 8) list.lastElementChild.remove();
}

function verdict(title, reason, tone, icon) {
  $("hero").dataset.tone = tone;
  setText("state", title);
  setText("reason", reason);
  setText("state-icon", icon);
}

function values(data) {
  const valid = Boolean(data?.on && data.dht_ok);
  const gas = data?.on ? data.mq : null;
  setText("temperature", valid ? number.format(data.temperature) : "—");
  setText("humidity", valid ? number.format(data.humidite) : "—");
  const atmosphere = valid && sampleReceived ? "Relevé reçu à " + clock.format(sampleReceived) :
    data?.on ? "Aucun relevé DHT11 valide" : data ? "Surveillance désactivée" : "Données indisponibles";
  setText("temp-detail", atmosphere);
  setText("hum-detail", atmosphere);
  setText("gas", gas === true ? "Seuil déclenché" : gas === false ? "Seuil non déclenché" : data ? "Désactivé" : "Indisponible");
  $("gas").className = "flag" + (gas === true ? " danger" : gas === false ? " good" : "");
  setText("gas-detail", gas === true ? "DO = LOW" : gas === false ? "DO = HIGH" : "Sortie numérique du comparateur");
  setText("switch", data ? (data.on ? "ON · Activé" : "OFF · Désactivé") : "—");
}

function drawChart() {
  chartFrame = 0;
  if (!context) return;
  const width = chart.clientWidth;
  const height = chart.clientHeight;
  if (!width || !height) return;
  const ratio = Math.min(window.devicePixelRatio || 1, 2);
  const pixelWidth = Math.round(width * ratio);
  const pixelHeight = Math.round(height * ratio);
  if (chart.width !== pixelWidth || chart.height !== pixelHeight) {
    chart.width = pixelWidth;
    chart.height = pixelHeight;
  }
  context.setTransform(ratio,0,0,ratio,0,0);
  context.clearRect(0,0,width,height);
  context.font = "11px monospace";
  context.fillStyle = "#52645e";
  if (!points.length) {
    context.fillText("En attente d’un relevé valide", 8, height / 2);
    chart.setAttribute("aria-label", "Historique DHT11, en attente d’un relevé valide");
    return;
  }
  const temperature = chartField === "temperature";
  const unit = temperature ? "°C" : "%";
  const name = temperature ? "Température" : "Humidité";
  const readings = points.map(point => point[chartField]);
  const min = Math.min(...readings);
  const max = Math.max(...readings);
  const lower = Math.floor(min) - 1;
  const upper = Math.ceil(max) + 1;
  const left = 39;
  const plotWidth = width - left - 11;
  const plotHeight = height - 19;
  const color = temperature ? "#ad5a2b" : "#426e9f";
  for (let i = 0; i <= 4; i++) {
    const y = 9 + i * plotHeight / 4;
    context.strokeStyle = "#e4ebe4";
    context.lineWidth = 1;
    context.beginPath();
    context.moveTo(left, y);
    context.lineTo(width - 7, y);
    context.stroke();
    context.fillText(number.format(upper - (upper - lower) * i / 4), 0, y + 3);
  }
  const first = points[0].time;
  const last = points.at(-1).time;
  let end;
  context.beginPath();
  points.forEach((point, index) => {
    const x = left + (first === last ? plotWidth / 2 : plotWidth * (point.time - first) / (last - first));
    const y = 9 + plotHeight * (upper - point[chartField]) / (upper - lower);
    if (index === 0 || point.gap) context.moveTo(x, y);
    else context.lineTo(x, y);
    end = {x,y};
  });
  context.strokeStyle = color;
  context.lineWidth = 2.4;
  context.stroke();
  context.beginPath();
  context.arc(end.x,end.y,3.6,0,2 * Math.PI);
  context.fillStyle = color;
  context.fill();
  setText("chart-range", clock.format(first) + " – " + clock.format(last));
  setText("point-count", points.length + (points.length === 1 ? " relevé" : " relevés"));
  chart.setAttribute("aria-label", name + ", " + points.length + " relevés, de " + number.format(min) + " à " + number.format(max) + " " + unit);
}

function requestChart() {
  if (!chartFrame) chartFrame = requestAnimationFrame(drawChart);
}

function selectChart(field) {
  chartField = field;
  $("chart-temp").setAttribute("aria-pressed", field === "temperature");
  $("chart-hum").setAttribute("aria-pressed", field === "humidite");
  requestChart();
}

function validPayload(data) {
  if (!data || typeof data.on !== "boolean" || typeof data.dht_ok !== "boolean" ||
      typeof data.mq !== "boolean" || !Object.hasOwn(states, data.etat) ||
      !Number.isFinite(data.sample) || data.sample < 0 || !Number.isFinite(data.uptime) || data.uptime < 0) return false;
  if (!data.on) return true;
  if (data.etat === "OFF" || (statesWithData.has(data.etat) && !data.dht_ok && !data.mq)) return false;
  return data.dht_ok ? Number.isFinite(data.temperature) && Number.isFinite(data.humidite) :
    data.temperature === null && data.humidite === null;
}

function render(data) {
  const received = new Date();
  if (transport === "offline") addEvent("back", "Connexion rétablie");
  transport = "online";
  setText("received", "Dernière réponse · " + clock.format(received));
  setText("cadence", "Interrogation · 1 s après chaque réponse");
  if (latest && data.uptime < latest.uptime) {
    lastSample = null;
    pendingGap = true;
    addEvent("restart", "Redémarrage de la source");
  }
  latest = data;
  if (!data.on || !data.dht_ok) {
    pendingGap = true;
  } else if (data.sample !== lastSample) {
    lastSample = data.sample;
    sampleReceived = received;
    points.push({time:received.getTime(), temperature:data.temperature, humidite:data.humidite, gap:pendingGap});
    if (points.length > 40) points.shift();
    pendingGap = false;
    requestChart();
  }
  values(data);
  const state = data.on ? data.etat : "OFF";
  const [title, defaultReason, tone, icon] = states[state];
  let reason = defaultReason;
  if (data.on && data.mq) reason = "Seuil numérique du module MQ déclenché.";
  else if (state === "CRITICAL") reason = "Température supérieure ou égale à 35 °C.";
  else if (state === "WARNING") {
    const reasons = [];
    if (data.temperature >= 30) reasons.push("Température supérieure ou égale à 30 °C.");
    if (data.humidite < 30 || data.humidite > 70) reasons.push("Humidité hors de la plage de 30 à 70 %.");
    if (reasons.length) reason = reasons.join(" ");
  }
  verdict(title, reason, tone, icon);
  addEvent(state + ":" + (data.on && data.mq), title + (data.on && data.mq ? " · seuil MQ déclenché" : ""));
}

function markOffline(delay) {
  if (transport !== "offline") addEvent("offline", "Aucune réponse de la source");
  transport = "offline";
  pendingGap = true;
  values(null);
  verdict("Connexion indisponible", "Mesures en attente d’une nouvelle réponse.", "muted", "!");
  setText("cadence", "Nouvelle tentative dans " + delay / 1000 + " s");
}

const simulationStart = performance.now();
let simulatedTick = -1;
let simulatedClimate = null;
let wasOn = true;
function simulate() {
  const scenario = $("scenario").value;
  if (scenario === "offline") throw new Error("Coupure simulée");
  const elapsed = Math.floor(performance.now() - simulationStart);
  const tick = Math.floor(elapsed / 3000);
  const on = scenario !== "off";
  const mq = on && scenario === "mq";
  if (on) {
    if (tick !== simulatedTick || !wasOn) {
      simulatedTick = tick;
      const variation = Math.sin(tick * .9);
      simulatedClimate = {
        temperature: scenario === "warm" ? 31 + variation * .3 : scenario === "critical" ? 36 + variation * .3 : 26.4 + variation * .6,
        humidite: scenario === "humid" ? 74 + variation : 52 + variation * 2,
        sample: elapsed + 1
      };
    }
  }
  wasOn = on;
  const dht_ok = on && scenario !== "sensor";
  const temperature = dht_ok ? simulatedClimate.temperature : null;
  const humidite = dht_ok ? simulatedClimate.humidite : null;
  let etat = "NORMAL";
  if (!on) etat = "OFF";
  else if (mq || (dht_ok && temperature >= 35)) etat = "CRITICAL";
  else if (!dht_ok) etat = "SENSOR_ERROR";
  else if (temperature >= 30 || humidite < 30 || humidite > 70) etat = "WARNING";
  return {on, temperature, humidite, dht_ok, mq, etat, sample:simulatedClimate?.sample || 0, uptime:Math.floor(elapsed / 1000)};
}

async function readData(signal) {
  if (demo) return simulate();
  const response = await fetch("/api/data", {cache:"no-store", signal});
  if (!response.ok) throw new Error("HTTP " + response.status);
  return response.json();
}

function visible() {
  return !paused && !document.hidden;
}

async function poll() {
  if (active || !visible()) return;
  const request = new AbortController();
  active = request;
  const timeout = setTimeout(() => request.abort(), 2500);
  let delay = 1000;
  try {
    const data = await readData(request.signal);
    if (!visible() || request.signal.aborted) return;
    if (!validPayload(data)) throw new Error("Réponse invalide");
    render(data);
    failures = 0;
  } catch {
    if (visible() && !wake) {
      failures = Math.min(failures + 1, 3);
      delay = 1000 * 2 ** failures;
      markOffline(delay);
    }
  } finally {
    clearTimeout(timeout);
    active = null;
    if (visible()) timer = setTimeout(poll, wake ? 0 : delay);
    wake = false;
  }
}

function syncPolling() {
  clearTimeout(timer);
  if (!visible()) {
    active?.abort();
    return;
  }
  if (active) {
    wake = true;
    return;
  }
  poll();
}

$("chart-temp").addEventListener("click", () => selectChart("temperature"));
$("chart-hum").addEventListener("click", () => selectChart("humidite"));
window.addEventListener("resize", requestChart);
document.addEventListener("visibilitychange", syncPolling);
setText("source", demo ? "Données simulées" : "ESP8266 · AirSense-Lab");
if (demo) {
  $("demo").hidden = false;
  for (let i = 11; i > 0; i--) {
    points.push({time:Date.now() - i * 3000, temperature:26.4 + Math.sin(i * 1.4) * .6, humidite:52 + Math.sin(i * .8) * 2});
  }
  $("scenario").addEventListener("change", syncPolling);
  $("pause").addEventListener("click", () => {
    paused = !paused;
    $("pause").setAttribute("aria-pressed", paused);
    setText("pause", paused ? "Reprendre" : "Pause");
    if (paused) setText("cadence", "Actualisation en pause");
    syncPolling();
  });
}
requestChart();
syncPolling();

// Les données ESP32 proviennent du broker MQTT via l'ESP8266.
// Aucune valeur simulée n'est présentée comme mesure réelle.
async function pollEsp32(){
  if(demo){setText("esp32-link","Simulation ESP8266 seulement");return;}
  try{
    const r=await fetch('/api/esp32',{cache:'no-store'});
    if(!r.ok)throw new Error('HTTP');
    const d=await r.json();
    const recent=d.online===true;
    setText('esp32-link',recent?'MQTT reçu':'Données indisponibles');
    setText('esp32-pir',recent&&d.pir!==null?(d.pir?'Mouvement':'Aucun mouvement'):'—');
    setText('esp32-ldr',recent&&d.ldr!==null?d.ldr:'—');
    setText('esp32-buzzer',recent&&d.buzzer!==null?d.buzzer:'—');
    setText('esp32-age',recent?'Dernier message MQTT reçu il y a '+Math.floor(d.age_ms/1000)+' s':'ESP32 non reçu ou données périmées (> 10 s)');
  }catch(e){setText('esp32-link','Connexion perdue');}
}
if(!demo){pollEsp32();setInterval(()=>{if(!document.hidden)pollEsp32()},2000);}
</script>
</body>
</html>
)airsenseHtml";
