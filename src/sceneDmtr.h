struct sceneDmtr {
public:
	sceneConfig * config = NULL;
	string name = "";
	
	bool isSetup = false;
	ofxMicroUI * u = NULL;
	ofxMicroUI * ui = NULL;
    ofxMicroUI * uiC = NULL;
//	ofxMicroUI * uiColors = NULL;
	ofFbo * fbo = NULL;
	glm::vec2 middle;
	map <string, float> incrementadorTemporal;
	
	// apenas pra nao causar furor
	float updown = 0.5;

	sceneDmtr() { }

	sceneDmtr(sceneConfig * _c, string n) : config(_c), name(n) {
		u = config->u;
		ui = config->ui;
		uiC = config->uiC;
		fbo = config->fbo;
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}

	sceneDmtr(sceneConfig * _c) : config(_c) {
		u = config->u;
		ui = config->ui;
		uiC = config->uiC;
		fbo = config->fbo;
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}

	// TODO
	float getFreq(int index) {
		return 0;
	}
	
	virtual ofColor getCor(float n) {
		return getColor(n, config->uiColors);
	}

	float incrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
//		incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
		// 07/09/2020
		incrementadorTemporal[uniqueId] += uiC->pEasy[qual];
		return incrementadorTemporal[uniqueId];
	}

	void resetIncrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] = 0;
	}

	virtual void checkSetup() {
		if (!isSetup) {
			setup();
			isSetup = true;
		}
	}

	virtual void uiEvents(ofxMicroUI::element & e) {
		// cout << "uiEvent primitive " << endl;
	}
	
	virtual void setup() {
		isSetup = true;
	}

	virtual void update() {		
	}
	
	virtual void draw() {
		checkSetup();
	}
};


struct sceneBaseType : public virtual ofxScenes::sceneDmtr {
	public:
	using sceneDmtr::sceneDmtr;
	ofTrueTypeFont * type = &uiC->pFont["type"];

	void uiEvents(ofxMicroUI::element & e) override {
		typeUIEvent(e);
	}

	void typeUIEvent(ofxMicroUI::element & e) {
		if (e.name == "fontSize") {
			ofxMicroUI::element* el = uiC->getElement("type");
			if (el != NULL) {
				ofxMicroUI::fontList* f = (ofxMicroUI::fontList*)el;
				f->size = *e.i;
				f->reload();
			}
		}
	}
};
