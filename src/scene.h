#include "sceneNova.h"

//float updown = 0;
//
//map <string, float> incrementadorTemporal;
//float incrementa(string qual) {
//	string uniqueId = uiC->uiName + qual;
//	incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
//	return incrementadorTemporal[uniqueId];
//}

#ifdef USECOLOR
ofColor getCor(float qual) {
	ofColor c = ui->pColor["cor"].getLerped(ui->pColor["cor2"], qual);
	c.a = ui->pEasy["alpha"];
	return c;
	//return ofColor(ui->pColor["cor"], ui->pEasy["alpha"]);
}
#else
ofColor getCor(float qual) {
	return ofColor(255, ui->pEasy["alpha"]);
}
#endif


//void drawMesh(ofMesh * m) {
//	if (ui->pString["draw"] == "wire") {
//		m->drawWireframe();
//	}
//	if (ui->pString["draw"] == "faces") {
//		m->drawFaces();
//	}
//	if (ui->pString["draw"] == "points") {
//		//cout << "aqui " << endl;
//		glDisable(GL_POINT_SMOOTH);
//		glPointSize(ui->pEasy["pointSize"]);
////		glDisable(GL_POINT_SMOOTH);
//		m->draw(OF_MESH_POINTS);
//	}
//}

//void setupScene() {
//}

void drawScene(string scene) {
	
#ifdef USEBPM
	if (uiBpm->pBool["bpm"]) {
		updown = bpm.getPercent();
		if (uiBpm->pString["wave"] == "sin") {
			updown = sin(bpm.getPercent() * 3.1415926) *0.5 + .5;
//			cout << updown << endl;
		}
	} else {
		updown = fft.updown;
	}
#endif
	
	
	drawScenesNova();

	if (scene == "image") {
		uiC->pImage["image"].draw(0,0);
	}
	
	else if (scene == "rect") {
		float audio = fmod(ofGetElapsedTimef() * .5, 1.0);
		float w = uiC->pFloat["w"] * fbo->getWidth();
		float h = uiC->pFloat["h"] * fbo->getWidth();
		float x = (uiC->pFloat["x"] + uiC->pFloat["xAudio"]*audio) * fbo->getWidth();
		float y = uiC->pFloat["y"] * fbo->getWidth();
		float qualCor = audio * uiC->pFloat["qualCor"];
		ofSetColor(getCor(qualCor));
		ofDrawRectangle(x, y, w, h);
	}
	
	// solido pulsante pra festas
	else if (scene == "pulse") {
		ofColor cor = getCor(0);
		if (!uiC->pBool["solid"]) {
			cor.a = ofMap(sin(ofGetElapsedTimef() * uiC->pFloat["multTime"]), -1, 1, 0, 255);
		}
		ofSetColor(cor);
		ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}

	// cena teste pra interface nova. besta. preciso tirar.
	else if (scene == "circles") {
		for (int a=1; a<4; a++) {
			string id = ofToString(a);
			if (uiC->pBool["circle" + id]) {
				ofSetColor(
					uiC->pFloat["r" + id],
					uiC->pFloat["g" + id],
					uiC->pFloat["b" + id]);
				ofDrawCircle(
					uiC->pFloat["x" + id],
					uiC->pFloat["y" + id],
					uiC->pFloat["radius" + id]);
			}
		}
	}
}
