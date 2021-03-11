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
//        string * scene = NULL;
//		int margem = 100;
//		ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);
        
		void afterSetup() {
			ui = &u->uis["ui"];
//            if (scene == NULL) {
//                scene = &ui->pString["scene"];
//            }
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
            cout << "sceneConfig init, uiC =" << uiC->uiName << endl;
		};
		
		sceneConfig(ofFbo * _fbo, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors) :
		fbo(_fbo), u(_u), uiC(_uiC), uiColors(_uiColors) {
			afterSetup();
		};
	} config;
	
	#include "sceneDmtr.h"

//	ofFbo * fbo = NULL;
//	ofxMicroUI * u = NULL;
//	ofxMicroUI * uiC = NULL;
//	ofxMicroUI * uiColors = NULL;
	ofxMicroUI * ui = NULL;
    sceneDmtrBasic * _scene = NULL;
    vector <sceneDmtrBasic *> scenes;
    string lastScene = "";
//	ofxScenes() {}
    
    void afterSetup() {
        ofAddListener(ui->uiEvent, this, &ofxScenes::uiEvents);
    }
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, string s) : sceneName(s)
//    : fbo(_f), u(_u), scene(s)
    {
		config = sceneConfig(_f, _u);
//		uiColors = &u->uis["colors"];
		ui = &_u->uis["ui"];
        afterSetup();
	}
	
	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, string s) : sceneName(s)
//    : fbo(_f), u(_u), uiC(_uiC), scene(s)
    {
        config = sceneConfig(_f, _u, _uiC);
//		uiColors = &_u->uis["colors"];
		ui = &_u->uis["ui"];
        afterSetup();
	}

	ofxScenes(ofFbo * _f, ofxMicroUI * _u, ofxMicroUI * _uiC, ofxMicroUI * _uiColors, string s) : sceneName(s)
//    : fbo(_f), u(_u), uiC(_uiC), uiColors(_uiColors), scene(s)
    {
        config = sceneConfig(_f, _u);
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
		cout << "------" << endl;
		for (auto & s : scenes) {
			cout << "ofxScenes :: adding " << s->name << endl;
            s->setup();
		}
		cout << "------" << endl;
	}
    
    void onUpdate(ofEventArgs &data) {
        if (_scene != NULL) {
            _scene->update();
        }
    }

	void draw() {
		ofSetLineWidth(ui->pEasy["lineWidth"]);
//        cout << config.scene << endl;
//        cout << sceneName ;

        if (_scene != NULL) {
//            cout << " : " ;
//            cout << _scene->name ;
            _scene->draw();
        }
//        cout << endl;
	}
    
    string sceneName = "scene";
    
    void uiEvents(ofxMicroUI::element & e) {
        if (e.name == sceneName) {
            string scene = *e.s;
            if (scene != lastScene) {
//                cout << "change scene, name = " << scene << endl;
                if (_scene != NULL) {
                    _scene->unselect();
                }
                _scene = NULL;
                bool found = false;
                if (scene != "_") {
                    for (auto & s : scenes) {
    //                    cout << s << endl;
    //                    cout << scene << endl;
    //                    cout << "----" << endl;
                        if (s->name == scene) {
                            _scene = s;
                            _scene->select();
                            found = true;
                        }
                    }
                    if (!found) {
                        cout << "|||||||||||||||||||| ofxScenes not found :: " << scene << endl;
                    }
                }
                lastScene = scene;
            }
        }
    }
};
