"***

# GreenHub & WorkWell – Plataforma Web + Sistema IoT de Bem-Estar no Trabalho.

Este repositório reúne dois pilares de uma mesma solução para o futuro do trabalho.

- **GreenHub / Lifonix (frontend + backend)** – Plataforma web em React com um backend Node simples para servir perfis e recomendações, conectando talentos, empresas e oportunidades, com foco em propósito, competências do futuro e economia verde.
- **WorkWell (IoT)** – Sistema baseado em ESP32 + MQTT + Node-RED + dashboard React para monitorar indicadores de estresse e bem-estar de trabalhadores em casa ou no escritório.

Juntos, GreenHub e WorkWell formam um ecossistema que conecta pessoas, tecnologia e bem-estar, alinhado às transformações do futuro do trabalho, com dashboards de bem-estar e engajamento disponibilizadas para as empresas que fazem parte da comunidade GreenHub.

***

## Problema



As empresas e profissionais ainda têm dificuldade em monitorar, de forma contínua e orientada por dados, as condições reais de bem-estar no trabalho e em conectar isso a oportunidades de carreira alinhadas a propósito e sustentabilidade. Faltam ferramentas integradas que coletem dados ambientais, traduzam essas informações em indicadores de estresse e qualidade de vida e, ao mesmo tempo, aproximem talentos e organizações comprometidas com um futuro do trabalho mais saudável, humano e sustentável.

## 🔭 Visão geral da solução

### GreenHub (site e API)

Link do repositorio: https://github.com/Lifonix/GreenHub.git

- Aplicação SPA em React no diretório `frontend/`, com páginas como Home, Dashboard, Empresas, Cadastro de Empresas, About e Notícias Ambientais.
- Backend Node/Express simples em `backend/`, servindo dados de profissionais e recomendados a partir de arquivos JSON.
- Tela inicial com hero, botões de ação, lista de profissionais com filtros e seções institucionais (Trending Skills, Valores, Métricas da Comunidade, Histórias).
- Suporte a modo claro/escuro controlado na Home.

### WorkWell (IoT + bem-estar)

Link do repositorio: https://github.com/Lifonix/IoT-Lifonix.git

- Dispositivo com ESP32 coleta dados de sensores (ex.: temperatura, umidade e luminosidade) como proxies para condições de estresse.
- Broker MQTT recebe e encaminha mensagens usando tópicos dedicados ao WorkWell.
- Node-RED assina esses tópicos, calcula um índice de estresse, grava em arquivo e expõe uma API `/dadosGreenHub` que a dashboard em React consome.

***

## 🧱 Arquitetura de pastas do repositório

A estrutura atual do projeto GreenHub está organizada da seguinte forma:

```bash
GREENHUB/
├─ backend/
│  ├─ data/
│  │  ├─ profiles.json          # Base de dados de profissionais
│  │  └─ recomendados.json      # Base de dados de perfis recomendados
│  ├─ routes/                   # (opcional) Rotas separadas da API
│  ├─ server.js                 # Servidor Node/Express para expor a API
│  ├─ package.json
│  └─ node_modules/
│
├─ frontend/
│  ├─ src/
│  │  ├─ components/
│  │  │  ├─ CommunitySection.jsx
│  │  │  ├─ CreateProfiles.jsx
│  │  │  ├─ Footer.jsx
│  │  │  ├─ GreenHubStressChart.jsx
│  │  │  ├─ Header.jsx
│  │  │  ├─ LoginModal.jsx
│  │  │  ├─ ProfileCard.jsx
│  │  │  ├─ ProfileModal.jsx
│  │  │  ├─ SearchFilter.jsx
│  │  │  ├─ StoriesSection.jsx
│  │  │  ├─ TrendingSkills.jsx
│  │  │  └─ ValueSection.jsx
│  │  ├─ images/
│  │  │  └─ folha.png           # Asset visual usado na interface
│  │  ├─ pages/
│  │  │  ├─ About.jsx
│  │  │  ├─ CadastroEmpresa.jsx
│  │  │  ├─ Dashboard.jsx       # Tela de dashboard (ex.: integração WorkWell)
│  │  │  ├─ Empresas.jsx
│  │  │  ├─ Home.jsx
│  │  │  └─ NoticiasAmbientais.jsx
│  │  ├─ App.jsx
│  │  ├─ index.css
│  │  └─ main.jsx
│  ├─ index.html
│  ├─ vite.config.js
│  ├─ package.json
│  └─ node_modules/
│
├─ README.md                    # Este arquivo (visão geral)
├─ package-lock.json
└─ node_modules/                # (se criado na raiz, opcional)
```

