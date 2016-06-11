// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <iostream>
// Include GLEW
#include <GL/glew.h>
#include <time.h>
#include <set>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
#include "objloader.hpp"
#include "texture.hpp"


// Achtung, die OpenGL-Tutorials nutzen glfw 2.7, glfw kommt mit einem veränderten API schon in der Version 3 

// Befindet sich bei den OpenGL-Tutorials unter "common"
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"
#include <string>
#include <ctime>

// Erklären ObenGL-Statemachine, lowlevel
// Version 1: seit 1992, elegantes API für die Plattformunabhägige 3D-Programmierung 
// Version 2: seit 2002, Ergänzung durch Shader-Programme, die auf Grafikkarte laufen
// Version 3: seit 2008, Fixedfunction-Pipeline nur noch als Compability-Extension
// Version 4: seit 2010 noch kaum unterstützt
// Wir setzen nun auf Version 3, schwieriger zu erlernen als Version 1
// glm-Bibliothek für Matrix-Operationen (fehlen in Version3), glew-Bibliothek, um API-Funktionen > 1.1 nutzen zu können
// glfw-Bibliothek, um OpenGL plattformunabhängig nutzen zu können (z. B. Fenster öffnen)
#define UEBUNG1 /* feste Rotation */
// Pflichtteil: Ergebnis zeigen ...
// Kode zeilenweise erklären. (Orientiert sich an http://www.opengl-tutorial.org/, Evtl. diese Tutorials dort ausführen, um zu vertiefen.)
// Dort wird glfw in der Version 2 genutzt, wir verwenden die aktuelle Version 3 http://www.glfw.org/
// Schwarze Linien wg. Shader. Kurz erklaeren, was die Shader machen...
// Vorgehensweise erklaeren, Jeweils alte cpp-Datei nach CGTutorial.cpp.n kopieren, damit jede Aenderung nachvollziehbar bleibt.
// (Voraussetzung fuer Support)
// Aufgabe Rotation: glm::rotate    (http://glm.g-truc.net/glm.pdf)
#define UEBUNG2 /* variable Rotation ueber R-Taste */
// Eventloop, kann man muss man aber nicht in glfw (Alternativ Glut, dort muss man)
// Aufgabe: Hinweis globale Variable, Taste...
#define UEBUNG3 /* Kamerasteuerung */
// Aufgabe drei Unabhängige Rotationen, wird zu erstem Teil des Pflichtteils
// Hinweis auf andere Kaperasteuerungen ->  http://www.opengl-tutorial.org
#define UEBUNG4 /* Ausgemaltes Objekt und Z-Buffer */
// OpenGL-Befehle: http://wiki.delphigl.com/index.php/Hauptseite auf Deutsch!
// GLEW http://glew.sourceforge.net/
// Wireframe vs. Solid thematisieren, Z-Buffer wird noetig, um Verdeckungsproblem zu lösen
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
///////////////////////////////////////////////////////
#define UEBUNG5 /* Einlesen eines Objekts */
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// Bessere OBJ-Loader vorhanden, basieren aber auf OpenGL1.1
// Andere Formate möglich, z. B. 3ds, s. a. Link am Ende der Seite
// Wir nehmen Teapot, Anekdote zu Teapot:  von Newell 1975 (hat berühmtes Lehrbuch geschrieben) glut, 3dsmax, Toystory, Monster AG, ...
//
// Vertex Buffer Objects (VBO) (ab 1.5) enthalten alle Daten zu Oberflaechen eines Objekts (Eckpunktkoordinaten, Normalen, Texturkoordinaten -> Uebung 6)
// koennen in separatem Speicher einer Grafikkarte gehalten werden (haengt vom Modell ab)
// koennen ueber API-Aufrufe veraendert werden --> glBufferData, glMapBuffer (GL_STATIC_DRAW, um mitzuteilen, dass sich nichts aendern wird)
// Vertex Array Objects (VAO) (ab 3) kapseln mehrere VBOs eines Objects zwecks Optimierung und einfachrer Benutzung:
// Beim Zeichnen nur 2 Aufrufe: glBindVertexArray und glDrawArrays
//#define UEBUNG5TEDDY  /* Nach der Teddy-Übung fuer Uebung6 wieder ausschalten */
// Alternativ: Teddy wenn die Zeit reicht: google nach teddy chameleon, teapot.obj mit xxx.obj austauschen....
// Modellieren kann also auch einfach sein, Freies Tool Blender (open Source), Professionelle Werkzeuge Maya, 3dsmax, etc. (nicht gegenstand der LV)
#define UEBUNG6 /* Beleuchten, neuer Shader */
// Teddy-Modell hat keine Normalen, passt nicht zu Shadern, wieder zur Teekanne zurueck.
// Shader anschauen, Alter Shader "ColourFragmentShader" setzt Farbe direkt, wird interpoliert ("Gouraud-Shading")
// "TransformVertexShader" gibt Farben weiter, legt layout der Eingabe-Daten fest, verwendet MVP um Eckpunkte zu transvormieren ("Eine Matrix reicht")
// Neue Shader koennen mehr (Immer noch nicht, was die Fixed-Function-Pipeline konnte) 
// Normalen stehen senkrecht auf Oberflaechen, wichtig fuer die Beleuchtung
// Normalen muessen anders behandelt werden als Eckpunkte, deshalb Trennung von MVP in Model View Projection
// -> Shader verändern in Uebung 15
#define UEBUNG7 /* Texturieren */
// Farbtexturen sind digitale Rasterbilder (andere Texturen in Kombination moeglich "Multi-Texturing" nicht Gegenstand der Uebung -> VL Textur)
// Imageloader fuer png und jpg nicht in den Bibliotheken enthalten -> SOIL
// DDS-Format kompatibel zu Texturkompression der Hardware. Wir nehmen aber BMP !
// s. a.:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/, Achtung glfwLoadTexture2D gibt' nicht mehr in glfw-version 3
// Granularitaet: Textur pro Objekt. kann also gewechselt werden ! --> Uebung 8
// Texturierte Teekanne ist 2. Teil des Pflichtteils 
///////////////////////////////////////////////////////
#define UEBUNG8 /* Mehrere Objekte */
// Polygone bilden Objekt, Objekte haben eigene Transformationen ("Model"-Matrix) eigene Texturen, etc.
// VAOs abwechselnd binden, MVP-Matrizen abwechselnd setzen, ggf Texturen abwechselnd binden, OpenGL verwendet jeweils positive Zahlen als
// Namen, die man sich generieren lassen muss.
// Model-Matrix fuer jedes Objekt anders, Szenen haben meist hierarchische Struktur, Multiplikationskette von der Wurzel zum Blatt
// Roboter-Beispiel, um Ketten zu lernen
#define UEBUNG9 /* Koordinatenachse */
// Notwendigkeit Koordinatensysteme anzeigen zu lassen... -> drawCS-Funktion mit drei Balkenen
// Wie erhaelt man langen duennen Balken mit bekannten Geometrien ?
// Aufgabe: Balken (leider mit M, V, P, ... als Parameter) Hinweis ginge auch als Singleton...
// Skalierungsparameter ruhig ausprobieren.
// sieht man den Balken im Bauch der Teekanne ?
#define UEBUNG10 /* Koordinatenkreuz */
// Aufgabe: Drei Balken, spaeter entsteht die Notwendigkeit Matrizen zu sichern (evtl. Mechanismus von OpenGL1 erwaehnen)
#define UEBUNG11 /* Ein Robotersegment */
// Teekanne ausblenden, Kugel zeichnen, Transformationsreihenfolge nochmal thematisieren
// Aufgabe: -Funktion mit Parameter!
#define UEBUNG12 /* Drei Robotersegmente uebereinander */
// Aufgabe: statischer Roboter, Unterschied lokales vs. globales Translate
#define UEBUNG13 /* Rotationen in den Gelenken */
// Aufgabe Roboter mit Tastensteuerung, Reihenfolge der Transformationen von oben nach unten und umgekehrt mit Stiften erläutern
#define UEBUNG14 /* Lampe am Ende */
// Uebung 15 im StandardShading pixelshader
// Hier mal exemplarisch den Pixelshader aendern, um die Beleuchtung ansprechender zu machen
// Erwaehnen, dass man Parameter wie "MVP" kontrollieren koennte.
// Hier beginnt die Wellt der Shader-programmierung, die nicht Gegenstand der Uebung ist.
// Lampe am Ende eines steuerbaren Roboters ist dritter Teil des Pflichtteils
// (5 Punkte: 3 Rotationen, Teekanne, texturiert, Roboter, Licht am Ende) 

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
float turnXg;
float turnYg;
float turnZg;
float turnX;
float turnY;
float turnZ;
float turnX1;
float turnY1;
float turnZ1;
float turnX2;
float turnY2;
float turnZ2;

