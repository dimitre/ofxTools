
// #include "sceneGirinos.h"

struct sceneGirinosNeue : public ofxScenes::sceneDmtr, ofxScenes::polarVec2 {
public:
	using sceneDmtr::sceneDmtr;

	struct settings {
	public:
		ofRectangle bounds;
		ofxMicroUI * ui = NULL;
		int maxPositions = 10;
//        settings();
//		settings(ofxMicroUI * ui) : ui(ui) {}
	} set;

	struct girino {
	public:
		settings * _set = NULL;
		glm::vec2 pos;
		uint8_t cursor = 0;
//		glm::vec2 positions[20];
        glm::vec2 positions[20];
		float qual;
		bool alive = true;
		ofPolyline p;
        float angulo;
        float vel = 2;

		girino(glm::vec2 p, float q, settings * s)
		: pos(p), qual(q), _set(s) {}

		void draw() {
			if (alive) {
				ofDrawRectangle(pos.x, pos.y, 10, 10);
				p.draw();
				
			}
		}

		void addX(float v) {
			pos.x += v;
			for (auto & p : positions) { p.x += v; }
		}
		
		void addY(float v) {
			pos.y += v;
			for (auto & p : positions) { p.y += v; }
		}

		void update() {
//            cout << _set->ui->pFloat["densidade"] << endl;
			angulo = fmod(ofNoise((pos.x)/_set->ui->pFloat["densidade"],
				(pos.y)/_set->ui->pFloat["densidade"],
				qual * _set->ui->pFloat["qual"],
				_set->ui->pFloat["incrementador"]
			) * 360.0 * _set->ui->pFloat["anguloMult"], 360.0);

            vel = _set->ui->pFloat["vel"];
			pos += p2c(glm::vec2(angulo, vel));
				
			if (pos.x > _set->bounds.width) {
				addX(-_set->bounds.width);
			}
			else if (pos.x < _set->bounds.x) {
				addX(_set->bounds.width);
			}

			if (pos.y > _set->bounds.height) {
				addY(-_set->bounds.height);
			}
			else if (pos.y < _set->bounds.y) {
				addY(_set->bounds.height);
			}

			positions[cursor] = pos;
            positions[cursor + _set->maxPositions] = pos;
            cursor = (cursor + 1) % _set->maxPositions;
//            cursor ++ ;
//			if (cursor >= _set->maxPositions) {
//				cursor -= _set->maxPositions;
//			}

//            glGenBuffers(1, VertexVBOID);
//            glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
//
//            glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex)*3, &pvertex[0].x, GL_STATIC_DRAW);
//
//            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
//            glEnableVertexAttribArray(0);
//            glDrawArrays(GL_LINE_STRIP, 0, sizeof(positions)/4);
//            glDisableVertexAttribArray()
//            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
			p.clear();
//            p.addVertices( positions[cursor], 10);
            
            for (int a=cursor; a<(_set->maxPositions + cursor); a++) {
//                glm::vec2 point = positions[a % _set->maxPositions];
                glm::vec2 point = positions[a];
                p.addVertex(point.x, point.y);
            }
		}
	};

	vector <girino> girinos;
	
	void setup() override {
		set.ui = uiC;
		int margem = 100;
		set.bounds = ofRectangle(-margem, -margem, fbo->getWidth() + margem * 2, fbo->getHeight() + margem * 2);
		build();
	}

	void build() {
        girinos.clear();
		for (int i = 0; i < uiC->pInt["numero"]; i++) {
			float qual = i/(float)uiC->pInt["numero"];
			girinos.emplace_back(randomPos(),qual, &set);
		}
	}

	void update() override {
		for (auto & g : girinos) {
			g.update();
		}
	}

	void draw() override {
		ofNoFill();
		ofSetColor(255);
		for (auto & g : girinos) { 
			g.draw();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "numero") {
            build();
        }
    }
};
	
	



struct sceneGirinos : public ofxScenes::sceneDmtr, ofxScenes::polarVec2 {
public:
	using sceneDmtr::sceneDmtr;
	// name = "girinos";

	
	bool girinoChanged = false;
	
	void uiEvents(ofxMicroUI::element & e) override {
		girinoChanged = true;
	}

