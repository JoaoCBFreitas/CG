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


typedef struct grupo {
	char* transf;
	double params[4];
	struct grupo* next;
} Grupo;

typedef struct transforma {
	char* transf;
	double params[4];
} Transforma;

typedef struct ficheiros {
	char* file;
	vector<Transforma*> transfs;
} Ficheiros;

typedef Grupo* Group[20];

typedef struct ponto {
	float x;
	float y;
	float z;
}Point;



void readFile(char* fich, vector<Point>& vertices);
void parser(char* xmlDoc,vector<Ficheiros*>& ficheiros,vector<Point>& vertices);

#endif