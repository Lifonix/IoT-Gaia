
# GreenHub & WorkWell – Plataforma Web + Sistema IoT de Bem-Estar no Trabalho

Este repositório reúne dois pilares de uma mesma solução para o futuro do trabalho.[web:296][web:335]  

- **GreenHub / Lifonix (frontend + backend)** – Plataforma web em **React** com um backend Node simples para servir perfis e recomendações, conectando talentos, empresas e oportunidades, com foco em propósito, competências do futuro e economia verde.[web:309][web:320]  
- **WorkWell (IoT)** – Sistema baseado em **ESP32 + MQTT + Node-RED + dashboard React** para monitorar indicadores de estresse e bem-estar de trabalhadores em casa ou no escritório.[web:290][web:293]  

Juntos, GreenHub e WorkWell formam um ecossistema que conecta pessoas, tecnologia e bem-estar, alinhado às transformações do futuro do trabalho. 

---

## 🔭 Visão geral da solução

### GreenHub (site e API)

- Aplicação **SPA em React** no diretório `frontend/`, com páginas como Home, Dashboard, Empresas, Cadastro de Empresas, About e Notícias Ambientais.[web:309][web:320]  
- Backend Node/Express simples em `backend/`, servindo dados de **profissionais** e **recomendados** a partir de arquivos JSON.[web:293][web:303]  
- Tela inicial com hero, botões de ação, lista de profissionais com filtros e seções institucionais (Trending Skills, Valores, Métricas da Comunidade, Histórias).[web:309][web:320]  
- Suporte a **modo claro/escuro** controlado na Home.[web:244][web:320]  

### WorkWell (IoT + bem-estar)

- Dispositivo com **ESP32** coleta dados de sensores (ex.: temperatura, umidade e luminosidade) como proxies para condições de estresse.[web:293][web:298]  
- Broker MQTT recebe e encaminha mensagens usando tópicos dedicados ao WorkWell.[web:347][web:355]  
- **Node-RED** assina esses tópicos, calcula um índice de estresse, grava em arquivo e expõe uma API `/dadosGreenHub` que a dashboard em React consome.[web:290][web:368]  

---

## 🧱 Arquitetura de pastas do repositório

A estrutura atual do projeto GreenHub está organizada da seguinte forma.[web:331][web:336]  

```
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

- **`backend/`** concentra a API de dados de profissionais, servindo o frontend GreenHub.[web:293][web:303]  
- **`frontend/`** é a aplicação React que constrói toda a experiência visual da plataforma.[web:309][web:320]  
- O código do WorkWell (ESP32 + MQTT + Node-RED + dashboard React) pode ficar em outro repositório ou em uma pasta dedicada (`workwell-iot/`) caso seja versionado junto.

---

## 🌐 GreenHub – Plataforma Web em React

### Funcionalidades principais

- Página inicial com.[web:309][web:320]  
  - Hero apresentando a proposta da plataforma.  
  - Botões para explorar profissionais, criar perfil e acesso de empresas.  
  - Lista de profissionais com filtros (nome, área, cidade, tecnologias).  
  - Toggle para exibir apenas perfis recomendados (consumindo `recomendados.json`).[web:293][web:303]  
- Páginas complementares (`src/pages/`):  
  - `Dashboard.jsx` – área para visualização de gráficos (ex.: dados ambientais ou de estresse).  
  - `Empresas.jsx` – visão voltada a empresas e vagas.  
  - `CadastroEmpresa.jsx` – fluxo de cadastro de empresas.  
  - `About.jsx` e `NoticiasAmbientais.jsx` – conteúdo institucional e notícias relacionadas a futuro do trabalho e sustentabilidade.  
- Componentes reutilizáveis em `src/components/` para seções como TrendingSkills, ValueSection, CommunitySection, StoriesSection, além de Header, Footer e modais.
- **Modo claro/escuro** controlado na `Home` via estado `dark` e classes condicionais em todos os blocos principais.[web:244][web:320]  

### Tecnologias principais (frontend)

- React + React Router para SPA.[web:309][web:320]  
- Axios para consumo da API do backend.[web:293][web:320]  
- Vite como bundler (conforme presença do `vite.config.js`).[web:322][web:320]  
- CSS utilitário / Tailwind-like para estilização responsiva.[web:312][web:316]  

### Como rodar o backend (API de perfis)

```
cd backend
npm install
npm start    # ou node server.js, conforme script definido
```

Endpoints típicos (ajuste para o que estiver configurado em `server.js`). 

- `GET /api/profissionais` → lê `data/profiles.json`  
- `GET /api/recomendacoes` → lê `data/recomendados.json`  

### Como rodar o frontend (GreenHub)

```
cd frontend
npm install
npm run dev   # ou npm start
```

Acesse no navegador.[web:322][web:320]  

```
http://localhost:5173
ou
http://localhost:3000
```

Certifique-se de que as URLs usadas em Axios apontam para o backend (ex.: `http://localhost:5000/api/...`). 

