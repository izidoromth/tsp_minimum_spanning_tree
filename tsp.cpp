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

//PROFUNDIDADE

class Aresta
{
  public:
  int u, v;
  float peso;
  Aresta(){};
  ~Aresta(){};
};

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

//Calcula arvore geradora minima
vector<Vertice> AGMPrim(vector<Vertice> &grafo)
{
  Heap heap;
  for(int i = 0; i < grafo.size(); i++)
  {
    InserirHeap(heap, grafo[i]);
  }

  heap.vetor[0]->c = 0;

  while(heap.tamanho > 0)
  {
    Vertice *u = RemoverMenor(heap);
    for(int a = 0; a < u->adjacencias.size(); a++)
    {
      int v = u->adjacencias[a].v;
      if(grafo[v].c > u->adjacencias[a].peso && grafo[v].indice_heap < heap.tamanho)
      {
        grafo[v].c = u->adjacencias[a].peso;
        grafo[v].p = u;
        AtualizarHeap(heap, grafo[v].indice_heap);
      }
    }
  }

  vector<Vertice> agm;

  for(int i=0; i < grafo.size(); i++)
  {
    Vertice v;

    v.id = grafo[i].id;
    v.x = grafo[i].x;
    v.y = grafo[i].y;

    agm.push_back(v);
  }

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

void InserirHeap(Heap &heap, Vertice &v)
{
  heap.vetor.push_back(&v);
  v.indice_heap = heap.tamanho;
  heap.tamanho++;
  int indice = heap.tamanho - 1;
  while(indice > 0 && heap.vetor[Pai(indice)]->c > heap.vetor[indice]->c)
  {
    InverterVertice(heap, Pai(indice), indice);
    indice = Pai(indice);
  }
}

void InverterVertice(Heap &heap, int i1, int i2)
{
  Vertice *aux = heap.vetor[i1];
  aux->indice_heap = i2;
  heap.vetor[i2]->indice_heap = i1;
  heap.vetor[i1] = heap.vetor[i2];
  heap.vetor[i2] = aux;
}

Vertice* RemoverMenor(Heap &heap)
{
  Vertice *menor = heap.vetor[0];  
  InverterVertice(heap, 0, heap.tamanho - 1);
  heap.tamanho--;
  Heapify(heap, 0);
  return menor;
}

int Esquerda(int i)
{
  return i * 2 + 1;
}

int Direita(int i)
{
  return i * 2 + 2;
}

int Pai(int i)
{
  return i % 2 == 0 ? ((i - 1) / 2) : (i / 2);
}

void Heapify(Heap &heap, int i)
{
  if(i >= heap.tamanho || Esquerda(i) >= heap.tamanho || Direita(i) >= heap.tamanho)
    return;

  int menor;

  if(Esquerda(i) < heap.tamanho && heap.vetor[Esquerda(i)]->c < heap.vetor[i]->c)
    menor = Esquerda(i);
  else
    menor = i;

  if(Direita(i) < heap.tamanho && heap.vetor[Direita(i)]->c < heap.vetor[menor]->c)
    menor = Direita(i);

  if(menor != i)
  {
    InverterVertice(heap, i, menor);
    Heapify(heap, menor);
  }
}

void AtualizarHeap(Heap &heap, int id)
{
  int i = id;
  while(i > 0 && heap.vetor[i]->c < heap.vetor[Pai(i)]->c)
  {
    InverterVertice(heap, i, Pai(i));
    i = Pai(i);
  }
}
