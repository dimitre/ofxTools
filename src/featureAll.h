struct featureLight : public microFeature {
	public:
	using microFeature::microFeature;

	ofLight luz;
	glm::vec3 pos;
    float pointsPerMeter = 100;
//	float pointsPerMeter = 1;

	void setup() override {
		luz.setAreaLight(100, 100);
	}

	void begin() override {
		if (ui->pBool["on"]) {
			ofEnableLighting();

			pos = glm::vec3(
				ui->pEasy["luzX"] * pointsPerMeter,
				ui->pEasy["luzY"] * pointsPerMeter,
				ui->pEasy["luzZ"] * pointsPerMeter
			);
			
//			ofSetColor(ui->pColorEasy["diffuseColorLuz"]);
//			luz.draw();
			luz.setPosition(pos);
				
			luz.setAttenuation(
				ui->pEasy["lightAttenuationConstant"],
				ui->pEasy["lightAttenuationLinear"],
				ui->pEasy["lightAttenuationQuadratic"]
			);

			luz.setDiffuseColor ( ui->pColorEasy["diffuseColor"]);
			luz.setSpecularColor( ui->pColorEasy["specularColor"]);
			luz.setAmbientColor ( ui->pColorEasy["ambientColor"]);
			luz.enable();
		}
	}

	void end() override {
		if (ui->pBool["on"]) {
			luz.disable();
		ofDisableLighting();
		}
	}
};


struct featureLuzinha : public microFeature {
public:

	// XAXA TEMP
	float beat = 0.0;

	using microFeature::microFeature;
	ofLight luzinha3d;

	void begin() override {
		if (ui->pBool["shadeFlat"]) {
			glShadeModel(GL_FLAT);
			glMatrixMode(GL_MODELVIEW);
		}
		if (ui->pBool["Lighting"]) {
			ofEnableLighting();
			
			if (ui->pBool["on"]) {
				luzinha3d.setAreaLight(ui->pEasy["areaLight"],ui->pEasy["areaLight"]);
				luzinha3d.setAttenuation(
									ui->pEasy["lightAttenuationConstant"],
									ui->pEasy["lightAttenuationLinear"],
									ui->pEasy["lightAttenuationQuadratic"]);
				luzinha3d.setPosition(
									ui->pEasy["luzX"] + ui->pEasy["luzXBeat"] * beat,
									ui->pEasy["luzY"] + ui->pEasy["luzYBeat"] * beat,
									ui->pEasy["luzZ"] + ui->pEasy["luzZBeat"] * beat);
				luzinha3d.setDiffuseColor( ofColor(ui->pEasy["diffuseColorLuz"]));
				luzinha3d.setSpecularColor( ofColor(ui->pEasy["specularColorLuz"]));
				luzinha3d.setAmbientColor( ofColor(ui->pEasy["ambientColorLuz"]));
				luzinha3d.enable();
			}			
			// beginLuzes();
		}
	}

	void end() override {
		if (ui->pBool["Lighting"]) {
			luzinha3d.disable();
			ofDisableLighting();
			ofDisableSeparateSpecularLight();
		}
		// endLuzes();
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "shadeFlat") {
	//		if (*e.b) {
	////			cout << e.name << *e.b << endl;
	//			glShadeModel(GL_FLAT);
	//			glMatrixMode(GL_MODELVIEW);
	//			
	//		} else {
	//			glShadeModel(GL_SMOOTH);
	//			glMatrixMode(GL_MODELVIEW);
	//		}
		}
		
		if (ui->pBool["enableSeparatapaecularLight"]) {
			ofEnableSeparateSpecularLight();
		} else {
			ofDisableSeparateSpecularLight();
		}		
	}
};

// struct featureCam : public microFeatureBase, featurePolar {
	// using microFeatureBase::microFeatureBase;

struct featureCam : public microFeature {
public:
	using microFeature::microFeature;

	ofCamera cam;
	// ofEasyCam cam;
	float pointsPerMeter = 100.0;
	glm::vec3 cameraLook3d = { 0.0f, 0.0f, 0.0f };
	glm::vec3 cameraLookUp = { 0.f, -1.0f, 0.0f };
	glm::vec3 cameraLookUp2 = { 0.f, 1.0f, 0.0f };
	ofNode lookNode;
	
	void setup() override {
        
		// cam.setNearClip(0.01);

	}
	
