// future user interface
struct scenePirose : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	struct microScene {
		public:
		ofRectangle rect = ofRectangle(0,0,100,100);
		virtual void setup() {}
		virtual void update() {}
		virtual void draw() {}
	};

	struct eq : public microScene {
		void setup() override {
			// cout << ">>>> setup horizon " << endl;
			rect.width = 300;
			rect.height = 75;
		}

		void draw() override {
			ofPushMatrix();
			ofDrawRectangle(rect);
			ofTranslate(rect.x, rect.y);
			float numero = 30;
			float w = 300/(numero+2) - 4;
			for (float a=0; a<numero; a++) {
				float x = ofMap(a, -1, numero+1, 0, rect.width);
				ofDrawRectangle(x,4,w,rect.height -8);
			}
			ofPopMatrix();
		}
	};


	struct horizon : public microScene {
		public:
		
		float rand = 0;
		float multNoise = 0.02;
		void setup() override {
			// cout << ">>>> setup horizon " << endl;
			rect.width = 300;
			rect.height = 75;
			rand = ofRandom(0,999);
			multNoise = ofRandom(0.005, 0.03);
		}
		void draw() override {
			ofPushMatrix();
			ofDrawRectangle(rect);
			ofTranslate(rect.x, rect.y);
			// ofDrawRectangle(0,0,300,75);
			vector <ofVec3f> pontos;
			for (int a=0; a<300; a+=6) {
				pontos.push_back(ofVec3f(a, ofNoise(a * multNoise , ofGetElapsedTimef() *.2 + rand)*75));
			}
			ofPolyline poly;
			for (auto & p : pontos) {
				poly.addVertex(p);
				ofDrawCircle(p, 3.0);
			}
			poly.draw();
			ofPopMatrix();
		}
	};

	struct number : public microScene {
		public:

		void setup() override {
			rect.width = 100;
			rect.height = 12;
			cout << "number setup" << endl;
			cout << rect << endl;
		}
		void draw() override {
			ofDrawBitmapString(ofToString(ofRandom(0,999)), rect.x, rect.y + 10);
		}
	};

	struct ascii : public microScene {
		public:

		string st[6] = { ".", ":" , "/", "|", "\\", "o" };

		void setup() override {
			rect.width = 300;
			rect.height = 75;
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			for (int x=0; x<rect.width; x+= 12) {
				for (int y=0; y<rect.height; y+= 12) {
					int n = (ofNoise(x*.02, y *.03, ofGetElapsedTimef() * .3) * 6.0);
					// int n = ofRandom(0,6);
					ofDrawBitmapString(st[n], x, y);
				}
			}
			ofPopMatrix();
		}
	};

	struct gridNumber : public microScene {
		public:

		void setup() override {
			rect.width = 300;
			rect.height = 75;
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			for (int x=0; x<rect.width; x+= 12) {
				for (int y=0; y<rect.height; y+= 12) {
					int n = ofRandom(0,9);
					ofDrawBitmapString(ofToString(n), x, y);
				}
			}
			ofPopMatrix();
		}
	};

	struct noiseGrid : public microScene {
		void setup() override {
			rect.width = 200;
			rect.height = 200;
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			for (int a=0; a<rect.width; a+=14) {
				for (int b=0; b<rect.height; b+=14) {
					float raio = ofNoise(ofGetElapsedTimef() * .2, a + b) * 7.0;
					ofDrawRectangle(a - raio*.5, b - raio*.5, raio, raio);
				}
			}
			ofPopMatrix();
		}
	};

	struct lathe : public microScene {
		public:
		float rotX, rotY, rotZ;
		float rand;
		void setup() override {
			rect.width = 200;
			rect.height = 200;
			rotX = ofRandom(-120,120);
			rotY = ofRandom(-120,120);
			rotZ = ofRandom(-120,120);
			rand = ofRandom(0,9999);
		}

		void draw() override {
			ofPushMatrix();
			// ofDrawRectangle(rect);
			ofTranslate(rect.x + rect.width*.5, rect.y + rect.height*.5);
			vector <glm::vec2> vertices;
			ofPolyline poly;
			for (int a=0; a<15; a++) {
				float x = a*5 - 50;
				float y = ofNoise(a*.08, ofGetElapsedTimef() * .15 + rand) * 40.0;
				vertices.emplace_back(x, y);
				poly.addVertex(x, y);
			}

			ofRotateXDeg(rotX);
			ofRotateYDeg(rotY);
			ofRotateZDeg(rotZ);
			for (int a = 0; a<360; a+=18) {
				ofPushMatrix();
				ofRotateYDeg(a);
				ofRotateZDeg(-90);
				poly.draw();
				// for (auto & v : vertices) {
					
				// }
				ofPopMatrix();
			}
			ofPopMatrix();
		}
	};
	
	struct grid : public microScene {
		public:
		
		void setup() override {
			rect.width = 200;
			rect.height = 200;
		}

		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);

			ofSetLineWidth(1);
			ofNoFill();
		//        ofSetColor(255);
			// fundo grid
			float modulo = 20.0;
			for (int x=0; x<=10; x++) {
				
				{
					ofDrawLine(x*modulo, -modulo*.5, x*modulo, 10.5*modulo);
					ofDrawLine(-modulo*.5, x*modulo, 10.5*modulo, x*modulo);
					for (int y=0; y<=10; y++) {
						ofDrawCircle(x*modulo, y*modulo, 2);
					}
				}
			}

			// pontinhos radiais
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
			ofPopMatrix();
			
		}
	};
	

	vector <microScene *> scenes;

	
	void loadFromText() {
        cout << "_pirose load from text " << endl;
		vector <string> lines = ofxMicroUI::textToVector("_pirose.txt");
		int margem = 20;
		glm::vec2 flow = glm::vec2(margem, margem);
		bool flowVert = true;
		int colWidth = 0;

		for (auto & l : lines) {
			cout << "--------------" << endl;
			cout << l << endl;
			vector <string> cols = ofSplitString(l, "\t");
			if (cols[0] == "flowVert") {
				flowVert = true;
			}
			else if (cols[0] == "flowHoriz") {
				flowVert = false;
			}

			else if (cols[0] == "scene") {

				if (cols[1] == "horizon") {
					scenes.push_back(new horizon());
				}
				else if (cols[1] == "number") {
					scenes.push_back(new number());
				}
				else if (cols[1] == "ascii") {
					scenes.push_back(new ascii());
				}

				else if (cols[1] == "gridNumber") {
					scenes.push_back(new gridNumber());
				}				
				else if (cols[1] == "grid") {
					scenes.push_back(new grid());
				}
				else if (cols[1] == "lathe") {
					scenes.push_back(new lathe());
				}
				else if (cols[1] == "noiseGrid") {
					scenes.push_back(new noiseGrid());
				}
				else if (cols[1] == "eq") {
					scenes.push_back(new eq());
				}				

				if (scenes.size()) {
					scenes.back()->rect.x = flow.x;
					scenes.back()->rect.y = flow.y;
					scenes.back()->setup();
					
					if (flowVert) {
						colWidth = MAX(colWidth, scenes.back()->rect.width);
						flow.y += scenes.back()->rect.height  + margem;
					} else {
						flow.x += scenes.back()->rect.width  + margem;
					}
					// cout << flow << endl;
					// cout << scenes.back()->rect << endl;
				}
 			}

			else if (cols[0] == "margem") {
				margem = ofToInt(cols[1]);
				flow = glm::vec2(margem, margem);
			}

			else if (cols[0] == "newCol") {
//				cout << colWidth << endl;
				flow.x += colWidth + margem;
				colWidth = 0;
				flow.y = margem;
			}
		}
	}

	void setup() override {
		//  setup3();
		loadFromText();
//		for (auto & s : scenes) {
//			s->setup();
//		}
		cout << "microScenes setup" << endl;
		cout << scenes.size() << endl;
	}
	
	void draw() override {
		ofSetColor(getCor(0));
		ofNoFill();
		for (auto & s : scenes) {
			s->draw();
		}

        
        // DEBUG ONLY
//		ofSetColor(255,0,70);
//		int i = 0;
//		for (auto & s : scenes) {
//			ofDrawBitmapString(ofToString(i), s->rect.x, s->rect.y + 10);
//			i++;
//		}

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
		float velFactor = 1;
		float qual = 0;
		bar(ofRectangle r) : rect(r) {
			qual = ofRandom(0,1);
			vel = ofRandom(-4, 4);
			w = ofRandom(2,140);
			rect.height = ofRandom(10,200);
			// cout << rect << endl;
			int x = -rect.width * .25;
			while (x < rect.width*1.25) {
				rects.emplace_back(ofRectangle(x, rect.y, w, rect.height));
				x += w * 2;
			}
		}

		void draw() {
			pos += vel * velFactor;
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
	float nextJump = 0;

	void build() {
		int y=0;
		while (y < fbo->getHeight()) {
			bars.emplace_back(ofRectangle(0,y,fbo->getWidth(),10));
			y+= bars.back().rect.height;
		}
//		cout << "setup bars size = " << bars.size() << endl;
	}

	void setup() override {
		build();
	}

	void draw() override {
		if (uiC->pBool["rebuild"]) {
			if (ofGetElapsedTimef() > nextJump) {
				nextJump = ofGetElapsedTimef() + uiC->pFloat["rebuildTime"];
				bars.clear();
				build();
			}
		}
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
			bars.clear(); 
			build();
		}
	}
};





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






