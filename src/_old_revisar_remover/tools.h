
// REVISAR ISSO

#ifdef BLENDING
void startBlendingMode() {
	string * s = &uiColors->pString["blend"];
	if (*s == "alpha") {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	}
	else if (*s == "add") {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	}
	else if (*s == "screen") {
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	}
	else if (*s == "multiply") {
		ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
	}
	else if (*s == "subtract") {
		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	}
}
#endif

//void shortcutUIEvent(ofxMicroUI::element & e) {
//	if (ofIsStringInString(e.name, "_shortcut")) {
//		if (!e._settings->presetIsLoading && *e.s != "") {
//			vector <string> explode = ofSplitString(e.name, "_shortcut");
//			float val = ofToFloat(*e.s);
//			e._ui->getSlider(explode[0])->set(val);
//		}
//	}
//}


// separar em outro .h depois
#ifdef USEAUTOPILOT
float nextChange = 0;
int nPreset = -1;
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





//void startBlending() {
//	ofEnableAlphaBlending();
//	if (ui->pString["blend"] == "no") {
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//	}
//	else if (ui->pString["blend"] == "add") {
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
//	}
//	else if (ui->pString["blend"] == "screen") {
//		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
//	}
//}
