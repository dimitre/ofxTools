struct sceneRadius : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	struct ring {
		public:
		vector <ofPolyline> lines;
		float radius;
		double angulo = 0;
		double v;
		float sizes[10] = { .5, .5, .5, 1, 2, 4, 8, 16, 32, 64 };
		float degs = 8.0;
		float qual = 0.0;
		float fatorVel = 1.0;
		
		~ring() {
			lines.clear();
		}
		ring(float r, float d = 8) : radius(r), degs(d) {
			qual = ofRandom(0,1);
			float velPixel = ofRandom(-0.03, 0.03);
			v = c2a(r, velPixel);
			v = c2a(r, 1) * ofRandom(-0.3, 0.3);
			// cout << "radius = " << r << " : v =" << v << endl;
			float a = 0;
			while (a < 360) {
				float ang = sizes[int(ofRandom(0,8))] * degs;

				ang = c2a(r, ang);
				ofPolyline line;

				line.arc(ofPoint(0,0), radius, radius, a, a+ang, 360/3);
				lines.emplace_back(line);
				a += ang + c2a(r, degs);
			}
		}

		void draw() {
			angulo+=v*fatorVel;
			ofPushMatrix();
			ofRotateDeg(angulo);
			// ofSetColor(cor);
			for (auto & p : lines) {
				p.draw();
			}
			ofPopMatrix();
		}
	};

	vector <ring> rings;

	bool haveToCreate = true;
	void setup() override {
	
	}

	void draw() override {
		if (haveToCreate) {
			haveToCreate = false;
			create();
		}
		ofTranslate(middle + glm::vec2(uiC->pFloat["offX"], uiC->pFloat["offY"]));
		ofSetLineWidth(uiC->pFloat["lineWidth"]);
		for (auto & r : rings) {
			ofSetColor(getCor(r.qual));
			r.fatorVel = uiC->pFloat["vel"];
			r.draw();
		}
	}

	void create() {
		rings.clear();
		for (int a=10; a<uiC->pInt["numero"]; a+=uiC->pInt["jump"]) {
			rings.emplace_back(ring(a, uiC->pFloat["degs"]));
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		haveToCreate = true;
		// if (e.name == "numero" || e.name == "jump") {
			// create();
		// }
	}
};




// future user interface
struct scenePirose : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "pirose";
	
	void setup() override {
		
	}
	
	void draw() override {
//        cout << "draw pirose" << endl;
//        checkSetup();
		// FUTURE USER INTERFACE #1
		ofTranslate(40,40);
		
		ofSetLineWidth(1);
		
		float modulo = 20.0;
		ofNoFill();
//        ofFill();
		ofSetColor(getCor(0));
//        ofSetColor(255);
		
		for (int x=0; x<=10; x++) {
			
			{
				ofDrawLine(x*modulo, -modulo*.5, x*modulo, 10.5*modulo);
				ofDrawLine(-modulo*.5, x*modulo, 10.5*modulo, x*modulo);
				for (int y=0; y<=10; y++) {
					ofDrawCircle(x*modulo, y*modulo, 2);
				}
			}
		}
		
		{
			vector <ofVec3f> pontos;
			for (int b=0; b<5; b++)
			for (int a=0; a<10; a++) {
				float ang = a*36.0 + ofNoise(a*1.3, b*1, ofGetElapsedTimef()*0.6) * 36.0;
				float mag = 40.0 + ofNoise(a*1.5, b*1.2, ofGetElapsedTimef()*0.4) * 40.0;
				float x = 100.0 + r2x(ang, mag);
				float y = 100.0 + r2y(ang, mag);
				pontos.push_back(ofVec3f(x,y));
			}
			ofPolyline poly;
			for (auto & p : pontos) {
				poly.addVertex(p);
				ofDrawCircle(p, 3.0);
			}
			poly.close();
			poly.draw();
		}

		
		ofTranslate(240,0);
		ofDrawRectangle(0,0,300,75);
		vector <ofVec3f> pontos;
		for (int a=0; a<300; a+=3) {
			pontos.push_back(ofVec3f(a, ofNoise(a*.04, ofGetElapsedTimef() *.5)*75));
		}
		
		ofPolyline poly;
		for (auto & p : pontos) {
			poly.addVertex(p);
			ofDrawCircle(p, 3.0);
		}
		poly.draw();
		
		ofTranslate(0,240);
		
		ofDrawBitmapString(ofToString(ofRandom(0,999)), 0, 0);
		
		ofTranslate(40,40);
	}
};



