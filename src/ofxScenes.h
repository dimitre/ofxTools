//
//#include <boost/signals2.hpp>
//#include <iostream>
//
//using namespace boost::signals2;


//#define USESVG 1
#ifdef USESVG
#include "ofxSvg.h"
#endif

//#define USEASSIMP 1
#ifdef USEASSIMP
#include "ofxAssimpModelLoader.h"
#endif

//#define USEFFT 1
#ifdef USEFFT
#include "ofxMicroUIFFT.h"
#endif

class ofxScenes : public ofBaseApp {
public:
	
	struct sceneConfig {
	public:
		
#ifdef USEFFT
		ofxMicroUIFFT * fft = NULL;
#endif
		ofFbo * fbo = NULL;
		ofxMicroUI * u = NULL;
		ofxMicroUI * uiC = NULL;
		ofxMicroUI * uiColors = NULL;

//		int margem = 100;
//		ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);

//		string & scene;

		sceneConfig() {} ;
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u) :
		fbo(_fbo), u(_u) {
			uiColors = &u->uis["colors"];
			uiC = &u->uis["scene"];
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC) :
		fbo(_fbo), u(_u), uiC(_uiC) {
			uiColors = &u->uis["colors"];
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors) :
		fbo(_fbo), u(_u), uiC(_uiC), uiColors(_uiColors) {
//			uiColors = &u->uis["colors"];
		};
	} config;
	
	#include "scenes.h"

	ofFbo * fbo = NULL;
	ofxMicroUI * u = NULL;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * uiColors = NULL;
	string & scene;
	
	// teste 17 nov 2020, nao sei se vai dar certo.
	// mais tarde aplicar essa como parametro de todas as sceneDmtr, como construtor passando como ponteiro.
	// com isso poder extender os parametros sem precisar mais mudar nada de sceneDmtr::sceneDmtr
	
//	ofxScenes() {}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) {
		config = sceneConfig(fbo, u);
//		config.uiColors = &u->uis["colors"];
		uiColors = &u->uis["colors"];
	}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, string & s) : fbo(_f), u(_u), uiC(_uiC), scene(s) {
		config = sceneConfig(fbo, u, uiC);
		uiColors = &u->uis["colors"];
	}

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors, string & s) :
	fbo(_f), u(_u), uiC(_uiC), uiColors(_uiColors), scene(s) {
		config = sceneConfig(fbo, u, uiC, uiColors);
//		uiColors = &u->uis["colors"];
	}

	
	static ofColor getColor(float n, ofxMicroUI * uiColors) {
		if (uiColors->pBool["usePalette"]) {
			// uiColors->pColor["colorPalette"] = ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getColor(n);
			// return uiColors->pColorEasy["colorPalette"];
			return ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getColor(n);
		} else {
			return uiColors->pColorEasy["color"];
		}
	}
	
	static void calcNormals( ofMesh & mesh, bool bNormalize, bool mode){
		for( int i=0; i < mesh.getIndices().size(); i+=3 ){
			const int ia = mesh.getIndices()[i];
			const int ib = mesh.getIndices()[i+1];
			const int ic = mesh.getIndices()[i+2];
			glm::vec3 e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
			glm::vec3 e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
			// depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped.
			glm::vec3 no = mode ? glm::cross(e2, e1) : glm::cross(e1, e2);
			mesh.getNormals()[ia] = no;
			mesh.getNormals()[ib] = no;
			mesh.getNormals()[ic] = no;
		}

		if (bNormalize) {
			for (auto & n : mesh.getNormals()) {
				n = glm::normalize(n);
			}
		}
	}


	static void drawMeshStatic(ofMesh * m, ofxMicroUI * ui) {
		if (ui->pString["draw"] == "wire") {
			m->drawWireframe();
		}
		else if (ui->pString["draw"] == "faces") {
			m->drawFaces();
		}
		else if (ui->pString["draw"] == "points") {
			//cout << "aqui " << endl;
			glDisable(GL_POINT_SMOOTH);
			glPointSize(ui->pEasy["pointSize"]);
			m->draw(OF_MESH_POINTS);
		}
	}

	static void drawMeshStatic(ofVboMesh * m, ofxMicroUI * ui) {
		if (ui->pString["draw"] == "wire") {
			m->drawWireframe();
		}
		else if (ui->pString["draw"] == "faces") {
			m->drawFaces();
		}
		else if (ui->pString["draw"] == "points") {
			//cout << "aqui " << endl;
			glDisable(GL_POINT_SMOOTH);
			glPointSize(ui->pEasy["pointSize"]);
			m->draw(OF_MESH_POINTS);
		}
	}

	
	vector <sceneDmtr *> scenes;
	map <string, sceneDmtr *> scenesMap;
	
	#define SCENESMIAW 1
	#ifdef SCENESMIAW
	#include "scenes.h"
	#endif	


	#define SCENESALL 1
	#ifdef SCENESALL
	#include "scenesAll.h"
	#endif

	#define SCENESMUTI 1
	#ifdef SCENESMUTI
	#include "scenesMuti.h"
	#endif	
	
	
	// vector <void()> _setupFunctions;
	// signal<void()> setups;

	void setup() {
		// setups.connect([]{ setupScenesMuti(); });
		// setups.connect([]{ setupScenes(); });
		// setups();
		// _setupFunctions.push_back(&ofxScenes::setupScenesMuti);


#ifdef SCENESMUTI
		setupScenesMuti();
#endif	
#ifdef SCENESALL
		setupScenesAll();
#endif

#ifdef SCENESMIAW
		setupScenes();
#endif

#ifdef USESVG
		scenes.push_back(new sceneSvg(&config, "svg"));
#endif
	
#ifdef USEASSIMP
		scenes.push_back(new sceneModel(&config, "model"));
		// scenes.push_back(new sceneModel(&config, "model2"));
#endif

		for (auto & s : scenes) {
			scenesMap[s->name] = s;
			scenesMap[s->name]->setup();
		}
	}
	
	void draw() {
		if (scene != "") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				ofSetColor(getColor(0, uiColors));
				scenesMap[scene]->draw();
			} else {
				cout << "scene not found " << scene << endl;
			}
		}
	}
};