- `backend/` concentra a API de dados de profissionais, servindo o frontend GreenHub.
- `frontend/` é a aplicação React que constrói toda a experiência visual da plataforma.
- O código do WorkWell (ESP32 + MQTT + Node-RED + dashboard React) pode ficar em outro repositório ou em uma pasta dedicada (`workwell-iot/`) caso seja versionado junto.

***

## 🌐 GreenHub – Plataforma Web em React

### Funcionalidades principais

- Página inicial com:
  - Hero apresentando a proposta da plataforma.
  - Botões para explorar profissionais, criar perfil e acesso de empresas.
  - Lista de profissionais com filtros (nome, área, cidade, tecnologias).
  - Toggle para exibir apenas perfis recomendados (consumindo `recomendados.json`).
- Páginas complementares (`src/pages/`):
  - `Dashboard.jsx` – área para visualização de gráficos (ex.: dados ambientais ou de estresse).
  - `Empresas.jsx` – visão voltada a empresas e vagas.
  - `CadastroEmpresa.jsx` – fluxo de cadastro de empresas.
  - `About.jsx` e `NoticiasAmbientais.jsx` – conteúdo institucional e notícias relacionadas a futuro do trabalho e sustentabilidade.
- Componentes reutilizáveis em `src/components/` para seções como TrendingSkills, ValueSection, CommunitySection, StoriesSection, além de Header, Footer e modais.
- Modo claro/escuro controlado na `Home` via estado `dark` e classes condicionais em todos os blocos principais.

### Tecnologias principais (frontend)

- React + React Router para SPA.
- Axios para consumo da API do backend.
- Vite como bundler (conforme presença do `vite.config.js`).
- CSS utilitário / Tailwind-like para estilização responsiva.

### Como rodar o backend (API de perfis)

```bash
cd backend
npm install
npm start    # ou node server.js, conforme script definido
```

Endpoints típicos (ajuste para o que estiver configurado em `server.js`):

- `GET /api/profissionais` → lê `data/profiles.json`
- `GET /api/recomendacoes` → lê `data/recomendados.json`

### Como rodar o frontend (GreenHub)

```bash
cd frontend
npm install
npm run dev   # ou npm start
```

Acesse no navegador:

```text
http://localhost:5173
ou
http://localhost:3000
```

Certifique-se de que as URLs usadas em Axios apontam para o backend (ex.: `http://localhost:5000/api/...`).

***

## 🧠 WorkWell – Sistema IoT de Monitoramento de Estresse

### Descrição geral

WorkWell é um sistema IoT que monitora condições relacionadas ao estresse (como temperatura, umidade e luminosidade do ambiente) e envia esses dados para um backend via MQTT, permitindo visualização em dashboards.

Camadas previstas:

1. **ESP32 + sensores**
   - Lê DHT22 (temperatura/umidade) e luminosidade em um pino analógico.
   - Publica leituras em tópicos MQTT usando a biblioteca PubSubClient.
2. **Broker MQTT / Node-RED**
   - Recebe mensagens do ESP32 em tópicos específicos `/lifonix/workwell/...`.
   - Processa, normaliza e opcionalmente expõe dados via HTTP/WebSocket para dashboards.
3. **Dashboard React (WorkWell ou GreenHub)**
   - Consome os dados enviados pelo Node-RED.
   - Exibe gráficos de evolução, gauges e indicadores de status de conforto/estresse.

***

## 🔬 Simulação no Wokwi (WorkWell)

O firmware do WorkWell pode ser testado sem hardware físico usando o simulador online **Wokwi**.

- Projeto de simulação do ESP32 + DHT22 + MQTT do WorkWell:  
  `https://wokwi.com/projects/447651098360541185`.

No projeto Wokwi, o ESP32:

- Conecta à rede Wi‑Fi de simulação (`Wokwi-GUEST`) usando a API padrão de WiFi do ESP32.
- Usa a biblioteca PubSubClient para publicar e assinar mensagens MQTT em um broker remoto.
- Publica dados em tópicos como:
  - `/lifonix/workwell/env` – JSON com `temp`, `umid` e `lum` (ex.: `{"temp":24.5,"umid":55.2,"lum":73}`).
  - `/lifonix/workwell/attrs` – estado da saída (ex.: `s|on` / `s|off`).
  - `/lifonix/workwell/attrs/h` – umidade isolada.
  - `/lifonix/workwell/attrs/t` – temperatura isolada.
  - `/lifonix/workwell/status` – status de conexão (`online` / `offline`).
