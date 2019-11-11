//	ofxMicroUIMidiController midiController;


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
#include "tools.h"

#ifdef USESYPHON
	ofxSyphonServer syphonOut;
#endif

void preSetupInventum() {
	setupCam_3d();
	u._settings->useFixedLabel = true;
	shadersSetup();
}

void setupInventum() {

//	logo.load("inv2b.png");
	
	soft.fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fbo2.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fboFinal = &soft.fbo2;
	
	soft.setUI(&u);

//	ofSetVerticalSync(false);

//	if (ofGetTargetPlatform() != OF_TARGET_OSX) {
//		u.toggleVisible();
//		ofSetFullscreen(true);
//		ofHideCursor();
//	}
}


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



//ofImage logo;
ofImage * logo = &ui->pImage["logo"];

void drawLogo() {
//	cout << "drawLogo" << endl;
	
	if (ui->pBool["logo"] && logo->isAllocated()) {
//		cout << "draw yes " << endl;
//		ofSetColor(255);
		float scale = ui->pFloat["logoScale"];
		float w = logo->getWidth() * scale;
		float h = logo->getHeight() * scale;
		logo->draw(ui->pInt["logoX"], ui->pInt["logoY"], w, h);
		//if () }
		{
			int x = ui->pInt["logoX"] + 7;
			int y = ui->pInt["logoY"] + h + 24;
			ofSetColor(getCor(0));
			u._settings->font.drawString("METAESPAÇO\nStudio Mirabilis & Dmtr.org", x, y);
			//ofDrawBitmapString("Studio Mirabilis & Dmtr.org", x, y);
		}
	}
	
}

#ifdef INFINITO
void drawSecondWindow(ofEventArgs & args) {
	ofBackground(0);
	ofSetColor(255);
	soft.fboFinal->draw(0,0);
	soft.fboFinal->draw(0,384);
}

#else
void drawSecondWindow(ofEventArgs & args) {
	ofBackground(0);

	ofSetColor(255);
	soft.fboFinal->draw(0,0);
	soft.fboFinal->draw(-1920,160);
	soft.fboFinal->draw(0,320);
	soft.fboFinal->draw(-1920,480);

	if (u.pBool["servico"]) {
		ofSetColor(255,0,0);
		ofDrawBitmapString("1", 20, 20);
		ofDrawBitmapString("2", 20, 180);
		ofDrawBitmapString("3", 20, 340);
		ofDrawBitmapString("4", 20, 500);
	}
}
#endif
