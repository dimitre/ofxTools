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



struct sceneRelogio : public sceneObjetos, ofxScenes::sceneType {
public:
	using sceneObjetos::sceneObjetos;
	struct relogio : public objeto {
	public:
//		float raio;
		float raio2;
//		float angulo = 0;
//		float velAngulo;
//        int numeros[6] = { 12, 20, 30, 40, 60, 120 };
//		int numeros[6] = { 30, 60, 30, 40, 60, 120 };
		int numeros[6] = { 20, 20, 30, 40, 50, 60 };

		int numero;
		float graus;
		float lineWidth;
		ofRectangle rect;
		
		/*
		 seria lindo fazer um qual aqui e deixar o raio e numero de dentes proporcional a isso.
		 seria mesmo.
		 */

		void setup() override {
			objeto::setup();
			raio2 = raio + ofRandom(raio*.1, raio*.3);
			numero = numeros[int(qual * 6)];
//            numero = numeros[int(ofRandom(0,5))];
			graus = 360.0 / (float) numero;
			lineWidth = ofRandom(0,5);
		}
		
		relogio(glm::vec2 pos, settings * s, float q) : objeto(pos, s, q) {
			setup();
		}
		
		relogio(glm::vec2 pos, settings * s) : objeto(pos, s) {
			setup();
		}
		
		relogio(ofRectangle r) : rect(r) {
			setup();
		}

		void draw() override {
			
			// objeto
			update();
			ofPushMatrix();
			ofTranslate(pos.x, pos.y);

			// se precisar fazer lookup aqui, tipo salvar varias polylines e dar um ofRotateDeg em cada frame.
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
	
	
	void setup() override {
		setupRectBounds(400);
		build();
		sceneType::setup();
		nextJump = ofGetElapsedTimef() + 1;
	}
	
	int contagem = 0;
	int lastSecond;
	float nextJump;
	int totalSeconds = 10;
	
	void update() override {
		for (auto & r : objetos) {
			r->pos = rectificate(r->pos);
		}
	}
	
	void draw() override {
		ofNoFill();
		ofSetColor(255);
		ofDrawRectangle(rectBounds);

		
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
			set.lineFactor = uiC->pEasy["lineFactor"];
			for (auto & r : objetos) {
				ofSetColor(getCor(r->qual * uiC->pFloat["colorRange"]));
				r->draw();
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
		sceneObjetos::uiEvents(e);
		sceneType::uiEvents(e);
		
		if (e.name == "margem") {
			setupRectBounds(*e.i);
		}

		if (e.name == "minuto") {
			totalSeconds = uiC->pBool["minuto"] ? 60 : 10;
			contagem = 1;
		}

//        cout << e.name << endl;
		// isso ja ta la no sceneObjetos::uiEvents
//		if (e.name == "numero" || e.name == "seed") {
//			build();
//		}
	}
	
	void build() override {
		objetos.clear();
		cout << "RELOGIO BUILD" << endl;
		if (uiC->pBool["seed"]) {
			ofSeedRandom(uiC->pInt["seed"]);
		}
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			float q = a/(float)uiC->pInt["numero"];
			// objetos.emplace_back(new relogio(rectBounds));
			objetos.emplace_back(new relogio(randomPos(), &set, q));
		}
	}
};




struct scenePlexus2022 : public sceneObjetos, ofThread {
	using sceneObjetos::sceneObjetos;
	void threadedFunction() {
		while(isThreadRunning()) {
			lock();
//                image.setFromPixels(cam.getPixels());
				// done with the resource
		   unlock();
		}
	}
};


//struct scenePlexus2021 : public sceneObjetos, ofThread {
//    void threadedFunction() override {
//        while(isThreadRunning()) {
//            lock();
//            updateMesh();
////                image.setFromPixels(cam.getPixels());
//                // done with the resource
//            unlock();
//        }
//    }


struct scenePlexus2021 : public sceneObjetos {
public:
	using sceneObjetos::sceneObjetos;
	
	struct ponto : public objeto {
		public:
		float raio = 6;
		float velFactor = 1.0;
		int index;
		float indexQual;
		float qual;
        
        void setup() override {
            
        }
		
		ponto(glm::vec2 p, int i) : objeto(p), index(i) {
			qual = ofRandom(0,1);
			float mag = 2;
			float angle = ofRandom(0,360);
			float vx = r2x(angle, mag);
			float vy = r2y(angle, mag);
			vel = glm::vec2(vx, vy);
//			vel = glm::vec2(ofRandom(-2,2), ofRandom(-2,2));
		}
		void update() override {
			pos += vel * velFactor;
		}
		void draw() override {
			if (raio) {
				ofDrawCircle(pos.x, pos.y, raio);
			}
		}
	};

	vector <ponto> pontos;
	ofVboMesh mesh;

	void build() override {
		pontos.clear();
		for (int i = 0; i < uiC->pInt["numero"]; i++) {
			pontos.emplace_back(randomPos(), i);
		}
	}

	void setup() override {
		mesh.setMode(OF_PRIMITIVE_LINES);
		setupRectBounds(100);
		build();
	}