	void begin() override {
        
        // era ra ser no setup
        cam.setNearClip(0.01 * pointsPerMeter);
        cam.setFarClip(160 * pointsPerMeter);
        
		// if (_ui->pBool["disableDepthTest"]) {
		// 	ofDisableDepthTest();
		// } else {
		// 	ofEnableDepthTest();
		// }
		if (isOk()) {
			ofEnableDepthTest();

			float cameraX = ui->pEasy["cameraX"] * pointsPerMeter;
			float cameraZ = ui->pEasy["cameraZ"] * pointsPerMeter;

			// 1.70, a altura de um adulto em p�
			if (ui->pBool["cameraPolar"]) {
				float a = ui->pEasy["cameraAngle"] + 90.0;
				float d = ui->pEasy["cameraDist"] * pointsPerMeter;
				cameraX = featurePolar::r2x(a, d);
				cameraZ = featurePolar::r2y(a, d);
				cameraX += lookNode.getPosition().x;
				cameraZ += lookNode.getPosition().z;
			}

			// if (!u.pBool["mouseCamera"])
			{
				cam.setPosition(cameraX,
								ui->pEasy["cameraY"] * pointsPerMeter,
								cameraZ);
				
				cameraLook3d = glm::vec3(ui->pEasy["lookX"] * pointsPerMeter,
									ui->pEasy["lookY"] * pointsPerMeter,
									ui->pEasy["lookZ"] * pointsPerMeter);
				
				lookNode.setPosition(cameraLook3d);
				
				// cam.lookAt(lookNode, cameraLookUp);
				cam.lookAt(lookNode, ui->pBool["up"] ? cameraLookUp : cameraLookUp2);
			}

			cam.setFov(ui->pEasy["cameraFov"]);
			cam.begin();

			float rotX = ui->pEasy["accelX"];
			float rotY = ui->pEasy["accelY"];
			float rotZ = ui->pEasy["accelZ"];

			if (ui->pBool["rotCam"]) {
				ui->pFloat["rotX_accum"] += ui->pFloat["rotCamXAuto"];
				ui->pFloat["rotY_accum"] += ui->pFloat["rotCamYAuto"];
				ui->pFloat["rotZ_accum"] += ui->pFloat["rotCamZAuto"];
			}

			ofPushMatrix();
			ofRotateXDeg(rotX + ui->pEasy["rotCamX"] + ui->pFloat["rotX_accum"]);
			ofRotateYDeg(rotY + ui->pEasy["rotCamY"] + ui->pFloat["rotY_accum"]);
			ofRotateZDeg(rotZ + ui->pEasy["rotCamZ"] + ui->pFloat["rotZ_accum"]);
		}
	}

	void end() override {
		if (isOk()) {
			ofPopMatrix();
			cam.end();
			ofDisableDepthTest();
		}
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
				cam.enableOrtho();
			} else {
				cam.disableOrtho();
			}
		}		
	}
};


struct featureShader : public microFeature {
public:
	using microFeature::microFeature;

	ofShader shader;
	string shaderLoaded = "";
	
	// talvez mudar essa pra outra.
	bool isOk() override {
		return shader.isLoaded() && ui->pBool[name];
	}

	void setup() override { 
	}

