ofFbo * fboSmall = &soft.mapFbos["fboSmall"];
string & scene3 = ui->pString["scene3"];

void drawTuboled() {
	fbo = fboSmall;
	uiC = &u.uis["scene3"];
	fboSmall->begin();
	ofClear(0,255);
	drawScene(scene3);
	fboSmall->end();
	fbo = &soft.fbo;

	ofSetColor(255);
	fboSmall->draw(1200,700);

}

void setupTuboled() {
	fboSmall->allocate(110,20);
	fboSmall->begin();
	ofClear(0,255);
	fboSmall->end();
}