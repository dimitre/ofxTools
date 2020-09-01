static ofColor getColor(float n, ofxMicroUI * ui) {
	return ui->pColor["color"];
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
	
	ofColor getCor(float n) {
		return ui->pColor["color"];
//		return ofColor(255);
	}

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
			if (!mesh.getNormals().size()) {
				addNormals();
			}
			calcNormals(mesh, true);
		}

		drawMeshStatic(&mesh, ui);
	}
};

//};

// comparar com a anterior e remover se for o caso.
struct sceneOcean0 : public sceneDmtr {
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
//			calcNormals(mesh, true);
		}

//		drawMeshStatic(&mesh, ui);
	}
};

// oceano = oceanScene(uiC)



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



struct sceneNovelo : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		ofPushMatrix();
		ofNoFill();
		float a = uiC->pEasy["contador"] / 100.0f + uiC->pEasy["contadorInicio"];
		float enrosco = uiC->pEasy["enrosco"] + uiC->pEasy["enroscoAudio"] * updown;
		float enrosco2 = uiC->pEasy["enrosco2"] + uiC->pEasy["enrosco2Audio"] * updown;

		float limite = uiC->pInt["limite"] + uiC->pInt["limiteAudio"] * updown;
		for (int y = 1; y < limite; y++)
		{
			ofSetColor(getCor(y/limite));
			//ofSetColor(255 - y/20, y%255, a);
			ofRotateYDeg (.02*10.0);
			ofRotateXDeg (sin(y/uiC->pEasy["desenrosco"]*a) * enrosco);
			ofRotateZDeg (sin(y/uiC->pEasy["desenrosco2"]*a) * enrosco2);
			ofTranslate (0,0, -a);
			
			ofDrawLine(y/2, 0, y/2, uiC->pEasy["comprimento"]);
		}
		ofPopMatrix();
	}
};





struct sceneRede : public sceneDmtr {
public:

	using sceneDmtr::sceneDmtr;

	struct rede {
	public:
		vector <glm::vec2> pontos;
		glm::vec2 pos, vel;
		float tempo, tempo2;
		int a;
		float qx, qy;
		int numero;
		int raio;
		ofRectangle & _rectBounds;
		
		rede(int aa, ofRectangle & _r) : a(aa), _rectBounds(_r) {
			tempo = ofRandom(0, .2);
			tempo2 = ofRandom(0, .2);
			float maxVel = 0.7;
			vel = glm::vec2(ofRandom(-maxVel, maxVel), ofRandom(-maxVel, maxVel));
//			pos = glm::vec2(ofRandom(0,w), ofRandom(0,h));
			pos = glm::vec2(ofRandom(0, _rectBounds.width), ofRandom(0, _rectBounds.height));
	//		float qx = a * uiC->pFloat["qx"];
	//		float qy = a * uiC->pFloat["qy"];
			numero = ofRandom(2,10);
			raio = numero * 10;
			for (int a=0; a<numero; a++) {
				float x = ofRandom(-30, 30);
				float y = ofRandom(-30, 30);
				pontos.emplace_back(x, y);
			}
		}
		
		int margem = 100;
		
		void update() {
			pos += vel;
			
			// atualizar tudo com margem no bounds. foda-se
			if (pos.x > (_rectBounds.width)) {
				pos.x = _rectBounds.x;
			}
			else if (pos.x < _rectBounds.x) {
				pos.x = _rectBounds.width;
			}

			if (pos.y > (_rectBounds.height)) {
				pos.y = _rectBounds.y;
			}
			else if (pos.y < _rectBounds.y) {
				pos.y = _rectBounds.height;
			}
			
			tempo = ofGetElapsedTimef() * .2;
			tempo2 = ofGetElapsedTimef() * .25;
		}

		
		void draw() {
			ofPushMatrix();
			ofTranslate(pos);
			
			int i=0;
			for (auto & p : pontos) {
				qx = i * 0.4 + a*.1;
				qy = i * 0.3 + a*.1;
				p.x = ofMap(ofNoise(qx, tempo), 0, 1, -raio, raio);
				p.y = ofMap(ofNoise(qy, tempo2), 0, 1, -raio, raio);
				i++;
			}

			for (auto & p : pontos) {
				for (auto & pp : pontos) {
					if (p != pp) {
						ofDrawLine(p, pp);
					}
				}
				
				//if (uiC->pBool["circles"])
				{
					ofDrawCircle(p, 5);
				}
			}
			ofPopMatrix();
		}
	};

	vector <rede> redes;
	
	
	int margem = 100;
	ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);
	
	void setup() override {
		int numero = 150;
		redes.reserve(numero);
		for (int a=0; a<numero; a++) {
//			redes.push_back(rede(a, boundsRect));
			redes.emplace_back(a, boundsRect);
		}
	}
	void draw() override {
		checkSetup();

		int numero = uiC->pInt["numero"];
		
		int index = 0;
		for (auto & r : redes) {
			float qual = index/(float)redes.size();
			index ++;
			ofSetColor(getCor(qual));
			r.update();
			r.draw();
		}
		
//		if (numero < redes.size()) {
//			redes.erase(redes.begin(), redes.begin() + numero);
//		}
//
//		if (numero > redes.size()) {
//			int dif = numero - redes.size();
//			int n = redes.size();
//			for (int a=0; a<dif; a++) {
//				redes.emplace_back(a + n);
//			}
//		}
	}
};



