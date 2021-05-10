#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>  
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <string.h>

using namespace std;

const float INFINITO = 0xEFFFFFFF;

//variavel para indexar os vértices do grafo
int ID = 0;

/*Esta classe é utilizada para modelar as arestas do grafo.
Foi utilizada a representação do grafo através de lista de 
adjacências, portanto cada vértice conterá um vetor guardando
todas as arestas de saída dele. Os valores u, v representam o
índice do vértice de onde sai e para onde vai a aresta
respectivamente (u, v). O valor peso armazena o custo da aresta.*/
class Aresta
{
  public:
  int u, v;
  float peso;
  Aresta(){};
  ~Aresta(){};
};

/*A classe vertice modela a estrutura do nó do grafo. Ela contém 
um indíce que corresponde a posição em que o nó encontra-se na
lista de vértices. Possui as coordenadas (x,y) do ponto que foi
lido através do arquivo de entrada. Possui o índice da posição em 
que o vértice se encontra no vetor do Heap utilizado na função Prim
que computa a árvore geradora mínima. A váriavel c descreve o custo
de um vértice até o corte (explicado na função AGMPrim). A flag visitado
é utilizada pelo algoritmo de busca em profundidade para determinar se
a busca deve continuar por um determinado caminho ou não. *p é utilizado
para apontar para o vértice que leva até o corte. Ao final do AGMPrim cada
vértice possui um pai (*p) que corresponderá às arestas da árvore geradora
mínima. adjacencias é o vetor que guarda todas as arestas que saem do vértice*/
class Vertice
{
  public:
  int id;
  int x, y;
  int indice_heap;
  float c;
  bool visitado;
  Vertice *p;
  vector<Aresta> adjacencias;
  Vertice(){ id = ID++; x = 0; y = 0; c = INFINITO; p = NULL; indice_heap = -1; visitado = false; }
  ~Vertice(){}
};

/* Esta classe representa o Heap binário de mínimo, estrutura de dados
auxiliar utilizada na função AGMPrim. A escolha do Heap binário
se dá devido ao custo computacional das suas operações de inserção,
remoção e atualização. Seus custos serão descritos e analisados 
em suas respectivas funções. O heap trata-se de uma fila de prioridades
comumente escrita em forma de vetor. Sua visualização entretanto é
através de uma árvore binária, onde os pais, no caso do heap de mínimo,
invariantemente devem suas chaves possuir valor menor que de ambos os
filhos da esquerda e direita. No caso em questão, o heap será composto
pelos vértices do grafo de entrada do algoritmo prim, sendo a váriavel
c a chave para o arranjo do heap. Existe na classe Heap uma variável tamanho
que é utilizada para descrever onde o vetor do Heap acaba, já que não é 
necessário remover completamente um vértice do vetor, pois isso resultaria
num custo maior de remoção, podendo os vértices removidos irem para o final 
da fila ao invés de serem completamente removidos*/
class Heap
{
  public:
  vector<Vertice*> vetor;
  int tamanho;
  Heap(){ tamanho = 0; }
  ~Heap(){}
};

vector<Vertice> DesserializarPontos(char* nome_do_arquivo);
void CriarArestas(vector<Vertice> &grafo);
float DistanciaEuclidiana(Vertice v1, Vertice v2);
vector<Vertice> AGMPrim(vector<Vertice> &grafo);
void TornarAGMNaoDirecionada(vector<Vertice> &agm);
float CalcularCustoCiclo(vector<Vertice> &agm);
void MontarVetorVisitacaoBP(vector<Vertice> &agm, int i, vector<Vertice> &visitacao);
void EscreverSaidaPrim(vector<Vertice> &agm);
void EscreverSaidaCiclo(vector<Vertice> &agm);
void EscreverSaidaBP(vector<Vertice> &agm, int i, ofstream &arquivo);

//Funcoes Heap
void InserirHeap(Heap &heap, Vertice &v);
void InverterVertice(Heap &heap, int i1, int i2);
Vertice* RemoverMenor(Heap &heap);
int Esquerda(int i);
int Direita(int i);
int Pai(int i);
void Heapify(Heap &heap, int i);
void AtualizarHeap(Heap &heap, int id);

