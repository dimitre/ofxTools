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
		int screen = 0;
		
#ifdef USEFFT
		ofxMicroUIFFT * fft = NULL;
#endif
		ofFbo * fbo = NULL;
		ofxMicroUI * u = NULL;
		ofxMicroUI * ui = NULL;
		ofxMicroUI * uiC = NULL;
		ofxMicroUI * uiColors = NULL;
        
		void afterSetup() {
			ui = &u->uis["ui"];
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
//            cout << "sceneConfig init, uiC =" << uiC->uiName << endl;
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors) :
		fbo(_fbo), u(_u), uiC(_uiC), uiColors(_uiColors) {
			afterSetup();
		};
	} config;
	
	#include "sceneDmtr.h"

	ofxMicroUI * ui = NULL;
    sceneDmtrBasic * _scene = NULL;
    vector <sceneDmtrBasic *> scenes;
    string lastScene = "";
    string sceneName = "scene";

    void afterSetup() {
//        scenes.clear();
        ofAddListener(ui->uiEvent, this, &ofxScenes::uiEvents);
        ofAddListener(ui->uiEventMaster, this, &ofxScenes::uiEventMaster);
    }
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string s) : sceneName(s)
    {
		config = sceneConfig(_f, _u);
//		uiColors = &u->uis["colors"];
		ui = &_u->uis["ui"];
        afterSetup();
	}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, string s) : sceneName(s)
    {
        config = sceneConfig(_f, _u, _uiC);
//		uiColors = &_u->uis["colors"];
		ui = &_u->uis["ui"];
        afterSetup();
	}

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors, string s) : sceneName(s)
    {
        cout << "|||||| ofxSCENES uiColors = " << _uiColors->uiName << endl;
        config = sceneConfig(_f, _u, _uiC, _uiColors);
		ui = &_u->uis["ui"];
        afterSetup();
	}
	
	static ofColor getColor(float n, ofxMicroUI * uiColors) {
		if (uiColors->pBool["usePalette"]) {
			return ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getColor(n);
		} else {
			return uiColors->pColorEasy["color"];
		}
	}

	void setup() {
		ofAddListener(ofEvents().update, this, &ofxScenes::onUpdate);
//        cout << "ofxScenes Adding number of scenes: " << scenes.size() << endl;
        
		for (auto & s : scenes) {
            if (s->name != "") {
                cout << "ofxScenes " << sceneName << " :: adding " << s->name << endl;
                if (s->config == NULL) {
                    s->addConfig(&config);
                } else {
                }
                s->setup();
            }
		}
//		cout << "------" << endl;
	}
    
    void onUpdate(ofEventArgs &data) {
        if (_scene != NULL) {
            _scene->update();
        }
    }

	void draw() {
		ofSetLineWidth(ui->pEasy["lineWidth"]);
        if (_scene != NULL) {
//            cout << _scene->name << endl;
            _scene->draw();
        } else {
//            cout << "scene is null" << endl;
        }
	}
    
    void uiEventMaster(string & s) {
        if (s == "setup") {
            setup();
        }
    }
    void uiEvents(ofxMicroUI::element & e) {
//        cout << "uiEvents " << e.name << ":" << sceneName << endl;
        if (e.name == sceneName) {
            string scene = *e.s;
            if (scene != lastScene) {
                if (_scene != NULL) {
                    _scene->unselect();
                }
                _scene = NULL;
                bool found = false;
                if (scene != "_") {
                    for (auto & s : scenes) {
                        if (s->name == scene) {
                            _scene = s;
                            _scene->select();
                            found = true;
                        }
                    }
                    if (!found) {
                        cout << "|||| ofxScenes not found :: " << scene << endl;
                    }
                }
                lastScene = scene;
            }
        }
    }
};
