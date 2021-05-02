struct sceneDmtrBasic {
public:
	sceneConfig * config = NULL;
	string name = "";
	//feito apenas pra cena stocks do muti pra carregar texto no sceneStocks
	string text = "";

	bool isSetup = false;
	ofxMicroUI * u = NULL;
	ofxMicroUI * ui = NULL;
	ofxMicroUI * uiC = NULL;
	//	ofxMicroUI * uiColors = NULL;
	ofFbo * fbo = NULL;
	glm::vec2 middle;
	ofRectangle rectBounds;
    
//	sceneDmtrBasic() { }

	glm::vec2 randomPos() {
		return glm::vec2(ofRandom(0,fbo->getWidth()), ofRandom(0,fbo->getHeight()));
	}
    
    sceneDmtrBasic(string n = "") : name(n) {
    }

	sceneDmtrBasic(sceneConfig * _c, string n = "") : config(_c), name(n) {
        addConfig(_c);
	}
    
    void addConfig(sceneConfig * _c) {
//        cout << "addConfig " << name << endl;
        config = _c;
        u = config->u;
        ui = config->ui;
        uiC = config->uiC;
        fbo = config->fbo;
        middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		// todo : depois adicionar software aqui e pegar um rect ja existente la.
		rectBounds = ofRectangle(0,0,fbo->getWidth(), fbo->getHeight());
    }

	bool hasListener = false;

	void select() {
		// cout << "Scene selected, name = " << name << endl;
        if (config != NULL) {
            ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
            hasListener = true;
        }
	}

	void unselect() {
		if (hasListener) {
			ofRemoveListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
            hasListener = false;
		}
	}

	virtual ofColor getCor(float n) {
        if (config != NULL) {
            return getColor(n, config->uiColors);
        } else {
            cout << "getCor config is NULL" << endl;
            return ofColor(255, 0, 70);
        }
	}

	virtual void checkSetup() {
//		if (!isSetup) {
//			setup();
//			isSetup = true;
//		}
	}

	virtual void uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvent primitive " << endl;
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



struct sceneDmtr : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;
};



struct polar : public virtual ofxScenes::sceneDmtrBasic {
    public:
    using sceneDmtrBasic::sceneDmtrBasic;
    
    static float r2x (float a, float m) { return m * cos(ofDegToRad(a)); }
    static float r2y (float a, float m) { return m * sin(ofDegToRad(a)); }
    static float c2a (float x, float y) { return ofRadToDeg(atan2(y,x)); }
	// static float c2a (float x, float y) { return atan2(y,x); }
	// static float c2a (float x, float y) { return glm::atan2(y,x); }
    static float c2m (float x, float y) { return sqrt(pow(x,2)+pow(y,2)); }
};



struct polarVec2 : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;

	static glm::vec2 c2p (glm::vec2 pos) {
		return glm::vec2(
			ofRadToDeg(atan2(pos.y, pos.x)),
			sqrt(pow(pos.x,2)+pow(pos.y,2))
		);
	}

	static glm::vec2 p2c (glm::vec2 pos) {
		return glm::vec2(pos.y * cos(ofDegToRad(pos.x)), pos.y * sin(ofDegToRad(pos.x)));
	}
};




struct sceneAudio : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;
    
	float updown = 0.5;
    float beat = 0;
    float audio = 0;

	float getFreq(int index) {
		return 0;
	}
};


struct sceneIncrementa : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;
	map <string, float> incrementadorTemporal;

	float incrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] += uiC->pEasy[qual];
		return incrementadorTemporal[uniqueId];
	}

	void resetIncrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] = 0;
	}
};


struct sceneType : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;
    
    ofTrueTypeFont * type = NULL;
    ofxMicroUI::fontList* fontElement = NULL;
    int lastSize = 0;

    void setup() override {
        type = &uiC->pFont["type"];
    }
    
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "fontSize") {
			if (*e.i != lastSize) {
				ofxMicroUI::element* el = uiC->getElement("type");
				if (el != NULL) {
					ofxMicroUI::fontList* f = (ofxMicroUI::fontList*)el;
                    f->_size = &uiC->pInt["fontSize"];
					f->size = *e.i;
					f->reload();
				}
				lastSize = *e.i;
			}
		}
	}
};