struct sceneLeparc : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void draw() override {
		checkSetup();
		
		if (uiC->pBool["on"])
		{
			ofSetLineWidth(ui->pFloat["espessura"]);
			ofPushMatrix();
			float x = fbo->getWidth() * .5 + fbo->getWidth() * (uiC->pEasy["x"] + ofNoise(incrementa("xNoiseTime")) * uiC->pEasy["xNoise"] - uiC->pEasy["xNoise"]*.5);
			float y = fbo->getHeight() * .5 + fbo->getHeight() * (uiC->pEasy["y"] + ofNoise(incrementa("yNoiseTime")) * uiC->pEasy["yNoise"] - uiC->pEasy["yNoise"]*.5);

	//		ofTranslate(fbo->getWidth()*.5 + uiC->pEasy["offX"], fbo->getHeight()*.5 + uiC->pEasy["offY"]);
			ofTranslate(x, y);

			float mag = uiC->pEasy["mag"];
			float nLinhas = uiC->pEasy["nLinhas"];
			float anguloTotal = incrementa("rotVel");
			float noiseAlphaTime = incrementa("noiseAlphaTime");
			float noiseAngleTime = incrementa("noiseAngleTime");
			float noiseAngle2Time = incrementa("noiseAngle2Time");
			float noiseCTime = incrementa("noiseCTime");
			
			for (int c=0; c<uiC->pInt["steps"]; c++) {
				float ang = c * uiC->pEasy["stepAngle"] + anguloTotal;
				
				ang += ofNoise(c, noiseCTime) * uiC->pEasy["noiseC"];
				for (int a=0; a<nLinhas; a++) {
					
					float qual = (float)a/(float)nLinhas;
					float noiseAlpha = ofNoise(c + noiseAlphaTime, qual * uiC->pEasy["noiseAlphaQual"]);
					
					float alpha = ofClamp((uiC->pEasy["alpha"] + noiseAlpha * uiC->pEasy["noiseAlpha"]), 0, 1);
					
					ofColor cor = getCor(qual);
					cor.a *= alpha;
					
					if (a == int(nLinhas)) {
						float nLinhasAlpha = 1.0;
						nLinhasAlpha = fmod(nLinhas, 1.0);
						cor.a *= nLinhasAlpha;
					}

					ofSetColor(cor);
					
					float noiseAngle = ofNoise(noiseAngleTime, qual* uiC->pEasy["noiseAngleQual"]) * uiC->pEasy["noiseAngle"] - uiC->pEasy["noiseAngle"]*.5;
					
					float noiseAngle2 = ofNoise(noiseAngle2Time, qual* uiC->pEasy["noiseAngle2Qual"]) * uiC->pEasy["noiseAngle2"] - uiC->pEasy["noiseAngle2"]*.5;

					float angulo1 = uiC->pEasy["angulo"] - uiC->pEasy["leque"]*.5;
					float anguloFim1 = uiC->pEasy["angulo"] + uiC->pEasy["leque"]*.5;
					float angulo2 = angulo1 + 180 + uiC->pEasy["desencontro"];
					float anguloFim2 = anguloFim1 + 180 + uiC->pEasy["desencontro"];

					float a1 = ofMap(a, 0, nLinhas, angulo1, anguloFim1) + ang + noiseAngle;
					float a2 = ofMap(a, 0, nLinhas, angulo2, anguloFim2) + ang + noiseAngle2;
					
					float x1 = r2x(a1, mag);
					float y1 = r2y(a1, mag);
					float x2 = r2x(a2, mag);
					float y2 = r2y(a2, mag);
					
					if (uiC->pBool["drawLine"]) {
						ofDrawLine(x1, y1, x2, y2);
					} else {
						float a3 = ofMap(a, 0, nLinhas, uiC->pEasy["a5"], uiC->pEasy["a6"]) + ang + noiseAngle;
						
						float x3 = r2x(a3, -mag*.2);
						float y3 = r2y(a3, -mag*.2);
						float x4 = r2x(a3, mag*.2);
						float y4 = r2y(a3, mag*.2);
						ofPolyline p;
						p.curveTo(x1, y1);
						p.curveTo(x3, y3);
						p.curveTo(x4, y4);
						p.curveTo(x2, y2);
						p.draw();
					}
				}
			}
			ofPopMatrix();
		}
	}
};




struct scene3d : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofSpherePrimitive sphere;

	void draw() override {
		checkSetup();
		
		float aresta = uiC->pEasy["aresta"];
		float max = uiC->pInt["max"] * 0.5;
		int index = 0;
		float noise = uiC->pEasy["noiseMult"] + updown*uiC->pEasy["noiseMultAudio"];
		for (float x=-max; x<=max; x++) {
			for (float y=-max; y<=max; y++) {
				for (float z=-max; z<=max; z++) {
					float n = ofNoise(index * noise, uiC->pEasy["seed"]);
					//float nn = ofNoise(index * uiC->pFloat["noiseMult"] * 1.5, uiC->pFloat["seed"]);
					if (n > uiC->pEasy["showTreshold"]) {
						float nn = ofMap(n, uiC->pEasy["showTreshold"], 1, 0, 1);
						ofPushMatrix();
						float raio = uiC->pEasy["raio"] + uiC->pEasy["raioNoise"] * nn;
						float res = uiC->pEasy["resolution"] + n * uiC->pEasy["resolutionNoise"] * nn;
//						sphere.set(raio, uiC->pInt["resolution"]);
						
						// XAXA TODO
						sphere.set(raio, res);
						ofMesh m = sphere.getMesh();
						float aa = aresta + uiC->pEasy["arestaNoise"] * nn;
						ofTranslate(x*aa, y*aa, z*aa);
						//sphere.drawWireframe();
						drawMeshStatic(&m, ui);
						ofPopMatrix();
					}
					index++;
				}
			}
		}
	}
};


