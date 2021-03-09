struct featureLight : public microFeature {
	public:
	using microFeature::microFeature;

	ofLight luz;
	glm::vec3 pos;
	// float pointsPerMeter = 100;
	float pointsPerMeter = 1;

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
		cam.setNearClip(0.01 * pointsPerMeter);
		cam.setFarClip(160 * pointsPerMeter);
	}
	
	void begin() override {
		// if (_ui->pBool["disableDepthTest"]) {
		// 	ofDisableDepthTest();
		// } else {
		// 	ofEnableDepthTest();
		// }
		ofEnableDepthTest();

		float cameraX = ui->pEasy["cameraX"] * pointsPerMeter;
		float cameraZ = ui->pEasy["cameraZ"] * pointsPerMeter;

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

	void end() override {
		ofPopMatrix();
		cam.end();
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
    
    bool isOk() {
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
                        cout << "SHADERS LOAD " << e.name << " :: " << *e.s << endl;
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
//        cout << "::::::::::::: syphon updated list " << endl;
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
//        ui->createFromLines(lines);
        ui->createFromLines(lines,true);
//        ui->redraw();
    }
};

#endif




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
    
    map <string, _cairo_operator> cairoBlendModes = {
        { "add", CAIRO_OPERATOR_ADD },
        { "screen", CAIRO_OPERATOR_SCREEN },
        { "multiply", CAIRO_OPERATOR_MULTIPLY },
        { "darken", CAIRO_OPERATOR_DARKEN },
        { "lighten", CAIRO_OPERATOR_LIGHTEN },
    };
    
    void setup() override {
        rect = ofRectangle(0,0,soft->fboFinal->getWidth(), soft->fboFinal->getHeight());
        cout << "setupCairo :: " << rect << endl;
        opengl = ofGetGLRenderer();
        cairo = make_shared<ofCairoRenderer>();
        cairoOut = make_shared<ofCairoRenderer>();
        cairo->setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, rect);
//        bool useCairo = soft->_ui->uis["ui"].pBool["useCairo"];

    }
    
    void beginSave() {
        cout << "SAVINGCAIRO!" << endl;
        savingCairoFilename = "_output/syntype_"+ofGetTimestampString()+".svg";
        cout << "SAVING " << savingCairoFilename << endl;
        cairoOut->setup(savingCairoFilename, ofCairoRenderer::SVG, false, false, rect);
        ofSetCurrentRenderer(cairoOut);
        ofGetCurrentRenderer()->setupGraphicDefaults();
        ofStyle style = ofGetCurrentRenderer()->getStyle();
        ofGetCurrentRenderer()->setStyle(style);
        cairo_set_miter_limit(cairoOut->getCairoContext(), 2);
        cairo_set_line_join(cairoOut->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
        cairo_set_line_cap(cairoOut->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
    }
    
    void endSave() {
        render.loadData(cairoOut->getImageSurfacePixels());
        cairoOut->close();
        savingCairo = false;
        string resultado = ofSystem("open " + ofToDataPath(savingCairoFilename));
    }
    
    void begin() override {
        if (isOk()) {
            if (savingCairo) {
                beginSave();
            } else {
                
                ofSetCurrentRenderer(cairo);
                ofGetCurrentRenderer()->setupGraphicDefaults();
                ofStyle style = ofGetCurrentRenderer()->getStyle();
                ofGetCurrentRenderer()->setStyle(style);
                cairo_set_miter_limit(cairo->getCairoContext(), 2);
    //            cairo_set_line_join(cairo->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
    //            cairo_set_line_cap(cairo->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
                cairo_set_line_join(cairo->getCairoContext(), CAIRO_LINE_JOIN_BEVEL); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
                cairo_set_line_cap(cairo->getCairoContext(), CAIRO_LINE_CAP_BUTT); // ROUND SQUARE
                

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

                cairo->clearAlpha();
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
    
    void uiEvents(ofxMicroUI::element & e) override { }
    
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
			ndiSender.CreateSender(name.c_str(), soft->fbo.getWidth(), soft->fbo.getHeight());
		}
		void begin() override {
		}
		void end() override {
			checkSetup();
			ndiSender.SendImage(soft->fboFinal->getTexture());
		}
		void uiEvents(ofxMicroUI::element & e) override {}
	};
	
	featureSendNDI senderNDI = featureSendNDI(&soft, "NDISenderTest");

#endif

	
#ifdef USESYPHON
	struct featureSyphonOut : public microFeature {
		public:
		using microFeature::microFeature;
        ofxSyphonServer syphonOut;

		void setup() override {
			syphonOut.setName(name);
		}
		void begin() override {}
		void end() override {
			syphonOut.publishTexture(&soft->fboFinal->getTexture());
		}
		void uiEvents(ofxMicroUI::element & e) override {}
	};
	featureSyphonOut senderSyphon = featureSyphonOut(&soft, "senderSyphon");

#endif



struct featureTest : public microFeature {
    public:
    using microFeature::microFeature;
    void setup() override {}
    void begin() override {}
    void end() override { }
    void uiEvents(ofxMicroUI::element & e) override {}
};



struct featureMiawBg : public microFeature {
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