float zoom;

// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID;


glm::vec3 f1(50.0 / 10.0, 2.5 / 100.0, 2.5 / 100.0);
glm::vec3 f2(2.5 / 100.0, 50.0 / 10.0, 2.5 / 100.0);
glm::vec3 f3(2.5 / 100.0, 2.5 / 100.0, 50.0 / 10.0);



void sendMVP()
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform, konstant fuer alle Eckpunkte
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]); //schick ich Matrix in Shader rein (zunächst Vertexshader)
}


//------------------------------------------------------------------
//------------------------------------------------------------------ New Code Ben
enum color { transparent, red, blue, green, yellow, black, white };
const int xLen = 6;
const int yLen = 12;
const int zLen = 6;



struct cube {
	vec3 koordinate;
	color col;

	cube::cube(int x, int y, int z, color c) {
		koordinate = vec3(x, y, z);
		col = c;

	}

	cube::cube() {
		koordinate = vec3(xLen, yLen, zLen);
		this->col = transparent;

	}

	vec3 getKoor() {
		return koordinate;
	};

	void setKoor(int x, int y, int z) {
		koordinate = vec3(x, y, z);
	};

	void cube::transform(char achse, bool uhrzeigersinn, cube* anker, vec3 *diff) {
		//vec3 *diff = new vec3();
		*diff = this->koordinate - anker->koordinate;
		vec3 temp = *diff;

		if (achse == 'x' && uhrzeigersinn == true) {
			diff->y = diff->z;
			diff->z = -1 * temp.y;

		}
		else if (achse == 'x' && uhrzeigersinn == false) {
			diff->y = -1 * diff->z;
			diff->z = temp.y;
		}
		else if (achse == 'y' && uhrzeigersinn == true) {
			diff->x = diff->z;
			diff->z = -1 * temp.x;

		}
		else if (achse == 'y' && uhrzeigersinn == false) {
			diff->x = -1 * diff->z;
			diff->z = temp.x;
		}
		else if (achse == 'z' && uhrzeigersinn == true) {
			diff->y = diff->x;
			diff->x = -1 * temp.y;

		}
		else {
			diff->y = -1 * diff->x;
			diff->x = temp.y;
		};
		//return *diff;
	};
};