struct sceneRadial : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();

		if (uiC->pBool["on"]) {
			ofPushMatrix();
			float x = fbo->getWidth() * .5 + fbo->getWidth() * (uiC->pEasy["x"] + ofNoise(incrementa("xNoiseTime")) * uiC->pEasy["xNoise"] - uiC->pEasy["xNoise"]*.5);
			float y = fbo->getHeight() * .5 + fbo->getHeight() * (uiC->pEasy["y"] + ofNoise(incrementa("yNoiseTime")) * uiC->pEasy["yNoise"] - uiC->pEasy["yNoise"]*.5);
			
			ofTranslate(x, y);
			ofRotateDeg(incrementa("anguloTime"));
			if (uiC->pBool["circle"]) {
				ofSetColor(255);
				ofDrawEllipse(0,0,100,100);
			}
			int divisoes = uiC->pInt["divisoes"];
			float degs = 360.0 / (float) divisoes;
			ofSetLineWidth(uiC->pFloat["lineWidth"]);
			float interval = uiC->pEasy["interval"];
			
			// UNUSED - porque?
			// float alphaTime = 		incrementa("alphaTime");
			float noiseAlphaTime = 	incrementa("noiseAlphaTime");
			float noiseRaioTime = 	incrementa("noiseRaioTime");
			
			for (int a=0; a<divisoes; a++) {
				float angulo0 = a * degs;
				float angulo = angulo0;
				float qual = angulo / 360.0f;
				ofColor cor = getCor(qual);
				
				for (int c=1; c<uiC->pInt["concentricos"]; c++) {
					float noiseAlpha = ofNoise(c *.1 + noiseAlphaTime, qual * uiC->pEasy["noiseAlphaQual"]);
					
					float alpha = ofClamp((uiC->pEasy["alpha"] + noiseAlpha * uiC->pEasy["noiseAlpha"]) , 0, 1);
					cor.a *= alpha;
					ofSetColor(cor);
					float m1 = interval * (c * c * uiC->pEasy["spacing"]);
					float mend = interval * ((c+1)*(c+1) * uiC->pEasy["spacing"]);
					
					float noise = ofNoise(noiseRaioTime + c*.1,  angulo * uiC->pEasy["noiseMult"]);
					
					float m2 = ofMap(noise, 0, 1, m1, mend);
					float m0 = m1 *.97;
					float x0 = r2x(angulo, m0);
					float y0 = r2y(angulo, m0);
					float raio = c * uiC->pEasy["raioCirculo"];
					ofDrawCircle(x0,y0,raio);
					float x1 = r2x(angulo, m1);
					float y1 = r2y(angulo, m1);
					float x2 = r2x(angulo, m2);
					float y2 = r2y(angulo, m2);
					ofDrawLine(x1, y1, x2, y2);
				}
			}
			ofPopMatrix();
		}
	}
};



struct sceneGalaxia : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();

		ofMesh mesh;
//		mesh.setMode(OF_PRIMITIVE_POINTS);
		//mesh.setMode(OF_PRIMITIVE_LINES);