---

## 🧠 WorkWell – Sistema IoT de Monitoramento de Estresse

### Descrição geral

WorkWell é um sistema IoT que monitora condições relacionadas ao estresse (como temperatura, umidade e luminosidade do ambiente) e envia esses dados para um backend via MQTT, permitindo visualização em dashboards. 

Camadas previstas.[web:290][web:293]  

1. **ESP32 + sensores**  
   - Lê DHT22 (temperatura/umidade) e luminosidade em um pino analógico.
   - Publica leituras em tópicos MQTT usando a biblioteca PubSubClient.
2. **Broker MQTT / Node-RED**  
   - Recebe mensagens do ESP32 em tópicos específicos `/lifonix/workwell/...`. 
   - Processa, normaliza e opcionalmente expõe dados via HTTP/WebSocket para dashboards.
3. **Dashboard React (WorkWell ou GreenHub)**  
   - Consome os dados enviados pelo Node-RED.  
   - Exibe gráficos de evolução, gauges e indicadores de status de conforto/estresse. 

---

## 🔬 Simulação no Wokwi (WorkWell)

O firmware do WorkWell pode ser testado sem hardware físico usando o simulador online **Wokwi**.

- Projeto de simulação do ESP32 + DHT22 + MQTT do WorkWell:  
  `https://wokwi.com/projects/447651098360541185`.

No projeto Wokwi, o ESP32.[web:347][web:350]  

- Conecta à rede Wi‑Fi de simulação (`Wokwi-GUEST`) usando a API padrão de WiFi do ESP32.
- Usa a biblioteca **PubSubClient** para publicar e assinar mensagens MQTT em um broker remoto. 
- Publica dados em tópicos como.  
  - `/lifonix/workwell/env` – JSON com `temp`, `umid` e `lum` (ex.: `{"temp":24.5,"umid":55.2,"lum":73}`).
  - `/lifonix/workwell/attrs` – estado da saída (ex.: `s|on` / `s|off`).  
  - `/lifonix/workwell/attrs/h` – umidade isolada.  
  - `/lifonix/workwell/attrs/t` – temperatura isolada.  
  - `/lifonix/workwell/status` – status de conexão (`online` / `offline`).
- Recebe comandos no tópico `/lifonix/workwell/cmd`, usando mensagens como `workwell@on|` e `workwell@off|` para ligar/desligar a saída digital (`default_D4`).

Esse cenário permite validar.

- Publicação de dados ambientais em tempo quase real.  
- Consumo dos tópicos pelo Node-RED (ou outro cliente MQTT) e visualização em dashboards.  
- Lógica de comando → resposta, controlando saídas do ESP32 via MQTT.  

---

## ⚙️ Fluxo WorkWell no Node‑RED

O fluxo principal do WorkWell no Node‑RED é dividido em **dois blocos**: ingestão e API para o frontend.

### 1. Bloco de ingestão e cálculo de estresse

Fluxo visual aproximado:

```
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
  - Calcula um índice simples de estresse (exemplo):  
    - Estresse maior com temperatura alta, umidade extrema e baixa luminosidade.  
  - Adiciona `msg.payload.stress` e timestamp (ex.: `msg.payload.time = Date.now()`).
- **dados.json (file)**  
  - Salva as leituras em arquivo (um ponto por linha).  
- **Ponto (time,temp,umid,lum,stress)**  
  - Prepara os dados no formato esperado pelo gráfico/timeline da dashboard (por exemplo, convertendo cada linha em `{ time, temp, umid, lum, stress }`).  

### 2. Bloco de API para o GreenHub

Fluxo visual aproximado:

```
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

---

## 🚀 Passo a passo para rodar o IoT WorkWell

### 1. Abrir a simulação no Wokwi

1. Acesse o projeto Wokwi do WorkWell:  
   `https://wokwi.com/projects/447651098360541185`.