std::vector <cube> schacht_liste; //Brauchen wir wahrscheinllich nicht
std::array< std::array< std::array<color, zLen >, yLen >, xLen > schacht_array;
std::array< std::array< std::array<color, zLen >, yLen >, xLen > old_schacht;
std::array< std::array< std::array<color, zLen >, yLen >, xLen > fresh_schacht;
void initSchacht() {
	for (int i = 0; i < xLen; i++) {
		for (int j = 0; j < yLen; j++) {
			for (int k = 0; k < zLen; k++) {
				schacht_array[i][j][k] = transparent;
			}
		}
	}
};


struct stein {
	std::vector<cube> cube_elems;


	stein::stein(int type, color col) {
		switch (type) {
		case 1: //I - stein 
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 2, yLen, zLen / 2, col));
			break;

		case 2: //L - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			break;

		case 3: //S - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			break;

		case 4: //O - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			break;
		};
	}
	stein::stein() {
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
	}

	//
	void stein::bewegen(char achse, int richtung) {
		switch (achse) {
		case('x'):
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				it->koordinate.x += richtung;
			}
			if (inside_arena() == false || cube_free() == false) {
				for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
					it->koordinate.x -= richtung;
				}
			}
			break;

		case ('z'):
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				it->koordinate.z += richtung;
			}
			if (inside_arena() == false || cube_free() == false) {
				for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
					it->koordinate.z -= richtung;
				}
			}
			break;
		}
	}


	bool stein::pruefe_ob_unten() {

	}
	//Prüft ob sich ein Stein innerhalb der erlaubten Spielfläche/Arena befindet
	bool stein::inside_arena() {
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			if (!(x >= 1 && y >= 1 && z >= 1 && x <= xLen && y <= yLen && z <= zLen)) {
				return false;
			}
		}
		return true;
	}
	//Prüft ob ein Quadrant im Schacht/Arena frei ist. 
	//Prüft NICHT ob der Quadrant innerhalb der Arena ist!
	bool stein::cube_free() {
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
				return false;
			}
		}
		return true;
	}

	//Kopiert Stein/ Farbe des Steins in die Arenakoordinaten hinein
	void stein::kopiere_in_schacht() {			// Methode 
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			schacht_array[x - 1][y - 1][z - 1] = it->col;
		}
	}

	void stein::drehen(char achse, bool uhrzeigersinn) {

		vec3 *diff = new vec3(); //reserviert Speicher für den Differenzvektor (Ankercube - thisCube)
		cube *anker = &this->cube_elems[0]; //
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) { //Iteriert durch alle Cubes des Steins
			it->transform(achse, uhrzeigersinn, anker, diff);// Verändert den Differenzvektor entsprechend der gewünschten Drehung
			it->koordinate = anker->koordinate + *diff; // Aktualisiert die Koordinaten des cubes 
		}
		delete diff;
		if (inside_arena() == false || cube_free() == false) {
			drehen(achse, !uhrzeigersinn);
		}

	}

};
stein *falling = new stein();

