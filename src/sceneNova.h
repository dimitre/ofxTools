
static void calcNormals( ofMesh & mesh, bool bNormalize ){
//	cout << "calcNormals2" << endl;
//    for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(ofPoint(0,0,0));
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
//        ofVec3f e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
//        ofVec3f e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
//        ofVec3f no = e2.cross( e1 );
		
		glm::vec3 e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
		glm::vec3 e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
		glm::vec3 no = glm::cross(e2, e1);

        // depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped.
        mesh.getNormals()[ia] = no;
        mesh.getNormals()[ib] = no;
        mesh.getNormals()[ic] = no;
    }

	if (bNormalize) {
		for (auto & n : mesh.getNormals()) {
			n = glm::normalize(n);
		}
	}
}


static void drawMeshStatic(ofMesh * m, ofxMicroUI * ui) {
	if (ui->pString["draw"] == "wire") {
		m->drawWireframe();
	}
	if (ui->pString["draw"] == "faces") {
		m->drawFaces();
	}
	if (ui->pString["draw"] == "points") {
		//cout << "aqui " << endl;
		glDisable(GL_POINT_SMOOTH);
		glPointSize(ui->pEasy["pointSize"]);
//		glDisable(GL_POINT_SMOOTH);
		m->draw(OF_MESH_POINTS);
	}
}


struct sceneDmtr {
public:
	bool isSetup = false;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * ui = NULL;
	ofFbo * fbo = NULL;

	sceneDmtr() { }

	sceneDmtr(ofxMicroUI * _uiC) : uiC(_uiC) {
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}

	sceneDmtr(ofxMicroUI * _uiC, ofxMicroUI * _ui) : uiC(_uiC), ui(_ui) {
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}

	sceneDmtr(ofxMicroUI * _uiC, ofxMicroUI * _ui, ofFbo * _fbo) : uiC(_uiC), ui(_ui), fbo(_fbo) {
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}

	virtual void checkSetup() {
		if (!isSetup) {
			setup();
			isSetup = true;
		}
	}

	virtual void uiEvents(ofxMicroUI::element & e) {
	}
	
	virtual void setup() {
		isSetup = true;
	}
	
	virtual void draw() {
		if (!isSetup) {
			setup();
		}
	}
};


struct sceneOcean : public sceneDmtr {
public:
	
	ofMesh mesh;
	int width = 110;
	float multiplicador = 0.4;
	
	using sceneDmtr::sceneDmtr;
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "normals") {
			if (*e.b) {
				for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(glm::vec3(0,0,0));
			} else {
				mesh.clearNormals();
			}
		}
	}
	
	void setup() override {
		for (int x=0; x<width; x++) {
			for (int z=0; z<width; z++) {
				float y = 0;
				glm::vec3 pos = glm::vec3(x*multiplicador - width/2*multiplicador, y, z*multiplicador - width/2*multiplicador);
				mesh.addVertex(pos);
			}
		}

		for (int x=0; x<width-1; x++) {
			for (int z=0; z<width-1; z++) {
				mesh.addIndex(x+z*width);				// 0
				mesh.addIndex((x+1)+z*width);			// 1
				mesh.addIndex(x+(z+1)*width);			// 10
				mesh.addIndex((x+1)+z*width);			// 1
				mesh.addIndex((x+1)+(z+1)*width);		// 11
				mesh.addIndex(x+(z+1)*width);			// 10
			}
		}
		
//		for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(glm::vec3(0,0,0));
	}
	
	void draw() override {
		checkSetup();
		
		float updown = 0;
		float noiseMult1 = uiC->pEasy["noiseMult1"] + uiC->pEasy["noiseMult1Audio"] * updown;
		float noiseMult2 = uiC->pEasy["noiseMult2"] + uiC->pEasy["noiseMult2Audio"] * updown;
		
		float multY = uiC->pEasy["multY"] + uiC->pEasy["multYAudio"] * updown;
		float multY2 = uiC->pEasy["multY2"] + uiC->pEasy["multY2Audio"] * updown;
		
		float easy1 = ofGetElapsedTimef() * uiC->pEasy["multiVel1"];
		float easy2 = ofGetElapsedTimef() * uiC->pEasy["multiVel2"];
		float offXTime = ofGetElapsedTimef() * uiC->pFloat["offXTime"];
		float offYTime = ofGetElapsedTimef() * uiC->pFloat["offYTime"];

		//ofSetColor(getCor(0));
		//cout << multY << endl;
		//cout << width << endl;
		for (int y=0; y<width; y++) {
			for (int x=0; x<width; x++) {
				int index = x + y*width;
				glm::vec3 tmpVec = mesh.getVertex(index);
				
				tmpVec.y = (ofNoise(x * noiseMult1 + offXTime, y * noiseMult1 + offYTime, easy1)-.5)
				* multiplicador * multY;
				
				tmpVec.y += (ofNoise(x * noiseMult2 + offXTime, y * noiseMult2 + offYTime, easy2)-.5)
				* multiplicador * multY2;
				
				if (uiC->pBool["esferas"]) {
					//objeto3d(tmpVec, uiC->pFloat["raio"]);
				} else {
					mesh.setVertex(index, tmpVec);
				}
				//mesh.setColor(index, getCor((x + y*width)/(float)total));
			}
		}
		
		if (uiC->pBool["normals"]) {
			calcNormals(mesh, true);
		}

		drawMeshStatic(&mesh, ui);
	}
};