int main(int argc, char* argv[]) {
  if(!argv[0])
  {
      cout << "Informe um arquivo de entrada" << endl;
      return -1;
  }

  vector<Vertice> grafo = DesserializarPontos(argv[1]);
  
  if(grafo.size() == 0)
  {
      cout << "Ocorreu algo errado ao obter os pontos de entrada" << endl;
      return -1;
  }

  CriarArestas(grafo);

  clock_t t;
    
  t = clock();

  vector<Vertice> agm = AGMPrim(grafo);

  EscreverSaidaPrim(agm);

  TornarAGMNaoDirecionada(agm);

  EscreverSaidaCiclo(agm);

  float custo_ciclo = CalcularCustoCiclo(agm);

  t = clock() - t;

  cout << fixed << setprecision(6);
    cout << t*1.0/CLOCKS_PER_SEC << ' ' << custo_ciclo << endl;

  return 0;
}

//Desserializa os pontos do arquivo de entrada em vértices e inicializa o grafo
vector<Vertice> DesserializarPontos(char* nome_do_arquivo)
{
    ifstream arquivo;
    string line;

    vector<Vertice> grafo;
    int num_pontos;

    int x, y;

    arquivo.open(nome_do_arquivo);
    if(!arquivo)
    {
        cout << "Arquivo nao encontrado!" << endl;
    }
    else
    {
      //obtem o numero de pontos do grafo
      getline(arquivo, line);
      num_pontos = stoi(line);
      
      for(int i = 0;i < num_pontos;i++)
      {
        //cria o vertice
        Vertice v;

        getline(arquivo, line); 
        stringstream ss(line);
        string valor;

        //atribui o valor x do vertice
        getline(ss, valor, ' ');
        v.x = stoi(valor);

        //atribui o valor y do vertice
        getline(ss, valor, ' ');
        v.y = stoi(valor);   

        grafo.push_back(v);
      }
    }
    return grafo;
}

//Inicializa arestas entre todos os vértices existentes criando um grafo completo
void CriarArestas(vector<Vertice> &grafo)
{
  for(int i = 0; i < grafo.size(); i++)
  {
    for(int j = i+1; j < grafo.size(); j++)
    {
      Aresta ida;
      Aresta volta;

      ida.u = volta.v = grafo[i].id;
      ida.v = volta.u = grafo[j].id;

      ida.peso = volta.peso = DistanciaEuclidiana(grafo[i], grafo[j]);

      grafo[i].adjacencias.push_back(ida);
      grafo[j].adjacencias.push_back(volta);
    }
  }
}

//Calcula a distancia euclidiana entre os pontos do grafo
float DistanciaEuclidiana(Vertice v1, Vertice v2)
{
    int dx = v1.x-v2.x;
    int dy = v1.y-v2.y;

    return (float)sqrt(pow(dx,2) + pow(dy,2));
}