//ofxScenes::sceneDmtr, 
struct sceneRelogio : public ofxScenes::sceneDmtr, ofxScenes::sceneType, ofxScenes::polar {
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
//        int numeros[6] = { 12, 20, 30, 40, 60, 120 };
		int numeros[6] = { 30, 60, 30, 40, 60, 120 };
		
		int numero;
		float graus;
		float qual;
		float lineWidth;
		ofRectangle rect;
		relogio(ofRectangle r) : rect(r) {
			raio = ofRandom(60,300);
			pos = glm::vec2(ofRandom(rect.x,rect.x+rect.width), ofRandom(rect.y, rect.y+rect.height));
			vel = glm::vec2(ofRandom(-1, 1), ofRandom(-1, 1));
			vel *= .1;
			velAngulo = ofRandom(-.5,.5);
			numero = numeros[int(ofRandom(0,5))];
			graus = 360.0 / (float) numero;
			raio2 = raio + ofRandom(10,100);
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
	
	ofRectangle rect;
	void setup() override {
		rect.width = fbo->getWidth();
		rect.height = fbo->getHeight();
		float margem = 400;
		rect.x -= margem;
		rect.y -= margem;
		rect.width += margem * 2;
		rect.height += margem * 2;

		build();
		sceneType::setup();
		nextJump = ofGetElapsedTimef() + 1;
	}
	
	int contagem = 0;
	int lastSecond;
	float nextJump;
    int totalSeconds = 10;
	
	void draw() override {
        if (uiC->pBool["auto"]) {
            if (ofGetElapsedTimef() > nextJump) {
                contagem -=1;
                if (contagem == -1) {
                    contagem = totalSeconds;
                }
                build();
                nextJump = ofGetElapsedTimef() + 1;
            }
        }
		
		ofNoFill();
		checkSetup();
		ofSetColor(getCor(0));
//        cout << relogios.size() << endl;

        if (uiC->pBool["neue"]) {
            float raio = uiC->pFloat["raio"];
            ofPoint meio = ofPoint(middle.x, middle.y);

            ofSetLineWidth(uiC->pFloat["stroke1"]);
            ofSetColor(getCor(0));

            ofDrawCircle(meio.x, meio.y,raio);

            ofSetLineWidth(uiC->pFloat["stroke2"]);
            ofPolyline arc;
            ofSetColor(getCor(0.7));
//            ofSetColor(255,0,0);
//            void ofPolyline_::arc(const T &center, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution=20)
            float angulo = ofMap(contagem, 0, totalSeconds, 0, 360);
            if (angulo > 0) {
                arc.arc(meio, raio, raio, -90, -90 + angulo, 100);
                arc.draw();
            }
            
        } else {
            for (auto & r : relogios) {
                ofSetColor(getCor(r.qual));
                if (uiC->pBool["lineWidth"]) {
                    ofSetLineWidth(r.lineWidth * uiC->pEasy["lineFactor"]);
                }
                r.draw();
            }
        }
		
        if (type->isLoaded()) {
            ofSetColor(getCor(0));
            ofFill();
            ofTranslate(middle);
            string t = uiC->pString["texto"];
            t = ofToString(contagem);
            ofRectangle r = type->getStringBoundingBox(t, 0,0);
            type->drawStringAsShapes(t, uiC->pInt["fontX"] - r.width *.5, uiC->pInt["fontY"] + r.height *.5);
//            ofSetColor(0,0,255);
//            ofNoFill();
//            ofTranslate(- r.width *.5, r.height*.5);
//            ofDrawRectangle(r);
        }
	}

	void uiEvents(ofxMicroUI::element & e) override {
		sceneType::uiEvents(e);

        if (e.name == "minuto") {
            totalSeconds = uiC->pBool["minuto"] ? 60 : 10;
            contagem = 1;
        }

//        cout << e.name << endl;
		if (e.name == "numero") {
			build();
		}
	}
	
	void build() {
		relogios.clear();
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			relogios.emplace_back(relogio(rect));
		}
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
            float space = s.space * uiC->pFloat["space"];
            for (float x=0; x<fbo->getWidth(); x+=space) {
				ofDrawLine(x, 0, x, fbo->getHeight());
			}
			for (float y=0; y<fbo->getWidth(); y+=space) {
				ofDrawLine(0, y, fbo->getWidth(), y);
			}
		}
	}
};