	struct girinoSettings {
	public:
		bool circuit2;
		ofRectangle boundsRect;
		ofRectangle fboRect;
		bool useCurve = true;
		bool drawRect = false;
		bool circuit = true;
		bool drawText = false;
		float incrementador,
				densidade, fatorQual, anguloMult,
				densidade2, fatorQual2, anguloMult2;
	} girinoSet, girinoSet2;


	
	
	
	struct girino {
	public:
		girino() {}
		float rand;
		
		glm::vec2 pos;
		float qual;
		int cursor = 0;
		glm::vec2 positions[60];
		int posSize = 10;
		float angulo;
		float vel = 2;
		bool alive = false;
		
		girinoSettings * _settings = NULL;
		
		girino(float q, girinoSettings & _s) : qual(q) {
			_settings = &_s;
			pos = glm::vec2(ofRandom(_settings->boundsRect.x, _settings->boundsRect.width),
							ofRandom(_settings->boundsRect.y, _settings->boundsRect.height));
			rand = ofRandom(0,1);
			posSize = ofRandom(10,60);
			angulo = ofRandom(0,360);
			alive = true;
		}
		
		void addX(float v) {
			pos.x += v;
			for (auto & p : positions) { p.x += v; }
		}
		
		void addY(float v) {
			pos.y += v;
			for (auto & p : positions) { p.y += v; }
		}

		void preUpdate() {
			if (_settings != NULL) {
				
				angulo = fmod(ofNoise((pos.x)/_settings->densidade,
							   (pos.y)/_settings->densidade,
							   qual * _settings->fatorQual,
							   _settings->incrementador
							) * 360.0 * _settings->anguloMult, 360.0);
				if (_settings->densidade2) {
					angulo += fmod(ofNoise((pos.x)/_settings->densidade2,
									(pos.y)/_settings->densidade2,
									qual * _settings->fatorQual2,
									_settings->incrementador
								) * 360.0 * _settings->anguloMult2, 360.0);
				}
			}
		}
		
		void update() {
			if (alive) {
				preUpdate();
				if (_settings->circuit) {
					angulo = int(angulo / 45) * 45;
				}
				if (_settings->circuit2) {
					angulo = int(angulo / 90) * 90;
				}
				pos += p2c(glm::vec2(angulo, vel));
				
				if (pos.x > _settings->boundsRect.width) {
					addX(-_settings->fboRect.width);
				}
				else if (pos.x < _settings->boundsRect.x) {
					addX(_settings->fboRect.width);
				}

				if (pos.y > _settings->boundsRect.height) {
					addY(-_settings->fboRect.height);
				}
				else if (pos.y < _settings->boundsRect.y) {
					addY(_settings->fboRect.height);
				}
				
				positions[cursor] = pos;
				cursor ++ ;
				if (cursor >= posSize) {
					cursor -= posSize;
				}
			}
		}
		
		void draw() {
			//update();
			if (alive) {
				ofPolyline p;
				for (int a=0; a<posSize; a++) {
					glm::vec2 point = positions[(a+cursor)%posSize];

					if (_settings->useCurve) {
						if (a == 0) {
							p.lineTo(point.x, point.y);
						} else {
							p.curveTo(point.x, point.y);
						}
					} else {
						p.addVertex(point.x, point.y);
					}
				}
				p.draw();
				
				if (_settings->drawRect) {
					ofDrawRectangle((ofPoint)pos - ofPoint(3,3), 6, 6);
				}

				if (_settings->drawText) {
					string s = ofToString(int(angulo));
					ofDrawBitmapString(s, pos.x, pos.y);
				}
			}
		}
	};



	class girinosThread : public ofThread {
		public:
		bool doUpdate = true;
		vector <girino> girinosDraw, girinos, girinosMiddle;
		ofxMicroUI * uiColors = NULL;
		
		girinosThread() {
			girinosDraw.reserve(5000);
			girinos.reserve(5000);
			girinosMiddle.reserve(5000);
		}

		~girinosThread() {
			cout << "girinos destructor" << endl;
			waitForThread();
		}
		
		void threadedFunction() { while(isThreadRunning()) { update(); } }
		
		void draw() {
			lock();
			girinosDraw = girinosMiddle;
			unlock();

			for (auto & g : girinosDraw) {
				if (uiColors != NULL) {
					ofSetColor(ofxScenes::getColor(g.qual, uiColors));
				}
				g.draw();
			}
		}
		
		void update() {
			if (doUpdate) {
				for (auto & g : girinos) { g.update(); }
				lock();
				girinosMiddle = girinos; // copy from one thread to the other
				unlock();
				//doUpdate = false;
			}
		}
		