2. Confira no código.  
   - Conexão com `Wokwi-GUEST`.  
   - Uso de `WiFi.h`, `PubSubClient.h` e `DHT.h`.  
   - Publicação nos tópicos `/lifonix/workwell/...` e envio de JSON em `/lifonix/workwell/env`. 

### 2. Verificar/ajustar o broker MQTT

1. No código, o broker está definido como algo similar a.

   ```
   const char* default_BROKER_MQTT = "44.223.43.74";
   const int   default_BROKER_PORT = 1883;
   ```

2. Se você usar outro broker (Mosquitto local, por exemplo), altere esses valores para o IP/host e porta corretos e certifique-se de que o Node‑RED consegue acessar o mesmo broker.

### 3. Rodar o ESP32 no Wokwi

1. Clique em **Start** no Wokwi para iniciar a simulação.
2. Observe o monitor serial para mensagens de inicialização, conexão ao Wi‑Fi e ao broker MQTT.
3. Após conectado, o ESP32 começa a ler DHT22 e luminosidade e a publicar nos tópicos configurados, incluindo o JSON em `/lifonix/workwell/env`. 

### 4. Conectar Node‑RED ao broker e aos tópicos

1. Inicie o Node‑RED.[web:290][web:368]  

   ```
   node-red
   ```  

2. Acesse o editor em `http://localhost:1880`.
3. Adicione um **nó MQTT in** e configure o mesmo broker/porta do ESP32.
4. Assine o tópico principal de ambiente:  

   - `/lifonix/workwell/env`.

5. Conecte o nó MQTT ao fluxo de ingestão (JSON → Calcula estresse → dados.json / Ponto).
6. Clique em **Deploy** e confirme que o nó MQTT está “connected”. 

### 5. (Opcional) Criar dashboard Node‑RED

1. Garanta que o `node-red-dashboard` esteja instalado/ativado.
2. Crie um **tab** e um **group** para o WorkWell.  
3. Adicione componentes de dashboard (gauge, chart) conectados ao nó `Ponto (time,temp,umid,lum,stress)` ou diretamente ao JSON já processado. 
4. Acesse `http://localhost:1880/ui` para ver os dados em tempo real.

### 6. Enviar comandos para o WorkWell (ligar/desligar saída)

1. Crie um nó **MQTT out** apontando para o tópico `/lifonix/workwell/cmd`.
2. Conecte-nos a dois nós `inject`:  
   - Payload `workwell@on|` → liga saída.  
   - Payload `workwell@off|` → desliga saída.  
3. Faça deploy e clique nos injects para enviar os comandos; veja o estado mudar no monitor serial e na saída D4. 

### 7. Integrar com dashboard React (WorkWell / GreenHub)

1. No Node‑RED, use o bloco **API /dadosGreenHub** (HTTP in) ligado ao fluxo `Ler dados.json` → `Linhas → Array JSON` → `Resposta JSON` para expor o histórico pelo endpoint `/dadosGreenHub`. 
2. Na dashboard React (WorkWell ou `Dashboard.jsx` do GreenHub), use Axios ou fetch para consumir esse endpoint periodicamente e atualizar os gráficos.
3. Assim, a mesma simulação do Wokwi alimenta tanto o Node‑RED quanto a interface React da solução WorkWell.

---

## 🔗 Conexão entre GreenHub e WorkWell

- GreenHub é a **fachada web** que usuários acessam para visualizar perfis, histórias, valores e, futuramente, dados agregados de bem-estar.
- WorkWell complementa com uma vertente **IoT de bem-estar**, monitorando condições ambientais e, em versões futuras, indicadores mais diretos de estresse.
- A página `Dashboard.jsx` do GreenHub pode funcionar como porta de entrada visual para os dados do WorkWell (por exemplo, embutindo o dashboard WorkWell ou consumindo a API `/dadosGreenHub`).

Essa integração fortalece a narrativa de uma solução completa para o futuro do trabalho: **talentos + empresas + bem-estar monitorado**.

---

## 🧪 Testes e validação

- Testes manuais no GreenHub.
  - Carregamento da Home, filtros, modais e alternância de tema.  
  - Consumo da API de `profiles.json` e `recomendados.json`.  
- Testes do WorkWell.
  - Simulação no Wokwi com envio de dados e recepção de comandos.  
  - Assinatura dos tópicos MQTT por Node-RED e verificação em gráfico ou debug.  
  - Verificação do endpoint `/dadosGreenHub` servindo o JSON para o frontend.  

---


## 👥 Equipe

- Arthur Serrano Veloso – RM 561542  
- Hyann dos Santos Espindas – RM 563421  