struct sceneModulo : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	struct modulo {
		public:
		vector <ofRectangle> rects;
		vector <ofPolyline> lines;
		float qual;
        ofRectangle rect = ofRectangle(0,0,100,100);

		modulo(glm::vec2 pos, float m ) {
            rect = ofRectangle(0,0,m,m);
			qual = ofRandom(0,1);
			rect.x = pos.x;
			rect.y = pos.y;
			float raio = 6;
			rects = { 
				ofRectangle(rect.width * .25 - raio * .5, rect.height * .25 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .75 - raio * .5, rect.height * .25 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .75 - raio * .5, rect.height * .75 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .25 - raio * .5, rect.height * .75 - raio * .5, raio, raio), 
			};
			for (int y =0; y<rect.height; y+=10) {
				{
					ofPolyline line;
					line.addVertex(glm::vec3(0,y,0));
					line.addVertex(glm::vec3(rect.width,y,0));
					lines.emplace_back(line);
				}
				{
					ofPolyline line;
					line.addVertex(glm::vec3(y,0,0));
					line.addVertex(glm::vec3(y,rect.width,0));
					lines.emplace_back(line);
				}
			}
		}

		void draw() {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			ofSetLineWidth(3);
			ofDrawLine(-5, 0, 5, 0);
			ofDrawLine(0, -5, 0, 5);
			ofSetLineWidth(1);
			ofNoFill();
			for (auto & r : rects) {
				ofDrawRectangle(r);
			}
			for (auto & l : lines) {
				l.draw();
			}
			ofPopMatrix();
		}
        

	};

	vector <modulo> modulos;
    
    void build() {
        cout << "build modulos" << endl;
        modulos.clear();
        int x = 0, y = 0;
        float size = uiC->pFloat["size"];
        size = MAX(20, size);
//        cout << size << endl;
        while (x < fbo->getWidth()) {
//            cout << x << endl;
            y = 0;
            while (y < fbo->getHeight()) {
                modulos.emplace_back(glm::vec2(x,y), size);
//                y += modulos.back().rect.width;
                y += size;
            }
//            x += modulos.back().rect.height;
            x += size;
        }
    }
    
	void setup() override {
        build();
	}
    
	void draw() override {
		for (auto & m : modulos) {
			ofColor cor = getCor(m.qual * uiC->pEasy["colorRange"]);
			cor.a = ofNoise(m.qual * uiC->pEasy["colorAlphaQual"], ofGetElapsedTimef() * uiC->pEasy["colorAlphaTempo"]) * 255.0;
			ofSetColor(cor);
			m.draw();
		}
	}
    
    void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "size") {
            build();
        }
    }
    
    
};



// trazer relogio pra ca.
struct sceneCount : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	void draw() {

	}
};



// trazer relogio pra ca.
struct sceneBars : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	struct bar {
		public:
		ofRectangle rect;
		vector <ofRectangle> rects;
		int w = 5;
		float pos = 0;
		float vel;
		float qual = 0;
		bar(ofRectangle r) : rect(r) {
			qual = ofRandom(0,1);
			vel = ofRandom(-4, 4);
			w = ofRandom(1,140);
			rect.height = ofRandom(10,100);
			// cout << rect << endl;
			int x = -rect.width * .25;
			while (x < rect.width*1.25) {
				rects.emplace_back(ofRectangle(x, rect.y, w, rect.height));
				x += w * 2;
			}
		}