		void setup() {
		}
	} gi;


	void createGirino(float qual) {
		gi.girinos.emplace_back(qual,girinoSet);
	}

	void setup() override {
//        girinoSet.fboRect = config->soft->fboRect;
		girinoSet.fboRect = ofRectangle(0,0,fbo->getWidth(), fbo->getHeight());
		int margem = 100;
		girinoSet.boundsRect = ofRectangle(-margem, -margem,
											 fbo->getWidth() + margem * 2,
											 fbo->getHeight() + margem * 2);
		
		gi.uiColors = config->uiColors;
		gi.startThread();
		int max = 200;
		gi.lock();
		for (int a=0; a<max; a++) {
			float qual = a/(float)max;
			createGirino(qual);
		}
		gi.unlock();
	}

	float incrementador = 0;

	void draw() override {
		checkSetup();

		if (girinoChanged)
		{
			cout << "updating changed " << ofRandom(0,9999) << endl;
			gi.lock();
			int index = 0;
			for (auto & g : gi.girinos) {
				g.qual = index/(float)gi.girinos.size();
				index++;
				g.vel = uiC->pEasy["vel"] + g.rand * uiC->pEasy["velRand"];
			}
			girinoChanged = false;
			gi.unlock();
		}
		
		if (uiC->pBool["numeroWaves"]) {
			float s = ofMap(sin(ofGetElapsedTimef() * 3), -1, 1, 100, 3000);
			uiC->getSlider("numero")->set(int(s));
		}
		ofNoFill();
		incrementador += uiC->pFloat["perFrame"];
		

		
		if (gi.girinos.size() < uiC->pInt["numero"]) {
			int numero = uiC->pInt["numero"] - gi.girinos.size();
			gi.lock();
			for (int a=0; a<numero; a++) {
				createGirino(ofRandom(0,1));
			}
			gi.unlock();
		}
		else if (gi.girinos.size() > uiC->pInt["numero"]) {
			int numero = gi.girinos.size() - uiC->pInt["numero"];
			gi.lock();
			for (int a=0; a<numero; a++) {
	//			gi.girinos.begin()->alive = false;
				gi.girinos.begin()->_settings = NULL;
				gi.girinos.erase(gi.girinos.begin());
			}
			gi.unlock();
		}
		
	//	float halfPosX = fbo->getWidth() * .5;
	//	for (auto & g : gi.girinos) {
	//		g._settings = g.pos.x < halfPosX ? &girinoSet : &girinoSet2;
	//	}
		
		gi.draw();
		gi.doUpdate = true;
		
		gi.lock();
			girinoSet.incrementador = incrementador;
			girinoSet2.incrementador = incrementador;
			girinoSet.densidade = 		uiC->pEasy["densidade_1"];
			girinoSet.fatorQual = 		uiC->pEasy["fatorQual_1"];
			girinoSet.anguloMult =		uiC->pEasy["anguloMult_1"];
			girinoSet.densidade2 = 		uiC->pEasy["densidade2_1"];
			girinoSet.fatorQual2 = 		uiC->pEasy["fatorQual2_1"];
			girinoSet.anguloMult2 =		uiC->pEasy["anguloMult2_1"];
			girinoSet.circuit = uiC->pBool["circuit_1"];
			girinoSet.circuit2 = uiC->pBool["circuit90_1"];
		
			girinoSet.drawRect = uiC->pBool["drawRect"];
			girinoSet.drawText = uiC->pBool["drawText"];

			girinoSet.useCurve = uiC->pBool["useCurve"];
			
//			girinoSet2.densidade = uiC->pEasy["densidade_2"];
//			girinoSet2.fatorQual = uiC->pEasy["fatorQual_2"];
//			girinoSet2.anguloMult = uiC->pEasy["anguloMult_2"];
//			girinoSet2.densidade2 = uiC->pEasy["densidade2_2"];
//			girinoSet2.fatorQual2 = uiC->pEasy["fatorQual2_2"];
//			girinoSet2.anguloMult2 = uiC->pEasy["anguloMult2_2"];
//			girinoSet2.circuit = uiC->pBool["circuit_2"];
//			girinoSet2.circuit2 = uiC->pBool["circuit90_2"];
//
//			girinoSet2.drawRect = uiC->pBool["drawRect"];
//			girinoSet2.drawText = uiC->pBool["drawText"];
		gi.unlock();
	}
};