	void update() override {
		for (auto & p : pontos) {
			p.raio = uiC->pEasy["raio"] + p.qual * uiC->pEasy["raioRandom"];
			p.velFactor = uiC->pEasy["vel"] + p.qual * uiC->pEasy["velRandom"];
			p.update();
			p.pos = rectificate(p.pos);
		}
		
		if (uiC->pBool["mesh"]) {
			updateMesh();
		}
	}
	
	void drawLines() {
		float distance = uiC->pEasy["distance"];
		ofColor cor;
		for (auto & p : pontos) {
			if (uiC->pBool["randomAlpha"]) {
				p.indexQual = p.index * uiC->pFloat["noiseQual"];
			}
//            cout << p.pos << endl;
			cor = getCor(p.qual * uiC->pFloat["colorRange"]);
			for (auto & pp : pontos) {
				if (p.index != pp.index && p.pos.x <= pp.pos.x ) { //&& p.pos.y >= pp.pos.y
					float d1 = (pp.pos.x - p.pos.x);
					if (d1 < distance) {
						float d2 = ABS(pp.pos.y - p.pos.y);
						if (d2 < distance) {
							
							if (uiC->pBool["useAlpha"]) {
								float d = glm::distance(pp.pos, p.pos);
								cor.a = ofClamp(ofMap(d, 0, distance, uiC->pFloat["mapMaxAlpha"], 0), 0, 255);
								if (uiC->pBool["randomAlpha"]) {
//                                    cor.a = ofClamp(ofRandom(-355,cor.a), 0, 255);
									
									float n = ofNoise(p.indexQual + pp.indexQual, ofGetElapsedTimef() * uiC->pFloat["noiseTempo"]);
									float n2 = ofMap(n, 0, 1, -355, cor.a);
									cor.a = ofClamp(n2, 0, 255);
								}
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
				ofSetColor(cor);
			}
			// ofSetColor(getCor(0));
			p.draw();
		}
	}

	void drawMesh() {
		updateMesh();
		mesh.draw();
	}
	
	void updateMesh() {
		mesh.clear();
		float distance = uiC->pEasy["distance"];
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
								float d = glm::distance(pp.pos, p.pos);
								cor.a = ofClamp(ofMap(d, 0, distance, 512, 0), 0, 255);
							}
							if (cor.a > 0) {
//                                ofSetColor(cor);
								mesh.addColor(cor);
								mesh.addVertex(glm::vec3(p.pos.x, p.pos.y, 0));
								mesh.addColor(cor);
								mesh.addVertex(glm::vec3(pp.pos.x, pp.pos.y, 0));

//                                ofDrawLine(p.pos, pp.pos);
							}
						}
					}
				}
			}
			
//            if (uiC->pBool["recoverAlpha"]) {
//                cor.a = 255;
//                ofSetColor(cor);
//            }
//            // ofSetColor(getCor(0));
//            p.draw();
		}
//        mesh.draw();
	}
	
	
	void draw() override {
		ofNoFill();
		ofDrawRectangle(rectBounds);
		ofFill();
		if (uiC->pBool["mesh"]) {
//            lock();
			mesh.draw();
//            unlock();
		} else {
			drawLines();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "margem") {
			setupRectBounds(*e.i);
		}
		if (e.name == "numero" || e.name == "seed") {
			build();
		}
	}
};


struct sceneGears : public sceneObjetos {
public:
	using sceneObjetos::sceneObjetos;

	struct gear : public objeto {
		public:
		int ndentes[6] = { 24, 30, 36, 36, 40, 40 }; //,60
		int d;

		void setup() override {
			objeto::setup();
			// int dentes = 20;
//			float dentes = ndentes[int(ofRandom(0,6))];
			float dentes = ndentes[int(qual * 6)];

			
			d = dentes;

			float fatia = 360.0 / dentes * 2.0;
			float dentinho = fatia / 6.0;
			float mag1 = 0;
			float mag2 = ofRandom(raio*.1,raio*.3);
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
		
		gear(glm::vec2 p, settings * s) : objeto(p, s) {
			qual = ofRandom(0,1);
			setup();
		}
		
		gear(glm::vec2 p, settings * s, float q) : objeto(p, s, q) {
			setup();
		}
		
//        void draw() override {
//            ofPushMatrix();
//            ofTranslate(pos);
//            ofDrawBitmapString(ofToString(d), 0, 0);
//            ofRotateDeg(angulo);
//            for (auto & p : polys) {
//                p.draw();
//            }
//            ofPopMatrix();
//        }
	};
	

	void build() override {
		cout << "GEAR BUILD" << endl;
		if (uiC->pBool["seed"]) {
			ofSeedRandom(uiC->pInt["seed"]);
		}
		objetos.clear();
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			float qual = a/(float)uiC->pInt["numero"];
			objetos.emplace_back(new gear(randomPos(), &set, qual));
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
			ofSetColor(getCor(o->qual * uiC->pFloat["colorRange"]));
			o->draw();
		}
	}

	// void uiEvents(ofxMicroUI::element & e) override {
	// 	if (e.name == "numero") {
	// 		build();
	// 	}
		
	// 	if (e.name == "vel") {
	// 		set.velFactor = *e.f;
	// 	}
	// 	else if (e.name == "velRot") {
	// 		set.velRotFactor = *e.f;
	// 	}
	// }
};