//		mesh.clear();
		if (ui->pString["draw"] == "points") {
			mesh.setMode(OF_PRIMITIVE_POINTS);
		}
		if (ui->pString["draw"] == "wire") {
			mesh.setMode(OF_PRIMITIVE_LINES);
		}

		float limite = uiC->pEasy["limite"];
		for (int b=0; b<uiC->pInt["bracos"]; b++) {
			float aa = b/(float)uiC->pInt["bracos"] * 360.0;
			for (int a=0; a<limite; a++) {
				float qual = a/(float)limite;
				
				//				ofSetColor(255);
				// ofSetColor(getCor(qual));
				float magnitude = a * uiC->pEasy["offset"];
				//cout << magnitude << endl;
				float angulo = aa + a * uiC->pEasy["torsao"] + ofGetElapsedTimef()*uiC->pEasy["torsaoTempo"];
				float x = r2x(angulo, magnitude);
				float y = r2y(angulo, magnitude);
				
				
				float rr = ofMap(qual, 0, 1, uiC->pEasy["raioInicial"], uiC->pEasy["raioFinal"]);
				float raio = rr + updown * uiC->pEasy["raioAudio"] +
				ofNoise(b, qual * (uiC->pEasy["raioRandMult"]+ofGetFrameNum()/10.0f)) * uiC->pEasy["raioRand"];
				float meioRaio = raio *.5;
				ofPushMatrix();
				ofTranslate(x,y);
				ofRotateYDeg(-angulo);
				
				glm::mat4 rotation = glm::rotate(ofDegToRad(-angulo), glm::vec3(0.0f, 1.0f, 0.0f));

				
				//ofDrawBox(0,0,0,raio,raio,raio);
				
				
				float espalha = ofMap(qual, 0, 1, uiC->pEasy["espalhaInicial"], uiC->pEasy["espalhaFinal"]);
				float espalhaRaio = ofMap(qual, 0, 1, uiC->pEasy["espalhaRaioInicial"], uiC->pEasy["espalhaRaioFinal"]);
				for (int c=0; c<espalha; c++) {
					
					float qq = qual * uiC->pEasy["fatorespalha"] +
					ofGetElapsedTimef() * uiC->pEasy["fatorespalhaTempo"];
					//ofNoise(qual*uiC->pEasy["fatorespalha"]
					float xx = (ofNoise(qq, b,c*1.0)*2-1.0) * espalhaRaio;
					float yy = (ofNoise(qq, b,c*1.5)*2-1.0) * espalhaRaio;
					float zz = (ofNoise(qq, b,c*1.3)*2-1.0) * espalhaRaio;
					
					ofSetColor(getCor(qual));
					
					if (uiC->pString["solido"] == "box") {
						ofDrawBox(xx,yy,zz, raio,raio,raio);
					}
					else if (uiC->pString["solido"] == "vert") {
						auto coords = glm::vec4(xx,yy,zz, 1.0) * rotation;
						mesh.addVertex(coords);
					}
					else if (uiC->pString["solido"] == "sphere") {
						ofDrawSphere(ofPoint(xx,yy,zz), raio);
					}
					else if (uiC->pString["solido"] == "rect") {
						ofPushMatrix();
						ofTranslate(xx,yy,zz);
						ofDrawRectangle(-meioRaio, -meioRaio, raio, raio);
						ofPopMatrix();
					}
					else if (uiC->pString["solido"] == "line") {

						// otimizar muito fora do loop
						float r = raio/2;
						// glm::vec3 ponto = glm::vec3(xx,yy,zz);
						// glm::vec3 ponto2 = glm::vec3(xx + raio,yy + raio,zz);
						
						auto coords = glm::vec4(xx - r,yy - r,zz, 0.0) * rotation;
						auto coords2 = glm::vec4(xx + r, yy + r,zz, 0.0) * rotation;
													 
						mesh.addVertex(coords);
						mesh.addVertex(coords2);
					}
					
				}
				ofPopMatrix();
			}
		}
		
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			ofPushMatrix();
			float x = ofMap(a, -1, uiC->pInt["numero"]-1+1, -uiC->pEasy["translate"], uiC->pEasy["translate"] );
			ofTranslate(x, 0);
			for (int b=0; b<uiC->pInt["bracos2"]; b++) {
				float aa = b/(float)uiC->pInt["bracos2"] * 360.0;
				ofPushMatrix();
				ofRotateYDeg(aa);
//				drawMesh(&mesh);
				drawMeshStatic(&mesh, ui);

				ofPopMatrix();
			}
			ofPopMatrix();
		}
	}
};




struct sceneBox : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	void draw() override {
		checkSetup();
		float aresta = uiC->pEasy["aresta"] + updown * uiC->pEasy["arestaAudio"];
		ofDrawBox(aresta);
	}
};




struct scenePoeira : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	

	struct poeira {
	public:
		float qual;
		glm::vec3 pos;
		ofVboMesh mesh;
		ofBoxPrimitive box;

		poeira(float q) : qual(q) {
			
			float x = ofRandom(400, 2000);
			float y = ofRandom(400, 2000);
			float z = ofRandom(400, 2000);
			if (ofRandom(0,10) > 5) x = -x;
			if (ofRandom(0,10) > 5) y = -y;
			if (ofRandom(0,10) > 5) z = -z;
			pos = glm::vec3( x, y, z );
			box.setResolution(2);
			box.setPosition(pos);
			box.set(15);
			mesh = box.getMesh();
			
		}
		void draw() {
			ofPushMatrix();
			ofTranslate(pos);
			mesh.draw();
			ofPopMatrix();
			//box.draw();
		}
	};

	vector <poeira> poeiras;

	void setup() override {
		int n = 400;
		poeiras.reserve(n);
		for (int a=0; a<n; a++) {
			poeiras.emplace_back(a/(float)n);
		}
	}
	
	void draw() override {
		checkSetup();

		int index = 0;
//		cout << poeiras.size() << endl;
		for (auto & p : poeiras) {
			ofSetColor(getCor(p.qual));
			p.draw();
			if (index++ > uiC->pInt["numero"]) {
				break;
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



// not working yet
struct scenePlexus : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		
		float tempo = ofGetElapsedTimef() * uiC->pFloat["tempoX"];
		float tempo2 = ofGetElapsedTimef() * uiC->pFloat["tempoY"];

		vector <glm::vec2> pontos;
		float numero = uiC->pEasy["numero"];

		float margin = uiC->pFloat["margin"];

		for (int a=0; a<numero; a++) {
			float qx = a * uiC->pFloat["qx"];
			float qy = a * uiC->pFloat["qy"];
			float x = (ofNoise(qx, tempo)*margin*2.0 - margin ) * fbo->getWidth() ;
			float y = (ofNoise(qy, tempo2)*margin*2.0 - margin ) * fbo->getHeight();
			pontos.push_back(glm::vec2(x, y));
		}

		float distance = uiC->pEasy["distance"];
		int index = 0;
		
//		cout << pontos.size() << endl;
		ofSetColor(255);
		for (auto & p : pontos) {
			for (auto & pp : pontos) {
				if (ABS(pp.x - p.x) < distance && ABS(pp.y - p.y) < distance) {
					
//					ofSetColor(index > numero *.5 ? ofColor(255,0,0,ui->pFloat["alpha"]) : ofColor(0,0,255,ui->pFloat["alpha"]));
					ofDrawLine(p, pp);
				}
			}
			index++;
		}

	}
};


struct sceneRede0 : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();

		float tempo = ofGetElapsedTimef() * uiC->pFloat["tempoX"];
		float tempo2 = ofGetElapsedTimef() * uiC->pFloat["tempoY"];
		vector <glm::vec2> pontos;
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			float qx = a * uiC->pFloat["qx"];
			float qy = a * uiC->pFloat["qy"];
			float x = ofNoise(qx, tempo) * fbo->getWidth();
			float y = ofNoise(qy, tempo2) * fbo->getHeight();
			pontos.push_back(glm::vec2(x, y));
		}
		
		for (auto & p : pontos) {
			for (auto & pp : pontos) {
				if (p != pp) {
					ofDrawLine(p, pp);
				}
			}
			if (uiC->pBool["circles"]) {
				ofDrawCircle(p, 10);
			}
		}
	}
};




