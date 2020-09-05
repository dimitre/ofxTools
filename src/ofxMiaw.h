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

	// #include "polar.h"

	ofxMicroUI * u = NULL;
	ofFbo * fbo = NULL;
	// ofxMicroUI * ui = NULL;
	// ofxMicroUI * uiC = NULL;
	// ofxMicroUI * uiColors = NULL;
	string & scene;
	
//	ofxScenes(ofFbo * _f, ofxMicroUI * _ui, ofxMicroUI * _uiC, ofxMicroUI * _uiColors, string & s) :
//	fbo(_f), ui(_ui), uiC(_uiC), uiColors(_uiColors), scene(s) { }

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) { }



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
//		scenes.push_back(new sceneSvg(uiC, ui, fbo));
		scenes.push_back(new sceneSvg(u, fbo));
		scenesMap["svg"] = scenes.back();
#endif
	

#ifdef USEASSIMP
		scenes.push_back(new sceneModel(uiC, ui, fbo));
		scenesMap["model"] = scenes.back();
#endif

		for (auto & s : scenesMap) {
			s.second->setup();
			s.second->isSetup = true;
		}
	}
	
	void draw() {
		if ( scenesMap.find(scene) != scenesMap.end() ) {
			scenesMap[scene]->draw();
		} else {
			cout << "scene not found " << scene << endl;
		}
	}
};
