
struct sceneLines : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "lines";
	
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
	// name = "lines";
	
	void draw() override {
		checkSetup();
        for (int a=0; a<100; a++) {
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
	// name = "lines";
	
	void draw() override {
		checkSetup();
        float tempo = ofGetElapsedTimef();
        for (int a=0; a<10; a++) {
            float raio = ofNoise(tempo, a) * 200;
            float x = ofNoise(tempo * .5, a * .5) * fbo->getWidth();
            float y = ofNoise(tempo * .4, a * .3) * fbo->getHeight();
            ofDrawCircle(x, y, raio, raio);
        }	
    }
};
