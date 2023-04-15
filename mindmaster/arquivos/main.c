#include "tela.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char coresJogo[10] = {'V', 'A', 'X', 'M', 'C', 'R', 'L', 'P', 'B'};

typedef struct {
  int x, y;
} ponto;

typedef struct {
  int x, y, tamanho, cor;
  char texto[];
} mensagemStr;

typedef struct {
  int xEsqSup, yEsqSup, xDirInf, yDifInf, cor;
  bool clicado;
} retangulo;

typedef struct {
  int quantidadeCaracteres, quantidadeTentativas, cores[10], pontos;
  ponto coordXYrato;
  retangulo quadrados[9], botoes[3], respostas[9][8];
  char chute[5], segredo[5], resposta[5], historicoChutes[9][5];
  bool localValido, acertou;
} jogoStr;

void inicializaCoresQuadrados(jogoStr *jogo) {
  for (int i = 0; i < 7; i++) {
    jogo->quadrados[i].cor = jogo->cores[i];
  }
}

void inicializaQuadrados(jogoStr *jogo) {
  for (int i = 0; i < 7; i++) {
    jogo->quadrados[i].xEsqSup = (i * 50) + 75;
    jogo->quadrados[i].yEsqSup = 650;
    jogo->quadrados[i].xDirInf = (i * 50) + 125;
    jogo->quadrados[i].yDifInf = 700;
  }

  inicializaCoresQuadrados(jogo);
  jogo->quadrados[7].xEsqSup = 75;
  jogo->quadrados[7].yEsqSup = 730;
  jogo->quadrados[7].xDirInf = 225;
  jogo->quadrados[7].yDifInf = 780;
  jogo->quadrados[7].cor = 3;
  jogo->quadrados[7].clicado = false;

  jogo->quadrados[8].xEsqSup = 275;
  jogo->quadrados[8].yEsqSup = 730;
  jogo->quadrados[8].xDirInf = 425;
  jogo->quadrados[8].yDifInf = 780;
  jogo->quadrados[8].cor = 2;
  jogo->quadrados[8].clicado = false;
}

void desenhaQuadradosClicaveis(int n, retangulo q[n]) {
  for (int i = 0; i < n; i++) {
    tela_retangulo(q[i].xEsqSup, q[i].yEsqSup, q[i].xDirInf, q[i].yDifInf, 2, 8,
                   q[i].cor);
  }
  tela_texto(150, 755, 20, 8, "Confirmar");
  tela_texto(350, 755, 20, 8, "Apagar");
}

void desenhaQuadradosChute(int n, int m, retangulo q[n][m]) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      tela_retangulo(q[i][j].xEsqSup, q[i][j].yEsqSup, q[i][j].xDirInf,
                     q[i][j].yDifInf, 2, 8, q[i][j].cor);
    }
  }
}

void armazenaCoordenadasXYrato(jogoStr *jogo) {
  jogo->coordXYrato.x = tela_rato_x();
  jogo->coordXYrato.y = tela_rato_y();
}

bool pontoNoQuadrado(ponto p, retangulo ret) {
  if ((p.x >= ret.xEsqSup && p.x <= ret.xDirInf) &&
      (p.y <= ret.yDifInf && p.y >= ret.yEsqSup)) {
    return true;
  }
  return false;
}

int quadradoNoPonto(int n, retangulo retangulos[n], ponto p) {
  for (int i = 0; i < n; i++) {
    if (pontoNoQuadrado(p, retangulos[i])) {
      return i;
    }
  }

  return -1;
}

void atualizaResposta(int repeticao, int indexChute, jogoStr *jogo) {
  for (int i = 0; i < 7; i++) {
    if (indexChute == i) {
      jogo->chute[repeticao] = coresJogo[i];
    }
  }
}

bool cliqueValido(jogoStr *jogo) {
  int indexQuadradoClicado =
      quadradoNoPonto(9, jogo->quadrados, jogo->coordXYrato);
  if (indexQuadradoClicado != -1) {
    jogo->quadrados[indexQuadradoClicado].clicado = true;
    return true;
  }
  return false;
}

void inicializaQuadradosChute(jogoStr *jogo) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 8; j++) {
      jogo->respostas[i][j].xEsqSup = 0;
      jogo->respostas[i][j].yEsqSup = 0;
      jogo->respostas[i][j].xDirInf = 0;
      jogo->respostas[i][j].yDifInf = 0;
      jogo->respostas[i][j].cor = 0;
    }
  }
}

void imprimeJogo(jogoStr *jogo) {
  inicializaQuadrados(jogo);
  desenhaQuadradosClicaveis(9, jogo->quadrados);
  desenhaQuadradosChute(9, 8, jogo->respostas);
  tela_circulo(tela_rato_x(), tela_rato_y(), 5, 2, vermelho, branco);
}