struct sceneLines : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		for (int a=0; a<100; a++) {
			float x = ofRandom(0, fbo->getWidth());
			float y = ofRandom(0, fbo->getHeight());
			ofDrawLine(0,0,x,y);
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



// festa da uva mirabilis. portado pra ca mas nao testado .
struct sceneUva : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	

	struct uva {
	public:
		vector <uva> uvas;
		ofSpherePrimitive bola;
		float radius;
		ofNode position;
		//glm::vec3 pos;
		int gen = 0;
		ofVboMesh mesh;
		
		string & drawMode;

		uva (int g, string & d) : gen(g), drawMode(d) {
			radius = ofRandom(20,40);
			bola.setRadius(radius);
			bola.setResolution(16);
			
			mesh = bola.getMesh();
			mesh.disableColors();
			
			if (gen < 4) {
				int numero = ofRandom(2,8);
				for (int a=0; a<numero; a++) {
					//ofNode pos;
					uvas.emplace_back(gen+1, drawMode);
					uvas.back().position = position;
					uvas.back().position.orbitDeg(ofRandom(-30, 30) , ofRandom(-30,30), radius*7, position);
				}
			}
		}
		
		void draw() {
			ofPushMatrix();
			ofTranslate(position.getPosition());
			//bola.draw();
			
			ofVboMesh *m = &mesh;
			
			if (drawMode == "wire") {
				m->drawWireframe();
			}
			if (drawMode == "faces") {
				m->drawFaces();
			}
			if (drawMode == "points") {
				glDisable(GL_POINT_SMOOTH);
				m->draw(OF_MESH_POINTS);
			}
			
			ofPopMatrix();
			for (auto & u : uvas) {
				u.draw();
			}
		}
	};

	
	uva uvinha = uva(0, ui->pString["draw"]);
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		
		ofSetColor(getCor(0));
		uvinha.draw();


	}
};




