# GS-Edge_Computing_-_Computer_Systems

Global Solution desenvolvida por Cainã Sandes Batista, RM: 568571. 
Apresentado ao curso de Engenharia de Software. <hr>

<h3>Nome do projeto:</h3> 
<p>SAOA Dispatch: Sistema Autônomo de Orientação e Auxílio</p>

<h3>Descrição do caso:</h3> 
<p>
  O projeto foi desenvolvido para a Global Solution FIAP 2025 com o tema <b>"O Futuro do Trabalho – Conectando pessoas, competências e propósito por meio da tecnologia"</b>. O objetivo é propor uma solução que use inovação e tecnologia para tornar o futuro do trabalho mais justo, inclusivo e sustentável, alinhada com os ODS 4, 8, 9 e 10.
</p>

<h3>Problema e justificativa acadêmica:</h3> 
<p>
A produtividade e a segurança em empresas com alta demanda por tarefas técnicas e repetitivas são drasticamente comprometidas por dois fatores interligados no cenário do Futuro do Trabalho:
</p>

<ul>
  <li>Crise de competências: Há uma crescente escassez de talentos qualificados em áreas como manutenção e operações, resultando em uma grande lacuna de habilidades (Maintenance Talent Crunch, conforme BCG e Maintenance World). Isso força a contratação de mão de obra inexperiente ou mal treinada.</li>
  <li>Risco de Erro e Automação: O mal treinamento leva à inconsistência na execução, aumentando o erro humano e os custos operacionais. Esta falta de padronização torna a equipe mais vulnerável à substituição por automação e robótica, conforme alertado por estudos da McKinsey e IPEA.</li>
</ul>

<p>
O projeto SAOA Dispatch se justifica pela necessidade de uma ferramenta que atue na requalificação profissional e na padronização, entregando conhecimento no ponto de execução para mitigar esses riscos e alinhar o trabalhador com a nova economia de competências (OECD).
</p>

<h3>Solução: SAOA Dispatch - Instrutor Digital</h3>
<p>
O SAOA Dispatch é um sistema IoT (futuramente) construído em Arduino que utiliza uma Máquina de Estados para guiar o operador com precisão e fornecer suporte imediato. A solução consiste em:
</p>

<ul>
  <li>Padronização passo a passo: O sistema exibe uma instrução de cada vez no LCD, garantindo que o procedimento correto seja seguido (ODS 4 - Educação de Qualidade).</li>
  <li>Feedback em tempo real: O operador usa o Botão Verde para confirmar e o Botão Vermelho para sinalizar um erro ou necessidade de ajuda.</li>
  <li>Sistema de auxílio hierárquico: Ao apertar o Botão Vermelho, o sistema inicia a rotina de erro: 1. Dica Simples (relembra o detalhe do passo) -> 2. Verificação (sugere checar o passo anterior) -> 3. Chamada de supervisor (alerta a central via Buzzer/LED, e futuramente, via MQTT).</li>
  <li>Registro de dados: A futura integração IoT permitirá coletar dados sobre erros (tentativas), tempo de execução e acionamentos de supervisão, crucial para o desenvolvimento da força de trabalho (Number Analytics).</li>
</ul>

<h3>Estrutura do projeto:</h3> <p>
<ul>
  <li>SAOA-Dispatch (folder)</li>
  <ul>
    <li>sketch.ino - Arquivo principal com toda a lógica de estados, I/O e base de dados.</li>
    <li>README.md - Este documento.</li>
  </ul>
</ul>

<h3>Dependências (Hardware e bibliotecas):</h3> <p>
Para rodar o projeto, você precisará dos seguintes componentes e bibliotecas:
</p>

<ul>
  <li>Hardware: Arduino Uno, LCD 16x2 I2C, Keypad 4x4, Módulo LED RGB (Ânodo Comum), Buzzer, Botão Verde, Botão Vermelho.</li>
  <li>Bibliotecas Arduino:
    <ul>
      <li>Wire.h (Padrão)</li>
      <li>LiquidCrystal_I2C.h (Frank de Brabander)</li>
      <li>Keypad.h (Mark Stanley, Alexander Brevig)</li>
    </ul>
  </li>
</ul>

<h3>Instruções de Uso (Fluxo MPV):</h3> <p>
</p>

<ol>
  <li>Inicialização: O LCD exibe "SAOA MPV v3.7".</li>
  <li>Seleção de Cargo (MENU_CARGO): Selecione o perfil desejado no Keypad:
    <ul>
      <li>Pressione 1 (Elétrica)</li>
      <li>Pressione 2 (Hidráulica)</li>
    </ul>
  </li>
  <li>Login (LOGIN): Digite a senha de 3 dígitos no Keypad.
    <ul>
      <li>A senha de teste é: 123</li>
    </ul>
  </li>
  <li>Seleção de tarefa (MENU_TAREFA): Escolha a tarefa no Keypad.
    <ul>
      <li>Pressione 1 (Única tarefa implementada no MPV)</li>
    </ul>
  </li>
  <li>Execução passo a passo (EXECUTAR_TAREFA):
    <ul>
      <li>Pressione Botão Verde para confirmar o passo e avançar.</li>
      <li>Pressione Botão Vermelho para acionar a rotina de ajuda.</li>
    </ul>
  </li>
  <li>Menu final (MENU_FINAL): Após a conclusão ou falha crítica:
    <ul>
      <li>Pressione 1 para fazer uma Nova Tarefa.</li>
      <li>Pressione 2 para Mudar de Cargo.</li>
      <li>Pressione 3 para Encerrar o sistema.</li>
    </ul>
  </li>
</ol>

<h3>Desenvolvimento:</h3> 
<p>
A aplicação foi desenvolvida seguindo os requisitos das disciplinas de Front-End Design e Web Development, utilizando HTML, CSS e JavaScript para composição estrutural, estilização, interatividade e exibição dinâmica dos dados dos profissionais.  
Foram aplicados:
</p>

<ul>

  <li>HTML</li>
  <ul>
    <li>sketch.ino - Uso da estrutura de um loop de setup() e loop() com controle de fluxo por Máquina de Estados.</li>
  </ul>

  <li>Lógica de código</li>
  <ul>
    <li>Máquina de Estados: Uso do enum Estado e switch no loop() principal para controle.</li>
    <li>Estrutura de Dados: Matrizes multi-dimensionais (String passosMPV) para armazenar o conteúdo hierárquico das tarefas.</li>
    <li>Anti-Flicker: Uso de flags (menuExibido, passoExibido) para otimizar o desenho do LCD.</li>
  </ul>

  <li>Conteúdo e propósito</li>
  <ul>
    <li>Referência aos ODS 4, 8, 9 e 10, reforçando alinhamento com temas de educação, trabalho digno, inovação e inclusão.</li>
  </ul>

</ul>

🔗 Link Wokwi: https://wokwi.com/projects/448275563830224897




🔗 Link repositório: [INSERIR LINK DO REPOSITÓRIO GIT AQUI]
</p>
