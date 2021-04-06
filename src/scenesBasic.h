
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


struct sceneVideoSimple : public ofxScenes::sceneDmtr { //, ofxScenes::sceneIncrementa
public:
    using sceneDmtr::sceneDmtr;

    ofVideoPlayer * video = NULL;
    
    void setup() override {
        video = &uiC->pVideo["video"];
    }
    
    void draw() override {
        video = &uiC->pVideo["video"];
        if (video->isLoaded()) {
            video->update();
            float x = uiC->pFloat["offX"];
            float y = uiC->pFloat["offY"];
            float w = uiC->pFloat["scale"] * video->getWidth();
            float h = uiC->pFloat["scale"] * video->getHeight();
            video->draw(x, y, w, h);
        }
        ((ofxMicroUI::bar*)uiC->getElement("bar"))->set(video->getPosition());
    }
    void uiEvents(ofxMicroUI::element & e) override {
        if (video->isLoaded()) {
            if (e.name == "speed") {
                video->setSpeed(*e.f);
            }
            else if (e.name == "pos") {
                video->setPosition(*e.f);
            }
            else if (e.name == "vol") {
                video->setVolume(*e.f);
            }
		}
	}
    
};

