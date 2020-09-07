ofxMicroUI u;
ofxMicroUI *ui = &u.uis["ui"];
ofxMicroUI *uiC  = &u.uis["scene"];
ofxMicroUI *uiColors  = &u.uis["colors"];
string & scene = ui->pString["scene"];

ofxMicroUISoftware soft;
ofFbo * fbo = &soft.fbo;//

#ifdef USESYPHON
ofxSyphonServer syphonOut;
#endif

// #include "polar.h"
#include "DmtrCairo.h"
// #include "tools.h"

ofxMicroUIRemote uiRemote;

void remoteMessage(string & e) {
	cout << "remote message !" << endl;
	if (ofIsStringInString(e, "/p/")) {
		vector <string> partes = ofSplitString(e, "/");
		string p = partes[2];
		u.presetElement->set(p);
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


void beginMiaw() {
	ofPushMatrix();

	useCairo = (ui->pBool["useCairo"] || savingCairo) && !uiC->pBool["is3d"];
	if (useCairo) {
		beginCairo();

//		if (uiColors->pString["background"] == "no") {
//			ofClear(0,0);
//		}
//		else if (uiColors->pString["background"] == "solid") {
//			ofClear(uiColors->pColorEasy["bg"]);
//		}
		miawBg();
		startCairoBlendingMode();
	} else {

		if (!uiColors->pBool["bgAfter"]) {
			miawBg();
		}
		startBlendingMode();
	}
//	ofSetColor(uiColors->pColorEasy["color"]);
	ofSetLineWidth(ui->pEasy["lineWidth"]);
}

void endMiaw() {
	ofDisableBlendMode();

	if (useCairo) {
		endCairo();
	} else {
		
	}
	ofPopMatrix();
}

void beginGl() {
	if (!useCairo) {
		if (ui->pBool["rebind"]) {
			fbo2->draw(0,0,200,200);
			fbo2->getTexture().bind();
		}

		if (ui->pBool["useTexture"]) {
			ui->pImage["tex"].getTexture().bind();
		}
	
		if (ui->pBool["usePointSprite"]) {
			ofEnablePointSprites();
			ofDisableDepthTest();
			ui->pImage["pointSprite"].getTexture().bind();
		}
		glPointSize(ui->pEasy["pointSpriteSize"]);
	}
}

void endGl() {
	if (!useCairo) {
		if (ui->pBool["usePointSprite"]) {
			ofDisablePointSprites();
			ui->pImage["pointSprite"].getTexture().unbind();
		}
	
		if (ui->pBool["useTexture"]) {
			ui->pImage["tex"].getTexture().unbind();
		}

		if (ui->pBool["rebind"]) {
			fbo2->getTexture().unbind();
		}
	}
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
		ofBackground(0);
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
