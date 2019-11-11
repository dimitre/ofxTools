map <string, ofShader> shadersMap;
map <string, string> shadersMapLoaded;

//ofxMicroUI * uiShader = ui;
ofxMicroUI * uiShader = &u.uis["shaders"];
ofxMicroUI * uiShaders2d = &u.uis["shaders2d"];
ofxMicroUI * uiShaders3d = &u.uis["shaders3d"];

void shadersSetup() {
	ofAddListener(uiShader->uiEvent, this, &ofApp::shadersUIEvent);
//	ofAddListener(uiShaders2d->uiEvent, this, &ofApp::shadersUIEvent);
//	ofAddListener(uiShaders3d->uiEvent, this, &ofApp::shadersUIEvent);
}

void shadersReload() {
	cout << "SHADERS RELOAD" << endl;
	for (auto & s : shadersMap) {
		s.second.load(shadersMapLoaded[s.first]);
	}
	uiShader->reload();
	uiShaders2d->reload();
	uiShaders3d->reload();
}

void shadersUIEvent(ofxMicroUI::element & e) {
	if (e.name == "shaders2d" || e.name == "shaders3d" || e.name == "shadersgenerators") {
		if (ofxMicroUI::dirList * r = dynamic_cast<ofxMicroUI::dirList*>(&e)) {
			string f = r->getFileName();
			if (f != "" & f != shadersMapLoaded[e.name]) {
//				cout << "SHADERS " << e.name << "fileName :: " << f << endl;
				shadersMap[e.name].load(f);
				shadersMapLoaded[e.name] = f;
			}
		} else {
		}
		// aqui vai ter bool e tudo mais
	}
}

void beginShader(string name) {
	if (uiShader->pBool[name] && shadersMap[name].isLoaded()) {
		ofShader *s = &shadersMap[name];
		s->begin();
		s->setUniform1f("time", ofGetElapsedTimef());
//		cout << fbo->getWidth() << "x" << fbo->getHeight() << endl;
		s->setUniform2f("outputDimensions", fbo->getWidth(), fbo->getHeight());
		ofxMicroUI * ui = &u.uis[name]; //"ui"+
		
		for (auto & p : ui->pFloat) {
			float val = ui->pEasy[p.first];
			s->setUniform1f(p.first, val);
		}
		
		for (auto & p : ui->pInt) {
			s->setUniform1i(p.first, p.second);
		}
		
		for (auto & p : ui->pBool) {
			s->setUniform1i(p.first, p.second);
		}

//		for (auto & p : ui->pPoint) {
//            s->setUniform2f(p.first, p.second.x, p.second.y);
//		}
	}
}

void endShader(string name) {
	if (uiShader->pBool[name] && shadersMap[name].isLoaded()) {
		shadersMap[name].end();
	}
}
