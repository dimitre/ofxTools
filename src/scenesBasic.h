
struct sceneLines : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();
		for (int a=0; a<100; a++) {
			float x = ofRandom(0, fbo->getWidth());
			float y = ofRandom(0, fbo->getHeight());
			ofDrawLine(0,0,x,y);
		}
	}
};

struct sceneVaretas : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();
		ofSetLineWidth(4);
        for (int a=0; a<60; a++) {
            ofSetColor(ofColor::fromHsb(ofRandom(0,255), 255, 255));
            float x1 = ofRandom(0,fbo->getWidth());
            float x2 = ofRandom(0,fbo->getWidth());
            float y1 = ofRandom(0,fbo->getHeight());
            float y2 = ofRandom(0,fbo->getHeight());
            ofDrawLine(x1, y1, x2, y2);
        }	
    }
};

struct sceneCircles : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override
	{
		ofSetCircleResolution(64);
	}
	
	void draw() override {
		checkSetup();
		float tempo = ofGetElapsedTimef();
		for (int a=0; a<10; a++) {
			ofSetColor(ofColor::fromHsb(a*8, 255, 255));
			float raio = ofNoise(tempo * .3, a * .3) * 200;
			float x = ofNoise(tempo * .2, a * .2) * fbo->getWidth();
			float y = ofNoise(tempo * .4, a * .3) * fbo->getHeight();
			ofDrawCircle(x, y, raio, raio);
		}	
    }
};


struct sceneCam : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();
		ofSetColor(255);
		uiC->pCam["cam"].update();
		uiC->pCam["cam"].draw(0,0);
    }
};



struct sceneMilimetrado : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	struct sl {
		public:
		float space = 100;
		float line = 3;
		sl(float s, float l) : space(s), line(l) {}
	};

	vector <sl> sls = { 
		sl(50, 2), 
		// sl(50, 2), 
		sl(10,1) 
	};
	
	void setup() override {
	}
	void draw() override {
		checkSetup();
		ofSetColor(getCor(0));

		for (auto s : sls) {
			ofSetLineWidth(s.line);
			for (int x=0; x<fbo->getWidth(); x+=s.space) {
				ofDrawLine(x, 0, x, fbo->getHeight());
			}
			for (int y=0; y<fbo->getWidth(); y+=s.space) {
				ofDrawLine(0, y, fbo->getWidth(), y);
			}
		}
    }
};







struct sceneRelogio : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
    using sceneDmtr::sceneDmtr;
    
    struct relogio {
    public:
        float raio;
		float raio2;
        glm::vec2 pos;
        glm::vec2 vel;
		float angulo = 0;
		float velAngulo;
		int numeros[6] = { 12, 20, 30, 40, 60, 120 };
		int numero;
		float graus;
		float qual;
		float lineWidth;

        relogio() {
            raio = ofRandom(100,300);
            pos = glm::vec2(ofRandom(0,1000), ofRandom(0,1000));
			vel = glm::vec2(ofRandom(-1, 1), ofRandom(-1, 1));
			velAngulo = ofRandom(-.5,.5);
			numero = numeros[int(ofRandom(0,5))];
			graus = 360.0 / (float) numero;
			raio2 = raio*1.2;
			qual = ofRandom(0,1);
			lineWidth = ofRandom(0,5);
        }

        void draw() {
			angulo += velAngulo;
			pos += vel;
            // ofDrawEllipse(pos.x, pos.y, raio, raio);
			ofPushMatrix();
			ofTranslate(pos.x, pos.y);

            for (int n=0; n<numero; n++) {
				float ang = n*graus + angulo;
				float x = r2x(ang, raio);
				float y = r2y(ang, raio);
				float x2 = r2x(ang, raio2);
				float y2 = r2y(ang, raio2);
				ofDrawLine(x, y, x2, y2);
			}
			ofPopMatrix();
        }
    };
    
    vector <relogio> relogios;
    
    void setup() override {
        for (int a=0; a<16; a++) {
            relogios.emplace_back(relogio());
        }
    }
    
    void draw() override {
		ofNoFill();
        checkSetup();
        ofSetColor(getCor(0));

        for (auto & r : relogios) {

			ofSetColor(getCor(r.qual));
			if (uiC->pBool["lineWidth"]) {
				ofSetLineWidth(r.lineWidth * uiC->pEasy["lineFactor"]);
			}
            r.draw();
        }
    }

	void uiEvents(ofxMicroUI::element & e) override {
		cout << e.name << endl;
		if (e.name == "numero") {
			relogios.clear();
			for (int a=0; a<*e.i; a++) {

            	relogios.emplace_back(relogio());
        	}
		}
	}
};