void imprimeInformacoes() {
  tela_texto(250, 30, 20, branco, "MINDMASTER");
  tela_texto(250, 50, 12, branco, "Clique na tecla D para desistir da partida");
  tela_texto(250, 70, 12, branco,
             "Tente adivinhar a combinação, selecione 4 cores e clique em "
             "\"Confirmar\"");
  tela_retangulo(0, 0, 500, 1000, 2, branco, 0);
}

void removeUltimoCaractereResposta(jogoStr *jogo) {
  for (int i = 0; i < 5; i++) {
    if (i == jogo->quantidadeCaracteres) {
      jogo->chute[i - 1] = ' ';
      jogo->quantidadeCaracteres--;
    }
  }
}

bool jaAdicionado(jogoStr *jogo, int indexQuadradoClique) {
  for (int i = 0; i < 5; i++) {
    if (coresJogo[indexQuadradoClique] == jogo->chute[i]) {
      return true;
    }
  }
  return false;
}

int corDoQuadrado(int indexCor, jogoStr jogo) {
  for (int i = 0; i < 9; i++) {
    if (indexCor == i) {
      return jogo.cores[i];
    }
  }
}

void atualizaQuadradoChute(jogoStr *jogo, int indexQuadradoClique,
                                int quantidadeCaracteres) {
  int espaco = 10;

  espaco *= jogo->quantidadeTentativas;

  if (quantidadeCaracteres > 3) {
    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .xEsqSup = ((quantidadeCaracteres - 1) * 50) + 105;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .yEsqSup = 40 + (jogo->quantidadeTentativas * 50) + espaco;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .xDirInf = ((quantidadeCaracteres - 1) * 50) + 155;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .yDifInf = 90 + (jogo->quantidadeTentativas * 50) + espaco;
  } else {
    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .xEsqSup = ((quantidadeCaracteres - 1) * 50) + 95;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .yEsqSup = 40 + (jogo->quantidadeTentativas * 50) + espaco;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .xDirInf = ((quantidadeCaracteres - 1) * 50) + 145;

    jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres]
        .yDifInf = 90 + (jogo->quantidadeTentativas * 50) + espaco;
  }

  jogo->respostas[jogo->quantidadeTentativas - 1][quantidadeCaracteres].cor =
      corDoQuadrado(indexQuadradoClique, *jogo);
}

void removeDadosQuadradoChute(jogoStr *jogo) {
  jogo->respostas[jogo->quantidadeTentativas - 1][jogo->quantidadeCaracteres]
      .xEsqSup = 0;
  jogo->respostas[jogo->quantidadeTentativas - 1][jogo->quantidadeCaracteres]
      .yEsqSup = 0;
  jogo->respostas[jogo->quantidadeTentativas - 1][jogo->quantidadeCaracteres]
      .xDirInf = 0;
  jogo->respostas[jogo->quantidadeTentativas - 1][jogo->quantidadeCaracteres]
      .yDifInf = 0;
  jogo->respostas[jogo->quantidadeTentativas - 1][jogo->quantidadeCaracteres]
      .cor = 0;
}

void inverteString(char resposta[5]) {
  char aux;

  int i = 0, j = 4 - 1;

  while (i < j) {
    aux = resposta[i];
    resposta[i] = resposta[j];
    resposta[j] = aux;
    i++, j--;
  }
}

int comparaCaracteres(char caractere1, char caractere2) {
  if (caractere1 == caractere2) {
    return 1;
  }
  return 0;
}

void troca(char resposta[5], int i, int j) {
  char aux;
  aux = resposta[i];
  resposta[i] = resposta[j];
  resposta[j] = aux;
}

void ordenaResposta(char resposta[5]) {
  char aux;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (resposta[i] == 'b' && resposta[j] == 'p')
        troca(resposta, j, i);
      else if (resposta[i] == 'n' && resposta[j] == 'b')
        troca(resposta, j, i);
      else if (resposta[i] == 'n' && resposta[j] == 'p')
        troca(resposta, j, i);
    }
  }
  inverteString(resposta);
}

void comparaChuteComSorteio(char resposta[5], char chute[5],
                            char coresSorteadas[5]) {
  int teste = 0;
  for (int i = 0; i < 5; i++) {
    resposta[i] = '\0';
  }

  for (int i = 0; i < 4; i++) {
    for (int k = 0; k < 4; k++) {
      teste = comparaCaracteres(chute[i], coresSorteadas[k]);
      if (teste == 1 && i == k) {
        resposta[i] = 'p';
      } else if (teste == 1) {
        resposta[i] = 'b';
      } else if (teste == 0 && resposta[i] == '\0') {
        resposta[i] = 'n';
      }
    }
  }
  ordenaResposta(resposta);
}

