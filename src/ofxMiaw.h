//

#define USESVG 1
#ifdef USESVG
#include "ofxSvg.h"
#endif

#define USEASSIMP 1
#ifdef USEASSIMP
#include "ofxAssimpModelLoader.h"
#endif

class ofxScenes : public ofBaseApp {
public:

	#include "scenes.h"

	ofFbo * fbo = NULL;
	ofxMicroUI * u = NULL;
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
		
		scenes.push_back(new sceneModel(u, fbo));
		scenesMap["model2"] = scenes.back();
#endif

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

		scenes.push_back(new sceneText(u, fbo));
		scenesMap["text"] = scenes.back();

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