- Recebe comandos no tópico `/lifonix/workwell/cmd`, usando mensagens como `workwell@on|` e `workwell@off|` para ligar/desligar a saída digital (`default_D4`).

Esse cenário permite validar:

- Publicação de dados ambientais em tempo quase real.
- Consumo dos tópicos pelo Node-RED (ou outro cliente MQTT) e visualização em dashboards.
- Lógica de comando → resposta, controlando saídas do ESP32 via MQTT.

***

## ⚙️ Fluxo WorkWell no Node‑RED

O fluxo principal do WorkWell no Node‑RED é dividido em dois blocos: ingestão e API para o frontend.

### 1. Bloco de ingestão e cálculo de estresse

Fluxo visual aproximado:

```text
ESP32 ENV JSON (MQTT in)
/lifonix/workwell/env
   ↓
JSON → Objeto (node JSON)
   ↓
Calcula estresse (function)
   ↙                ↘
dados.json (file)   Ponto (time,temp,umid,lum,stress) → (ex.: gráfico/timeline)
```

- **ESP32 ENV JSON (MQTT in)**
  - Assina o tópico `/lifonix/workwell/env` e recebe mensagens JSON com `temp`, `umid` e `lum`.
- **JSON → Objeto (node JSON)**
  - Converte `msg.payload` de string JSON para objeto JavaScript.
- **Function “Calcula estresse”**
  - Lê `msg.payload.temp`, `msg.payload.umid`, `msg.payload.lum`.
  - Calcula um índice simples de estresse, aumentando o valor quando temperatura sai da faixa confortável, umidade está fora do ideal e luminosidade está muito baixa ou alta.
  - Adiciona `msg.payload.stress` e timestamp (ex.: `msg.payload.time = Date.now()`).
- **dados.json (file)**
  - Salva as leituras em arquivo (um ponto por linha).
- **Ponto (time,temp,umid,lum,stress)**
  - Prepara os dados no formato esperado pelo gráfico/timeline da dashboard (por exemplo, convertendo cada linha em `{ time, temp, umid, lum, stress }`).

### 2. Bloco de API para o GreenHub

Fluxo visual aproximado:

```text
API /dadosGreenHub (HTTP in)
   ↓
Ler dados.json (file in)
   ↓
Linhas → Array JSON (function)
   ↓
Resposta JSON (HTTP response)
```

- **API /dadosGreenHub (HTTP in)**
  - Endpoint HTTP (método GET) exposto por Node‑RED, ex.: `/dadosGreenHub`.
- **Ler dados.json (file in)**
  - Lê o arquivo `dados.json` onde as leituras foram armazenadas.
- **Linhas → Array JSON (function)**
  - Converte cada linha do arquivo em um objeto JSON com `{ time, temp, umid, lum, stress }`.
  - Monta `msg.payload` como um array com todos os pontos.
- **Resposta JSON (HTTP response)**
  - Devolve o array em `msg.payload` como resposta HTTP para o frontend GreenHub consumir.

Com esse fluxo, o GreenHub pode chamar `GET /dadosGreenHub` (apontando para o Node‑RED) e renderizar um gráfico de histórico WorkWell em `Dashboard.jsx`.

### Importar o fluxo WorkWell no Node‑RED (JSON)

Em vez de montar o fluxo do zero, é possível importar diretamente o fluxo JSON do WorkWell no editor do Node‑RED:

1. Copie todo o conteúdo JSON abaixo (incluindo `[` e `]`).
2. No Node‑RED, clique no menu no canto superior direito (três barras) → **Import** / **Importar**.
3. Na aba “Clipboard”, cole o JSON no campo de texto.
4. Clique em **Import** e escolha o workspace onde o fluxo será criado.
5. Clique em **Deploy** para aplicar as alterações.

Opcionalmente, você pode salvar esse JSON em um arquivo `.json` e usar a opção de importação via arquivo em vez de via clipboard.

Fluxo JSON do WorkWell (Node‑RED):

