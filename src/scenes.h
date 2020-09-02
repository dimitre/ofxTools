static ofColor getColor(float n, ofxMicroUI * ui) {
	return ui->pColorEasy["color"];
//		return ofColor(255);
}


static void calcNormals( ofMesh & mesh, bool bNormalize ){
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
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
	else if (ui->pString["draw"] == "faces") {
		m->drawFaces();
	}
	else if (ui->pString["draw"] == "points") {
		//cout << "aqui " << endl;
		glDisable(GL_POINT_SMOOTH);
		glPointSize(ui->pEasy["pointSize"]);
		m->draw(OF_MESH_POINTS);
	}
}


struct sceneDmtr {
public:
	bool isSetup = false;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * ui = NULL;
	ofFbo * fbo = NULL;
	float updown = 0.0;
	
	map <string, float> incrementadorTemporal;
	float incrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
		return incrementadorTemporal[uniqueId];
	}
	
	glm::vec2 middle;
	
	ofColor getCor(float n) {
		return ui->pColorEasy["color"];
//		return ofColor(255);
	}

	sceneDmtr() { }

	sceneDmtr(ofxMicroUI * _uiC, ofxMicroUI * _ui, ofFbo * _fbo) : uiC(_uiC), ui(_ui), fbo(_fbo) {
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
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
		checkSetup();
	}
};




struct sceneOcean : public sceneDmtr {
public:
	
	ofMesh mesh;
	int width = 110;
	float multiplicador = 0.4;
	
	using sceneDmtr::sceneDmtr;
	
	void addNormals() {
		for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(glm::vec3(0,0,0));
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "normals") {
			if (*e.b) {
				addNormals();
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
		float offXTime = ofGetElapsedTimef() * uiC->pEasy["offXTime"];
		float offYTime = ofGetElapsedTimef() * uiC->pEasy["offYTime"];

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
			if (!mesh.getNormals().size()) {
				addNormals();
			}
			calcNormals(mesh, true);
		}

		drawMeshStatic(&mesh, ui);
	}
};




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
				if (ui->pBool["color"]) {
					ofSetColor(ofColor::fromHsb(h, 255, 255));
				} else {
				}
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

		ofSetColor(getColor(0, ui));

		for (auto & w : worms) {
			w.update();
			w.draw();
		}
	}
};




struct sceneGridbox : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		ofSetColor(255);
		ofSetLineWidth(uiC->pEasy["linewidth"]);
		int numero = 0;
		float aresta = uiC->pEasy["aresta"];
		float limite = uiC->pInt["nx"] * aresta * .5;
		
		float w = uiC->pEasy["w"] * aresta;
		float h = uiC->pEasy["h"] * aresta;
		float d = uiC->pEasy["d"] * aresta;
		
		if (ui->pString["draw"] == "wire") {
			ofNoFill();
		} else {
			ofFill();
		}
		
		for (int a=0; a<uiC->pInt["nx"]; a++) {
			for (int b=0; b<uiC->pInt["ny"]; b++) {
				float x = ofMap(a, 0, uiC->pInt["nx"], -limite, limite);
				float y = ofMap(b, 0, uiC->pInt["ny"], -limite, limite);
				if (uiC->pBool["color"]) {
					float hue = fmod(numero*uiC->pEasy["hueMult"] + uiC->pEasy["hue"], 255);
					ofSetColor(ofColor::fromHsb(hue, uiC->pEasy["sat"], 255));
				}
				ofDrawBox(x, 0, y, w, h, d);
				numero ++;
			}
		}
	}
};




