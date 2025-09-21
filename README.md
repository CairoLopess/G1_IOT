🚨 Projeto GrupoCairo

Integrante: Cairo

📖 Visão Geral
O projeto consiste em um sistema de monitoramento simples e eficaz baseado em um ESP32. Ele utiliza um sensor de movimento (PIR) e um sensor de som (microfone) para detectar atividades em um ambiente. Quando movimento ou um ruído acima de um limiar predefinido é detectado, um buzzer é acionado como um alerta sonoro imediato.

Este sistema é ideal para aplicações de segurança residencial de baixo custo, servindo como um protótipo para monitoramento de presença e ruídos.

🛠️ Componentes Utilizados
📡 Sensores
Sensor de Presença (PIR): Detecta movimento no ambiente.

Sensor de Som (Microfone): Captura níveis de ruído.

🎛️ Atuador
Buzzer: Emite um alerta sonoro quando ativado.

🧠 Controlador
ESP32

⚙️ Como Funciona
O fluxo de operação do sistema é o seguinte:

O ESP32 monitora continuamente as leituras do sensor de movimento e do sensor de som.

Se o sensor PIR detectar movimento ou se o microfone captar um som que ultrapasse o limiar configurado, um evento de alerta é gerado.

Ao detectar o evento, o ESP32 envia um sinal para o buzzer.

O buzzer é ativado, emitindo um alarme sonoro para notificar sobre a atividade detectad