void dropStein(int value) {

	for (std::vector<cube>::iterator it = falling->cube_elems.begin(); it != falling->cube_elems.end(); ++it) {
		it->koordinate.y += value;
	}
	if (falling->inside_arena() == false || falling->cube_free() == false) {
		for (std::vector<cube>::iterator it = falling->cube_elems.begin(); it != falling->cube_elems.end(); ++it) {
			it->koordinate.y -= value;
		}
		falling->kopiere_in_schacht();
		srand(time(NULL));
		int	randStone = rand() % 4 + 1;
		int randColor = rand() % 5;
		falling = new stein(randStone, white);
	}
}
void turboDrop() {
	for (int i = 0; i < yLen; i++) {
		//	while (falling->inside_arena() == true || falling->cube_free() == true) {
		dropStein(-1);
		//}
		break;
	}
}


float cus = 1.;			// cube-unit-size
float tilesize = .005;	// dicke des bodens
void drawTiles() {
	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, -cus - tilesize, 0));
	//Model = glm::rotate(Model, 45.0f, glm::vec3(0, 1, 0));

	Model = glm::scale(Model, glm::vec3(1, tilesize, 1));
	for (int i = 0; i < xLen; i++) {
		for (int j = 0; j < zLen; j++) {

			Model = glm::translate(Model, glm::vec3(0, 0, 2 * cus));
			sendMVP();
			drawCube();



		}
		Model = glm::translate(Model, glm::vec3(2 * cus, 0, 2 * cus*(-zLen)));
	};
	Model = Save;

}
float wallsize = .1;
void drawWallX() {
	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, 0, 0));
	//Model = glm::rotate(Model, -45.0f, glm::vec3(0, 1, 0));
	Model = glm::scale(Model, glm::vec3(1, 1, wallsize));
	for (int i = 0; i < yLen; i++) {
		for (int j = 0; j < xLen; j++) {
			Model = glm::translate(Model, glm::vec3(2 * cus, 0, 0));
			sendMVP();

			drawWireCube();

		}
		Model = glm::translate(Model, glm::vec3(2 * cus*(-xLen), 2 * cus, 0));
	};
	Model = Save;

}