```json
[
  {
    "id": "7f9ee98e15b102fc",
    "type": "tab",
    "label": "Fluxo 1",
    "disabled": false,
    "info": "",
    "env": []
  },
  {
    "id": "d903a16bb375d3f6",
    "type": "mqtt in",
    "z": "7f9ee98e15b102fc",
    "name": "ESP32 ENV JSON",
    "topic": "/lifonix/workwell/env",
    "qos": "0",
    "datatype": "auto",
    "broker": "mqtt_broker_cfg",
    "nl": false,
    "rap": true,
    "rh": 0,
    "inputs": 0,
    "x": 490,
    "y": 320,
    "wires": [
      [
        "4dc5c9a2e981cadd"
      ]
    ]
  },
  {
    "id": "4dc5c9a2e981cadd",
    "type": "json",
    "z": "7f9ee98e15b102fc",
    "name": "JSON → Objeto",
    "property": "payload",
    "action": "",
    "pretty": false,
    "x": 710,
    "y": 320,
    "wires": [
      [
        "5e8214940262f44c"
      ]
    ]
  },
  {
    "id": "5e8214940262f44c",
    "type": "function",
    "z": "7f9ee98e15b102fc",
    "name": "Calcula estresse",
    "func": "// Espera msg.payload = { temp: 25.3, umid: 60.1, lum: 42 }\n\nconst t = Number(msg.payload.temp);\nconst h = Number(msg.payload.umid);\nconst l = Number(msg.payload.lum);\n\nlet stress = 0;\n\n// Temperatura: faixa confortável ~20–26 ºC\nif (t < 20 || t > 26) stress += 1;\nif (t < 18 || t > 30) stress += 1;\n\n// Umidade: confortável ~40–80%\nif (h < 40 || h > 80) stress += 1;\n\n// Luminosidade: exemplo 30–70 (ajuste depois)\nif (l < 30 || l > 70) stress += 1;\n\nconst ponto = {\n    time: Date.now(),\n    temp: t,\n    umid: h,\n    lum: l,\n    stress: stress\n};\n\n// Vamos salvar em formato JSON em uma linha\nmsg.payload = JSON.stringify(ponto);\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 960,
    "y": 320,
    "wires": [
      [
        "96f9542af46a1b4b",
        "f0fb14156856eda9"
      ]
    ]
  },
  {
    "id": "96f9542af46a1b4b",
    "type": "file",
    "z": "7f9ee98e15b102fc",
    "name": "dados.json",
    "filename": "dados.json",
    "filenameType": "str",
    "appendNewline": true,
    "createDir": false,
    "overwriteFile": "false",
    "encoding": "none",
    "x": 1200,
    "y": 320,
    "wires": [
      []
    ]
  },
  {
    "id": "f0fb14156856eda9",
    "type": "debug",
    "z": "7f9ee98e15b102fc",
    "name": "Ponto (time,temp,umid,lum,stress)",
    "active": true,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "payload",
    "targetType": "msg",
    "statusVal": "",
    "statusType": "auto",
    "x": 1260,
    "y": 380,
    "wires": []
  },
  {
    "id": "http_in_greenhub",
    "type": "http in",
    "z": "7f9ee98e15b102fc",
    "name": "API /dadosGreenHub",
    "url": "/dadosGreenHub",
    "method": "get",
    "swaggerDoc": "",
    "x": 480,
    "y": 560,
    "wires": [
      [
        "file_in_greenhub"
      ]
    ]
  },
  {
    "id": "file_in_greenhub",
    "type": "file in",
    "z": "7f9ee98e15b102fc",
    "name": "Ler dados.json",
    "filename": "dados.json",
    "filenameType": "str",
    "format": "utf8",
    "chunk": false,
    "sendError": true,
    "encoding": "none",
    "allProps": false,
    "x": 710,
    "y": 560,
    "wires": [
      [
        "func_lines_to_array"
      ]
    ]
  },
  {
    "id": "func_lines_to_array",
    "type": "function",
    "z": "7f9ee98e15b102fc",
    "name": "Linhas → Array JSON",
    "func": "// Conteúdo de msg.payload é um texto com várias linhas JSON\n// Vamos transformar em array de objetos\n\nconst texto = msg.payload || \"\";\nconst linhas = texto.split(/\\r?\\n/).filter(l => l.trim() !== \"\");\n\nconst arr = [];\n\nfor (const linha of linhas) {\n  try {\n    const obj = JSON.parse(linha);\n    arr.push(obj);\n  } catch (e) {\n    // ignora linha inválida\n  }\n}\n\nmsg.payload = arr;\n\nmsg.headers = msg.headers || {};\nmsg.headers[\"content-type\"] = \"application/json\";\nmsg.headers[\"Access-Control-Allow-Origin\"] = \"*\";\nmsg.headers[\"Access-Control-Allow-Methods\"] = \"GET, OPTIONS\";\nmsg.headers[\"Access-Control-Allow-Headers\"] = \"Content-Type\";\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 990,
    "y": 560,
    "wires": [
      [
        "http_resp_greenhub"
      ]
    ]
  },
  {
    "id": "http_resp_greenhub",
    "type": "http response",
    "z": "7f9ee98e15b102fc",
    "name": "Resposta JSON",
    "statusCode": "",
    "headers": {},
    "x": 1260,
    "y": 560,
    "wires": []
  },
  {
    "id": "mqtt_broker_cfg",
    "type": "mqtt-broker",
    "name": "Broker ESP32",
    "broker": "44.223.43.74",
    "port": "1883",
    "tls": "",
    "clientid": "",
    "usetls": false,
    "protocolVersion": "4",
    "keepalive": "60",
    "cleansession": true,
    "birthTopic": "",
    "birthQos": "0",
    "birthPayload": "",
    "birthMsg": {},
    "closeTopic": "",
    "closeQos": "0",
    "closePayload": "",
    "closeMsg": {},
    "willTopic": "",
    "willQos": "0",
    "willPayload": "",
    "willMsg": {},
    "userProps": "",
    "sessionExpiry": ""
  }
]
```



