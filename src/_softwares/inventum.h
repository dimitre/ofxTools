//	ofxMicroUIMidiController midiController;
ofxMicroUI u;
ofxMicroUI *ui  = &u.uis["ui"];
ofxMicroUI *uiC  = &u.uis["scene"];
string & scene = ui->pString["scene"];

ofxMicroUISoftware soft;
ofFbo * fbo = &soft.fbo;
ofFbo * fbo2 = &soft.fbo2;
//ofFbo * fbo3 = &soft.fbo3;

void uiEvents(ofxMicroUI::element & e);

#include "shaders.h"
#include "cam.h"
//#include "scenes.h"
#include "tools.h"

void preSetupInventum() {
	setupCam_3d();
//	u._settings->useFixedLabel = true;
	shadersSetup();
}

void setupInventum() {
	soft.fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fbo2.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
//	soft.fbo3.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	soft.fboFinal = &soft.fbo2;
	soft.setUI(&u);
}


//ofImage logo;
ofImage * logo = &ui->pImage["logo"];



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



//void drawLogo(string name = "") {
////	cout << "drawLogo" << endl;
//	
//	if (ui->pBool["logo"] && logo->isAllocated()) {
////		cout << "draw yes " << endl;
////		ofSetColor(255);
//		float scale = ui->pFloat["logoScale"];
//		float w = logo->getWidth() * scale;
//		float h = logo->getHeight() * scale;
//		logo->draw(ui->pInt["logoX"], ui->pInt["logoY"], w, h);
//		//if () }
//		if (name != "")
//		{
//			int x = ui->pInt["logoX"] + 7;
//			int y = ui->pInt["logoY"] + h + 24;
//			ofSetColor(getCor(0));
////			u._settings->font.drawString(name + "\nStudio Mirabilis & Dmtr.org", x, y);
//			u._settings->font.drawString(name + "\nStudio Mirabilis & Vision Artes", x, y);
//		}
//	}
//}