struct compareVec3 {
	bool operator() (vec3 const &a, vec3 const &b) {
		if (a.y != b.y) {
			return a.y < b.y;
		}
		else if (a.z != b.z) {
			return a.z < b.z;
		}
		else
			return a.x < b.x;
	}
};

std::set<vec3, compareVec3> completedLines() {
	std::set<glm::vec3, compareVec3> vanishing_cubes;
	int xline = 0;
	int zline = 0;

	for (int y = 1; y <= yLen; y++) {
		for (int x = 1; x <= xLen; x++) {
			for (int z = 1; z <= zLen; z++) {
				if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					zline += 1;
				}
			}
			if (zline == zLen) {
				for (int i = 1; i <= zLen; i++) {
					vanishing_cubes.insert(glm::vec3(x, y, i));

				}
			}
			zline = 0;

		}
		for (int z = 1; z <= zLen; z++) {
			for (int x = 1; x <= xLen; x++) {
				if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					xline += 1;
				}
			}
			if (xline == xLen) {
				for (int i = 1; i <= xLen; i++) {
					vanishing_cubes.insert(glm::vec3(i, y, z));
				}
			}
			xline = 0;
		}

	}
	return vanishing_cubes;
}

void deleteCompleteLines(std::set<vec3, compareVec3> delcubes) {
	std::set<vec3, compareVec3>::iterator it;
	old_schacht=schacht_array;
	for (it = delcubes.begin(); it != delcubes.end(); ++it)
	{
		int x = it->x;
		int y = it->y;
		int z = it->z;

		for (int i = y; i < yLen; i++) {
			schacht_array[x - 1][i - 1][z - 1] = schacht_array[x - 1][i][z - 1];
		}
		schacht_array[x - 1][yLen - 1][z - 1] = transparent;
		fresh_schacht = schacht_array;
	}
}

void draw() {								//Zeichnet den Schacht

	glm::mat4 Save = Model;

	//	Model = glm::rotate(Model, 45.0f, glm::vec3(0, 1, 0));
	for (int y = 1; y <= yLen; y++) {
		for (int x = 1; x <= xLen; x++) {
			for (int z = 1; z <= zLen; z++) {
				Model = glm::translate(Model, glm::vec3(0, 0, 2 * cus));
				sendMVP();
				vec3* temp = new vec3(x, y, z);
				if (
					*temp == falling->cube_elems[0].koordinate ||
					*temp == falling->cube_elems[1].koordinate ||
					*temp == falling->cube_elems[2].koordinate ||
					*temp == falling->cube_elems[3].koordinate) {
					drawCube();
				}

				else if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
					drawCube();
					//drawCube(schacht_array[x][y][z].col)
				}
				else {
					//	drawWireCube();
				}
				delete temp;

			}
			Model = glm::translate(Model, glm::vec3(2 * cus, 0, 2 * cus*(-zLen)));
		}

		Model = glm::translate(Model, glm::vec3(2 * cus*(-xLen), 2 * cus, 0));
	};
	//drawCubeUnit(cus);

	Model = Save;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	switch (key)
	{
	case GLFW_KEY_A:
		break;
	case GLFW_KEY_W:
		zoom += 1.5;
		break;
	case GLFW_KEY_S:
		zoom -= 1.5;
		break;
	case GLFW_KEY_Q:
		turnXg = turnYg = turnZg = turnX = turnY = turnZ = turnX1 = turnY1 = turnZ1 = turnX2 = turnY2 = turnZ2 = 0.0;
		break;
	case GLFW_KEY_1:
		turnX += 2.5;
		break;
	case GLFW_KEY_2:
		turnX -= 2.5;
		break;
	case GLFW_KEY_3:
		turnY += 2.5;
		break;
	case GLFW_KEY_4:
		turnY -= 2.5;
		break;
	case GLFW_KEY_5:
		turnZ += 2.5;
		break;
	case GLFW_KEY_6:
		turnZ -= 2.5;
		break;
	case GLFW_KEY_7:
		zoom += 2.5;
		break;
	case GLFW_KEY_8:
		zoom -= 2.5;
		break;
	case GLFW_KEY_9:
		break;
	case GLFW_KEY_O:
		turboDrop();
		break;
	}

	if (action == GLFW_PRESS) {
		switch (key) {

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_RIGHT:
			falling->bewegen('x', 1);
			break;
		case GLFW_KEY_LEFT:
			falling->bewegen('x', -1);
			break;
		case GLFW_KEY_UP:
			falling->bewegen('z', 1);
			break;
		case GLFW_KEY_DOWN:
			falling->bewegen('z', -1);
			break;

		case GLFW_KEY_E:
			falling->drehen('x', true);
			break;
		case GLFW_KEY_D:
			falling->drehen('x', false);
			break;
		case GLFW_KEY_R:
			falling->drehen('y', true);
			break;
		case GLFW_KEY_F:
			falling->drehen('y', false);
			break;
		case GLFW_KEY_T:
			falling->drehen('z', true);
			break;
		case GLFW_KEY_G:
			falling->drehen('z', false);
			break;

		default:
			break;
		}
	}
}