void atualizandoValoresQuadradosResposta(jogoStr *jogo) {
  for (int i = 0; i < 4; i++) {
    if (jogo->resposta[i] == 'p') {
      atualizaQuadradoChute(jogo, 7, i + 4);
    } else if (jogo->resposta[i] == 'b') {
      atualizaQuadradoChute(jogo, 8, i + 4);
    }
  }
}

bool testaResposta(char resposta[5]) {
  int contador = 0;
  for (int i = 0; i < 4; i++) {
    if (resposta[i] == 'p') {
      contador++;
    }
  }
  if (contador == 4) {
    return true;
  }
  return false;
}

void processaResposta(jogoStr *jogo) {
  atualizandoValoresQuadradosResposta(jogo);

  if (testaResposta(jogo->resposta)) {
    jogo->acertou = true;
  }
}

void limpaChute(jogoStr *jogo) { 
  strcpy(jogo->chute, "    \0"); 
}

void atualizaParaProximaTentativa(jogoStr *jogo) {
  jogo->quantidadeCaracteres = 0;
  limpaChute(jogo);
}

void calculaPontos(jogoStr *jogo) {
  for (int i = 0 ; i < 4 ; i++) {
    if (jogo->resposta[i] == 'p') {
      jogo->pontos += 5;
    } else if (jogo->resposta[i] == 'b') {
      jogo->pontos += 3;
    }
  }
}

void acaoJogo(jogoStr *jogo) {
  if (tela_rato_clicado()) {
    armazenaCoordenadasXYrato(jogo);
    bool teste = cliqueValido(jogo);
    int indexQuadradoClique =
        quadradoNoPonto(9, jogo->quadrados, jogo->coordXYrato);
    if (teste && jogo->quantidadeCaracteres < 4 && indexQuadradoClique < 7) {
      if (jaAdicionado(jogo, indexQuadradoClique) == false) {
        atualizaResposta(jogo->quantidadeCaracteres,
                                 indexQuadradoClique, jogo);
        atualizaQuadradoChute(jogo, indexQuadradoClique,
                                   jogo->quantidadeCaracteres);
        jogo->quantidadeCaracteres++;
      } else {
        tela_texto(250, 500, 20, vermelho, "Cor já selecionada");
      }
    } else if (jogo->quadrados[7].clicado && jogo->quantidadeCaracteres == 4) {
      strcpy(jogo->historicoChutes[jogo->quantidadeTentativas], jogo->chute);
      comparaChuteComSorteio(jogo->resposta, jogo->chute, jogo->segredo);
      processaResposta(jogo);
      atualizaParaProximaTentativa(jogo);
      calculaPontos(jogo);
      jogo->quantidadeTentativas++;
    } else if (jogo->quadrados[8].clicado && jogo->quantidadeCaracteres > 0) {
      removeUltimoCaractereResposta(jogo);
      removeDadosQuadradoChute(jogo);
    }
  }
}

bool verificaJaRepetido(char coresDisponiveis[8], char coresSorteadas[5],
                        int indexSorteado) {

  for (int i = 0; i < 4; i++) {
    if (coresDisponiveis[indexSorteado] == coresSorteadas[i]) {
      return true;
    }
  }
  return false;
}

void sorteioCores(jogoStr *jogo) {

  char coresDisponiveis[8] = {'V', 'A', 'X', 'M', 'C', 'R', 'L'};
  int indexSorteado;
  bool jaAdicionado = false;

  srand(time(NULL));

  for (int i = 0; i < 4; i++) {
    do {
      indexSorteado = rand() % 7;
      jaAdicionado =
          verificaJaRepetido(coresDisponiveis, jogo->segredo, indexSorteado);

      if (!jaAdicionado) {
        jogo->segredo[i] = coresDisponiveis[indexSorteado];
      }
    } while (jaAdicionado == true);
  }
}

void inicializaCores(jogoStr *jogo) {
  int roxo = tela_cria_cor(0.41, 0.13, 0.55),
      cinza = tela_cria_cor(0.74, 0.74, 0.74);
  jogo->cores[0] = 2;
  jogo->cores[1] = 1;
  jogo->cores[2] = roxo;
  jogo->cores[3] = 9;
  jogo->cores[4] = cinza;
  jogo->cores[5] = 7;
  jogo->cores[6] = 6;
  jogo->cores[7] = 5;
  jogo->cores[8] = 8;
}

void calculaBonus(jogoStr *jogo) {
  for (int i = 1 ; i < 10 ; i++) {
    if (jogo->quantidadeTentativas == i) {
      jogo->pontos += 10 - i;
    }
  }
}

