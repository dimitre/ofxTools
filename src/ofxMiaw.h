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

	
#define USEASSIMP 1
#define USESVG 1
	ofxMicroUI * _u = NULL;
	ofFbo * fbo = NULL;
	ofxMicroUI * ui = NULL;
	ofxMicroUI * uiC = NULL;
	string & scene;
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _ui, ofxMicroUI * _uiC, string & s) :
	fbo(_f), ui(_ui), uiC(_uiC), scene(s) {
//		scene = ui->pString["scene"];
	}
	
	void setup() {
		setupScenesNova();
	}
	
	void draw() {
		drawScenesNova();
	}

#include "scenes.h"
	
};