		void draw() {
			pos += vel;
			ofPushMatrix();
			ofTranslate(fmod(pos, w*2) + rect.x, rect.y);
			// ofDrawRectangle(rect);
			for (auto & r : rects) {
				ofDrawRectangle(r);
			}
			ofPopMatrix();
		}
	};

	vector<bar> bars;

	void build() {
		int y=0;
		while (y < fbo->getHeight()) {
			bars.emplace_back(ofRectangle(0,y,fbo->getWidth(),10));
			y+= bars.back().rect.height;
		}
		cout << "setup bars size = " << bars.size() << endl;
	}

	void setup() override {
		build();
	}

	void draw() override {
		for (auto & b : bars) {
			ofSetColor(getCor(b.qual));
			b.draw();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "build") {
			build();
		}
		else if (e.name == "clear") {
			bars.clear();
		}
		else if (e.name == "rebuild") {
			bars.clear(); build();
		}
	}
};





// trazer relogio pra ca.
struct sceneImages : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	ofImage bg;
	vector <ofImage> images;

	struct img {
		public:
		ofImage image;
		glm::vec2 pos;
		int index = 0;
		img(ofImage i, int in) : image(i), index(in) {
			pos = glm::vec2(ofRandom(0,2000), ofRandom(0,2000));
		}
		void draw() {
//			image.draw(pos.x, pos.y);
			pos.x = ofNoise(ofGetElapsedTimef() * 0.02, index * 10) * 1420;
			pos.y = ofNoise(ofGetElapsedTimef() * 0.04, index * 11) * 980;
//            cout << pos << endl;
			image.draw(pos.x, pos.y, 400, 400);
		}
	};

	vector <img> imgs;

	void setup() override {
		bg.load("bg.jpg");
		ofDirectory dir;
		dir.allowExt("jpg");
		dir.allowExt("gif");
		dir.allowExt("jpeg");
		dir.listDir("_images");
		dir.sort();
		int index = 0;
		for (auto & d : dir) {
			ofImage i;
			i.load("_images/"+d.getFileName());
			imgs.emplace_back(i, index);
			index++;
		}
		
		for (auto & d : dir) {
			ofImage i;
			i.load("_images/"+d.getFileName());

			imgs.emplace_back(i, index);
			index++;
		}
	}
	
	map <string, ofBlendMode> blendModes = {
		{ "alpha", OF_BLENDMODE_ALPHA },
		{ "add", OF_BLENDMODE_ADD },
		{ "subtract", OF_BLENDMODE_SUBTRACT },
		{ "screen", OF_BLENDMODE_SCREEN },
		{ "multiply", OF_BLENDMODE_MULTIPLY },
		// { "darken", OF_BLENDMODE_DARKEN },
		// { "lighten", OF_BLENDMODE_LIGHTEN },
	};

	void draw() override {
		ofSetColor(255);
		bg.draw(0,0, 1920, 1080);
		ofEnableBlendMode(blendModes[uiC->pString["blend"]]);
		for (auto & i : imgs) {
			i.draw();
		}
	}
};


struct sceneStripes : public ofxScenes::sceneDmtr {
	public:
	using sceneDmtr::sceneDmtr;
	ofFbo fbo;
	void setup() override {
		fbo.allocate(3000, 200, GL_RGBA);
	}