struct sceneSolidos : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofBoxPrimitive box;
	ofCylinderPrimitive cylinder;
	ofConePrimitive cone;
	ofSpherePrimitive sphere;
	ofIcoSpherePrimitive icoSphere;
	
	struct objeto {
	public:
		float raio;
		float rand;
		ofPoint pos;
		int corIndice;
		objeto() {}
	};

	vector <objeto> objetos;
	
	void setup() override {
		objetos.reserve (500);
		for (int a=0; a<500; a++) {
			objetos.emplace_back();
			objetos.back().raio = ofRandom(.1, 5);
			objetos.back().rand = ofRandom(0,1);
			objetos.back().pos = ofPoint(
										 ofRandom(-40,40),
										 ofRandom(-40,40),
										 ofRandom(-40,40)
										 );
			int corIndice = 0;
		}
//		for (auto & o : objetos) {
//		}
	}
	
	void draw() override {
		checkSetup();

		int numero = uiC->pInt["numero"] + updown * (float)uiC->pInt["numeroAudio"];
		for (int a=0; a<numero; a++ ) {
//			cout << a << endl;
			of3dPrimitive * objeto;
			objeto = &box;
			if (uiC->pString["objeto"] == "box")
				objeto = &box;
			else if (uiC->pString["objeto"] == "sphere")
				objeto = &sphere;
			else if (uiC->pString["objeto"] == "cylinder")
				objeto = &cylinder;
			else if (uiC->pString["objeto"] == "cone")
				objeto = &cone;
			
			
			float raioRandom = ofNoise(a/10.0f) * uiC->pFloat["raioRandom"];
			float raio = (uiC->pEasy["raio"] + uiC->pFloat["raioAudio"] * updown + raioRandom) * objetos[a].rand;
			glm::vec3 posicao = objetos[a].pos * (uiC->pEasy["fatorDistancia"] + uiC->pFloat["fatorDistanciaAudio"] * updown);
			objeto->setScale(raio);
			objeto->setPosition(posicao);
			
			if (ui->pString["draw"] == "wire") {
				objeto->drawWireframe();
			}
			else if (ui->pString["draw"] == "faces") {
				objeto->drawFaces();
			}
			else if (ui->pString["draw"] == "points") {
				objeto->getMesh().draw(OF_MESH_POINTS);
			}
//			if (ui->pBool["point"]) {
//				objeto->getMesh().draw(OF_MESH_POINTS);
//			}
		}
	}
};




struct scenePunchcard : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		float aresta = uiC->pFloat["aresta"];
		float margem = 10;
		ofSetColor(getCor(0));
		float r = aresta * uiC->pFloat["raio"];
		for (int x=0; x<100; x++) {
			for (int y=0; y<8; y++) {
				float noise = ofNoise((x), (y), ofGetElapsedTimef() * uiC->pFloat["velNoise"]);
				if (noise > uiC->pFloat["minNoise"]) {
					int raio = noise * r + 2;
					ofDrawCircle(x*aresta + margem, y*aresta + margem + uiC->pFloat["offY"], raio);
				}
			}
		}
	}
};