/*Esta função é o principal passo na computação de uma aproximação
do ciclo hamiltoniano de um conjunto de pontos. Ciclo hamiltoniano
trata-se do caminho de menor custo passando por todos os vértices
do grafo retornando ao vértice inicial. A função é responsável
por calcular uma árvore geradora mínima do grafo de entrada. A árvore
geradora mínima trata-se de uma árvore que contém o mínimo de arestas,
com o menor custo que possa descrever o grafo de entrada. É um
algoritmo guloso que cria um corte C a partir de um vértice inicial
que se expande a cada iteração, incluindo ao corte o vértice que possui
a aresta de menor custo que leva a este corte. O algoritmo possui um laço
principal que atualiza os custos das arestas do último vértice incluído
sempre que uma aresta que leva ao corte tenha seu custo menor que o
custo armazenado. Com armazenado, entende-se como sendo o valor da
chave (variável c da classe Vértice) no heap mínimo utilizado para
guardar os menores custos de todas os vértices que levam ao corte.
O uso do heap binário de mínimo se justifica devido ao custo de suas 
funções de remoção, inserção e atualização que são detalhados no 
cabeçalho de cada função. Cada vértice v do grafo (menos o vértice
escolhido como inicial) possui um ponteiro que aponta para o vértice p
de C que é o pai do vértice v fora de C que levou v para dentro deste 
corte C. Ao final do algoritmo cada ponteiro p resultará numa aresta
da árvore geradora mínima.
Entrada: lista de vértices do grafo que se deseja calcular a AGM
Saída: árvore geradora mínima do grafo de entrada*/
vector<Vertice> AGMPrim(vector<Vertice> &grafo)
{
  /*Linhas 245 à 249 inicializam o Heap de mínimo
  auxiliar da função. O custo destas linhas é
  O(n) pois a cada inserção não é necessário 
  rearranjar o heap dado que todas as chaves 
  tem seus valor c = INFINITO bastando apenas
  inserir o vértice ao final do heap com custo O(1)*/
  Heap heap;
  for(int i = 0; i < grafo.size(); i++)
  {
    InserirHeap(heap, grafo[i]);
  }

  /*Escolhe o primeiro vértice do grafo como
  sendo o vértice inicial do corte e atualiza
  sua chave para custo 0*/
  heap.vetor[0]->c = 0;

  /*Laço principal do algoritmo que será
  percorrido n vezes correspondendo a cada
  nova inserção no corte C*/
  while(heap.tamanho > 0)
  {
    /*Escolha do vértice que é incluído ao
    corte C. Esta operação tem custo O(lgn)
    devido à operação de Heapify que ocorre
    após uma remoção. Entretanto, como Heapify
    não está sendo realizado num vetor não
    "heapificado", seu custo real será O(1),
    bastando apenas a troca da primeira posição
    com um dos seus filhos a esquerda ou direita
    no pior caso. Como esta operação é 
    realizada para cada novo vértice no corte, 
    no total a linha 277 terá custo O(n)*/
    Vertice *u = RemoverMenor(heap);

    /*Este laço tem por objetivo atualizar 
    as chaves e atribuir o endereço dos pais
    de todos os vértices fora do corte que possuem
    arestas ao último vértice íncluído. Como este
    laço ocorrerá para todas as arestas de cada um
    dos vértices do grafo, o laço será executado
    em custo O(m), onde m é o número de arestas total
    do grafo de entrada*/
    for(int a = 0; a < u->adjacencias.size(); a++)
    {
      /*armazena o indíce do vértice v que
      saí de u para fora do corte C*/
      int v = u->adjacencias[a].v;

      /*Compara se existe uma aresta de V / C que leva ao corte
      e contém um custo menor do que o já armazenado pelo heap*/
      if(grafo[v].c > u->adjacencias[a].peso && grafo[v].indice_heap < heap.tamanho)
      {
        /*Atualiza o custo e o pai de v*/
        grafo[v].c = u->adjacencias[a].peso;
        grafo[v].p = u;

        /*Atualiza o heap para a nova chave de valor menor
        ao anteriormente armazenado. Cada operação tem custo
        O(lgn) levando à um total de O(mlgn)*/
        AtualizarHeap(heap, grafo[v].indice_heap);
      }
    }
  }

  /*Instancia o vetor que conterá os vértices
  da agm de saída do algoritmo*/
  vector<Vertice> agm;

  /*Instancia cada vértice da agm e inclui
  no vetor. O(n)*/
  for(int i=0; i < grafo.size(); i++)
  {
    Vertice v;

    v.id = grafo[i].id;
    v.x = grafo[i].x;
    v.y = grafo[i].y;

    agm.push_back(v);
  }

  /*Instancia as arestas da árvore geradora
  mínima e as inclui no vetor de saída.
  Custo O(n). Note que cada vértice possui
  apenas um pai, entretanto um vértice pode
  ser pai de mais de um*/
  for(int i=0; i < grafo.size(); i++)
  {
    if(grafo[i].p != NULL)
    {
      Aresta ida;

      ida.u = grafo[i].id;
      ida.v = grafo[i].p->id;
      ida.peso = grafo[i].c;
      
      agm[i].adjacencias.push_back(ida);
    }
  }

  return agm;
}

