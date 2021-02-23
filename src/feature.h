struct microFeature {
public:

	ofxMicroUI * ui = NULL;
	ofxMicroUI * ui2 = NULL;
	ofxMicroUISoftware * soft = NULL;

	virtual void begin() {
		cout << "begin in primitive feature" << endl;
	};
	virtual void end() {
		cout << "end in primitive feature" << endl;
	};
	virtual void setup() {
		cout << "setup in primitive feature" << endl;
	};
	virtual void update() {
		cout << "update in primitive feature" << endl;
	};
	virtual void uiEvents(ofxMicroUI::element & e) {
		cout << "uiEvents in primitive feature" << endl;
	}

	microFeature(ofxMicroUI * _ui) : ui(_ui) {
		ofAddListener(ui->uiEvent, this, &microFeature::uiEvents);
		setup();
	}
	microFeature(ofxMicroUI * _ui, ofxMicroUI * _ui2, ofxMicroUISoftware * _soft) : ui(_ui), ui2(_ui2), soft(_soft) {
		ofAddListener(ui->uiEvent, this, &microFeature::uiEvents);
		setup();
	}

};







struct featureCam : public microFeature {
public:
	using microFeature::microFeature;

	ofCamera camera3d;
	float pointsPerMeter = 100.0;
	glm::vec3 cameraLook3d = { 0.0f, 0.0f, 0.0f };
	glm::vec3 cameraLookUp = { 0.f, -1.0f, 0.0f };
	glm::vec3 cameraLookUp2 = { 0.f, 1.0f, 0.0f };
	ofNode lookNode;
	
	void setup() override {
		camera3d.setNearClip(0.01);
		camera3d.setFarClip(160 * pointsPerMeter);
	}
	
	void begin() override {
		// if (_ui->pBool["disableDepthTest"]) {
		// 	ofDisableDepthTest();
		// } else {
		// 	ofEnableDepthTest();
		// }
		ofEnableDepthTest();

		float cameraX = ui->pEasy["cameraX"];
		float cameraZ = ui->pEasy["cameraZ"];

		// 1.70, a altura de um adulto em p�
		if (ui->pBool["cameraPolar"]) {
			float a = ui->pEasy["cameraAngle"] + 90.0;
			float d = ui->pEasy["cameraDist"] * pointsPerMeter;
			cameraX = r2x(a, d);
			cameraZ = r2y(a, d);
			cameraX += lookNode.getPosition().x;
			cameraZ += lookNode.getPosition().z;
		}

		// if (!u.pBool["mouseCamera"])
		{
			camera3d.setPosition(cameraX,
							ui->pEasy["cameraY"] * pointsPerMeter,
							cameraZ);
			
			cameraLook3d = glm::vec3(ui->pEasy["lookX"] * pointsPerMeter,
								ui->pEasy["lookY"] * pointsPerMeter,
								ui->pEasy["lookZ"] * pointsPerMeter);
			
			lookNode.setPosition(cameraLook3d);
			
			// camera3d.lookAt(lookNode, cameraLookUp);
			camera3d.lookAt(lookNode, ui->pBool["up"] ? cameraLookUp : cameraLookUp2);
		}

		camera3d.setFov(ui->pEasy["cameraFov"]);
		camera3d.begin();
		ofPushMatrix();

		float rotX = ui->pEasy["accelX"];
		float rotY = ui->pEasy["accelY"];
		float rotZ = ui->pEasy["accelZ"];

		if (ui->pBool["rotCam"]) {
			ui->pFloat["rotX_accum"] += ui->pFloat["rotCamXAuto"];
			ui->pFloat["rotY_accum"] += ui->pFloat["rotCamYAuto"];
			ui->pFloat["rotZ_accum"] += ui->pFloat["rotCamZAuto"];
		}

		ofRotateXDeg(rotX + ui->pEasy["rotCamX"] + ui->pFloat["rotX_accum"]);
		ofRotateYDeg(rotY + ui->pEasy["rotCamY"] + ui->pFloat["rotY_accum"]);
		ofRotateZDeg(rotZ + ui->pEasy["rotCamZ"] + ui->pFloat["rotZ_accum"]);
	}