struct sceneMicroscenes : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
		
	struct microScene {
	public:
		ofFbo fbo;
		int s = 0;
		glm::vec2 pos;
		ofRectangle rect;
		ofxMicroUI * ui;
		
		ofImage images[15];
		
		microScene(ofxMicroUI * _ui) : ui(_ui) {
			int w = int(ofRandom(1,3)) * 100;
			int h = int(ofRandom(1,3)) * 100;
			// cout << "begin " << w << " " << h << endl;
			

			fbo.allocate(w, h, GL_RGBA);
			fbo.begin();
			ofClear(0,255);
			fbo.end();
			fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
			pos = glm::vec2(int(ofRandom(0,136)) * 55, int(ofRandom(0,20)) * 55);
			rect = ofRectangle(pos.x, pos.y, w, h);
			s = ofRandom(0,4);
			
			if (s == 3) {
				for (int a=0; a<15; a++) {
					images[a].allocate(w/4, h/4, OF_IMAGE_GRAYSCALE);
					images[a].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
					for (int x=0; x<images[a].getWidth(); x++) {
						for (int y=0; y<images[a].getHeight(); y++) {
							images[a].setColor(x, y, ofColor(ofRandom(0,1)>0.3 ? 0 : 255));
						}
					}
				}
			}
			// cout << pos << endl;
		}
		
		void draw() {
			float tempo = ofGetElapsedTimef();
			fbo.begin();
//			ofClear(127,255);
			ofClear(getColor(0, ui));
//			ofClear(getCor(0));
//			ofClear(0,255,0,255);
			if (s==0) {
				for (int a=0; a<1; a++) {
					// ofSetColor(255,0,0);
					ofSetColor(a * 127);
					ofDrawEllipse(
						  ofNoise(a, ofGetElapsedTimef()) * fbo.getWidth(),
						  ofNoise(a*1.2, ofGetElapsedTimef() * 1.2) * fbo.getHeight(),
						  80, 80
						  );
				}
			}
			else if (s==1) {
				ofSetColor(0);
				for (int a=0; a<20; a++) {
					ofDrawLine(0,0,ofRandom(0,1000),ofRandom(0,1000));
				}
			}

			else if (s == 2) {
				float y = ofNoise(tempo, 1) * fbo.getHeight();
				float y2 = ofNoise(tempo, 2) * fbo.getHeight();
				ofSetColor(255);
				ofSetLineWidth(60);
				ofDrawLine(0, y, fbo.getWidth(), y2);
			}
			
			else if (s == 3) {
				int index = ofRandom(0, 15);
				images[index].draw(0,0,fbo.getWidth(), fbo.getHeight());
			}

			fbo.end();
			// ofSetColor(ofRandom(0,255));

			// ofDrawRectangle(pos.x, pos.y, fbo.getWidth(), fbo.getHeight());
			
			ofSetColor(255);
			// ofPushMatrix();
			// ofTranslate(pos.x, pos.y);
//			fbo.draw(pos.x, pos.y);
			fbo.draw(rect.x, rect.y);
			// fbo.draw(0,0);
			// ofPopMatrix();
		}
	};
	

	vector <microScene> scenes;
	// TODO
	ofRectangle rect = ofRectangle(0,0,1280,720);

	void setup() override {
		for (int a=0; a<233; a++) {
			scenes.push_back(microScene(ui));
		}
	}
	
	void draw() override {
		checkSetup();
		
		for (auto & s : scenes) {
			// if (rect.inside(s.pos + s.fbo.getWidth())) {
			if (rect.intersects(s.rect)) {
				s.draw();
			}
			s.rect.x -= .5;
		}
	}
};



// CocaCymatics
struct sceneWave : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
		float noiseTempo = uiC->pEasy["noiseTempo"] * ofGetElapsedTimef();
		for (int a=-uiC->pInt["maxX"]; a<uiC->pInt["maxX"]; a++) {
			for (int b=-uiC->pInt["maxY"]; b<uiC->pInt["maxY"]; b++) {
				float x = a*uiC->pEasy["distX"] + b * uiC->pEasy["offX"];
				float y = b * uiC->pEasy["distY"] +
				ofNoise(noiseTempo, a/uiC->pEasy["divNoiseY"],b/uiC->pEasy["divNoiseY"])*uiC->pEasy["noiseY"]
				-
				ofNoise(noiseTempo, a/uiC->pEasy["divNoiseY2"],b/uiC->pEasy["divNoiseY2"])*uiC->pEasy["noiseY2"];


				float raio = uiC->pEasy["raio"] +
				ofNoise(noiseTempo, a/uiC->pEasy["divRaioNoise"],b/uiC->pEasy["divRaioNoise"])*uiC->pEasy["raioNoise"];
				ofDrawCircle(x, y, raio);
			}
		}
	}
};











struct sceneBola2 : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	struct objeto {
	public:
		ofVec2f pos;
		float raio;
		float angle;
		float mag;
		float index;

		objeto(ofVec2f p, float r, float a, float m, float i) : pos(p), raio(r), angle(a), mag(m), index(i) {}

		float r2x (float a, float m) { return m * cos(M_PI * a / 180.0f); }
		float r2y (float a, float m) { return m * sin(M_PI * a / 180.0f); }


		void draw(float tempo, float mult, float deforma, float deformaAngulo) {
			float a = sin(tempo)*180 * sin((angle+.1)*mult);
			float offx = r2x(angle + a * deformaAngulo,raio*deforma);
			float offy = r2y(angle + a * deformaAngulo,raio*deforma);
			ofDrawCircle(pos.x + offx, pos.y + offy, raio);
		}