void TornarAGMNaoDirecionada(vector<Vertice> &agm)
{
  for(int i=0; i < agm.size(); i++)
  {
    for(int e=0; e < agm[i].adjacencias.size(); e++)
    {
      int v = agm[i].adjacencias[e].v;

      Aresta volta;

      volta.u = v;
      volta.v = agm[i].id;
      volta.peso = agm[i].adjacencias[e].peso;

      agm[v].adjacencias.push_back(volta);
    }
  }  
}

float CalcularCustoCiclo(vector<Vertice> &agm)
{
  vector<Vertice> visitacao;

  for(int i = 0; i < agm.size(); i++)
    agm[i].visitado = false;

  MontarVetorVisitacaoBP(agm, 0, visitacao);

  visitacao.push_back(agm[0]);

  float custo = 0;

  Vertice aux = visitacao[0];
  for(int i=1; i < visitacao.size(); i++)
  {
    custo += DistanciaEuclidiana(aux, visitacao[i]);
    aux = visitacao[i];
  }

  return custo;
}

void MontarVetorVisitacaoBP(vector<Vertice> &agm, int i, vector<Vertice> &visitacao)
{
  agm[i].visitado = true;

  visitacao.push_back(agm[i]);

  for(int e=0; e < agm[i].adjacencias.size(); e++)
  {
    int v = agm[i].adjacencias[e].v;
    if(!agm[v].visitado)
    {
      MontarVetorVisitacaoBP(agm, v, visitacao);
    }
  }
}

void EscreverSaidaPrim(vector<Vertice> &agm)
{
  ofstream arquivo;
  arquivo.open("tree.txt");

  for(int i=0; i < agm.size(); i++)
  {
    for(int e=0; e < agm[i].adjacencias.size(); e++)
    {
      int u = agm[i].adjacencias[e].u;
      int v = agm[i].adjacencias[e].v;

      arquivo << to_string(agm[u].x) << " " << to_string(agm[u].y) << endl;
      arquivo << to_string(agm[v].x) << " " << to_string(agm[v].y) << endl;

      // if(!(i == agm.size() - 1 && e == agm[i].adjacencias.size() - 1))
      //   arquivo << endl;
    }
  }
}


void EscreverSaidaCiclo(vector<Vertice> &agm)
{
  ofstream arquivo;
  arquivo.open("cycle.txt");

  EscreverSaidaBP(agm, 0, arquivo);

  arquivo << to_string(agm[0].x) << " " << to_string(agm[0].y) << endl;
}

void EscreverSaidaBP(vector<Vertice> &agm, int i, ofstream &arquivo)
{  
  agm[i].visitado = true;

  arquivo << to_string(agm[i].x) << " " << to_string(agm[i].y) << endl;

  for(int e=0; e < agm[i].adjacencias.size(); e++)
  {
    int v = agm[i].adjacencias[e].v;
    if(!agm[v].visitado)
    {
      EscreverSaidaBP(agm, v, arquivo);
    }
  }
}

/*Função utilizada para incluir um vértice ao heap de
armazenamento dos custos que levam ao corte.
Entrada: heap e vértice a ser incluído*/
void InserirHeap(Heap &heap, Vertice &v)
{
  /*Insere o vértice ao final do vetor*/
  heap.vetor.push_back(&v);
  /*Atribui o índice do heap ao vértice incluído*/
  v.indice_heap = heap.tamanho;
  /*Aumenta o tamanho do heap*/
  heap.tamanho++;
  int indice = heap.tamanho - 1;
  /*Corrige de baixo para cima no heap sempre que o valor inserido for menor que seu pai.*/
  while(indice > 0 && heap.vetor[Pai(indice)]->c > heap.vetor[indice]->c)
  {
    InverterVertice(heap, Pai(indice), indice);
    indice = Pai(indice);
  }
}