	void end() override {
		ofPopMatrix();
		camera3d.end();
		ofDisableDepthTest();
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (!e._settings->presetIsLoading && e._ui->uiIsCreated) {
			if (e.name == "resetLook") {
				ui->getSlider("lookX")->set(0.0);
				ui->getSlider("lookY")->set(0.0);
				ui->getSlider("lookZ")->set(0.0);
			}
			
			 else if (e.name == "resetAutoRotate") {
			 	ui->pFloat["rotX_accum"] = ui->pFloat["rotY_accum"] = ui->pFloat["rotZ_accum"] = 0;
			 }

			 else if (e.name == "resetCam") {
			 	ui->pFloat["rotX_accum"] = ui->pFloat["rotY_accum"] = ui->pFloat["rotZ_accum"] = 0;

			 	ui->getSlider("rotCamX")->set(0);
			 	ui->getSlider("rotCamY")->set(0);
			 	ui->getSlider("rotCamZ")->set(0);

			 	ui->getSlider("rotCamXAuto")->set(0);
			 	ui->getSlider("rotCamYAuto")->set(0);
			 	ui->getSlider("rotCamZAuto")->set(0);
			 }
		}
			
		if (e.name == "ortho") {
			if (*e.b) {
				camera3d.enableOrtho();
			} else {
				camera3d.disableOrtho();
			}
		}		
	}
};









struct featureShader : public microFeature {
public:
	using microFeature::microFeature;

	ofShader shader;
	string shaderLoaded = "";
	// map <string, ofShader> shadersMap;
	// map <string, string> shadersMapLoaded;
	string name = "shaders2d";


	void setup() override { 
	}

	void begin() override { 
		if (shader.isLoaded()) {
		// if (ui2->pBool[name + "_on"] && shadersMap[name].isLoaded()) {			
			
			shader.begin();
			shader.setUniform1f("time", ofGetElapsedTimef());
	//		cout << fbo->getWidth() << "x" << fbo->getHeight() << endl;
			shader.setUniform2f("outputDimensions", soft->fboFinal->getWidth(), soft->fboFinal->getHeight());
			// ofxMicroUI * ui = &u.uis[name]; //"ui"+
			
			for (auto & p : ui2->pFloat) {
				float val = ui2->pEasy[p.first];
				shader.setUniform1f(p.first, val);
			}
			
			for (auto & p : ui2->pInt) {
				shader.setUniform1i(p.first, p.second);
			}
			
			for (auto & p : ui2->pBool) {
				shader.setUniform1i(p.first, p.second);
			}

			for (auto & p : ui2->pColor) {
				ofFloatColor c = ui2->pColorEasy[p.first];
				// ofFloatColor c = p.second;
				shader.setUniform4f(p.first, c.r, c.g, c.b, c.a);
			}
			
			for (auto & p : ui2->pImage) {
				shader.setUniform1f("paletaWidth", 200);
				if (ui2->pBool["feed"]) {
					shader.setUniformTexture("paleta", soft->fboFinal->getTexture(), 1);
				} else {
					shader.setUniformTexture(p.first, p.second.getTexture(), 1);
				}
			}
			

	//		for (auto & p : ui2->pPoint) {
	//            shader.setUniform2f(p.first, p.second.x, p.second.y);
	//		}
		}
	}

	void end() override { 
		if (shader.isLoaded()) {
		// if (ui2->pBool[name + "_on"] && shadersMap[name].isLoaded()) {
			shader.end();
		}
	}

	// void reload() {
	// 	cout << "SHADERS RELOAD" << endl;
	// 	for (auto & s : shadersMap) {
	// 		cout << s.first << endl;
	// 		s.second.load(shadersMapLoaded[s.first]);
	// 		// u.uis[s.first].reload();
	// 	}
	// 	ui->reload();
	// }

	void uiEvents(ofxMicroUI::element & e) override {

		if (
		e.name == "shaders2d" ||
		e.name == "shaders3d" ||
		e.name == "shadersgen" ||
		e.name == "shadersGen" ||
		e.name == "shaders2d2"
		) 
		{
			if (ofxMicroUI::dirList * r = dynamic_cast<ofxMicroUI::dirList*>(&e)) {
				string f = r->getFileName();
				if (f != "" & f != shaderLoaded) {
					cout << "SHADERS LOAD " << e.name << "fileName :: " << f << endl;
					shader.load(f);
					shaderLoaded = f;
				}
			} else {
				
			}
		}
		// aqui vai ter bool e tudo mais
	}
};


struct featureCamTest : public microFeature {
public:
	using microFeature::microFeature;
	void setup() override { }
	void begin() override { }
	void end() override { }
	void uiEvents(ofxMicroUI::element & e) override { }
};