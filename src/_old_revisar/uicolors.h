ofxMicroUI * uiColors	= &u.uis["colors"];

void uiColorsSetup() {
	uiColors->saveMode = ofxMicroUI::MASTER;
	uiColors->loadMode = ofxMicroUI::MASTER;
	ofAddListener(uiColors->uiEvent, this, &ofApp::colorsUIEvent);
}

void colorsUIEvent(ofxMicroUI::element & e) {
	u._settings->uiColorBg 			= ofColor(uiColors->pFloat["colorUIBg"], uiColors->pFloat["colorUIOpacity"]);
	u._settings->colorVal 			= uiColors->pFloat["colorVal"];
	u._settings->colorBg 			= uiColors->pFloat["colorBg"];
	u._settings->useBgRainbow 		= uiColors->pBool["useBgRainbow"];
	u._settings->useLabelRainbow 	= uiColors->pBool["useLabelRainbow"];
	//u._settings->colorLabel 		= uiColors->pFloat["colorLabel"];
	u._settings->colorLabel			= ofColor::fromHsb(
												uiColors->pFloat["hueLabel"],
												uiColors->pFloat["satLabel"],
												255);
	u._settings->alertColor			= ofColor::fromHsb(uiColors->pFloat["hueAlert"], 255, 255);
	u._settings->alertColor2			= ofColor::fromHsb(uiColors->pFloat["hueAlert2"], 255, 255);
}
