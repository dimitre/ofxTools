ofVideoGrabber webcam;
ofxMicroUI * uiCam = &u.uis["cam"];
map<string,int> webcamIDFromName;
float webcamNextCheck = 0;
int webcamDevicesNum = 0;
// this approach doesnt work
vector <ofVideoDevice> webcamDevices;

void webcamSetup() {
	//	webcam.setDesiredFrameRate(60);
	webcam.setDeviceID(0);
	webcam.initGrabber(1280, 720);
	ofAddListener(uiCam->uiEvent, this, &ofApp::webcamUIEvent);
	webcamUpdateUI();
	
//	ofAddListener(ofEvents().update, this, &ofApp::webcamUpdate);
}

void webcamUpdateUI() {
	cout << "webcamUpdateUI" << endl;
	
	//vector <ofVideoDevice> webcamDevices = webcam.listDevices();

	//webcamDevices = webcam.listDevices();
	// TODO : Modernizar
	vector <string> cams;
	//vector<ofVideoDevice>
	//int size = webcam.listDevices().size();
	
	// iterar o webcamdevices apenas aqui.
	for (auto & dev : webcam.listDevices()) {
		webcamIDFromName[dev.deviceName] = dev.id;
		cams.push_back(dev.deviceName);
	}
	
	string vals = ofJoinString(cams, "|");
	vector <string> lines = { "radioPipeNoLabel	webcam	"+vals };
	uiCam->clear();
	uiCam->createFromLines(lines);
	webcamDevicesNum = webcam.listDevices().size();

//	uiCam->futureLines.push_back();
}


//
//
////void webcamUpdate(ofEventArgs &data) {
void webcamUpdate() {
	webcam.update();

	// check for new devices. update UI if new devices are connected.
	if (ofGetElapsedTimef() > webcamNextCheck) {
		//cout << "webcamNextCheck" << endl;
		webcamNextCheck = ofGetElapsedTimef() + 1.0;
		if (webcamDevicesNum != webcam.listDevices().size()) {
			webcamUpdateUI();
		}
	}
}

void webcamUIEvent(ofxMicroUI::element & e) {
	cout << "webcamUIEvent" << endl;
	if (e.name == "webcam") {
		if (*e.s != "") {
			cout << *e.s << endl;
			//if (webcam.isOpen()) {
				webcam.close();
			//}
			webcam.setDeviceID(webcamIDFromName[*e.s]);
			//	webcam.setDesiredFrameRate(60);
			webcam.setup(1280, 720);
			//webcam.initGrabber(1280, 720);
		}
	}
}
