map <string, ofShader> shadersMap;
map <string, string> shadersMapLoaded;
ofxMicroUI * uiShader = &u.uis["shaders"];

void shadersSetup() {
	ofAddListener(uiShader->uiEvent, this, &ofApp::shadersUIEvent);
}

void shadersReload(string s = "") {
	cout << "SHADERS RELOAD" << endl;
	for (auto & s : shadersMap) {
		cout << s.first << endl;
		s.second.load(shadersMapLoaded[s.first]);
		u.uis[s.first].reload();
	}
	uiShader->reload();
}

void shadersUIEvent(ofxMicroUI::element & e) {
//	cout << "shadersUIEvent " << e.name << endl;
	if (e.name == "shaders2d" || e.name == "shaders3d" || e.name == "shadersGen") {
		if (ofxMicroUI::dirList * r = dynamic_cast<ofxMicroUI::dirList*>(&e)) {
			string f = r->getFileName();
			if (f != "" & f != shadersMapLoaded[e.name]) {
				cout << "SHADERS LOAD " << e.name << "fileName :: " << f << endl;
				shadersMap[e.name].load(f);
				shadersMapLoaded[e.name] = f;
			}
		} else {
			
		}
		// aqui vai ter bool e tudo mais
	}
}

void beginShader(string name) {
	if (uiShader->pBool[name + "_on"] && shadersMap[name].isLoaded()) {
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

		for (auto & p : ui->pColor) {
			ofFloatColor c = p.second;
			s->setUniform4f(p.first, c.r, c.g, c.b, c.a);
		}
		
		for (auto & p : ui->pImage) {
			s->setUniform1f("paletaWidth", 200);
			if (ui->pBool["feed"]) {
				s->setUniformTexture("paleta", soft.fbo.getTexture(), 1);
			} else {
				s->setUniformTexture(p.first, p.second.getTexture(), 1);
			}
		}
		

//		for (auto & p : ui->pPoint) {
//            s->setUniform2f(p.first, p.second.x, p.second.y);
//		}
	}
}

void endShader(string name) {
	if (uiShader->pBool[name + "_on"] && shadersMap[name].isLoaded()) {
		shadersMap[name].end();
	}
}