	void begin() override {
		if (isOk()) {
			shader.begin();
			shader.setUniform1f("time", ofGetElapsedTimef());
			shader.setUniform2f("outputDimensions", soft->fboFinal->getWidth(), soft->fboFinal->getHeight());
			
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
		if (isOk()) {
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
//        cout << "shader ui event " << e.name << endl;
//        cout << e.tag << endl;
		if (e.tag == "shaderFile" && e.name == name)
		{
			if (ofxMicroUI::dirList * r = dynamic_cast<ofxMicroUI::dirList*>(&e)) {
				string f = r->getFileName();
				if (*e.s != "" && f != shaderLoaded) {
					if (*e.s == "_") {
//                        cout << "shader unload" << endl;
						shader.unload();
						shaderLoaded = "";
					} else {
						shader.load(f);
						shaderLoaded = f;
//                        cout << "SHADERS LOAD " << e.name << " :: " << *e.s << endl;
//                        cout << "shader loaded = " << shaderLoaded << endl;
//                        cout << "this name " << name << endl;
					}
				}
			} else {
				
			}
		}
		// aqui vai ter bool e tudo mais
	}
};





struct featureShaderSimple : public microFeature {
public:
	using microFeature::microFeature;

	ofShader shader;
	bool * _on = NULL;

	void setup() override { 
	}

	void begin() override { 
		if (shader.isLoaded() && ui->pBool["on"]) {
			shader.begin();
			shader.setUniform1f("time", ofGetElapsedTimef());
			shader.setUniform2f("outputDimensions", soft->fboFinal->getWidth(), soft->fboFinal->getHeight());
			
			for (auto & p : ui->pFloat) {
				float val = ui->pEasy[p.first];
				shader.setUniform1f(p.first, val);
			}
			
			for (auto & p : ui->pInt) {
				shader.setUniform1i(p.first, p.second);
			}
			
			for (auto & p : ui->pBool) {
				shader.setUniform1i(p.first, p.second);
			}

			for (auto & p : ui->pColor) {
				ofFloatColor c = ui->pColorEasy[p.first];
				shader.setUniform4f(p.first, c.r, c.g, c.b, c.a);
			}
			
			for (auto & p : ui->pImage) {
				shader.setUniform1f("paletaWidth", 200);
				if (ui->pBool["feed"]) {
					shader.setUniformTexture("paleta", soft->fboFinal->getTexture(), 1);
				} else {
					shader.setUniformTexture(p.first, p.second.getTexture(), 1);
				}
			}
	//		for (auto & p : ui->pPoint) {
	//            shader.setUniform2f(p.first, p.second.x, p.second.y);
	//		}
		}
	}

	void end() override { 
		if (shader.isLoaded() && ui->pBool["on"]) {
			shader.end();
		}
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


#ifdef USESYPHON
struct featureSyphonOut : public microFeature {
	public:
	using microFeature::microFeature;
	ofxSyphonServer syphonOut;
	void setup() override {
//        cout << "this is syphonout setup" << endl;
//        cout << soft->name << endl;
		name = soft->name;
		syphonOut.setName(name);
	}
	void end() override {
		syphonOut.publishTexture(&soft->fboFinal->getTexture());
	}
	void send() { end(); }
	void uiEvents(ofxMicroUI::element & e) override {}
};
//featureSyphonOut senderSyphon = featureSyphonOut(&soft, "senderSyphon");



struct featureSyphonIn : public microFeature {
public:
	using microFeature::microFeature;
	
	ofxSyphonClient syphonIn;
	ofxSyphonServerDirectory syphonList;

	void setup() override {
		syphonIn.setup();
		syphonList.setup();
		ofAddListener(syphonList.events.serverAnnounced, this, &featureSyphonIn::syphonUpdated);
		ofAddListener(syphonList.events.serverRetired, this, &featureSyphonIn::syphonUpdated);
//        ofAddListener(uiSyphon->uiEvent, this, &featureSyphonIn::syphonInUIEvent);
	}
	void begin() override { }
	
	void end() override { }
    
    void draw(int x = 0, int y = 0) {
        syphonIn.draw(x, y);
    }
	
	void uiEvents(ofxMicroUI::element & e) override {
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
	
	void syphonUpdated(ofxSyphonServerDirectoryEventArgs &arg) {
		cout << "::::::::::::: syphon updated list " << endl;
		ui->clear();
		vector <string> options;
		for (auto & s : syphonList.getServerList()) {
			cout << "-------" << endl;
			cout << s.appName << endl;
			cout << s.serverName << endl;
			string nome = s.serverName + "--" + s.appName;
			options.push_back(nome);
		}	
		vector <string> lines = { "radioPipeNoLabel	syphon	" + ofJoinString(options, "|")};
		ui->createFromLines(lines,true);
	}
};

#endif


//#CAIRO_OPERATOR_CLEAR
//#CAIRO_OPERATOR_SOURCE
//    ;     #CAIRO_OPERATOR_OVER
//    ;     #CAIRO_OPERATOR_IN
//    ;     #CAIRO_OPERATOR_OUT
//    ;     #CAIRO_OPERATOR_ATOP
//    ;     #CAIRO_OPERATOR_DEST
//    ;     #CAIRO_OPERATOR_DEST_OVER
//    ;     #CAIRO_OPERATOR_DEST_IN
//    ;     #CAIRO_OPERATOR_DEST_OUT
//    ;     #CAIRO_OPERATOR_DEST_ATOP
//    ;     #CAIRO_OPERATOR_XOR
//    ;     #CAIRO_OPERATOR_ADD
//    ;     #CAIRO_OPERATOR_SATURATE
//    ;     #CAIRO_OPERATOR_MULTIPLY
//    ;     #CAIRO_OPERATOR_SCREEN
//    ;     #CAIRO_OPERATOR_OVERLAY
//    ;     #CAIRO_OPERATOR_DARKEN
//    ;     #CAIRO_OPERATOR_LIGHTEN
//    ;     #CAIRO_OPERATOR_COLOR_DODGE
//    ;     #CAIRO_OPERATOR_COLOR_BURN
//    ;     #CAIRO_OPERATOR_HARD_LIGHT
//    ;     #CAIRO_OPERATOR_SOFT_LIGHT
//    ;     #CAIRO_OPERATOR_DIFFERENCE
//    ;     #CAIRO_OPERATOR_EXCLUSION
//    ;     #CAIRO_OPERATOR_HSL_HUE
//    ;     #CAIRO_OPERATOR_HSL_SATURATION
//    ;     #CAIRO_OPERATOR_HSL_COLOR
//    ;     #CAIRO_OPERATOR_HSL_LUMINOSITY



struct featureCairo : public microFeature {
public:
	using microFeature::microFeature;
	shared_ptr<ofBaseGLRenderer> opengl;
	shared_ptr<ofCairoRenderer> cairo;
	shared_ptr<ofCairoRenderer> cairoOut;
	bool savingCairo = false;
	ofTexture render;
	string savingCairoFilename = "";
	ofRectangle rect;
	string * cairoBlend = NULL;

	map <string, cairo_line_join_t> lineJoin = {
		{ "round", CAIRO_LINE_JOIN_ROUND },
		{ "bevel", CAIRO_LINE_JOIN_BEVEL },
		{ "miter", CAIRO_LINE_JOIN_MITER },
	};

	map <string, cairo_line_cap_t> lineCap = {
		{ "round", CAIRO_LINE_CAP_ROUND },
		{ "butt", CAIRO_LINE_CAP_BUTT },
		{ "square", CAIRO_LINE_CAP_SQUARE },
	};

	map <string, _cairo_operator> cairoBlendModes = {
		{ "add", CAIRO_OPERATOR_ADD },
		{ "screen", CAIRO_OPERATOR_SCREEN },
		{ "multiply", CAIRO_OPERATOR_MULTIPLY },
		{ "darken", CAIRO_OPERATOR_DARKEN },
		{ "lighten", CAIRO_OPERATOR_LIGHTEN },
	};
	
	void setup() override {
		rect = ofRectangle(0,0,soft->fboFinal->getWidth(), soft->fboFinal->getHeight());
		opengl = ofGetGLRenderer();
		cairo = make_shared<ofCairoRenderer>();
		cairoOut = make_shared<ofCairoRenderer>();
		cairo->setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, rect);
//        bool useCairo = soft->_ui->uis["ui"].pBool["useCairo"];
	}

	void save() {
		savingCairo = true;
	}
	
	void beginSave() {
		cout << "SAVINGCAIRO!" << endl;
		savingCairoFilename = "_output/syntype_"+ofGetTimestampString()+".svg";
		cout << "SAVING " << savingCairoFilename << endl;
		cairoOut->setup(savingCairoFilename, ofCairoRenderer::SVG, false, false, rect);
		ofSetCurrentRenderer(cairoOut);
//		ofGetCurrentRenderer()->setupGraphicDefaults();		
//        ofGetCurrentRenderer()->background(ofColor(0,0));
//        ofGetCurrentRenderer()->setBackgroundColor(ofColor(0,0));

		ofStyle style = ofGetCurrentRenderer()->getStyle();
		ofGetCurrentRenderer()->setStyle(style);
		cairo_set_miter_limit(cairoOut->getCairoContext(), 2);
//		cairo_set_line_join(cairoOut->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
//		cairo_set_line_cap(cairoOut->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
	}
	
	void endSave() {
		render.loadData(cairoOut->getImageSurfacePixels());
		cairoOut->close();
		string resultado = ofSystem("open " + ofToDataPath(savingCairoFilename));
		savingCairo = false;
	}
	
	void begin() override {
		if (isOk()) {
			if (savingCairo) {
				beginSave();
			} else {
				ofSetCurrentRenderer(cairo);
				ofGetCurrentRenderer()->setupGraphicDefaults();
//				ofStyle style = ofGetCurrentRenderer()->getStyle();
//				ofGetCurrentRenderer()->setStyle(style);
//                cairo_set_source_rgba(cairo->cr,0,0,0,1);
//                cairo_paint(cairo->cr);
//                cairo->clear(0,0,0,255);
//                        ofGetCurrentRenderer()->background(ofColor(0,255));
//                        ofGetCurrentRenderer()->setBackgroundColor(ofColor(0,255));

				cairo_set_miter_limit(cairo->getCairoContext(), 2);

				if (ui != NULL && ui->pBool["cairoStroked"]) {
					static const double dashes[] = { 100.0,  /* ink */
						40.0,  /* skip */
						20.0,  /* ink */
						40.0   /* skip*/
					};
					int    ndash  = sizeof (dashes)/sizeof(dashes[0]);
					double offset = -50.0;
					cairo_set_dash(cairo->getCairoContext(), dashes, ndash, offset);
				} else {
					static const double nodash[] = { 0.0 };
					cairo_set_dash(cairo->getCairoContext(), nodash, 0, 0);
				}

//				cairo->clearAlpha();
			}
			ofPushMatrix();
		}
		startBlendingMode();
	}

	
	void end() override {
		if (isOk()) {
			ofPopMatrix();
			ofSetCurrentRenderer(opengl, true);
			
			if (savingCairo) {
				endSave();
			} else {
				render.loadData(cairo->getImageSurfacePixels());
//                render.loadData(cairo->getImageSurfacePixels(), GL_RGBA);
				ofSetColor(255);
				render.draw(0,0);
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override { 
			//            cairo_set_line_join(cairo->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
	//            cairo_set_line_cap(cairo->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
		if (e.name == "join") {
			cairo_set_line_join(cairo->getCairoContext(), lineJoin[*e.s]); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
		}
		else if (e.name == "cap") {
			cairo_set_line_cap(cairo->getCairoContext(), lineCap[*e.s]); // ROUND SQUARE
		}
	}

	
	void startBlendingMode() {
		cairoBlend = &soft->_ui->uis["colors"].pString["blend"];
		if (cairoBlend != NULL && *cairoBlend != "" && *cairoBlend != "no") {
//            cout << *cairoBlend << endl;
			cairo_set_operator(cairo->getCairoContext(), cairoBlendModes[*cairoBlend]);
		}
	}
};


#ifdef USENDI
	struct featureSendNDI : public microFeature {
		public:
		using microFeature::microFeature;
		
		ofxNDIsender ndiSender;    // NDI sender
		
		void setup() override {
			cout << "featureSendNDI Setup" << endl;
			cout << ndiSender.GetNDIversion() << " (http://ndi.tv/)" << endl;
			// Optionally set fbo readback using OpenGL pixel buffers
			ndiSender.SetReadback(); // Change to false to compare

			// Optionally set NDI asynchronous sending
			// instead of clocked at the specified frame rate (60fps default)
			ndiSender.SetAsync();
			ndiSender.CreateSender(soft->name.c_str(), soft->fbo.getWidth(), soft->fbo.getHeight());
		}
		void begin() override {
		}
		void end() override {
			checkSetup();
			ndiSender.SendImage(soft->fboFinal->getTexture());
		}
		void uiEvents(ofxMicroUI::element & e) override {}
	};
	
//	featureSendNDI senderNDI = featureSendNDI(&soft, "NDISenderTest");

#endif





struct featureColorBackground : public microFeature {
	public:
	using microFeature::microFeature;
	ofMesh fundoMesh;

	void bgMesh(ofColor & c1, ofColor & c2) {
		fundoMesh.clear();
		fundoMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		fundoMesh.addVertex(glm::vec3(0, 0, 0));
		// fundoMesh.addColor(ui->pColorEasy["bg"]);
		fundoMesh.addColor(c1);
		fundoMesh.addVertex(glm::vec3(soft->fboFinal->getWidth(), 0, 0));
		fundoMesh.addColor(c1);
		fundoMesh.addVertex(glm::vec3(0, soft->fboFinal->getHeight(), 0));
		fundoMesh.addColor(c2);
		fundoMesh.addVertex(glm::vec3(soft->fboFinal->getWidth(), soft->fboFinal->getHeight(), 0));
		fundoMesh.addColor(c2);
		fundoMesh.draw();
	}

	void setup() override {}

	void begin() override {
		
//        bool useCairo = soft->_ui->uis["ui"].pBool["useCairo"];
		bool useCairo = false;
		if (ui->pString["background"] == "solid") {
			ofClear(ui->pColorEasy["bg"]);
		}
		else if (ui->pString["background"] == "gradient") {
			if (!useCairo) {
				ofClear(0,0);
				bgMesh(ui->pColorEasy["bg"], ui->pColorEasy["bg2"]);
			} else {
				
			}
		}
		else if (ui->pString["background"] == "black") {
			ofClear(0,255);
		}
		else if (ui->pString["background"] == "no" || ui->pString["background"] == "") {
			ofClear(0,0);
		}
		else if (ui->pString["background"] == "palette") {
			ofClear(ui->pColorEasy["bgPalette"]);
		}

		else if (ui->pString["background"] == "gradpal") {
			if (!useCairo) {
				ofClear(0,0);
				bgMesh(ui->pColorEasy["bgPalette"], ui->pColorEasy["bgPalette2"]);
			} else {
				
			}
		}
	}
	void end() override { }
	void uiEvents(ofxMicroUI::element & e) override {}
};



struct featureSurface : public microFeature {
	using microFeature::microFeature;

	struct surface {
		public:
		ofFbo fbo;
		ofRectangle rect;
		int index = 0;

		surface(int w, int h) {
			rect.width = w; rect.height = h;
			fbo.allocate(w, h, GL_RGBA);
			fbo.begin(); ofClear(0,255); fbo.end();
		}

		void draw() {
			// fbo.draw(rect);
			fbo.draw(rect.x, rect.y, rect.width, rect.height);
		}
	};
	
	vector <surface> surfaces = {
		surface(720,160),
		surface(400,240),
		surface(160,480)
	};

	void setup() override {
		int offx = 0;
		int index = 0;
		for (auto & s : surfaces) {
			s.rect.x = offx;
			s.index = index;
			offx += s.rect.width + 20;
			index++;
		}
	}

	void draw() override {
		ofSetColor(255);
		ofPushMatrix();
		for (auto & s : surfaces) {
			s.draw();
		}
		ofPopMatrix();
	}
};






struct featureTex : public microFeature {
	public:
	using microFeature::microFeature;
	ofFbo fbo;
	void setup() override {
		ofDisableArbTex();
		fbo.allocate(512, 512, GL_RGBA);
		fbo.begin();
		ofClear(0,255);
		ofClear(255,0,255);
		fbo.end();
	}

	void begin() override {
		if (fbo.isAllocated() && ui->pBool["tex"]) {
		   fbo.getTexture().bind();
		} else {
		}
	}
	
	void end() override {
		if (fbo.isAllocated() && ui->pBool["tex"]) {
		   fbo.getTexture().unbind();
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		fbo.begin();
			ofClear(0,255);
			ofSetColor(255);
			ofImage * t = &ui->pImage["tex"];
			ofImage * i = &ui->pImage["image"];
		
			ofSetColor(255);
			t->draw(0,0, t->getWidth() * ui->pFloat["texScale"], t->getHeight() * ui->pFloat["texScale"]);
			ofSetColor(ui->pColor["imageColor"]);
			i->draw(ui->pFloat["imageX"], ui->pFloat["imageY"], i->getWidth() * ui->pFloat["imageScale"], i->getHeight() * ui->pFloat["imageScale"]);
		fbo.end();
		
	}
};





struct featureGlow0 : virtual public microFeature {
	using microFeature::microFeature;

	ofFbo f1;
	ofFbo f2;
	float fator = 0.1;
	float opacity = 255;
    
    ofFbo fbos[12];
	int sizes[12] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

	map <string, ofBlendMode> blendMap = {
		{ "add", OF_BLENDMODE_ADD },
		{ "alpha", OF_BLENDMODE_ALPHA },
		{ "screen", OF_BLENDMODE_SCREEN },
		{ "multiply", OF_BLENDMODE_MULTIPLY },
		{ "subtract", OF_BLENDMODE_SUBTRACT }
	};


	void setup() override {
		// cout << "GLOW SETUP" << endl;
        
		for (int a=0; a<10; a++) {
			int w = soft->fboFinal->getWidth() / sizes[a];
			int h = soft->fboFinal->getHeight() / sizes[a];
            cout << w << ":" << h << endl;
			fbos[a].allocate(w, h, GL_RGBA);
//			fbos[a].begin();
//			ofClear(0,255);
//			fbos[a].end();
		}
        
        
		f1.allocate(soft->fboFinal->getWidth(), soft->fboFinal->getHeight(), GL_RGBA);
		f1.begin();
		ofClear(0,255);
		f1.end();
		allocate();
		use = &ui->pBool["glow"];
	}

	void allocate() {
		f2.allocate(soft->fboFinal->getWidth()*fator, soft->fboFinal->getHeight()*fator, GL_RGBA);
		f2.begin();
		ofClear(0,255);
		f2.end();
	}

	void begin() override {
		if (isOk()) {
			fbos[0].begin();
			// f1.begin();
			ofClear(0,0);
		}
	}
	void end() override {
		if (isOk()) {
			fbos[0].end();
			// f1.end();
			int iteracoes = ui->pInt["iteracoes"];

			for (int a=1; a<=iteracoes; a++) {
				fbos[a].begin();
				ofClear(0,0);
				fbos[a-1].draw(0,0,fbos[a].getWidth(), fbos[a].getHeight());
				fbos[a].end();
			}
			// f2.begin();
			// ofClear(0,0);
			// f1.draw(0,0,f2.getWidth(),f2.getHeight());
			// f2.end();

			ofSetColor(255);
			ofEnableBlendMode(blendMap[ui->pString["blend"]]);
			// f1.draw(0,0);
			// ofSetColor(255, opacity);
			// f2.draw(0,0,f1.getWidth(), f1.getHeight());
			fbos[0].draw(0,0);
			ofSetColor(255, opacity);
			fbos[iteracoes].draw(0, 0, fbos[0].getWidth(), fbos[0].getHeight());
		}
	}
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "fator") {
			fator = *e.f;
			allocate();
		}

		if (e.name == "opacity") {
			opacity = *e.f;
		}
	}
};





struct featureGlow : virtual public microFeature {
	using microFeature::microFeature;
	float opacity = 255;
    
    ofFbo fbos[12];
    ofFbo fbosR[12];
	ofFbo fbosR2[12];

	map <string, ofBlendMode> blendMap = {
		{ "add", OF_BLENDMODE_ADD },
		{ "alpha", OF_BLENDMODE_ALPHA },
		{ "screen", OF_BLENDMODE_SCREEN },
		{ "multiply", OF_BLENDMODE_MULTIPLY },
		{ "subtract", OF_BLENDMODE_SUBTRACT }
	};

// 	string frag = R"V0G0N(
// //uniform float espalha;
// uniform sampler2DRect tex0;
// varying vec2 texcoord0;

// void main (void) 
// { 		
// 	vec2 xy = gl_FragCoord.xy;
// 	vec4 o = texcoord0.xyxy * vec2(-1, 1).xxyy;
// 	vec4 s = 
// 	texture2DRect(tex0, xy + o.xy) +
// 	texture2DRect(tex0, xy + o.zy) +
// 	texture2DRect(tex0, xy + o.xw) +
// 	texture2DRect(tex0, xy + o.zw);
// 	gl_FragColor = vec4(s*.25);
// } 

// )V0G0N";

// 	string vert = R"V0G0N(
// varying vec2 texcoord0;

// void main()
// {
//     gl_Position = ftransform();
//     texcoord0 = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
// }
// )V0G0N";


	string frag = R"V0G0N(
//#version 150	
#version 120	
//precision highp float;
uniform sampler2DRect tex0;
varying vec2 texcoord0;

uniform bool horizontal;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main (void) 
{ 		
	
    // vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec2 tex_offset = vec2(1.0 * 2.0, 1.0 * 2.0);

	vec2 xy = gl_FragCoord.xy;
	vec4 result = texture2DRect(tex0, xy).rgba * weight[0];
	if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture2DRect(tex0, xy + vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
            result += texture2DRect(tex0, xy - vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture2DRect(tex0, xy + vec2(0.0, tex_offset.y * i)).rgba * weight[i];
            result += texture2DRect(tex0, xy - vec2(0.0, tex_offset.y * i)).rgba * weight[i];
        }
    }
	gl_FragColor = vec4(result); //, texture2DRect(tex0, xy).a
} 

)V0G0N";

	string vert = R"V0G0N(
varying vec2 texcoord0;

void main()
{
    gl_Position = ftransform();
    texcoord0 = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
}
)V0G0N";

	ofShader shader;
	ofFbo * f = NULL;
	ofFbo * fz = NULL;

	void setup() override {
		shader.setupShaderFromSource( GL_VERTEX_SHADER, vert );
		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, frag );
		shader.bindDefaults();
		shader.linkProgram();

		int w = soft->fboFinal->getWidth();
		int h = soft->fboFinal->getHeight();
        float w2 = soft->fboFinal->getWidth();
        float h2 = soft->fboFinal->getHeight();

		for (int a=0; a<10; a++) {
			// int w = soft->fboFinal->getWidth() / sizes[a];
			// int h = soft->fboFinal->getHeight() / sizes[a];
			fbos[a].allocate(w, h, GL_RGBA);
            fbosR[a].allocate(w2, h2, GL_RGBA);
			fbosR2[a].allocate(w2, h2, GL_RGBA);
			w2 *= .5;
			h2 *= .5;
		}
		use = &ui->pBool["glow"];
	}

	void begin() override {
		if (isOk()) {
			f = ui->pBool["resize"] ? &fbosR[0] : &fbos[0];
			f->begin();
			ofClear(0,0);
		}
	}
	void end() override {
		if (isOk()) {
			f->end();
			int iteracoes = ui->pInt["iteracoes"];

			for (int a=1; a<=iteracoes; a++) {
                ofFbo * f1 = &fbos[a];
                ofFbo * f2 = &fbos[a-1];
				ofFbo * f3 = &fbosR2[a-1];
				if (ui->pBool["resize"]) {
					f1 = &fbosR[a];
					f2 = &fbosR[a-1];
				}

				f3->begin();
				ofClear(0,0);
				if (ui->pBool["shader"]) {
					shader.begin();
					shader.setUniform1i("horizontal", a%2);
				}
				f2->draw(0,0);
				if (ui->pBool["shader"]) {
					shader.end();
				}
				f3->end();



				f1->begin();
				ofClear(0,0);
				// if (ui->pBool["shader"]) {
				// 	shader.begin();
				// 	shader.setUniform1i("horizontal", a%2);
				// }
				// f2->draw(0,0,f1->getWidth(), f1->getHeight());
				// if (ui->pBool["shader"]) {
				// 	shader.end();
				// }
				f3->draw(0,0,f1->getWidth(),f1->getHeight());
				f1->end();
			}
			ofSetColor(255);
			ofEnableBlendMode(blendMap[ui->pString["blend"]]);
			f->draw(0,0);
			ofSetColor(255, opacity);
			fz = ui->pBool["resize"] ? &fbosR[iteracoes] : &fbos[iteracoes];
			fz->draw(0, 0, f->getWidth(), f->getHeight());
		}
	}
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "opacity") {
			opacity = *e.f;
		}
	}
};

#ifdef USEPICKERS
struct featurePickersMidi : public microFeature {
    public:
    using microFeature::microFeature;
    
    vector <picker> pickers;

    void setup() override {
        setupPickers();
    }

    void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "numero" || e.name == "cols" || e.name == "rows") {
            setupPickers();
        }
    }
    
    void setupPickers() {
        pickers.clear();
        for (int a=0; a<ui->pInt["numero"]; a++) {
            int x = a % ui->pInt["cols"];
            int y = floor(a / ui->pInt["cols"]);
            float xx = ofMap(x, -.5, ui->pInt["cols"] - .5, 0, soft->fboFinal->getWidth());
            float yy = ofMap(y, -.5, ui->pInt["rows"] - .5, 0, soft->fboFinal->getHeight());
            glm::vec2 pos = glm::vec2(xx,yy);
            pickers.emplace_back(picker(pos, a, soft->fboFinal, ui));
            pickers.back()._fbo = soft->fboFinal;
            pickers.back().name = "picker_" + ofToString(a);
        }
    }
};
#endif

struct featureTest : public microFeature {
    public:
    using microFeature::microFeature;
    void setup() override {}
    void begin() override {}
    void end() override { }
    void uiEvents(ofxMicroUI::element & e) override {}
};