// Zeichnet einzelne Koordinatensystemachsen
void drawCS(glm::vec3 factor) {
	glm::mat4 Save = Model;
	Model = glm::scale(Model, factor);
	sendMVP();
	drawWireCube();
	Model = Save;
}


int main(void)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);

	// Open a window and create its OpenGL context
	// glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
	GLFWwindow* window = glfwCreateWindow(1024, // Breite
		768,  // Hoehe
		"CG - Tutorial", // Ueberschrift
		NULL,  // windowed mode
		NULL); // shared windoe

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current (wird nicht automatisch gemacht)
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// GLEW ermöglicht Zugriff auf OpenGL-API > 1.1
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}


	//HIER GEHTS EIGENTLICH LOS

	std::vector<glm::vec3> vertices; //Punkte
	std::vector<glm::vec2> uvs;		//Texturkoordinaten
	std::vector<glm::vec3> normals; //Normalenvektoren
	bool res = loadOBJ("teapot.obj", vertices, uvs, normals);
	glm::vec3 lightPos = glm::vec3(4, 4, -2);


	// Jedes Objekt eigenem VAO zuordnen, damit mehrere Objekte moeglich sind
	// VAOs sind Container fuer mehrere Buffer, die zusammen gesetzt werden sollen.
	GLuint VertexArrayIDTeapot;
	glGenVertexArrays(1, &VertexArrayIDTeapot);
	glBindVertexArray(VertexArrayIDTeapot);

	// Ein ArrayBuffer speichert Daten zu Eckpunkten (hier xyz bzw. Position)
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer); // Kennung erhalten
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // Daten zur Kennung definieren
												 // Buffer zugreifbar für die Shader machen
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// Erst nach glEnableVertexAttribArray kann DrawArrays auf die Daten zugreifen...
	glEnableVertexAttribArray(0); // siehe layout im vertex shader: location = 0 
	glVertexAttribPointer(0,  // location = 0 
		3,  // Datenformat vec3: 3 floats fuer xyz 
		GL_FLOAT,
		GL_FALSE, // Fixedpoint data normalisieren ?
		0, // Eckpunkte direkt hintereinander gespeichert
		(void*)0); // abweichender Datenanfang ? 

	GLuint normalbuffer; // Hier alles analog für Normalen in location == 2
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); // siehe layout im vertex shader 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	GLuint uvbuffer; // Hier alles analog für Texturkoordinaten in location == 1 (2 floats u und v!)
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // siehe layout im vertex shader 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Load the texture
	GLuint Texture = loadBMP_custom("standardtextur.bmp");

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background: R, G, B, Alpha
	glClearColor(0.5f, 0.1f, 0.8f, 0.5f);

	// Create and compile our GLSL program from the shaders
