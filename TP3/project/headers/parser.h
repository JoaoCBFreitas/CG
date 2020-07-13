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
	vector<Transforma*> transfs;
} Ficheiros;

typedef Grupo* Group[20];




void readFile(vector<Ficheiros*>& ficheiros, vector<Point>& vertices);
void parser(char* xmlDoc,vector<Ficheiros*>& ficheiros,vector<Point>& vertices);

#endif