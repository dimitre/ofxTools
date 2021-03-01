// converter essa merda em feature


ofxMicroUI * uiSyphon = &u.uis["syphon"];
ofxSyphonClient syphonIn;
ofxSyphonServerDirectory syphonList;

void syphonUpdated(ofxSyphonServerDirectoryEventArgs &arg) {
//	cout << "syphonIn syphonUpdated!!!" << endl;
	uiSyphon->clear();
	vector <string> options;
	for (auto & s : syphonList.getServerList()) {
		cout << s.appName << endl;
		cout << s.serverName << endl;
		string nome = s.serverName + "--" + s.appName;
		options.push_back(nome);
	}
	vector <string> lines = { "radioPipeNoLabel	syphon	" + ofJoinString(options, "|")};
	uiSyphon->createFromLines(lines);
}

void syphonInSetup() {
	syphonIn.setup();
	syphonList.setup();
	ofAddListener(syphonList.events.serverAnnounced, this, &ofApp::syphonUpdated);
	ofAddListener(syphonList.events.serverRetired, this, &ofApp::syphonUpdated);
	ofAddListener(uiSyphon->uiEvent, this, &ofApp::syphonInUIEvent);
}

void syphonInUIEvent(ofxMicroUI::element & e) {
	if (e.name == "syphon") {
		vector <string> sp = ofSplitString(*e.s, "--");
		if (sp.size()>1) {
			string server = sp[0];
			string app = sp[1];
			cout << "connecting to syphonIn :: "<< server << " -- " << app << endl;
			syphonIn.set(server,app);
		}
	}
}
