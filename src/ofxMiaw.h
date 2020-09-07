//

#define USESVG 1
#ifdef USESVG
#include "ofxSvg.h"
#endif

//#define USEASSIMP 1
#ifdef USEASSIMP
#include "ofxAssimpModelLoader.h"
#endif

class ofxScenes : public ofBaseApp {
public:

	ofxMicroUI * u = NULL;
	ofFbo * fbo = NULL;
	ofxMicroUI * uiColors = NULL;
	string & scene;
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) {
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
	
	#define SCENESALL 1

	#include "scenes.h"
	#ifdef SCENESALL
	#include "scenesAll.h"
	#endif

	vector <sceneDmtr *> scenes;
	map <string, sceneDmtr*> scenesMap;
	
	void setup() {
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
#endif

		scenes.push_back(new sceneImage(u, fbo));
		scenesMap["image"] = scenes.back();


		for (auto & s : scenesMap) {
			s.second->setup();
			s.second->isSetup = true;
		}
	}
	
	
	
	void draw() {
		if (scene != "") {
			ofSetColor(getColor(0, uiColors));
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				scenesMap[scene]->draw();
			} else {
				cout << "scene not found " << scene << endl;
			}
		}
	}
};