	void build() {
		fbo.begin();
		ofClear(0,255);
		// ofSetColor(252, 186, 3);
		ofSetColor(255);
		int w = 250;
		int h = fbo.getHeight();
		ofPushMatrix();
		ofTranslate(-w*2,0);
		for (int x = -w*2; x<fbo.getWidth() + w*2; x+=w*2) {
			ofBeginShape();
			ofVertex(0, 0);
			ofVertex(w, 0);
			ofVertex(w+h, h);
			ofVertex(h, h);
			ofVertex(0, 0);
			ofEndShape();
			ofTranslate(w*2,0);
		}
		ofPopMatrix();

	glEnable(GL_BLEND);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glLogicOp(GL_INVERT);
	glEnable(GL_COLOR_LOGIC_OP);
	// glBlendFunc(GL_DST_COLOR,GL_ONE_MINUS_SRC_COLOR);

		ofSetColor(255);
		cout << uiC->pInt["sujeira"] << endl;
		for (int a=0; a<uiC->pInt["sujeira"]; a++) {
			float x = ofRandom(0, fbo.getWidth());
			float y = ofRandom(0, fbo.getHeight());
			// cout << x << endl;
			// cout << y << endl;
			float raio = ofRandom(-4,12);
			ofDrawCircle(x, y, raio);
		}

		glDisable(GL_COLOR_LOGIC_OP);

		fbo.end();
	}

	void uiEvents(ofxMicroUI::element & e) override {
		build();
	}

	void draw() override {
		ofSetColor(3, 86, 252);
		
		ofDrawRectangle(0,0,config->fbo->getWidth(),config->fbo->getHeight());
		ofSetColor(255);

		// ofSetColor(0);
		// ofDrawRectangle(0,400,fbo.getWidth(), fbo.getHeight());

		ofSetColor(252, 186, 3);
		fbo.draw(0,400);
	}
};




struct sceneRipple : public ofxScenes::sceneDmtr {
	using sceneDmtr::sceneDmtr;

	struct gota {
		public:
		glm::vec2 pos;
		float raio = 10;
		float vel = 2;

		gota(glm::vec2 p) : pos(p) {
		}

		void update() {
			raio += vel;
		}
		void draw() {
			ofDrawCircle(pos.x, pos.y, raio);
		}
	};

	struct emitter {
	public:
		glm::vec2 pos;
		emitter(glm::vec2 p) : pos(p) {
		}

		vector <gota> gotas;

		float nextJump = 0;
		void update() {
			if (ofGetElapsedTimef() > nextJump) {
				nextJump = ofGetElapsedTimef() + 1;
				gotas.emplace_back(gota(pos));
			}
			for (auto & g : gotas) {
				g.update();
			}	
		}

		void draw() {
			ofDrawRectangle(pos.x - 4, pos.y - 4, 8, 8);
			for (auto & g : gotas) {
				g.draw();
			}
		}
	};

	vector <emitter> emitters;
	glm::vec2 wh; 
	ofFbo f;

	void setup() override {
		wh = glm::vec2(fbo->getWidth(), fbo->getHeight());
		// cout << " ||||||| setup emitter" << endl;
		// f.allocate(wh.x * 3, wh.y * 3, GL_RGBA);
		f.allocate(wh.x * 2, wh.y * 2, GL_RGBA);
		for (int a=0; a<20; a++) {
			glm::vec2 pos = glm::vec2(ofRandom(0,wh.x), ofRandom(0,wh.y));
			emitters.emplace_back(pos);
		}
	}

	void drawFbo() {
		f.begin();
		ofClear(0,255);
		ofSetColor(0,255,0);
		ofDrawRectangle(0,0,100,100);
		ofPushMatrix();
		// ofTranslate(wh.x, wh.y);
		ofSetColor(255,0,0);
		ofDrawRectangle(0,0,wh.x, wh.y);

		ofNoFill();
		ofSetColor(255);
		for (auto & e : emitters) {
			// cout << e.pos << endl;
			e.update();
			e.draw();
		}
		ofPopMatrix();
		f.end();
	}

	void update() override {
		drawFbo();

		// for (auto & e : emitters) {
		// }

		// cout << "update" << endl;

	}

