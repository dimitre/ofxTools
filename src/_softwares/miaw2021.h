ofxMicroUI u;
ofxMicroUI *ui = &u.uis["ui"];
ofxMicroUI *uiC  = &u.uis["scene"];
ofxMicroUI *uiColors  = &u.uis["colors"];
ofxMicroUISoftware soft;
ofFbo * fbo = &soft.fbo;//
ofFbo * fbo2 = &soft.fbo2;
ofFbo * fbo3 = &soft.fbo3;

string & scene = ui->pString["scene"];

#ifdef USEREMOTE
ofxMicroUIRemote uiRemote;
#endif

// resquicios do DmtrCairo.h finado
bool useCairo = false;
bool savingCairo = false;
// substituir por modular em brevissimo
#include "cam.h"

// eliminar este aqui.
#include "tools.h"

#include "feature.h"
#include "featureAll.h"

#ifdef USESYPHON
featureSyphonOut syphonOut =  featureSyphonOut(&soft, "MIAWsyphonOut");
featureCairo cairo = featureCairo(&soft, "Cairo");

featureCam cam = featureCam(&soft, "cam");

#endif

ofxMicroUI * uiShaders = &u.uis["shaders"];
featureShader shaders2d =  featureShader(uiShaders, &u.uis["shaders2d"], &soft, "shaders2d");
featureShader shaders2d2 =  featureShader(uiShaders, &u.uis["shaders2d2"], &soft, "shaders2d2");
featureShader shaders3d =   featureShader(uiShaders, &u.uis["shaders3d"], &soft, "shaders3d");
featureShader shadersgen =  featureShader(uiShaders, &u.uis["shadersgen"], &soft, "shadersgem");
featureShader shadersfeed = featureShader(uiShaders, &u.uis["shadersfeed"], &soft, "shadersfeed");

ofxScenes scenes = ofxScenes(fbo, &u, ui->pString["scene"]);

void setupMiaw() {
    cairo.setup();
    ofSetEscapeQuitsApp(false);
    scenes.setup();
    setupCam_3d();
    soft.fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    soft.fbo2.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    soft.fbo3.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    soft.fboFinal = &soft.fbo3;
    soft.setUI(&u);
    
#ifdef USEREMOTE
    uiRemote.loadConfig("_osc.txt");
    uiRemote.oscInfo = ((ofxMicroUI::inspector*)u.getElement("oscInfo"));//()
    uiRemote.oscInfoReceive = ((ofxMicroUI::inspector*)u.getElement("oscInfoReceive"));//()
    ofAddListener(uiRemote.eventMessage, this, &ofApp::remoteMessage);
    uiRemote.setupServer();
    uiRemote.addUI(&u);
    for (auto & u : u.allUIs) {
        uiRemote.addUI(u);
    }
#endif
}

// preset part from OSC
void remoteMessage(string & e) {
	cout << "remote message !" << endl;
}


void miawBg() {
	if (uiColors->pString["background"] == "solid") {
		ofClear(uiColors->pColorEasy["bg"]);
	}
	else if (uiColors->pString["background"] == "gradient") {
		
		if (!useCairo) {
			ofClear(0,0);
			ofMesh fundoMesh;
			fundoMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
			fundoMesh.addVertex(glm::vec3(0, 0, 0));
			fundoMesh.addColor(uiColors->pColorEasy["bg"]);
			fundoMesh.addVertex(glm::vec3(fbo->getWidth(), 0, 0));
			fundoMesh.addColor(uiColors->pColorEasy["bg"]);
			fundoMesh.addVertex(glm::vec3(0, fbo->getHeight(), 0));
			fundoMesh.addColor(uiColors->pColorEasy["bg2"]);
			fundoMesh.addVertex(glm::vec3(fbo->getWidth(), fbo->getHeight(), 0));
			fundoMesh.addColor(uiColors->pColorEasy["bg2"]);
			fundoMesh.draw();
		} else {
			
		}
	}
	else if (uiColors->pString["background"] == "black") {
//		uiColors->pColor["bg"] = ofColor(0);
//		ofClear(uiColors->pColorEasy["bg"]);
		ofClear(0,255);
	}
	else if (uiColors->pString["background"] == "no" || uiColors->pString["background"] == "") {
		ofClear(0,0);
	}
	else if (uiColors->pString["background"] == "palette") {
		ofClear(uiColors->pColorEasy["bgPalette"]);
	}

	else if (uiColors->pString["background"] == "gradpal") {
		if (!useCairo) {
			ofClear(0,0);
			ofMesh fundoMesh;
			fundoMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
			fundoMesh.addVertex(glm::vec3(0, 0, 0));
			fundoMesh.addColor(uiColors->pColorEasy["bgPalette"]);
			fundoMesh.addVertex(glm::vec3(fbo->getWidth(), 0, 0));
			fundoMesh.addColor(uiColors->pColorEasy["bgPalette"]);
			fundoMesh.addVertex(glm::vec3(0, fbo->getHeight(), 0));
			fundoMesh.addColor(uiColors->pColorEasy["bgPalette2"]);
			fundoMesh.addVertex(glm::vec3(fbo->getWidth(), fbo->getHeight(), 0));
			fundoMesh.addColor(uiColors->pColorEasy["bgPalette2"]);
			fundoMesh.draw();
		} else {
			
		}
	}
}


void beginMiaw(bool background = true) {
	ofPushMatrix();

	useCairo = (ui->pBool["useCairo"] || savingCairo) && !uiC->pBool["is3d"];
	if (background) {
		if (useCairo) {
            cairo.begin();
			miawBg();
            
            // todo - atualizar
			cairo.startBlendingMode();
		} else {
			if (!uiColors->pBool["bgAfter"]) {
				miawBg();
			}
			startBlendingMode();
		}
	}
	ofSetLineWidth(ui->pEasy["lineWidth"]);
}

void endMiaw() {
	ofDisableBlendMode();

	if (useCairo) {
        cairo.end();
	} else {
		
	}
	ofPopMatrix();
}


#ifdef USEAUDIOBPM
	ofxMicroUI * uiAudio = &u.uis["audio"];
	ofxMicroUIFFT fft;
	#include "_bpm.h"
	void bpmInfinitoSetup() {
		ofAddListener(uiBpm->uiEvent, this, &ofApp::bpmUIEvent);
		bpmSetup();
		bpm.invokeDivision = std::bind(&ofApp::areiaReadDivision, this);
		bpm.invokeBeat = std::bind(&ofApp::areiaBeat, this);
	}
	void areiaReadDivision() { }
	void areiaBeat() {
		if (bpm.getBeatCount() == 0) {}
	}
#endif




	// WINDOW
	void drawWindow(int pos=0) {
		ofBackground(50);
		ofSetColor(255);
		
		// xaxa tem como otimizar
		ofRectangle fboRect;
		fboRect =
			ofRectangle(0, 0,
				  soft.fboFinal->getWidth() * u.pFloat["fboScaleOut"],
				  soft.fboFinal->getHeight() * u.pFloat["fboScaleOut"]
			);
		soft.fboFinal->draw(fboRect);
	}
	

	void drawSecondWindow1(ofEventArgs & args) {
		drawWindow(u.pInt["screen1"]);
	}
	void drawSecondWindow2(ofEventArgs & args) {
		drawWindow(u.pInt["screen2"]);
	}
	void drawSecondWindow3(ofEventArgs & args) {
		drawWindow(u.pInt["screen3"]);
	}
	void drawSecondWindow4(ofEventArgs & args) {
		drawWindow(u.pInt["screen4"]);
	}
