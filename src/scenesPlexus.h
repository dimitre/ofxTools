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
        
		ponto(glm::vec2 p, int i) : objeto(p), index(i) {
			qual = ofRandom(0,1);
			float mag = 2;
			float angle = ofRandom(0,360);
			float vx = r2x(angle, mag);
			float vy = r2y(angle, mag);
			vel = glm::vec2(vx, vy);
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
                                mesh.addColor(cor);
                                mesh.addVertex(glm::vec3(p.pos.x, p.pos.y, 0));
                                mesh.addColor(cor);
                                mesh.addVertex(glm::vec3(pp.pos.x, pp.pos.y, 0));
                            }
                        }
                    }
                }
            }
        }
    }
    
    
	void draw() override {
        ofNoFill();
        ofDrawRectangle(rectBounds);
        ofFill();
        if (uiC->pBool["mesh"]) {
            mesh.draw();
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