void iniciaPartida(jogoStr *jogo) {
  limpaChute(jogo);
  jogo->pontos = 0;
  jogo->quantidadeCaracteres = 0;
  jogo->quantidadeTentativas = 1;
  strcpy(jogo->segredo, "    \0");
  inicializaCores(jogo);
  sorteioCores(jogo);
  inicializaQuadradosChute(jogo);
  jogo->acertou = false;
}

void informaDesistencia() {
  double inicio = relogio();
  char tecla = 0;
  while (true) {
    tecla = tela_tecla();
    double t = 5 - (relogio() - inicio);
    if (t <= 0) break;
    tela_texto(250, 400, 30, vermelho, "Você desistiu!");
    char txt[10];
    sprintf(txt, "%.1f", t);
    tela_texto(250, 430, 20, vermelho, txt);
    tela_atualiza();
  }
}

void parabenizaJogador(jogoStr jogo) {
  double inicio = relogio();
  char tecla = 0;
  while (tecla == 0) {
    tecla = tela_tecla();
    double t = 10 - (relogio() - inicio);
    if (t <= 0)break;
    int cor = branco;
    if (t < 2) cor = vermelho;
    tela_texto(250, 400, 20, cor, "Parabéns, você acertou");
    char txt1[10], txt2[10];
    sprintf(txt2, "%d", jogo.pontos);
    tela_texto(250, 420, 20, cor, "sua pontuação foi de:");
    tela_texto(250, 445, 20, cor, txt2);
    sprintf(txt1, "%.1f", t);
    tela_texto(250, 470, 20, cor, txt1);
    tela_atualiza();
  }
}

void informaFimDeJogo (jogoStr jogo) {
  double inicio = relogio();
  char tecla = 0;
  while (tecla == 0) {
    tecla = tela_tecla();
    double t = 10 - (relogio() - inicio);
    if (t <= 0)break;
    int cor = branco;
    if (t < 2) cor = vermelho;
    tela_texto(250, 400, 20, cor, "Infelizmente você não acertou...");
    char txt1[10], txt2[10];
    sprintf(txt2, "%d", jogo.pontos);
    tela_texto(250, 420, 20, cor, "sua pontuação foi de:");
    tela_texto(250, 440, 20, cor, txt2);
    sprintf(txt1, "%.1f", t);
    tela_texto(250, 460, 20, cor, txt1);
    tela_atualiza();
  }
}
//não entendi como escrever um int + uma string na msm linha de um arquivo
void armazenaPontosNoArquivo(jogoStr *jogo) {
  FILE *arq;
  char nome[] = "Ranking mindmaster";
  arq = fopen(nome, "w");
  if (arq == NULL) {
    printf("Não foi possível abrir o arquivo '%s' para leitura\n", nome);
  }
  for (int i = 1 ; i < 11 ; i++) {
    int pontos;
    char iniciais[3];
    if ((fscanf(arq, "%d", &pontos) > jogo->pontos) && i % 2 == 0) {
      fprintf(arq, "%d\n", jogo->pontos);
      printf("Digite suas iniciais: ");
      fgets(iniciais, 3, stdin);
    }
  }
}

void partida() {
  jogoStr jogo;

  iniciaPartida(&jogo);
  puts(jogo.segredo);
  while (jogo.quantidadeTentativas < 10 && !jogo.acertou) {
    imprimeInformacoes();
    imprimeJogo(&jogo);
    acaoJogo(&jogo);

    if (tela_tecla() == 'd') {
      informaDesistencia();
      jogo.quantidadeTentativas = 10;
    }

    tela_atualiza();
  }
  calculaBonus(&jogo);
  //armazenaPontosNoArquivo(&jogo);
  if (jogo.acertou && jogo.quantidadeTentativas < 10) {
    parabenizaJogador(jogo);
  } else {
    informaFimDeJogo(jogo);
  }
}

bool terminouPartida() {
  if (tela_tecla() == 'f') {
    return false;
  }
  return true;
}

bool quer_jogar_de_novo(void) {
  double inicio = relogio();
  char tecla = 0;
  while (tecla == 0) {
    tecla = tela_tecla();
    double t = 20 - (relogio() - inicio);
    if (t <= 0)break;
    int cor = branco;
    if (t < 2) cor = vermelho;
    tela_texto(250, 400, 20, cor, "Digite S para jogar novamente");
    char txt[10];
    sprintf(txt, "%.1f", t);
    tela_texto(250, 430, 20, cor, txt);
    tela_atualiza();
  }
  if (tecla == 's' || tecla == 'S')
    return true;
  return false;
}

int main(void) {

  tela_inicio(500, 800, "mindmaster");

  do {
    partida();
  } while (quer_jogar_de_novo());
  printf("saiu");
  tela_fim();

  return 0;
}