// oceano = oceanScene(uiC)


struct sceneWorms : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	struct worm {
	public:
		#define NWORMS 40

		glm::vec2 pos;
		ofxMicroUI * ui = NULL;
//		glm::vec2 * _bounds = NULL;
		ofRectangle & _rectBounds;
		float angulo;
		float mag = 2.0;
		glm::vec2 corpo[NWORMS];
		ofColor cores[3] = {
			ofColor::fromHex(0xff0000),
			ofColor::fromHex(0xff00ff),
			ofColor::fromHex(0x00ffff)
		};
		float rand;
		float lineW;
		ofColor cor;
		int cursor = 0;
		ofPolyline poly;
		float maxAngle = 15;

		worm(ofRectangle & _r, ofxMicroUI * _uiC) : _rectBounds(_r), ui(_uiC) {
//			pos = glm::vec2(ofRandom(0,1000), ofRandom(0,500));
			pos = glm::vec2(ofRandom(_rectBounds.x, _rectBounds.width),
							ofRandom(_rectBounds.y, _rectBounds.height));
			angulo = ofRandom(0,360);
			lineW = ofRandom(1,80);
			mag = ofRandom(1,9);
			cor = cores[int(ofRandom(0,3))];
			cor = ofColor::fromHsb(ofRandom(0,100), 255, 255);
			rand = ofRandom(0,1);
			maxAngle = ofRandom(5,15);
		}

		void update() {
			maxAngle += ofRandom(-1,1);
			if (maxAngle > 20) {
				maxAngle = 20;
			}
			if (maxAngle < 0) {
				maxAngle = 1;
			}

			angulo += ofRandom(-maxAngle,maxAngle);
			pos += p2c(glm::vec2(angulo, mag));
			
			if (!(_rectBounds.inside(pos))) {
				angulo += 180;
			}
			corpo[cursor] = pos;
			cursor ++;
			if (cursor >= (NWORMS)) {
				cursor = 0;
			}
		}
		
		void draw() {
//			ofDrawRectangle(pos, 100, 100);
			poly.clear();
			for (int a=0; a<NWORMS; a+=4) {
				int index = (a + cursor) % NWORMS;
				poly.addVertex(ofPoint(corpo[index]));
			}
			ofSetLineWidth(lineW);
			
			if (ui != NULL) {
				float h = ui->pEasy["hueStart"] + rand * ui->pEasy["hueStep"];
				ofSetColor(ofColor::fromHsb(h, 255, 255));
				for (int a=0; a<(60-lineW)/20; a++) {
					ofPushMatrix();
					ofTranslate(a*lineW*2, a*lineW*2);
					poly.draw();
					ofPopMatrix();
				}
			}
		}
	};

	vector <worm> worms;
	int margem = 100;
	ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);

	void setup() override {
		for (int a=0; a<40; a++) {
			// worms.emplace_back()
			worms.push_back(worm(boundsRect, uiC));
		}
	}

	void draw() override {
		checkSetup();

		for (auto & w : worms) {
			w.update();
			w.draw();
		}
	}
};


vector <sceneDmtr *> scenes;
map <string, sceneDmtr*> scenesMap;

bool scenesNovaIsSetup = false;

void setupScenesNova() {
//	oceanScene oceano = oceanScene(uiC);
	scenes.push_back(new sceneOcean(uiC, ui, fbo));
	scenesMap["ocean"] = scenes.back();

	scenes.push_back(new sceneWorms(uiC, ui, fbo));
	scenesMap["minhoca"] = scenes.back();
}


void drawScenesNova() {
	if (!scenesNovaIsSetup) {
		scenesNovaIsSetup = true;
		setupScenesNova();
	}
	
	if ( scenesMap.find(scene) != scenesMap.end() ) {
		scenesMap[scene]->draw();
	}
}