	void draw() override {
		// cout << "ripple draw" << endl;
		ofSetColor(255);
		// pool.draw(-wh.x  + uiC->pInt["offX"], -wh.y + uiC->pInt["offY"]);

		f.draw( uiC->pInt["offX"],  uiC->pInt["offY"]);
		// ofSetColor(255, 127);
		// ofEnableBlendMode(OF_BLENDMODE_ADD);
		// f.getTexture().drawSubsection(0, 0, wh.x, wh.y, wh.x, wh.y, wh.x, wh.y);
		// f.getTexture().drawSubsection(0, 0, wh.x, wh.y, wh.x, 0, -wh.x, wh.y);

		// void ofTexture::drawSubsection(float x, float y, float w, float h, float sx, float sy, float sw, float sh)
	}
};




struct objeto {
public:
	glm::vec2 pos;
	glm::vec2 vel;
	float qual;
	float angulo = 0;
	float velAngle;
	float velFactor = 1.0;

	vector <ofPolyline> polys;

	virtual void setup() {
		// cout << "setup on primitive" << endl;
	}

	objeto() {}
	objeto(glm::vec2 p) : pos(p) {
		vel = glm::vec2(ofRandom(-1,1), ofRandom(-1,1));
		velAngle = ofRandom(-1, 1);
		qual = ofRandom(0,1);
		// setup();
	}

	virtual void update() {
		angulo += velAngle * velFactor;
		pos += vel;
	}

	virtual void draw() {
		ofPushMatrix();
		ofTranslate(pos);
		ofRotateDeg(angulo);
		for (auto & p : polys) {
			p.draw();
		}
		ofPopMatrix();
	}
};




struct scenePlexus2021 : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	struct ponto : public objeto {
		public:
		// glm::vec2 pos;
		// glm::vec2 vel;
		float raio = 6;
		float velFactor = 1.0;
        int index;
		
		float qual;
		ponto(glm::vec2 p, int i) : objeto(p), index(i) {
			qual = ofRandom(0,1);
            float mag = 2;
            float angle = ofRandom(0,360);
            float vx = r2x(angle, mag);
            float vy = r2y(angle, mag);
            vel = glm::vec2(vx, vy);
//			vel = glm::vec2(ofRandom(-2,2), ofRandom(-2,2));
		}
		void update() {
			pos += vel * velFactor;
		}
		void draw() {
            if (raio) {
                ofDrawCircle(pos.x, pos.y, raio);
            }
		}
	};

	vector <ponto> pontos;

	void build() {
		pontos.clear();
		for (int i = 0; i < uiC->pInt["numero"]; i++) {
			pontos.emplace_back(randomPos(), i);
		}
	}

	void setup() override {
		float margem = 300;
		rectBounds.x -= margem;
		rectBounds.y -= margem;
		rectBounds.width += margem * 2;
		rectBounds.height += margem * 2;
		build();
	}

	void update() override {
		for (auto & p : pontos) {
			p.raio = uiC->pEasy["raio"] + p.qual * uiC->pEasy["raioRandom"];
			p.velFactor = uiC->pEasy["vel"] + p.qual * uiC->pEasy["velRandom"];
			p.update();
			p.pos = rectificate(p.pos);
		}
	}

	void draw() override {
		float distance = uiC->pEasy["distance"];
		float dcube = distance + distance * uiC->pEasy["distanceFactor"];
		ofColor cor;
		for (auto & p : pontos) {
//            cout << p.pos << endl;
			cor = getCor(p.qual * uiC->pFloat["colorRange"]);
			for (auto & pp : pontos) {
                if (p.index != pp.index && p.pos.x <= pp.pos.x ) { //&& p.pos.y >= pp.pos.y
                    float d1 = (pp.pos.x - p.pos.x);
                    if (d1 < distance) {
                        float d2 = ABS(pp.pos.y - p.pos.y);
                        if (d2 < distance) {
                            if (uiC->pBool["useAlpha"]) {
                                cor.a = ofClamp(ofMap(d1 + d2, 0, dcube, 255, 0), 0, 255);
//                                if (p.index == 1 && pp.index == 2) {
//                                    cout << int(cor.a) << endl;
//                                }
//                            cor.a = ofMap(d1 + d2, 0, dcube, 255, 0);
                            }
                            if (cor.a > 0) {
                                ofSetColor(cor);
                                ofDrawLine(p.pos, pp.pos);
                            }
                        }
                    }
                }
			}
            
            if (uiC->pBool["recoverAlpha"]) {
                cor.a = 255;
            }
			// ofSetColor(getCor(0));
			p.draw();
		}
	}

	glm::vec2 rectificate(glm::vec2 pos) {
		if (pos.x < rectBounds.x) {
			pos.x += rectBounds.width;
		}
		else if (pos.x > rectBounds.width) {
			pos.x -= rectBounds.width;
		}

		if (pos.y < rectBounds.y) {
			pos.y += rectBounds.height;
		}
		else if (pos.y > rectBounds.height) {
			pos.y -= rectBounds.height;
		}
		return pos;
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "numero") {
			build();
		}
	}
};


