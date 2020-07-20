float nextChange = 0;
int nPreset = -1;

// separar em outro .h depois
#ifdef USEAUTOPILOT
ofxMicroUI * uiAutoPilot = &u.uis["autoPilot"];
void setupAutoPilot() {
	ofAddListener(ofEvents().draw, this, &ofApp::onAutoPilot);
	uiAutoPilot->saveMode = ofxMicroUI::MASTER;
	uiAutoPilot->loadMode = ofxMicroUI::MASTER;
}

void onAutoPilot(ofEventArgs &data) {
	autoPilot();
}

void autoPilot() {
	if (uiAutoPilot->pBool["autoPilot"]) {
		if (ofGetElapsedTimef() > nextChange) {
			nPreset = (nPreset + 1) % uiAutoPilot->pInt["maxPreset"];
			u.loadPresetByIndex(nPreset);
			if (uiAutoPilot->pBool["usePresetTime"]) {
				nextChange = ofGetElapsedTimef() + ui->pFloat["presetMinutes"] * 60.0;
//				nextChange = ofGetElapsedTimef() + ui->pFloat["presetMinutes"] * 1.0;
			} else {
				nextChange = ofGetElapsedTimef() + uiAutoPilot->pInt["nSeconds"];
			}
		}
		
		string debug =
		ofToString(nPreset) +
		" nc:" + ofToString(nextChange) +
		" t:" + ofToString(ofGetElapsedTimef())
		;
		
		uiAutoPilot->getInspector("debug")->set(debug);
		//uiAutoPilot->getInspector("nextChange")->set();
	}
}
#endif

//
//float shaper(float in, float inMin, float inMax, float outMin, float outMax, float shaper){
//	// (1) convert to pct (0-1)
//	float pct = ofMap (in, inMin, inMax, 0, 1, true);
//	// raise this number to a power
//	pct = powf(pct, shaper);
//	float out = ofMap(pct, 0,1, outMin, outMax, true);
//	return out;
//}



void shortcutUIEvent(ofxMicroUI::element & e) {
	if (ofIsStringInString(e.name, "_shortcut")) {
		if (!e._settings->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcut");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->set(val);
		}
	}
}



//
//class picker {
//public:
//	ofColor color = ofColor(0);
//	ofColor lastColor = ofColor(0);
//	ofVec2f pos;
//	int index;
//	int note;
//	string name = "";
//	
//	picker(ofPoint p, int i) : pos(p), index(i) {}
//	
//	ofPixels * _pixels = NULL;
//	float raio = 10;
//
//	void getColor() {
//		lastColor = color;
//		if (_pixels != NULL) {
//			color = _pixels->getColor(pos.x, pos.y);
//		}
//	}
//
//	void draw() {
//		ofFill();
//		ofSetColor(color);
//		// ofDrawCircle(pos.x, pos.y, raio);
//		//ofSetColor(255);
//		ofDrawRectangle(pos.x-raio*.5, pos.y-raio*.5, raio, raio);
//
//		ofNoFill();
//		//ofSetColor(0);
//		//ofDrawRectangle(pos.x-1, pos.y+1, raio, raio);
//		ofSetColor(255, 0,70);
//		ofDrawRectangle(pos.x -raio*.5, pos.y -raio*.5, raio, raio);
//		ofFill();
//	}
//	
//	void drawLabel() {
//		ofSetColor(0);
//		ofDrawBitmapString(name, pos.x - raio*.5,
//						   pos.y + raio*.5 + 4);
//		ofSetColor(255);
//		ofDrawBitmapString(name, pos.x - raio*.5 + 0.25,
//						   pos.y + raio*.5 + 4 - 0.25);
////		ofDrawBitmapString(name, pos.x - raio *.5, pos.y + raio*.5)
//	}
//};
//
//vector <picker> pickers;
//
//void generatePickers() {
//	pickers.clear();
//	int index = 0;
//	for (int a=0; a<11; a++) {
//		float x = ofMap(a, 0, 11, 50, fbo->getWidth() - 50);
//		float y = 100;
//		//pickers.push_back(picker(ofPoint(x,y), a));
//		pickers.emplace_back(ofPoint(x,y), a);
//		//pickers.back()->_pixels = &pixels;
//	}
//}