struct sceneSyntype : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	//float scaleSyntype = 20.0;
	float scaleSyntype = 0.03;

	vector <ofPolyline> polys;
	ofxMicroUI * uiL = uiC;

	// armazena um contorno de uma letra
	struct letra {
	public:
		float largura;
		float altura;
		vector <ofPolyline> polylines; // mais de um traço por letra
	};

	// armazena todas as letras de uma fonte.
	map <string, letra> letrasMap;


	struct fraseSyntype {
	public:
		fraseSyntype() {}
		vector <ofPolyline> polylines;
		vector <ofVboMesh> meshes;

		// ponteiros pras letrasmap global
		map<string, letra> * letrasMap = NULL;
		float scale = 1.0;
		float * scaleSyntype = NULL;
		
		float offX = 0.0;
		float offY = 0.0;

		float cursorX;
		float cursorY;
		float entreletra = 1.0;
		float maxx = -99999;
		float minx = 99999;
		
		float alturaTotalFrase;
		
		string palavra;
		
		void reset() {
			maxx = -99999;
			minx = 99999;
			cursorX = 0;
			polylines.clear();
			if (letrasMap != NULL) {
				for (auto l : ofUTF8Iterator(palavra)){
				//for (auto l : palavra) {


	//				vector <string> letras = ofSplitString(palavra, "");
	//				for (auto & l : letras) {
	//					cout << l << endl;
	//				}
					//string letra = ofToString(l);
					
					string letra;
	//				ofAppendUTF8(letra, l);
					ofUTF8Append(letra, l);
	//				cout << letra << endl;
					ofStringReplace(letra, "-", "endash");
					ofStringReplace(letra, "\303\207", "C cedilla");
					ofStringReplace(letra, ".", "period");
					//ofStringReplace(letra, "Ç", "cedilla");
					//ofStringReplace(letra, "ã", "atilde");
					//cout << letra << endl;
	//				if (scaleSyntype != NULL) {
	//					scale *= *scaleSyntype;
	//				}
					
					for (auto & p : (*letrasMap)[letra].polylines) {
						ofPolyline poly;
						for (auto & v : p.getVertices()) {
							float x = v.x * scale + cursorX + offX;
							float y = v.y * scale + cursorY + offY;
							poly.addVertex(x, y);
	//						cout << x << " : " << y << endl;
	//						cout << *scaleSyntype << endl;
							//cout << v << endl;
							maxx = MAX(maxx,x);
							minx = MIN(minx,x);
						}

						if (p.getVertices()[0] == p.getVertices()[p.getVertices().size()-1])
						{
							poly.close();
							cout << "close" << endl;
						}
						polylines.push_back(poly);
						//ildaFrame.addPoly(poly, ui->pColor["cor"]);
					}
					cursorX += (*letrasMap)[letra].largura * scale + (entreletra * scale);
					alturaTotalFrase = MAX(alturaTotalFrase,(*letrasMap)[letra].altura * scale );
				}
			}
			updateMeshes();
		}

		void updateMeshes() {
			for (auto & p : polylines) {
				ofVboMesh m;
				m.setMode(OF_PRIMITIVE_LINES);
				for (auto & v : p.getVertices()) {
					m.addVertex(v);
				}
				meshes.push_back(move(m));
			}
		}

		void setText(string t) {
			cout << "setText :: " << t << endl;
			palavra = t;
			reset();
		}

		void fit() {
			float largura = maxx - minx;
			float ratio = 0.9 / largura;
			for (auto & p : polylines) {
				int c = 0;
				for (auto & v : p.getVertices()) {
					v = v*ratio;
					v.x -= (minx - 0.05);//ofMap(v.x, minx, maxx, 0.05, 0.95);
				}
			}
			//cout << largura << endl;
		}
		
		void drawPolys() {
			ofPushMatrix();
			ofTranslate(-cursorX/2, -alturaTotalFrase/2);
			for (auto & p : polylines) {
				p.draw();
			}
			ofPopMatrix();
		}
		
		void drawRounds(float r) {
			ofPushMatrix();
			ofTranslate(-cursorX/2, -alturaTotalFrase/2);
			for (auto & p : polylines) {
				for (auto & v : p.getVertices()) {
					ofDrawCircle(v.x, v.y, r);
				}
			}
			ofPopMatrix();
		}
		
		void drawResampled(float space = 5, float radius = 10.0) {
			ofPushMatrix();
			ofTranslate(-cursorX/2, -alturaTotalFrase/2);
			for (auto & p : polylines) {
				ofPolyline pp = p.getResampledBySpacing(space);
				for (auto & v : pp.getVertices()) {
					ofDrawCircle(v.x, v.y, radius);
				}
			}
			ofPopMatrix();
		}
		
		

		void drawMeshes() {
			ofPushMatrix();
			ofTranslate(-cursorX/2, -alturaTotalFrase/2);
			ofSetColor(255,0,0);
			for (auto & m : meshes) {
				m.draw();
			}
			ofSetColor(205,0,255);
			for (auto & m : meshes) {
				m.draw();
			}
			ofPopMatrix();
		}
	} frase1;

	void setup() override {
		frase1.scaleSyntype = &scaleSyntype;
	}
	
	void loadLetter(string fileName) {
		cout << "loadletter :: " << fileName << endl;
		frase1.letrasMap = &letrasMap;
		letrasMap.clear();
		vector <string> linhas = ofxMicroUI::textToVector(fileName);
		int contagem = 0;
		for (auto & linha : linhas) {
			vector<string> dadosdalinha = ofSplitString(linha, ":");
			if (linha != "" && dadosdalinha.size() && linha.substr(0,1) != "#") {
				string letra = dadosdalinha[0];
				vector<string> tracos = ofSplitString(dadosdalinha[1], "|");
				vector <ofPolyline> tempPolylines;
				//letrasMap[letra].clear();

				float largura = 0;
				float maxx = -999;
				float minx = 999;
				for (auto & traco : tracos) {
					vector<string> tracocoords = ofSplitString(traco, " ");

					ofPolyline tempPolyline;
					for (auto & v : tracocoords) {
						vector <string> xy = ofSplitString(v, ",");
						ofPoint coords =
						ofPoint(
									ofToFloat(xy[0]) * 1.0,
									1.0-ofToFloat(xy[1]) * 1.0);
						largura = MAX(largura, coords.x);
						float x = coords.x;
						float y = coords.y;
						tempPolyline.addVertex(ofPoint(x, y));
						//cout << ofPoint(x, y) << endl;
						maxx = MAX(maxx,x);
						minx = MIN(minx,x);
					}

					letrasMap[letra].polylines.push_back(move(tempPolyline));
				}
				letrasMap[letra].largura = largura;
				contagem++;
			}
		}
	}

	void draw() override {
		ofTranslate(fbo->getWidth() * .5 , fbo->getHeight() * .5);

		// temporario vamos ver
		ofTranslate(uiL->pFloat["offsetX"], uiL->pFloat["offsetY"]);
		if (uiL->pBool["palavra"]) {
			float cursorX = uiL->pFloat["offX"] * 100.0;
			float cursorY = uiL->pFloat["offY"] * 100.0;
			string frase = uiL->pString["frase"];
			ofNoFill();
			if (uiL->pBool["drawPolys"]) {
				frase1.drawPolys();
			}
			if (uiL->pBool["drawRounds"]) {
				frase1.drawRounds(uiL->pEasy["radius"]);
			}
			if (uiL->pBool["drawResampled"]) {
				frase1.drawResampled(uiL->pEasy["resampledSpace"], uiL->pEasy["radius"]);
				if (uiL->pBool["radius2"]) {
					frase1.drawResampled(uiL->pEasy["resampledSpace"], uiL->pEasy["radius2"]);
				}
			}
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {

//	void syntypeUIEvent(ofxMicroUI::element & e) {
		//cout << e.name << endl;
		if (e.name == "scale") {
			frase1.scale = *e.f;
			cout << frase1.scale << endl;
			frase1.reset();
		}
		else if (e.name == "entreletra") {
			frase1.entreletra = *e.f;
			frase1.reset();
			// settext ja vem reset embutido
		}

		else if (e.name == "frase") {
			frase1.setText(*e.s);
			// settext ja vem reset embutido
		}
		else if (e.name == "offX") {
			frase1.offX = *e.f;
			frase1.reset();
		}
		else if (e.name == "offY") {
			frase1.offY = *e.f;
			frase1.reset();
		}

		
		else if (e.name == "letra") {
			string f = ((ofxMicroUI::dirList*)&e)->getFileName();
			
			//string f = uiL->getFileFullPath(e.name);
			if (f != "/" && f != "" && uiL->pString["loadedLetter"] != f) {
				uiL->pString["loadedLetter"] = f;
				loadLetter(f);
				frase1.reset();
			}
		}
	}



	#ifdef DMTRUI
	void syntypeUIEvent(uiEv e) {
		//cout << e.name << endl;
		if (e.name == "scale") {
			frase1.scale = e.f;
			cout << frase1.scale << endl;
			frase1.reset();
		}
		else if (e.name == "entreletra") {
			frase1.entreletra = e.f;
			frase1.reset();
			// settext ja vem reset embutido
		}

		else if (e.name == "frase") {
			frase1.setText(e.s);
			// settext ja vem reset embutido
		}
		else if (e.name == "offX") {
			frase1.offX = e.f;
			frase1.reset();
		}
		else if (e.name == "offY ") {
			frase1.offY = e.f;
			frase1.reset();
		}

		
		else if (e.name == "letra") {
			string f = uiL->getFileFullPath(e.name);
			if (f != "" && uiL->pString["loadedLetter"] != f) {
				uiL->pString["loadedLetter"] = f;
				loadLetter(f);
				frase1.reset();
			}
		}
	}
	#endif
};



struct sceneGirinos : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	#include "polar.h"

	bool girinoChanged = false;
	
	void uiEvents(ofxMicroUI::element & e) override {
		girinoChanged = true;
	}


	struct girinoSettings {
	public:
		bool circuit2;
		//glm::vec2 dimensions = glm::vec2(800, 480);
		glm::vec2 dimensions = glm::vec2(800, 480);
		int minX, minY, maxX, maxY, largura, altura;
		int margem = -50;
		bool drawRect = false;
		bool circuit = true;
		bool drawText = false;
		float incrementador,
				densidade, fatorQual, anguloMult,
				densidade2, fatorQual2, anguloMult2;
		
		void setDimensions(glm::vec2 d) {
			dimensions = d;
			minX = 		margem;
			minY = 		margem;
			maxX = 		dimensions.x - margem;
			maxY = 		dimensions.y - margem;
			largura = 	dimensions.x - margem*2;
			altura = 	dimensions.y - margem*2;
		}

	} girinoSet, girinoSet2;


	struct girino {
	public:
		girino() {}
		float rand;
		
		glm::vec2 pos;
		float qual;
		int cursor = 0;
		ofPoint positions[60];
		int posSize = 0;
		float angulo;
		float vel = 2;
		bool alive = false;
		
		girinoSettings * _settings = NULL;
		
		// border
		int minX, minY, maxX, maxY, largura, altura;

		
		girino(float q, girinoSettings & _s) : qual(q) {
			_settings = &_s;
			//fazer posicao negativa no inicio
			//dimensions = p;
			pos = glm::vec2(ofRandom(0,_settings->dimensions.x),
							ofRandom(0,_settings->dimensions.y));
			rand = ofRandom(0,1);
			//int n = ;
	//		posSize = ofRandom(10,30);
			//posSize = ofRandom(20,60);
			posSize = ofRandom(50,60);
			//positions.assign(n, ofPoint());
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
					
					
	//				if (_settings->densidade) {
	//					angulo += fmod(ofNoise((pos.x)/_settings->densidade,
	//								   (pos.y)/_settings->densidade,
	//								   qual * _settings->fatorQual,
	//								   _settings->incrementador
	//								) * 360.0 * _settings->anguloMult, 360.0);
	//				}
	//				if (_settings->densidade2) {
	//					angulo += fmod(ofNoise((pos.x)/_settings->densidade2,
	//									(pos.y)/_settings->densidade2,
	//									qual * _settings->fatorQual2,
	//									_settings->incrementador
	//								) * 360.0 * _settings->anguloMult2, 360.0);
	//				}
			}
		}
		
		void update() {
			if (alive) {
		//		angulo += ofRandom(-15, 15);
		//		if (angulo > 360) { angulo -= 360; }
		//		else if (angulo < 0) { angulo += 360; }
				preUpdate();
				if (_settings->circuit) {
					angulo = int(angulo / 45) * 45;
				}
				if (_settings->circuit2) {
					angulo = int(angulo / 90) * 90;
				}
				pos += p2c(glm::vec2(angulo, vel));
				
				if (pos.x > _settings->maxX) {  addX(-_settings->largura);  }
				else if (pos.x < _settings->minX) {  addX(_settings->largura);   }
				if (pos.y > _settings->maxY) {  addY(-_settings->altura);   }
				else if (pos.y < _settings->minY) {  addY(_settings->altura);    }

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
					p.addVertex(positions[(a+cursor)%posSize]);
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
		girinoSet.setDimensions(glm::vec2(fbo->getWidth(), fbo->getHeight()));
		girinoSet2.setDimensions(glm::vec2(fbo->getWidth(), fbo->getHeight()));
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

		//((inspector*)uiC->getElement("size"))->set(ofToString(gi.girinos.size()));
		if (girinoChanged)
		{
			cout << "updating changed" << endl;
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
			girinoSet.densidade = 		uiC->pEasy["densidade"];
			girinoSet.fatorQual = 		uiC->pEasy["fatorQual"];
			girinoSet.anguloMult =		uiC->pEasy["anguloMult"];
			girinoSet.densidade2 = 		uiC->pEasy["densidade2"];
			girinoSet.fatorQual2 = 		uiC->pEasy["fatorQual2"];
			girinoSet.anguloMult2 =		uiC->pEasy["anguloMult2"];
			girinoSet.circuit = uiC->pBool["circuit"];
			girinoSet.circuit2 = uiC->pBool["circuit90"];
			girinoSet.drawRect = uiC->pBool["drawRect"];
			girinoSet.drawText = uiC->pBool["drawText"];
			
			
			
			girinoSet2.densidade = 		uiC->pEasy["densidade3"];
			girinoSet2.fatorQual = 		uiC->pEasy["fatorQual3"];
			girinoSet2.anguloMult =		uiC->pEasy["anguloMult3"];
			girinoSet2.densidade2 = 		uiC->pEasy["densidade4"];
			girinoSet2.fatorQual2 = 		uiC->pEasy["fatorQual4"];
			girinoSet2.anguloMult2 =		uiC->pEasy["anguloMult4"];
			girinoSet2.circuit = uiC->pBool["circuit2"];
			girinoSet2.circuit2 = uiC->pBool["circuit290"];
			girinoSet2.drawRect = uiC->pBool["drawRect"];
			girinoSet2.drawText = uiC->pBool["drawText"];
		gi.unlock();
	}

};



// future user interface
struct scenePirose : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();
		// FUTURE USER INTERFACE #1
		ofTranslate(40,40);
		
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






struct sceneBasic : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
		
	}
	
	void draw() override {
		checkSetup();

	}
};