struct sceneGears : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	// esses structs dentro das cenas podiam ser filhos de um pai comum, com outras propriedades dentro dele.
	struct gear : public objeto {
		public:

		// using objeto::objeto;
		// vector <ofPolyline> lines;
		// float qual;
		// glm::vec2 pos;
		float raio = 100;
		int ndentes[6] = { 36,24,30,36,45,60 };

		void setup()  {
			raio = ofRandom(30,200);
			// int dentes = 20;
			float dentes = ndentes[int(ofRandom(0,6))];

			float fatia = 360.0 / dentes * 2.0;
			float dentinho = fatia / 6.0;
			float mag1 = 0;
			float mag2 = ofRandom(15,30);
			glm::vec2 dentinhos[4] = {
				glm::vec2(dentinho * 0, mag1),
				glm::vec2(dentinho * 1, mag2),
				glm::vec2(dentinho * 3, mag2),
				glm::vec2(dentinho * 4, mag1)
			};
			// float mags[6] = { 
			// 	mag1, mag2, mag2, mag1, mag1;
			// };
			ofPolyline line;
			ofPolyline line2;
			int index = 0;
			for (float a=0; a<=360; a+= fatia) {
				for (auto & d : dentinhos) {
					float angulo = a + d.x;
					float mag = raio + d.y;
					float x = r2x(angulo,mag);
					float y = r2y(angulo,mag);
					line.addVertex(x, y);
				}
                // cout << x << " : " << y << endl;
                line2.addVertex(r2x(a, raio*.25), r2y(a, raio*.25));
				index++;
			}
			polys.emplace_back(line);
			polys.emplace_back(line2);
		}

		gear() {}
		gear(glm::vec2 p) : objeto(p) {
			qual = ofRandom(0,1);
			setup();
		}
	};

	vector <objeto*> objetos;

	void build() {
		cout << "BUILD" << endl;
		objetos.clear();
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			objetos.emplace_back(new gear(randomPos()));
		}
	}

	void setup() override {
		float margem = 300;
		rectBounds.x -= margem;
		rectBounds.y -= margem;
		rectBounds.width += margem * 2;
		rectBounds.height += margem * 2;
		build();
	}

	void update() override {
		for (auto & o : objetos) {
			o->update();
			o->pos = rectificate(o->pos);
		}
	}

	void draw() override {
		for (auto & o : objetos) {
			ofSetColor(getCor(o->qual));
			o->draw();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "numero") {
			build();
		}
	}

	glm::vec2 rectificate(glm::vec2 pos) {
		if (pos.x < rectBounds.x) {
			pos.x += rectBounds.width;
		}
		else if (pos.x > rectBounds.width) {
			pos.x -= rectBounds.width;
		}

		if (pos.y < rectBounds.y) {
			pos.y += rectBounds.height;
		}
		else if (pos.y > rectBounds.height) {
			pos.y -= rectBounds.height;
		}
		return pos;
	}
};


#ifdef USEDELAUNAY

