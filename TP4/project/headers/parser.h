#ifndef CG_TP_PARSER_H
#define CG_TP_PARSER_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include <string>
#include <vector>

using namespace tinyxml2;
using namespace std;

typedef struct ponto {
	float x;
	float y;
	float z;
}Point;
typedef struct cor {
	char* type;
	double colors[3];
}Cor;
typedef struct luz {
	char* type;
	double params[3];
}Luz;
typedef struct orbitas {
	double tempo;
	vector<Point> pontos;
} Orbitas;

typedef struct grupo {
	char* transf;
	double params[4];
	Orbitas* orbita;
	struct grupo* next;
} Grupo;

typedef struct transforma {
	char* transf;
	double params[4];
	Orbitas* orbita;
} Transforma;

typedef struct ficheiros {
	char* file;
	int numpontos;
	char* textura;
	Cor* cor;
	vector<Transforma*> transfs;
} Ficheiros;

typedef Grupo* Group[20];




void readFile(vector<Ficheiros*>& ficheiros, vector<Point>& vertices, int* nVertices, vector<Point>& normals, vector<Point>& texturas);
void parser(char* xmlDoc, vector<Ficheiros*>& files, vector<Point>& vertices, vector<Luz*>& luzes, vector<Point>& normais, vector<Point>& texturas, int* nVertices);

#endif