vector <sceneDmtr *> scenes;
map <string, sceneDmtr*> scenesMap;

bool scenesNovaIsSetup = false;
void setupScenesNova() {
	scenes.push_back(new sceneOcean(uiC, ui, fbo));
	scenesMap["ocean"] = scenes.back();

	scenes.push_back(new sceneWorms(uiC, ui, fbo));
	scenesMap["minhoca"] = scenes.back();

	scenes.push_back(new sceneBox(uiC, ui, fbo));
	scenesMap["box"] = scenes.back();

	scenes.push_back(new sceneGridbox(uiC, ui, fbo));
	scenesMap["gridbox"] = scenes.back();

	scenes.push_back(new sceneNovelo(uiC, ui, fbo));
	scenesMap["novelo"] = scenes.back();

	scenes.push_back(new sceneRede(uiC, ui, fbo));
	scenesMap["redes"] = scenes.back();
	
	scenes.push_back(new scene3d(uiC, ui, fbo));
	scenesMap["3d"] = scenes.back();

	scenes.push_back(new sceneLeparc(uiC, ui, fbo));
	scenesMap["leparc"] = scenes.back();

	scenes.push_back(new sceneRadial(uiC, ui, fbo));
	scenesMap["radial"] = scenes.back();

	scenes.push_back(new sceneGalaxia(uiC, ui, fbo));
	scenesMap["galaxia"] = scenes.back();

	scenes.push_back(new scenePoeira(uiC, ui, fbo));
	scenesMap["poeira"] = scenes.back();

	scenes.push_back(new sceneSolidos(uiC, ui, fbo));
	scenesMap["solidos"] = scenes.back();

	scenes.push_back(new scenePlexus(uiC, ui, fbo));
	scenesMap["plexus"] = scenes.back();

	scenes.push_back(new sceneRede0(uiC, ui, fbo));
	scenesMap["redes0"] = scenes.back();

	scenes.push_back(new sceneLines(uiC, ui, fbo));
	scenesMap["lines"] = scenes.back();

	scenes.push_back(new scenePunchcard(uiC, ui, fbo));
	scenesMap["punchcard"] = scenes.back();

	scenes.push_back(new sceneSyntype(uiC, ui, fbo));
	scenesMap["syntype"] = scenes.back();

	scenes.push_back(new sceneGirinos(uiC, ui, fbo));
	scenesMap["girinos"] = scenes.back();

	scenes.push_back(new scenePirose(uiC, ui, fbo));
	scenesMap["pirose"] = scenes.back();

	scenes.push_back(new sceneMicroscenes(uiC, ui, fbo));
	scenesMap["microScenes"] = scenes.back();

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
