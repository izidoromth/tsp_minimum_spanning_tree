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

//variavel para indexar os vértices do grafo
int ID = 0;

//Grafo completo
//AGM(PRIM)
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
  vector<Aresta> adjacencias;
  Vertice(){ id = ID++; x = 0; y = 0; }
  ~Vertice(){}
};

vector<Vertice> DesserializarPontos(char* nome_do_arquivo);
void CriarArestas(vector<Vertice> grafo);
float DistanciaEuclidiana(Vertice p1, Vertice p2);


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
void CriarArestas(vector<Vertice> grafo)
{
  for(int i = 0; i < grafo.size(); i++)
  {
    for(int j = 0; j < grafo.size(); j++)
    {
      if(grafo[i].id == grafo[j].id)
        continue;

      Aresta a;

      a.u = grafo[i].id;
      a.v = grafo[j].id;
      a.peso = DistanciaEuclidiana(grafo[i], grafo[j]);

      grafo[i].adjacencias.push_back(a);
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