***

## 🚀 Passo a passo para rodar o IoT WorkWell

### 1. Abrir a simulação no Wokwi

Foto da simulação
<img width="929" height="733" alt="wokwicircuito" src="https://github.com/user-attachments/assets/ebe9ab81-6b0b-4f64-83ec-b686d9aed126" />


1. Acesse o projeto Wokwi do WorkWell:  
   `https://wokwi.com/projects/447651098360541185`.
2. Confira no código:
   - Conexão com `Wokwi-GUEST`.
   - Uso de `WiFi.h`, `PubSubClient.h` e `DHT.h`.
   - Publicação nos tópicos `/lifonix/workwell/...` e envio de JSON em `/lifonix/workwell/env`.

### 2. Verificar/ajustar o broker MQTT

1. No código, o broker está definido como algo similar a:

   ```cpp
   const char* default_BROKER_MQTT = "44.223.43.74";
   const int   default_BROKER_PORT = 1883;
   ```


### 3. Rodar o ESP32 no Wokwi

1. Clique em **Start** no Wokwi para iniciar a simulação.
2. Observe o monitor serial para mensagens de inicialização, conexão ao Wi‑Fi e ao broker MQTT.
3. Após conectado, o ESP32 começa a ler DHT22 e luminosidade e a publicar nos tópicos configurados, incluindo o JSON em `/lifonix/workwell/env`.

### 4. Conectar Node‑RED ao broker e aos tópicos

1. Inicie o Node‑RED:

   ```bash
   node-red
   ```

2. Acesse o editor em `http://localhost:1880`.
3. Adicione um nó **MQTT in** e configure o mesmo broker/porta do ESP32.
4. Importe o JSON fornecido acima do fluxo do Node-Red, ele ficara assim:
   
<img width="1614" height="641" alt="Node-Red-Circuito" src="https://github.com/user-attachments/assets/dd9bf8c8-1917-4c00-9cc9-e3e0091e4dc1" />


5. De o deploy no fluxo.
6. Abra o site da GreenHub

***

## 🔗 Conexão entre GreenHub e WorkWell

<img width="1452" height="912" alt="Dashboard" src="https://github.com/user-attachments/assets/2d896a13-ae9e-4a18-b383-bee6572b282f" />


- GreenHub é a fachada web que usuários acessam para visualizar perfis, histórias, valores e, futuramente, dados agregados de bem-estar.
- WorkWell complementa com uma vertente IoT de bem-estar, monitorando condições ambientais e, em versões futuras, indicadores mais diretos de estresse.
- A página `Dashboard.jsx` do GreenHub pode funcionar como porta de entrada visual para os dados do WorkWell (por exemplo, embutindo o dashboard WorkWell ou consumindo a API `/dadosGreenHub`).
- As dashboards de bem-estar, estresse ambiental e engajamento ficam disponíveis para as empresas que integram a comunidade GreenHub, permitindo acompanhar condições de trabalho e apoiar decisões mais humanas e sustentáveis.

Essa integração fortalece a narrativa de uma solução completa para o futuro do trabalho.

***

## 🧪 Testes e validação

- Testes manuais no GreenHub:
  - Carregamento da Home, filtros, modais e alternância de tema.
  - Consumo da API de `profiles.json` e `recomendados.json`.
- Testes do WorkWell:
  - Simulação no Wokwi com envio de dados e recepção de comandos.
  - Assinatura dos tópicos MQTT por Node‑RED e verificação em gráfico ou debug.
  - Verificação do endpoint `/dadosGreenHub` servindo o JSON para o frontend.

***

## Link do video:

https://youtu.be/TcRKw3Z6CBc 

## 👥 Equipe

- Arthur Serrano Veloso – RM 561542
- Hyann dos Santos Espindas – RM 563421"