//	programID = LoadShaders("TransformVertexShader.vertexshader","ColorFragmentShader.fragmentshader");
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	glEnable(GL_DEPTH_TEST);	//Aktivierung Tiefenbuffer
	glDepthFunc(GL_LESS);		//kleinster zWert ist näher an Kamera

	//glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);


	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, Texture);

		// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
	//initSchacht();
	time_t start = time(0);
	time_t start2 = time(0);
	time_t end;
	double timeStep = 3.8;
	srand(time(NULL));
	// Eventloop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); //Stichpunkt: Frontplane und Backplane

		// Camera matrix, ---- hier linkshändig (manipulierbar durch erste zeile. wenn positiver z wert dann rechtshändig
		View = glm::lookAt(glm::vec3(40 + zoom, 20, 40 + zoom), // Camera is at (0,0,-10), in World Space
			glm::vec3(0, 0, 0),  // and looks at the origin---- bildschirmmitte
			glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down) ---- in welche richtung gehts nach oben


// Model matrix : an identity matrix (model will be at the origin) erzeugt 4x4 Einheitsmatrix
		Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, turnY, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, turnX, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, turnZ, glm::vec3(0, 0, 1));
		//	Hier wird die Position des Objekts verschoben
		Model = glm::translate(Model, glm::vec3(0, 0, -2 * cus));
		Model = glm::translate(Model, glm::vec3(-xLen *cus + cus, 0, -zLen *cus + cus)); // Hier wird die Position des Objekts verschoben

		glm::mat4 Save = Model;
		//	Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));

		//

		//	
		//	// sendet Projection, Model, View an Grafikkarte
		//	// Hier kommt der Teapot rein
		//	//glBindVertexArray(VertexArrayIDTeapot);
		//	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		//	Model = Save;
		sendMVP();
		//	drawCube();		//oder 
		//	
		//	//drawWireCube();
		//	
		//	// Hier kommt die Kugeldazu
		//	Model = Save;
		//	Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
		//	sendMVP();
		//	//drawSphere(10, 10);
		//	drawCS(f1);
		//	drawCS(f2);
		//	drawCS(f3);

			/*Model = glm::rotate(Model, turnY, glm::vec3(0, 1, 0));
			Model = glm::rotate(Model, turnX, glm::vec3(1, 0, 0));
			Model = glm::rotate(Model, turnZ, glm::vec3(0, 0, 1));
			drawRobSeg(0.5);
			Model= glm::translate(Model, glm::vec3(0, 0.5, 0));
			Model = glm::rotate(Model, turnY1, glm::vec3(0, 1, 0));
			Model = glm::rotate(Model, turnX1, glm::vec3(1, 0, 0));
			Model = glm::rotate(Model, turnZ1, glm::vec3(0, 0, 1));
			drawRobSeg(0.4);
			Model= glm::translate(Model, glm::vec3(0, 0.4, 0));
			Model = glm::rotate(Model, turnY2, glm::vec3(0, 1, 0));
			Model = glm::rotate(Model, turnX2, glm::vec3(1, 0, 0));
			Model = glm::rotate(Model, turnZ2, glm::vec3(0, 0, 1));
			drawRobSeg(0.3);
			glm::vec4 lightPos = Model*glm::vec4(0, 0.3, 0, 1);
			glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
	*/


	//	drawWallX();
		drawTiles();
		draw();
		end = time(0);
		deleteCompleteLines(completedLines());
		
		if (end - start > timeStep) {
			start = time(0);
			dropStein(-1);
		}



		//drawCube();


		// Swap buffers
		glfwSwapBuffers(window);

		// Poll for and process events 
		glfwPollEvents();

	}
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);

	glDeleteBuffers(1, &uvbuffer);
	glDeleteTextures(1, &Texture);
	glDeleteProgram(programID);
	delete falling;
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