		void draw2(float tempo, float mult, float deforma, float deformaAngulo) {
			float a = sin(tempo)*180 * sin((angle+.1)*mult);
			float offx = r2x(angle + a * deformaAngulo,raio*deforma);
			float offy = r2y(angle + a * deformaAngulo,raio*deforma);
			ofPushMatrix();
			ofTranslate(pos.x + offx, pos.y + offy);
			ofRotateDeg(a);
			ofDrawRectangle(0,0, raio*.75, raio*3);
			ofPopMatrix();
		}
	};
	vector <objeto> objetos;

	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		ofTranslate(middle);

		for (auto & o : objetos) {
			if (uiC->pBool["rects"]) {
			o.draw2(ofGetFrameNum()/uiC->pEasy["tempoDiv"], uiC->pEasy["mult"], uiC->pEasy["deforma"], uiC->pEasy["deformaAngulo"]);
			} else {
				o.draw(ofGetFrameNum()/uiC->pEasy["tempoDiv"], uiC->pEasy["mult"], uiC->pEasy["deforma"], uiC->pEasy["deformaAngulo"]);
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		objetos.clear();
		int i = 0;
		float raio = uiC->pFloat["radius"];
		float spacing = uiC->pFloat["spacing"];
		float mag = -uiC->pFloat["iniRadius"] * raio;
		float reduce = uiC->pFloat["reduce"];
		for (int b =1; b<uiC->pInt["limite"]; b++) {
			mag += raio*2 + spacing;
			raio *= reduce;
			spacing *= reduce;
			ofVec2f p1 = ofVec2f(r2x(0,mag), r2y(0,mag));
			ofVec2f p2 = ofVec2f(r2x(0,mag), r2y(0,mag));
			float d = 0;
			float testAngle = 0;
			while (d < (raio*2+spacing)) {
				testAngle+=.1;
				p2 = ofVec2f(r2x(testAngle,mag), r2y(testAngle,mag));
				d = p1.distance(p2);
			}
			int divisoes = (360.0 / testAngle);
			for (int a=0; a<divisoes; a++) {
				float ang = 360.0/(float)divisoes * a + 90;
				float x = r2x(ang, mag);
				float y = r2y(ang, mag);
				objetos.push_back(objeto(ofVec2f(x,y),raio, ang, mag,i));
				i++;
			}
		}
	}
};






