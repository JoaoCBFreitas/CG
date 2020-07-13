#include "parser.h"


void inicializa(Group g) {
	for (int i = 0; i < 20; i++) g[i] = NULL;
}
void readFile(char* fich,vector<Point>& vertices) {
	std::string ficheiro(fich);
	string linha;
	string novo;
	string delimiter = " ";
	int pos;
	float xx, yy, zz;
	Point p;
	ifstream file;
	file.open(ficheiro);
	if (file.is_open()) {

		while (getline(file, linha)) {
			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			xx = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.x = xx;

			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			yy = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.y = yy;

			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			zz = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.z = zz;
			vertices.push_back(p);
			
		}
		Point e = { 2020, 2020, 2020 };
		vertices.push_back(e);
		vertices.push_back(e);
		vertices.push_back(e);
		file.close();
	}
	else {
		cout << "ERRO AO LER FICHEIRO" << endl;
	}
}
void addFicheiro(char* file, Group g, vector<Ficheiros*>& files)
{
	Ficheiros* ficheiros = new Ficheiros();
	ficheiros->file = new char[15];
	vector<Transforma*> transfs;

	for (int i = 0; g[i]; i++) {
		Grupo* grupo;

		for (grupo = g[i]; grupo; grupo = grupo->next) {
			Transforma* transforma = new Transforma();

			transforma->params[0] = grupo->params[0];
			transforma->params[1] = grupo->params[1];
			transforma->params[2] = grupo->params[2];
			transforma->params[3] = grupo->params[3];

			transforma->transf = new char[15];
			strcpy(transforma->transf, grupo->transf);

			transfs.push_back(transforma);
		}
	}

	strcpy(ficheiros->file, file);
	ficheiros->transfs = transfs;

	files.push_back(ficheiros);

}
void muda(Group g, int i, char* transf, double x, double y, double z, double angle)
{
	Grupo* ant = g[i];

	Grupo* grupo = (Grupo*)malloc(sizeof(struct grupo));

	grupo->transf = (char*)malloc(sizeof(char) * 15);
	strcpy(grupo->transf, transf);
	grupo->params[0] = x;
	grupo->params[1] = y;
	grupo->params[2] = z;
	grupo->params[3] = angle;
	grupo->next = NULL;
	
	if (g[i] == NULL) {
		g[i] = grupo;
	}
	else {
		while (ant->next != NULL) ant = ant->next;
		ant->next = grupo;
	}
}
void parseG(const XMLElement* scene, Group g, int i, vector<Ficheiros*>& files) {
	double x;
	double y;
	double z;
	double angulo;
	char* transX;
	char* transY;
	char* transZ;
	char* rotacaoAngulo;
	for (const XMLElement* group = scene->FirstChildElement(); group; group = group->NextSiblingElement()) {
		if (strcmp((char*)group->Value(), "translate") == 0) {
			char transf[15];
			strcpy(transf, (char*)group->Value());
			transX = (char*)group->Attribute("X");
			transY = (char*)group->Attribute("Y");
			transZ = (char*)group->Attribute("Z");
			x = 0;
			y = 0;
			z = 0;
			if (transX != NULL) x = atof(transX);
			if (transY != NULL) y = atof(transY);
			if (transZ != NULL) z = atof(transZ);
			muda(g, i, transf, x, y, z, 0);
		}
		if (strcmp((char*)group->Value(), "rotate") == 0) {
			char transf[15];
			strcpy(transf, (char*)group->Value());
			transX = (char*)group->Attribute("X");
			transY = (char*)group->Attribute("Y");
			transZ = (char*)group->Attribute("Z");
			rotacaoAngulo = (char*)group->Attribute("angle");
			x = 0;
			y = 0;
			z = 0;
			angulo = 0;
			if (transX != NULL) x = atof(transX);
			if (transY != NULL) y = atof(transY);
			if (transZ != NULL) z = atof(transZ);
			if (rotacaoAngulo != NULL) angulo = atof(rotacaoAngulo);
			muda(g, i, transf, x, y, z, angulo);
		}
		if (strcmp((char*)group->Value(), "scale") == 0) {
			char transf[15];
			strcpy(transf, (char*)group->Value());
			transX = (char*)group->Attribute("X");
			transY = (char*)group->Attribute("Y");
			transZ = (char*)group->Attribute("Z");
			x = 0;
			y = 0;
			z = 0;
			if (transX != NULL) x = atof(transX);
			if (transY != NULL) y = atof(transY);
			if (transZ != NULL) z = atof(transZ);
			muda(g, i, transf, x, y, z, 0);
		}
		if (strcmp((char*)group->Value(), "colour") == 0) {
			char transf[15];
			strcpy(transf, (char*)group->Value());
			transX = (char*)group->Attribute("R");
			transY = (char*)group->Attribute("G");
			transZ = (char*)group->Attribute("B");
			x = 0;
			y = 0;
			z = 0;
			if (transX != NULL) x = atof(transX);
			if (transY != NULL) y = atof(transY);
			if (transZ != NULL) z = atof(transZ);
			muda(g, i, transf, x, y, z, 0);
		}
		if (strcmp((char*)group->Value(), "models") == 0) {
			for (const XMLElement* models = group->FirstChildElement(); models; models = models->NextSiblingElement()) {
				char dir[4] = "../";
				char file[15];

				char* fileName = (char*)models->Attribute("file");
				strcpy(file, fileName);

				char file3d[40];
				file3d[0] = '\0';
				strcat(file3d, dir);
				strcat(file3d, file);
				addFicheiro(file3d, g, files);
			}
		}
		if (strcmp((char*)group->Value(), "group") == 0) {
			parseG(group, g, i + 1, files);
			for (int j = i + 1; j < 20; j++)
				g[j] = NULL;

		}
	}
}

void parser(char* xmlDoc,vector<Ficheiros*>& files,vector<Point>& vertices) {
	int i = 0;
	Group g;
	inicializa(g);

	XMLDocument doc;
	XMLError error = doc.LoadFile(xmlDoc);
	if (!error) {
		XMLElement* elem = doc.FirstChildElement();
		for (const XMLElement* child = elem->FirstChildElement(); child;child = child->NextSiblingElement()) {
			if (strcmp(child->Value(), "group") == 0) {
				for (int j = i; j < 20; j++)
					g[j] = NULL;
				parseG(child, g, i, files);
			}
		}
	}
	else {
		cout << "Deu erro a abrir" << endl;
	}
	
	for (vector<Ficheiros*>::iterator it = files.begin(); it != files.end(); it++) {
		Ficheiros* ficheiro = *it;
		char* fich = ficheiro->file;
		readFile(fich, vertices);
	}
}