struct sceneSorteio : public ofxScenes::sceneDmtr { //, ofxScenes::sceneType
public:
    using sceneDmtr::sceneDmtr;

    vector <ofRectangle> rects = {
        ofRectangle(768, 128, 512, 384),
        ofRectangle(1280, 128, 512, 384),
        ofRectangle(1792, 128, 512, 384),
        ofRectangle(2304, 128, 512, 384),
    };
    
    vector <ofRectangle> rectsMargem = rects;
    
    float margem = 12;
    void setup() override {
        for (auto & r : rectsMargem) {
            r.x += margem *.5;
            r.y += margem *.5;
            r.width -= margem;
            r.height -= margem;
        }
        
    }
    
    int indexRect = 0;
    float endTime = 0;
    void draw() override {
        ofNoFill();
        ofSetColor(getCor(0));
        ofSetLineWidth(margem);
        if (ofGetElapsedTimef() < endTime) {
            int nframes = uiC->pInt["nframes"];
            if (ofGetFrameNum() % nframes == 0) {
                int lastFrame = indexRect;
                while (indexRect == lastFrame) {
                    indexRect = ofRandom(0,4);
                }
            }
            ofDrawRectangle(rectsMargem[indexRect]);
        } else {
            if ((ofGetFrameNum()/2) % 2 == 0) {
                ofDrawRectangle(rectsMargem[uiC->pInt["vencedor"]]);
            }
        }
    }
    
    void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "reset") {
            endTime = ofGetElapsedTimef() + uiC->pInt["segundos"];
        }
    }
};



struct sceneLikeDislike : public ofxScenes::sceneDmtr {
public:
    using sceneDmtr::sceneDmtr;
    ofImage l, d;
    
    void setup() override {
        l.load("_img/Like.png");
        d.load("_img/Dislike.png");
    }
    
    void draw() override {
        float x = 0;
        bool dislike = true;
        ofSetColor(255);
        while (x < fbo->getWidth()) {
            dislike ^= 1;
            float y = 0;
//            float yy = fmod(y + incrementa("velY"), fbo->getHeight());
            while (y < fbo->getHeight()) {
                ofPushMatrix();
//                float xx = fmod(x + incrementa("velX"), fbo->getWidth());
                ofTranslate(x, y);
                ofImage * i = dislike ? &l : &d;
                i->draw(0,0);
                ofPopMatrix();
                y += 100;
            }
            x += 100;
        }
    }
};
