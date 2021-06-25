struct settings {
public:
	float raio;
	float raioRand;
	float velFactor = 1.0;
	float velRotFactor = 1.0;
	float lineFactor = 1.0;
	ofRectangle rectBounds;
};

struct objeto {
public:
	glm::vec2 pos;
	settings * _set = NULL;
	float qual;

	glm::vec2 vel;
	float angulo = 0;
	float velAngle;
	float raio = 100;

	vector <ofPolyline> polys;

	objeto() {}

	objeto(glm::vec2 p) : pos(p) {
		setup();
	}
	
	objeto(glm::vec2 p, settings * s) : pos(p), _set(s) {
		setup();
	}
	
	objeto(glm::vec2 p, settings * s, float q) : pos(p), _set(s), qual(q) {
		setup();
	}
	
	objeto(settings * s) : _set(s) {
		setup();
	}

	static float r2x (float a, float m) { return m * cos(ofDegToRad(a)); }
	static float r2y (float a, float m) { return m * sin(ofDegToRad(a)); }

	virtual void setup() {
		if (_set != NULL) {
			raio = _set->raio + qual * _set->raioRand;
		}
		vel = glm::vec2(ofRandom(-1,1), ofRandom(-1,1));
		velAngle = ofRandom(-1, 1);

		ofPolyline p;
		for (int a=0; a<3; a++) {
			float angulo = a * 120 + ofRandom(-40,40);
			float mag = ofRandom(_set->raio,_set->raioRand);
			p.addVertex(r2x(angulo,mag), r2y(angulo,mag));
		}
		p.close();
		p.setClosed(true);
		polys.emplace_back(p);
	}

	virtual void update() {
		angulo += velAngle * _set->velRotFactor;
		pos += vel * _set->velFactor;
	}

	virtual void draw() {
		ofPushMatrix();
		ofTranslate(pos);
		ofRotateDeg(angulo);
		// ofDrawRectangle(-10,-10,20,20);
		for (auto & p : polys) {
			p.draw();
		}
		ofPopMatrix();
	}
};


struct sceneObjetos : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	vector <objeto*> objetos;
	settings set;
	
	void setupRectBounds(float margem) override {
		rectBounds = ofRectangle(-margem,-margem,fbo->getWidth()+margem*2, fbo->getHeight()+margem*2);
		set.rectBounds = rectBounds;
	}

	glm::vec2 rectificate(glm::vec2 pos) {
		if (pos.x < rectBounds.x) {
			pos.x += rectBounds.width;
		}
		else if (pos.x > (rectBounds.x + rectBounds.width)) {
			pos.x -= rectBounds.width;
		}

		if (pos.y < rectBounds.y) {
			pos.y += rectBounds.height;
		}
		else if (pos.y > (rectBounds.y + rectBounds.height)) {
			pos.y -= rectBounds.height;
		}
		return pos;
	}
	
	virtual void build() {
		
	}

	void update() override {
		for (auto & o : objetos) {
			o->update();
			o->pos = rectificate(o->pos);
		}
	}

	void draw() override {
		// cout << objetos.size() << endl;
		if (uiC->pBool["drawRectBounds"]) {
			ofDrawRectangle(rectBounds);
		}		
		for (auto & o : objetos) {
			ofSetColor(getCor(o->qual * uiC->pFloat["colorRange"]));
			o->draw();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		set.raio = uiC->pFloat["raio"];
		set.raioRand = uiC->pFloat["raioRand"];

		if (e.name == "numero" || e.name == "seed") {
			build();
		}
		
		if (e.name == "vel") {
			set.velFactor = *e.f;
		}
		else if (e.name == "velRot") {
			set.velRotFactor = *e.f;
		}
	}

};

// #include "scenesIAVObjeto.h"

struct sceneTri : public sceneObjetos {
public:
	using sceneObjetos::sceneObjetos;
	struct tri : public objeto {
		using objeto::objeto;

		// void setup() override {

		// }

		// void draw() override {
		// 	ofPushMatrix();
		// 	ofTranslate(pos);
		// 	ofRotateDeg(angulo);
		// 	for (auto & p : polys) {
		// 		p.draw();
		// 	}
		// 	ofPopMatrix();
		// }
	};

	vector <glm::vec3> points;

	void setup() override {
		int margem = 200;
		rectBounds = ofRectangle(-margem,fbo->getHeight()*.6,fbo->getWidth()+margem*2, fbo->getHeight()*.25);
		set.rectBounds = rectBounds;
		build();

		ofPolyline p = ofPolyline::fromRectangle(rectBounds).getResampledBySpacing(100);
		for (auto & v : p.getVertices()) {
			points.push_back(v);
		}
	}

	void draw() override {
		int i =0;
		vector <ofPoint> verts;
		for (auto & p : points) { 
			float angulo = ofNoise(i, ofGetElapsedTimef()*.1) * 360;
			float mag = ofNoise(i, ofGetElapsedTimef()*.2) * 100;
			glm::vec2 point = p + glm::vec2(r2x(angulo, mag), r2y(angulo, mag));
			verts.emplace_back(point);
			i++;
		}

		ofPolyline p;
//		p.addVertices(verts);
	}

	void build() override {
		cout << "GEAR BUILD" << endl;
		if (uiC->pBool["seed"]) {
			ofSeedRandom(uiC->pInt["seed"]);
		}
		objetos.clear();
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			float qual = a/(float)uiC->pInt["numero"];
			objetos.emplace_back(new tri(randomPos(), &set, qual));
			objetos.back()->vel.y = 0;
		}
	}
};





struct sceneTri0 : public sceneObjetos {
public:
	using sceneObjetos::sceneObjetos;

	void setup() override {
		int margem = 200;
		rectBounds = ofRectangle(-margem,fbo->getHeight()*.6,fbo->getWidth()+margem*2, fbo->getHeight()*.25);
		set.rectBounds = rectBounds;
		build();
	}

	void build() override {
		cout << "GEAR BUILD" << endl;
		if (uiC->pBool["seed"]) {
			ofSeedRandom(uiC->pInt["seed"]);
		}
		objetos.clear();
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			float qual = a/(float)uiC->pInt["numero"];
			objetos.emplace_back(new objeto(randomPos(), &set, qual));
			objetos.back()->vel.y = 0;
		}
	}
};