struct sceneDelaunay : public ofxScenes::sceneDmtr { //, ofxScenes::sceneType
	public:
	using sceneDmtr::sceneDmtr;
	ofTrueTypeFont font;
	ofTrueTypeFont * type = NULL;
	vector <ofPath> paths;
	vector <ofPolyline> lines;
    ofColor color = ofColor(255);
	
	struct letra {
	public:
		ofPath path;
		vector <ofPolyline> lines;
        ofxMicroUI * uiC = NULL;
        float qual;
		ofColor color = ofColor(255);

		// se eu for tridimensionalizar.
		ofMesh front;
		ofMesh back;
		ofMesh middle;

		// se eu for delaunayzar
		ofRectangle rect;
		ofFbo fbo;

        
        letra (ofPath p, ofxMicroUI * u) : path(p), uiC(u) {
            qual = ofRandom(0,1);
			path.setPolyWindingMode(OF_POLY_WINDING_ODD);
			const vector<ofPolyline>& allLines = path.getOutline();
			lines = allLines;
			rect = lines[0].getBoundingBox();
			for (auto & l : lines) {
				rect.growToInclude(l.getBoundingBox());
			}

			fbo.allocate(rect.width, rect.height, GL_RGBA);
			fbo.begin();
			ofClear(0, 255);
			ofSetColor(255);
			ofPushMatrix();
			ofTranslate(-rect.x, -rect.y);
			path.draw();
			fbo.end();
			// cout << rect << endl;


			front = path.getTessellation();
			back = path.getTessellation();
		}

		void draw() {
            ofSetColor(color);

			for (auto & l : lines) {
				auto line = l.getResampledBySpacing(uiC->pEasy["spacing"]);
                if (uiC->pBool["smooth"]) {
                    line = line.getSmoothed(uiC->pEasy["smooth"]);
                }

				for (auto & v : line.getVertices()) {
					if (uiC->pBool["node"]) {
						for (auto & vv : line.getVertices()) {
							if (vv != v) {
                                if (uiC->pBool["vibra"]) {
                                    color.a = ofClamp(ofRandom(-255,255), 0, 255);
                                    ofSetColor(color);
                                }
								ofDrawLine(vv, v);
							}
						}
					}
                    if (uiC->pBool["circle"]) {
                        ofDrawCircle(v.x, v.y, uiC->pEasy["radius"]);
//                        ofDrawCircle(v.x, v.y, 6);
                    }
				}
				if (uiC->pBool["drawMesh"]) {
                    front.draw();
                }

                if (uiC->pBool["drawLine"]) {
                    line.draw();
                }
			}
		}
	};

	vector <letra> letras;
    glm::vec2 offset;

	void setup() override {
		int size = 290;
		font.load("_type/DINNextLTPro-Black.ttf", size, true, true, true);
		type = &font;

		bool vflip = true; // OF flips y coordinate in the default perspective, 
		bool filled = true;
        string texto = "IDEIAS";
        ofRectangle rect = type->getStringBoundingBox(texto, 0, 0);
        offset = middle - glm::vec2(rect.width, -rect.height) * .5;
		paths = type->getStringAsPoints(texto, vflip, filled);
		for (auto & p : paths) {
			letras.emplace_back(p, uiC);
		}

		int index = 0;
		for (auto & l : letras) {
			l.qual = index / (float) letras.size();
			index++; 
		}
	}

	void draw() override {
		ofTranslate(offset);
		ofNoFill();


//		ofSetColor(255);

		for (auto & l : letras) {
			if (l.qual <= uiC->pEasy["letters"]) {
				float qual = fmod(l.qual + ofNoise(ofGetElapsedTimef()), 1.0);
				// cout << qual << endl;
				// ofSetColor(getCor(qual));
				l.color = getCor(qual);
				l.draw();
			}
		}
		// for (auto & p : paths) {
		// 	p.setFilled(false);
		// 	p.setStrokeColor(ofColor(255));
		// 	p.setStrokeWidth(4);
		// 	p.draw();
		// }
	}
};
#endif
