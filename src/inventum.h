ofxMicroUI u;
ofxMicroUI *ui  = &u.uis["ui"];
ofxMicroUI *uiC  = &u.uis["scene"];
string *cena = &ui->pString["scene"];
string & scene = ui->pString["scene"];

ofxMicroUISoftware soft;
ofFbo * fbo = &soft.fbo;
ofFbo * fbo2 = &soft.fbo2;
void uiEvents(ofxMicroUI::element & e);

#include "shaders.h"
#include "cam.h"
#include "scene.h"

#ifdef USESYPHON
	ofxSyphonServer syphonOut;
#endif


void startBlending() {
	ofEnableAlphaBlending();
	if (ui->pString["blend"] == "no") {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	}
	else if (ui->pString["blend"] == "add") {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	}
	else if (ui->pString["blend"] == "screen") {
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	}
}



void shortcutUIEvent(ofxMicroUI::element & e) {
	if (ofIsStringInString(e.name, "_shortcut")) {
		if (!e._ui->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcut");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->set(val);
		}
	}
}

ofImage logo;

void setupInventum() {
	logo.load("inv.png");
	
	soft.fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fbo2.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fboFinal = &soft.fbo2;
	
	if (ofGetTargetPlatform() != OF_TARGET_OSX) {
		u.toggleVisible();
		ofSetFullscreen(true);
		ofHideCursor();
	}
}

void drawLogo() {
	if (ui->pBool["logo"] && logo.isAllocated()) {
		ofSetColor(255);
		float scale = ui->pFloat["logoScale"];
		float w = logo.getWidth() * scale;
		float h = logo.getHeight() * scale;
		logo.draw(ui->pInt["logoX"], ui->pInt["logoY"], w, h);
	}
}


void drawSecondWindow(ofEventArgs & args) {
	ofBackground(0);
	ofSetColor(255);
	soft.fboFinal->draw(0,0);
	soft.fboFinal->draw(-1920,160);
	soft.fboFinal->draw(0,320);
	soft.fboFinal->draw(-1920,480);
}
