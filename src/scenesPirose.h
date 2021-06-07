// future user interface

#define MARGEM 8

struct scenePirose : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	struct microScene {
		public:
		float wh = 192 - MARGEM*2;

		ofRectangle rect = ofRectangle(0,0,128,128);
		virtual void setup() {}
		virtual void update() {}
		virtual void draw() {}
	};

	struct eq : public microScene {
		void setup() override {
			if (rect.width == 0) {
				rect.width = 256 - MARGEM*2;
				rect.height = 64 - MARGEM*2;
			}
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
			if (rect.width == 0) {
				rect.width = 320 - MARGEM*2;
				rect.height = 64 - MARGEM*2;
			}
			rand = ofRandom(0,999);
			multNoise = ofRandom(0.005, 0.03);
		}
		void draw() override {
			ofPushMatrix();
			ofDrawRectangle(rect);
			ofTranslate(rect.x, rect.y);
			// ofDrawRectangle(0,0,300,75);
			vector <ofVec3f> pontos;
			for (int a=0; a<rect.width; a+=6) {
				pontos.push_back(ofVec3f(a, ofNoise(a * multNoise , ofGetElapsedTimef() *.2 + rand) * rect.height));
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
			if (rect.width == 0) {
				rect.width = 128 - MARGEM*2;
				rect.height = 64 - MARGEM*2;
			}
		}
		void draw() override {
			ofDrawBitmapString(ofToString(ofRandom(0,999)), rect.x, rect.y + 10);
		}
	};

	struct ascii : public microScene {
		public:

		string st[6] = { ".", ":" , "/", "|", "\\", "o" };

		void setup() override {
			if (rect.width == 0) {
				rect.width = 256 - MARGEM * 2;
				rect.height = 128 - MARGEM * 2;
			}
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			for (int x=0; x<rect.width; x+= 16) {
				for (int y=12; y<rect.height; y+= 16) {
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
			if (rect.width == 0) {
				rect.width = 256 - MARGEM*2;
				rect.height = 64 - MARGEM*2;
			}
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			for (int x=0; x<rect.width; x+= 12) {
				for (int y=12; y<=rect.height; y+= 12) {
					int n = ofRandom(0,9);
					ofDrawBitmapString(ofToString(n), x, y);
				}
			}
			ofPopMatrix();
		}
	};

	struct noiseGrid : public microScene {
		float modulo;
		
		void setup() override {
			if (rect.width == 0) {
				rect.width = wh;
				rect.height = wh;
			}
			modulo = 16.0;
		}
		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);

			for (float a=0; a<=rect.width; a+=modulo) {
				for (float b=0; b<=rect.height; b+=modulo) {
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
			if (rect.width == 0) {
				rect.width = wh;
				rect.height = wh;
			}
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
		

		float radius = 40;
		float radius2 = 32;
		void setup() override {
			if (rect.width == 0) {
				// cout << "wh = " << wh << endl;
				rect.width = wh;
				rect.height = wh;
			}
			radius = rect.height * .25;
			radius2 = rect.height * .2;
		}

		void draw() override {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);

			ofSetLineWidth(1);
			ofNoFill();
		//        ofSetColor(255);
			// fundo grid
			// float modulo = wh/10.0;
			float modulo = 16.0;
			float cols = rect.width / modulo;
			float rows = rect.height / modulo;
			// cout << cols << endl;
			// cout << rows << endl;
			// cout << "-------" << endl;

			// cout << rect << endl;

			// horiz
			for (int x=0; x<=rows; x++) {
				// ofDrawLine(-modulo*.5, x*modulo, cols*modulo, x*modulo);
				ofDrawLine(0, x*modulo, cols*modulo, x*modulo);
			}

			for (int x=0; x<=cols; x++) {
				// ofDrawLine(x*modulo, -modulo*.5, x*modulo, rows*modulo);
				ofDrawLine(x*modulo, 0, x*modulo, rows*modulo);
				for (int y=0; y<=rows; y++) {
					ofDrawCircle(x*modulo, y*modulo, 2);
				}
			}

			// pontinhos radiais
			{
				vector <ofVec3f> pontos;
				for (int b=0; b<5; b++)
				for (int a=0; a<10; a++) {
					float ang = a*36.0 + ofNoise(a*1.3, b*1, ofGetElapsedTimef()*0.6) * radius2;
					float mag = radius + ofNoise(a*1.5, b*1.2, ofGetElapsedTimef()*0.4) * radius;
					float x = rect.width*.5 + r2x(ang, mag);
					float y = rect.height*.5 + r2y(ang, mag);
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
		int margem = 8;
		glm::vec2 flow = glm::vec2(margem, margem);
		bool flowVert = true;
		int colWidth = 0;

		glm::vec2 dimensions;

		for (auto & l : lines) {
			cout << "--------------" << endl;
			cout << l << endl;
			vector <string> cols = ofSplitString(l, "\t");
			if (cols.size() == 3) {
				vector <string> vals = ofSplitString(cols[2], " ");
				dimensions = glm::vec2(ofToInt(vals[0]), ofToInt(vals[1]));
			}
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
					scenes.back()->rect.width = dimensions.x * 64 - MARGEM*2;
					scenes.back()->rect.height = dimensions.y * 64 - MARGEM*2;
					scenes.back()->rect.x = flow.x;
					scenes.back()->rect.y = flow.y;
					scenes.back()->setup();
					
					if (flowVert) {
						colWidth = MAX(colWidth, scenes.back()->rect.width);
						flow.y += scenes.back()->rect.height + margem * 2;
					} else {
						flow.x += scenes.back()->rect.width + margem * 2;
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
				flow.x += colWidth + margem*2;
				colWidth = 0;
				flow.y = margem;
			}
		}
	}

	void setup() override {
		//  setup3();
		loadFromText();
		cout << "microScenes setup" << endl;
		cout << scenes.size() << endl;
	}
	
	void draw() override {

		ofNoFill();

		// drawgrid . talvez fazer funcao primitiva na cena
		if (uiC->pBool["drawGrid"]) {
			float x = 0;
			ofSetLineWidth(.5);
			while (x < fbo->getWidth()) {
				ofDrawLine(x, 0, x, fbo->getHeight());
				ofDrawLine(0, x, fbo->getWidth(), x);
				x+=64;
			}
		}


		ofSetColor(getCor(0));
		for (auto & s : scenes) {
			if (uiC->pBool["drawRect"]) {
				ofSetColor(255, 0, 70);
				ofDrawRectangle(s->rect);
				ofSetColor(getCor(0));
			}
			s->draw();
		}

        
        // DEBUG ONLY

	}
};