struct sceneOndas : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
		ofNoFill();
		for (int b=0; b<2; b++) {
			ofPushMatrix();
			float transX = uiC->pFloat["centroX" + ofToString(b)] + updown*
			uiC->pFloat["centroX" + ofToString(b) + "Audio"];

			float transY = uiC->pFloat["centroY" + ofToString(b)] + updown*
			uiC->pFloat["centroY" + ofToString(b) + "Audio"];

			ofTranslate(transX, transY);
			float mult = uiC->pFloat["mult"] + updown * uiC->pFloat["multAudio"];
			int concentricos = uiC->pFloat["concentricos"] + updown * uiC->pFloat["concentricosAudio"];
			for (int a=0; a<concentricos; a++) {
				float e = uiC->pFloat["espessura"] + (sin(a * mult)*uiC->pFloat["espessuraOnda"]/2 + uiC->pFloat["espessuraOnda"]);
				if (e>0) {
					int raio = a * uiC->pFloat["distancia"];
					ofSetLineWidth(e);
					ofDrawCircle(0,0,raio);
				}
			}
			ofPopMatrix();
		}

		float raio = uiC->pEasy["raio"] ;
		float dist = uiC->pEasy["dist"] ;
		float variAngle = uiC->pEasy["variAngle"];
		float senoMag = uiC->pEasy["senoMag"] + updown * uiC->pFloat["senoMagAudio"];
		float multMag = uiC->pEasy["multMag"] + updown * uiC->pFloat["multMagAudio"];;
		float tempo = -uiC->pEasy["tempo"] * ofGetElapsedTimef();


		for (int a=-uiC->pInt["maxX"]; a<uiC->pInt["maxX"]; a++) {
			for (int b=-uiC->pInt["maxY"]; b<uiC->pInt["maxY"]; b++) {

//		for (int a=-uiC->pInt["max"]; a<uiC->pInt["max"]; a++) {
//			for (int b=-uiC->pInt["max"]; b<uiC->pInt["max"]; b++) {
				float x = a*dist ;
				float y = b*dist ;
				float x2 = a*dist + uiC->pFloat["centroX2"];
				float y2 = b*dist + uiC->pFloat["centroY2"];;
				float a = c2a(x + uiC->pFloat["centroX"],y + uiC->pFloat["centroY"]);
				float m = c2m(x + uiC->pFloat["centroX"],y + uiC->pFloat["centroY"]);

				float a2 = c2a(x2 + uiC->pFloat["centroX2"],y2+ uiC->pFloat["centroY2"]);
				float m2 = c2m(x2 + uiC->pFloat["centroX2"],y2+ uiC->pFloat["centroY2"]);
				//				float aa = sin(a * senoAngulo) * multAngulo;
				float mm = sin((m) * senoMag + tempo) * multMag;
				float mm2 = sin((m2) * senoMag + tempo) * multMag;
				//raio += ABS(a * uiC->pEasy["magRaio"]);
				float xx = x + r2x(a+a2, mm + mm2) * variAngle;
				float yy = y + r2y(a+a2, mm + mm2) * variAngle;

				float finalmag = c2m(xx,yy);
				if (finalmag < ui->pFloat["raioCirculo"]) {
					ofDrawCircle(xx, yy, raio);
				}
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneLatquad2 : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
		ofSetColor(getCor(0));
		float raio = uiC->pEasy["raio"] + updown*uiC->pEasy["raioAudio"];
		float dist = uiC->pEasy["dist"] ;
		float variAngle = uiC->pEasy["variAngle"];
		float senoMag = uiC->pEasy["senoMag"] + updown * uiC->pFloat["senoMagAudio"];
		float multMag = uiC->pEasy["multMag"] + updown * uiC->pFloat["multMagAudio"];;
		float tempo = (-uiC->pEasy["tempo"]+updown*uiC->pFloat["tempoAudio"]) * ofGetElapsedTimef();

		for (int a=-uiC->pInt["max"]; a<uiC->pInt["max"]; a++) {
			for (int b=-uiC->pInt["max"]; b<uiC->pInt["max"]; b++) {

//		for (int a=-uiC->pInt["maxX"]; a<uiC->pInt["maxX"]; a++) {
//			for (int b=-uiC->pInt["maxY"]; b<uiC->pInt["maxY"]; b++) {
				float x   = a*dist;
				float y   = b*dist;
				float x1  = x + uiC->pFloat["centroX"]  + updown* uiC->pFloat["centroXAudio"];
				float y1  = y + uiC->pFloat["centroY"]  + updown* uiC->pFloat["centroYAudio"];
				float x2  = x + uiC->pFloat["centroX2"] + updown* uiC->pFloat["centroX2Audio"];
				float y2  = y + uiC->pFloat["centroY2"] + updown* uiC->pFloat["centroY2Audio"];

				float a = c2a(x1 ,y1);
				float m = c2m(x1 ,y1);

				float a2 = c2a(x2, y2);
				float m2 = c2m(x2, y2);
				//				float aa = sin(a * senoAngulo) * multAngulo;
				float mm = sin(m * senoMag + tempo) * multMag;
				float mm2 = sin(m2 * senoMag + tempo) * multMag;
				//raio += ABS(a * uiC->pEasy["magRaio"]);


				float finalAngulo = a - a2; // a+a2;
				float finalMag = mm - mm2;
				if (uiC->pString["modoAngulo"] == "mult") {
					finalAngulo = a * a2 / 10.0f;
				}
				else if (uiC->pString["modoAngulo"] == "div") {
					finalAngulo = a / a2;
				}
				else if (uiC->pString["modoAngulo"] == "mod") {
					finalAngulo = int(a) ^ int(a2);
				}



				if (uiC->pString["modoMag"] == "mult") {
					finalMag = mm * mm2 / 10.0f;
				}
				else if (uiC->pString["modoMag"] == "div") {
					finalMag = mm / mm2;
				}

				else if (uiC->pString["modoMag"] == "mod") {
					finalMag = int(mm) ^ int(mm2);
				}

				float xx = x + r2x(finalAngulo, finalMag) * variAngle;
				float yy = y + r2y(finalAngulo, finalMag) * variAngle;

				float dist = c2a( r2x(finalAngulo, finalMag) * variAngle - x,  r2y(finalAngulo, finalMag) * variAngle - y);
				if (uiC->pBool["colorLerp"]) {
					ofColor cor = ofColor(235,0,0);
					ofColor cor2 = ofColor(30,0,30);
					ofSetColor(cor.getLerped(cor2, dist/100.0));
				}

				//raio += dist/30.0f;

//				float finalmag = c2m(xx,yy);
//				cout << finalMag << endl;
//				if (finalmag < ui->pFloat["raioCirculo"]) {
					ofDrawCircle(xx, yy, raio);
//				}
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};





struct sceneMoire : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
//		ofSetLineWidth(uiC->pEasy["lineWidth"]);
		int divisoes = uiC->pEasy["divisoes"];
		int raio = uiC->pEasy["radius"];

		ofPushMatrix();
		float s = sin(ofGetFrameNum() / 100.0);
		float ss = sin(ofGetFrameNum() / 75.0);
		for (int b=0; b<2; b++) {
			if (b==1) {
				ofTranslate(
							uiC->pEasy["offX"] + uiC->pEasy["offXAuto"] * s,
							uiC->pEasy["offY"] + uiC->pEasy["offYAuto"] * ss);
			}
			for (int a=0; a<divisoes; a++) {
				float angulo = 360.0/divisoes * a;
				float x = r2x(angulo, raio);
				float y = r2y(angulo, raio);
				ofDrawLine(0,0,x,y);
			}
		}
		ofPopMatrix();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneLatquad3 : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofMesh mesh;
	ofImage spriteImage;

	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
		float raio = uiC->pEasy["raio"] + updown*uiC->pEasy["raioAudio"];
		glPointSize(raio);
		//glPointSize(10);

		if (!ui->pBool["useCairo"]) {
			ofEnablePointSprites();
			spriteImage.getTexture().bind();
		}

		mesh.clear();

		float dist = uiC->pEasy["dist"] ;
		float variAngle = uiC->pEasy["variAngle"];
		float senoMag = uiC->pEasy["senoMag"] + updown * uiC->pFloat["senoMagAudio"];
		float multMag = uiC->pEasy["multMag"] + updown * uiC->pFloat["multMagAudio"];;
		float tempo = (-uiC->pEasy["tempo"]+updown*uiC->pFloat["tempoAudio"]) * ofGetElapsedTimef();

		int numero = 0;

		for (int a=-uiC->pInt["max"]; a<uiC->pInt["max"]; a++) {
			for (int b=-uiC->pInt["max"]; b<uiC->pInt["max"]; b++) {
				float x   = a*dist;
				float y   = b*dist;
				float x1  = x + uiC->pFloat["centroX"]  + updown* uiC->pFloat["centroXAudio"];
				float y1  = y + uiC->pFloat["centroY"]  + updown* uiC->pFloat["centroYAudio"];
				float x2  = x + uiC->pFloat["centroX2"] + updown* uiC->pFloat["centroX2Audio"];
				float y2  = y + uiC->pFloat["centroY2"] + updown* uiC->pFloat["centroY2Audio"];

				float a = c2a(x1 ,y1);
				float m = c2m(x1 ,y1);

				float a2 = c2a(x2, y2);
				float m2 = c2m(x2, y2);

				float mm = sin(m * senoMag + tempo) * multMag;
				float mm2 = sin(m2 * senoMag + tempo) * multMag;
				//raio += ABS(a * uiC->pEasy["magRaio"]);


				float finalAngulo = a - a2; // a+a2;
				float finalMag = mm - mm2;
				if (uiC->pString["modoAngulo"] == "mult") {
					finalAngulo = a * a2 / 10.0f;
				}
				else if (uiC->pString["modoAngulo"] == "div") {
					finalAngulo = a / a2;
				}
				else if (uiC->pString["modoAngulo"] == "mod") {
					finalAngulo = int(a) ^ int(a2);
				}


				if (uiC->pString["modoMag"] == "mult") {
					finalMag = mm * mm2 / 10.0f;
				}
				else if (uiC->pString["modoMag"] == "div") {
					finalMag = mm / mm2;
				}

				else if (uiC->pString["modoMag"] == "mod") {
					finalMag = int(mm) ^ int(mm2);
				}

				float xx = x + r2x(finalAngulo, finalMag) * variAngle;
				float yy = y + r2y(finalAngulo, finalMag) * variAngle;

				float dist = c2a( r2x(finalAngulo, finalMag) * variAngle - x,  r2y(finalAngulo, finalMag) * variAngle - y);
				if (uiC->pBool["colorLerp"]) {
					ofColor cor = ofColor(235,0,0);
					ofColor cor2 = ofColor(30,0,30);
					ofSetColor(cor.getLerped(cor2, dist/100.0));
				}

				//raio += dist/30.0f;

//				float finalmag = c2m(xx,yy);
//				if (finalmag < ui->pFloat["raioCirculo"]) {

//					if (u.pBool["savingPDF"]) {
						ofDrawCircle(xx, yy, raio/2.0);
//					} else {
//						mesh.addVertex(ofVec3f(xx,yy,00));
//						mesh.getVertices()[numero] = ofVec3f(xx,yy,00);
//					}
					numero ++;
//				}

			}
		}
		//cout << mesh.getVertices().size() << endl;
		//mesh.drawPoints();
		mesh.drawVertices();

		if (!ui->pBool["useCairo"]) {
			spriteImage.getTexture().unbind();
			ofDisablePointSprites();
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};



struct sceneLatquad : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		ofTranslate(middle);
		float raio = uiC->pEasy["raio"] ;
		float dist = uiC->pEasy["dist"] ;
		float variAngle = uiC->pEasy["variAngle"];
		float senoMag = uiC->pEasy["senoMag"];
		float multMag = uiC->pEasy["multMag"];
		float tempo = -uiC->pEasy["tempo"] * ofGetElapsedTimef();
		for (int a=-uiC->pInt["maxX"]; a<uiC->pInt["maxX"]; a++) {
			for (int b=-uiC->pInt["maxY"]; b<uiC->pInt["maxY"]; b++) {
				float x = a*dist;
				float y = b*dist;
				float a = c2a(x,y);
				float m = c2m(x,y);
//				float aa = sin(a * senoAngulo) * multAngulo;
				float mm = sin(m * senoMag + tempo) * multMag;
				//raio += ABS(a * uiC->pEasy["magRaio"]);
				float xx = x + r2x(a, mm) * variAngle;
				float yy = y + r2y(a, mm) * variAngle;

				float finalmag = c2m(xx,yy);
//				if (finalmag < ui->pFloat["raioCirculo"]) {
					ofDrawCircle(xx, yy, raio);
//				}
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct scenePulsar : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofMesh meshEq;
	ofMesh mesh;

	int width = 70;
	float	multiplicador = 80.0;


	void setup() override {

		for (int y=0; y<width; y++) {
			for (int x=0; x<width; x++) {
				float z = ofNoise(x/10.0, y/10.0, ofGetFrameNum() ) * multiplicador * 12.0;
				ofVec3f pos (x*multiplicador - width/2*multiplicador, y*multiplicador - width/2*multiplicador, z);
				mesh.addVertex(pos);
				mesh.addColor(ofFloatColor(1,1,1));
				
				
				z = 0;
				pos = glm::vec3(x*multiplicador - width/2*multiplicador, y*multiplicador - width/2*multiplicador, z);
				meshEq.addVertex(pos);
				meshEq.addColor(ofFloatColor(1,1,1));
			}
		}
		
		
		for (int y=0; y<width-1; y++) {
			for (int x=0; x<width-1; x++) {
				mesh.addIndex(x+y*width);				// 0
				mesh.addIndex((x+1)+y*width);			// 1
				mesh.addIndex(x+(y+1)*width);			// 10
				
				mesh.addIndex((x+1)+y*width);			// 1
				mesh.addIndex((x+1)+(y+1)*width);		// 11
				mesh.addIndex(x+(y+1)*width);			// 10
				
				meshEq.addIndex(x+y*width);				// 0
				meshEq.addIndex((x+1)+y*width);			// 1
				meshEq.addIndex(x+(y+1)*width);			// 10
				
				meshEq.addIndex((x+1)+y*width);			// 1
				meshEq.addIndex((x+1)+(y+1)*width);		// 11
				meshEq.addIndex(x+(y+1)*width);			// 10
			}
		}
		
		
	}
	
	void draw() override {
		checkSetup();
		
		float multY = uiC->pEasy["multY"] + uiC->pEasy["multYAudio"] * updown;

		//if (ofGetFrameNum()%2 == 0)
		{
			//			for (int y=1; y<width; y++) {
			for (int y=width; y>0; y--) {
				for (int x=0; x<width; x++) {
					int index = x + y*width;
					int index2 = x + (y-1)*width;
					glm::vec3 tmpVec = meshEq.getVertex(index);
					glm::vec3 tmpVec2 = meshEq.getVertex(index2);
					
					tmpVec.z = tmpVec2.z;
					meshEq.setVertex(index, tmpVec);
				}
			}
			
			int y = 0;
			for (int x=0; x<width; x++) {
				int index = x + y*width;
				ofVec3f tmpVec = meshEq.getVertex(index);
				
				float z;
				if (!ui->pBool["pulsarEq"]) {
					z = ofNoise(ofGetFrameNum() * uiC->pEasy["frameDiv"], x * uiC->pEasy["xDiv"]) * multY * 30.0;
				}
				
				else {
					// xaxa todo, implementar pointer pro fft.
//					int i = x * fft.FFTanalyzer.nAverages / width;
//					z = fft.FFTanalyzer.peaks[i] * multY;
				}
				
				tmpVec.z = z;
				meshEq.setVertex(index, tmpVec);
			}
		}
		
		ofPushMatrix();
		if (ui->pBool["wire"])
			meshEq.drawWireframe();
		if (ui->pBool["faces"]) {
			meshEq.drawFaces();
		}
		if (ui->pBool["point"]) {
			meshEq.draw(OF_MESH_POINTS);
		}
		ofPopMatrix();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneBasic : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};






vector <sceneDmtr *> scenes;
map <string, sceneDmtr*> scenesMap;

bool scenesIsSetup = false;

#include "scenesAll.h"

void setupScenesNova() {
	setupScenesAll();

	scenes.push_back(new sceneOcean(uiC, ui, fbo));
	scenesMap["ocean"] = scenes.back();

	scenes.push_back(new sceneWorms(uiC, ui, fbo));
	scenesMap["minhoca"] = scenes.back();
	
	scenes.push_back(new sceneGridbox(uiC, ui, fbo));
	scenesMap["gridbox"] = scenes.back();
	
	scenes.push_back(new sceneSolidos(uiC, ui, fbo));
	scenesMap["solidos"] = scenes.back();

	scenes.push_back(new scenePunchcard(uiC, ui, fbo));
	scenesMap["punchcard"] = scenes.back();

	scenes.push_back(new sceneMicroscenes(uiC, ui, fbo));
	scenesMap["microScenes"] = scenes.back();

	scenes.push_back(new sceneWave(uiC, ui, fbo));
	scenesMap["wave"] = scenes.back();
	
	scenes.push_back(new sceneBola2(uiC, ui, fbo));
	scenesMap["bola2"] = scenes.back();

	scenes.push_back(new sceneOndas(uiC, ui, fbo));
	scenesMap["ondas"] = scenes.back();

	scenes.push_back(new sceneLatquad(uiC, ui, fbo));
	scenesMap["latquad"] = scenes.back();

	scenes.push_back(new sceneLatquad2(uiC, ui, fbo));
	scenesMap["latquad2"] = scenes.back();

	scenes.push_back(new sceneLatquad3(uiC, ui, fbo));
	scenesMap["latquad3"] = scenes.back();

	scenes.push_back(new sceneMoire(uiC, ui, fbo));
	scenesMap["moire"] = scenes.back();

	scenes.push_back(new scenePulsar(uiC, ui, fbo));
	scenesMap["pulsar"] = scenes.back();

}


void drawScenesNova() {
	if (!scenesIsSetup) {
		scenesIsSetup = true;
		setupScenesNova();
	}
	
	if ( scenesMap.find(scene) != scenesMap.end() ) {
		scenesMap[scene]->draw();
	}
}
