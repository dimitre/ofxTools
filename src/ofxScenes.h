//#define USESVG 1
//#define USEASSIMP 1
//#define USEFFT 1

#ifdef USESVG
#include "ofxSvg.h"
#endif

#ifdef USEASSIMP
#include "ofxAssimpModelLoader.h"
#endif

#ifdef USEFFT
#include "ofxMicroUIFFT.h"
#endif

class ofxScenes { // : public ofBaseApp
public:
	struct sceneConfig {
	public:

		string name = "";
		string * scene = NULL;
		
#ifdef USEFFT
		ofxMicroUIFFT * fft = NULL;
#endif
		ofFbo * fbo = NULL;
		ofxMicroUI * u = NULL;
		ofxMicroUI * ui = NULL;
		ofxMicroUI * uiC = NULL;
		ofxMicroUI * uiColors = NULL;

//		int margem = 100;
//		ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);
        
		void afterSetup() {
			ui = &u->uis["ui"];
			scene = &ui->pString["scene"];
		}

		sceneConfig() {} ;
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u) :
		fbo(_fbo), u(_u) {
			uiC = &u->uis["scene"];
			uiColors = &u->uis["colors"];
			afterSetup();
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC) :
		fbo(_fbo), u(_u), uiC(_uiC) {
			uiColors = &u->uis["colors"];
			afterSetup();
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors) :
		fbo(_fbo), u(_u), uiC(_uiC), uiColors(_uiColors) {
			afterSetup();
		};
	} config;
	
	#include "sceneDmtr.h"

	ofFbo * fbo = NULL;
	ofxMicroUI * u = NULL;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * uiColors = NULL;
	string & scene;

	ofxMicroUI * ui = NULL;
	
	// teste 17 nov 2020, nao sei se vai dar certo.
	// mais tarde aplicar essa como parametro de todas as sceneDmtr, como construtor passando como ponteiro.
	// com isso poder extender os parametros sem precisar mais mudar nada de sceneDmtr::sceneDmtr
	
//	ofxScenes() {}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) {
		config = sceneConfig(fbo, u);
		uiColors = &u->uis["colors"];
		ui = &u->uis["ui"];
	}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, string & s) : fbo(_f), u(_u), uiC(_uiC), scene(s) {
		config = sceneConfig(fbo, u, uiC);
		uiColors = &u->uis["colors"];
		ui = &u->uis["ui"];
	}

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors, string & s) :
	fbo(_f), u(_u), uiC(_uiC), uiColors(_uiColors), scene(s) {
		config = sceneConfig(fbo, u, uiC, uiColors);
		ui = &u->uis["ui"];
	}

	
	static ofColor getColor(float n, ofxMicroUI * uiColors) {
		if (uiColors->pBool["usePalette"]) {
			return ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getColor(n);
		} else {
			return uiColors->pColorEasy["color"];
		}
	}

	vector <sceneDmtr *> scenes;
	map <string, sceneDmtr *> scenesMap;


	void onUpdate(ofEventArgs &data) {
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				scenesMap[scene]->update();
			}
		}
	}

	void setup() {
		ofAddListener(ofEvents().update, this, &ofxScenes::onUpdate);
		cout << "------" << endl;
		for (auto & s : scenes) {
			cout << ":: adding " << s->name << endl;
			scenesMap[s->name] = s;
			scenesMap[s->name]->setup();
		}
		cout << "------" << endl;
	}

	void draw() {
		ofSetLineWidth(ui->pEasy["lineWidth"]);
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				ofSetColor(getColor(0, uiColors));
				scenesMap[scene]->draw();
			} else {
				
				cout << "scene not found " << scene << endl;
			}
		}
	}
};