/*Função auxiliar para realizar a troca de 
posições no heap. Custo O(1)
Entrada: heap e indices dos vértices a 
serem trocados no heap*/
void InverterVertice(Heap &heap, int i1, int i2)
{
  Vertice *aux = heap.vetor[i1];
  /*Atualiza os índices do vértice dentro do heap*/
  aux->indice_heap = i2;
  heap.vetor[i2]->indice_heap = i1;
  heap.vetor[i1] = heap.vetor[i2];
  heap.vetor[i2] = aux;
}

/*Função utilizada para encontrar o vértice
com a menor chave do heap. A chave invariavelmente
encontra-se na posição 0 do heap. Entretanto*/
Vertice* RemoverMenor(Heap &heap)
{
  Vertice *menor = heap.vetor[0];  
  InverterVertice(heap, 0, heap.tamanho - 1);
  heap.tamanho--;
  /*Heapify possui custo O(lgn)
  como explicado na definição
  da função. Esta linha limita
  RemoverMenor à O(lgn)*/
  Heapify(heap, 0);
  return menor;
}

/*Retorna o índice do filho à esquerda.
Entrada: índice do vértice sendo analisado
Saída: índice do vértice à esquerda*/
int Esquerda(int i)
{
  return i * 2 + 1;
}

/*Retorna o índice do filho à direita.
Entrada: índice do vértice sendo analisado
Saída: índice do vértice à direita*/
int Direita(int i)
{
  return i * 2 + 2;
}

/*Retorna o índice do vértice pai.
Entrada: índice do vértice sendo analisado
Saída: índice do vértice pai*/
int Pai(int i)
{
  return i % 2 == 0 ? ((i - 1) / 2) : (i / 2);
}

/*Função recursiva utilizada para corrigir o heap para 
manter a invariância de que um vértice pai sempre tem 
sua chave com valor menor que seu filho
Entradas: heap e índice do vértice a ser heapificado*/
void Heapify(Heap &heap, int i)
{
  /*Caso base*/
  if(i >= heap.tamanho || Esquerda(i) >= heap.tamanho || Direita(i) >= heap.tamanho)
    return;

  /*variável que armazena o índice 
  do vértice com chave de menor valor*/
  int menor;

  /*verifica se a chave da esquerda possui menor valor
  Caso positivo atribui o valor da esquerda,
  senão atribui o valor do indíce sendo analisado*/
  if(Esquerda(i) < heap.tamanho && heap.vetor[Esquerda(i)]->c < heap.vetor[i]->c)
    menor = Esquerda(i);
  else
    menor = i;

  /*verifica se a chave da direita possui menor valor*/
  if(Direita(i) < heap.tamanho && heap.vetor[Direita(i)]->c < heap.vetor[menor]->c)
    menor = Direita(i);

  /*Caso uma das chaves filhas esquerda ou diraita possuam
  menor valor que a chave pai elas são invertidas e o algo-
  ritmo é feito novamente para a chave invertida, isto é,
  o algoritmo desce a árvore realizando as trocas necessá-
  rias para manter a invariância de heap de mínimo válida.
  Como esta correção é sempre feita uma escolha entre direi-
  ta ou esquerda, sendo o heap uma árvore binária, no pior
  caso toda a altura h da árvore será percorrida resultan-
  do em um custo de O(lgn)*/
  if(menor != i)
  {
    InverterVertice(heap, i, menor);
    Heapify(heap, menor);
  }
}

/*Função recursiva utilizada para corrigir o heap para 
manter a invariância de que um vértice pai sempre tem 
sua chave com valor menor que seu filho, entretanto,
esta correção ocorre de baixo para cima no vértice.
De igual modo à função Heapify, o custo é O(lgn)
devido a altura da árvore ser lg n*/
void AtualizarHeap(Heap &heap, int id)
{
  int i = id;
  /*Realiza a troca dos vértices sempre que o valor
  da chave do pai for maior que a do filho*/
  while(i > 0 && heap.vetor[i]->c < heap.vetor[Pai(i)]->c)
  {
    InverterVertice(heap, i, Pai(i));
    i = Pai(i);
  }
}
