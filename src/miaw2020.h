ofxMicroUI u;
ofxMicroUI *ui = &u.uis["ui"];
ofxMicroUI *uiC  = &u.uis["scene"];
ofxMicroUI *uiColors  = &u.uis["colors"];

ofxMicroUI *uiTex  = &u.uis["texture"];
string & scene = ui->pString["scene"];
//string scene = ui->pString["scene"];

ofxMicroUISoftware soft;
ofFbo * fbo = &soft.fbo;//

#ifdef USESYPHON
ofxSyphonServer syphonOut;
#endif

// #include "polar.h"
#include "DmtrCairo.h"
// #include "tools.h"

ofxMicroUIRemote uiRemote;

int contagemRandomPreset = 0;

void remoteMessage(string & e) {
	cout << "remote message !" << endl;
	vector <string> partes = ofSplitString(e, "/");
	if (partes.size() > 2) {
		string v = partes[2];
		if (partes[1] == "p") {
			if (v == "r") {
				string randomPresetString = ofToString(contagemRandomPreset);
				contagemRandomPreset = (contagemRandomPreset+1)%10;
				u.presetElement->set(randomPresetString);
			} else {
				u.presetElement->set(v);
			}
		}
		else if (partes[1] == "f") {
			u.set("presetsFolder", v);
		}

		else if (partes[1] == "r") {
			if (v == "c") {
				float v = ofRandom(0,50);
				cout << v << endl;
				uiCam->getSlider("cameraDist")->set(v);
				v = ofRandom(-180, 180);
				uiCam->getSlider("cameraAngle")->set(v);
			}
		}
	}
}

ofxScenes scenes = ofxScenes(fbo, &u, ui->pString["scene"]);


bool useCairo = false;

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
		uiColors->pColor["bg"] = ofColor(0);
		ofClear(uiColors->pColorEasy["bg"]);
		// ofClear(0,255);
	}
	else if (uiColors->pString["background"] == "no" || uiColors->pString["background"] == "") {
		ofClear(0,0);
	}

	// 	if (uiColors->pBool["useBg2"]) {

	// 	} else {
	// 	}
	// } else {
	// 	ofClear(0,0);
	// }
}


void beginMiaw(bool background = true) {
	ofPushMatrix();

	useCairo = (ui->pBool["useCairo"] || savingCairo) && !uiC->pBool["is3d"];
	if (background) {
		if (useCairo) {
			beginCairo();
			miawBg();
			startCairoBlendingMode();
		} else {
			if (!uiColors->pBool["bgAfter"]) {
				miawBg();
			}
			startBlendingMode();
		}
		ofSetLineWidth(ui->pEasy["lineWidth"]);
	}
}

void endMiaw() {
	ofDisableBlendMode();

	if (useCairo) {
		endCairo();
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

ofFbo * fbo2 = &soft.fbo2;
//ofFbo * fbo3 = &soft.fbo3;


#include "shaders.h"
#include "cam.h"
//#include "scenes.h"
#include "tools.h"

void preSetupMiaw() {
	
	ofxMicroUI::expires(1598719562, 30);
	ofSetEscapeQuitsApp(false);
	scenes.setup();

	setupCam_3d();
//	u._settings->useFixedLabel = true;
	shadersSetup();
}

void setupMiaw() {
	soft.fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fbo2.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fboFinal = &soft.fbo2;
	soft.setUI(&u);
	
	uiRemote.loadConfig("_osc.txt");
	uiRemote.oscInfo = ((ofxMicroUI::inspector*)u.getElement("oscInfo"));//()
	uiRemote.oscInfoReceive = ((ofxMicroUI::inspector*)u.getElement("oscInfoReceive"));//()
	ofAddListener(uiRemote.eventMessage, this, &ofApp::remoteMessage);
	uiRemote.setupServer();
	uiRemote.addUI(&u);
	for (auto & u : u.allUIs) {
		uiRemote.addUI(u);
	}
}



	
#ifdef USENDI
	private:
	ofPixels videoPixels;
	
	ofxNDISender sender_;
	ofxNDISendVideo video_;
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
