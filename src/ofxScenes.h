//

//#define USESVG 1
#ifdef USESVG
#include "ofxSvg.h"
#endif

//#define USEASSIMP 1
#ifdef USEASSIMP
#include "ofxAssimpModelLoader.h"
#endif

#define USEFFT 1
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
		ofxMicroUI * uiColors = NULL;
		
//		int margem = 100;
//		ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);

//		string & scene;
		sceneConfig() {} ;
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u) :
		fbo(_fbo), u(_u) {
			uiColors = &u->uis["colors"];
		};
	} config;
	
	

	#include "scenes.h"

	ofFbo * fbo = NULL;
	ofxMicroUI * u = NULL;
	ofxMicroUI * uiColors = NULL;
	string & scene;
	
	// teste 17 nov 2020, nao sei se vai dar certo.
	// mais tarde aplicar essa como parametro de todas as sceneDmtr, como construtor passando como ponteiro.
	// com isso poder extender os parametros sem precisar mais mudar nada de sceneDmtr::sceneDmtr
	

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) {
		config = sceneConfig(fbo, u);
//		config.uiColors = &u->uis["colors"];
		
		uiColors = &u->uis["colors"];
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
	
	
	
	// #define SCENESALL 1
	#ifdef SCENESALL
	#include "scenesAll.h"
	#endif

	#define SCENESMUTI 1
	#ifdef SCENESMUTI
	#include "scenesMuti.h"
	#endif	
	
	
	void setup() {
	#ifdef SCENESMUTI
		setupScenesMuti();
	#endif	


#ifdef SCENESALL
		setupScenesAll();
#endif

#ifdef USESVG
		scenes.push_back(new sceneSvg(u, fbo));
		scenesMap["svg"] = scenes.back();
#endif
	
#ifdef USEASSIMP
		scenes.push_back(new sceneModel(u, fbo));
		scenesMap["model"] = scenes.back();
		
		// scenes.push_back(new sceneModel(u, fbo));
		// scenesMap["model2"] = scenes.back();
#endif

#ifdef MIAW
		scenes.push_back(new sceneImage(u, fbo));
		scenesMap["image"] = scenes.back();

		scenes.push_back(new sceneVideo(u, fbo));
		scenesMap["video"] = scenes.back();

		scenes.push_back(new sceneToma(u, fbo));
		scenesMap["toma"] = scenes.back();

		scenes.push_back(new sceneConfetti(u, fbo));
		scenesMap["confetti"] = scenes.back();
		
		scenes.push_back(new sceneBasic(u, fbo));
		scenesMap["basic"] = scenes.back();
		
		scenes.push_back(new sceneSvank(u, fbo));
		scenesMap["svank"] = scenes.back();

		scenes.push_back(new sceneClaquete(u, fbo));
		scenesMap["claquete"] = scenes.back();

		scenes.push_back(new sceneNo(u, fbo));
		scenesMap["no"] = scenes.back();

		scenes.push_back(new sceneGrad(u, fbo));
		scenesMap["grad"] = scenes.back();
		
		scenes.push_back(new sceneRibbon(u, fbo));
		scenesMap["ribbon"] = scenes.back();

		scenes.push_back(new sceneRandom(u, fbo));
		scenesMap["random"] = scenes.back();

		scenes.push_back(new sceneNav(u, fbo));
		scenesMap["nav"] = scenes.back();
#endif
		
		scenes.push_back(new sceneTextFile(u, fbo));
		scenesMap["text"] = scenes.back();



		
		for (auto & s : scenesMap) {
			s.second->setup();
			s.second->isSetup = true;
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
