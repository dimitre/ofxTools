struct sceneLiner : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofPolyline poly;
	vector <glm::vec2> points;
	
	void setup() override {
		poly.setClosed(false);
	}

	void draw() override {
		checkSetup();

		for (float a=0; a<uiC->pEasy["numLines"]; a++) {
			ofSetLineWidth( a * uiC->pEasy["lineWidthA"] + ui->pEasy["lineWidth"]);
			float qual = a / uiC->pEasy["numLines"];
			ofSetColor(getCor(qual));

			
			poly.clear();
			points.clear();
			
			for (float i=0; i<uiC->pEasy["numero"]; i++) {
				float x = ofMap(i,0, uiC->pEasy["numero"], 0, fbo->getWidth());
				float y = ofMap(i,0, uiC->pEasy["numero"], uiC->pEasy["y"], uiC->pEasy["yX"] + a * uiC->pEasy["yAX"]);
				y += a * uiC->pEasy["yA"];
				for (int n=1; n<4; n++) {
					string index = ofToString(n);
					if (uiC->pBool["ok"+index]) {
						// tmpVec.y += (ofNoise(x * p.noiseMult + offXTime, p.ny, p.easy)-.5) * p.my;
//						cout << a * uiC->pEasy["noiseMult" + index] << endl;
						float noise = i * uiC->pEasy["noiseMult" + index] + a * uiC->pEasy["noiseMultA"];
//						cout << noise << endl;
						
						
						y += ofNoise(noise, uiC->pEasy["velManual" + index]) * uiC->pEasy["multY" + index];
					}
				}
				if (uiC->pBool["poly"]) {
					if (i == 0) {
						poly.lineTo(x,y);
					} else {
						poly.curveTo(x,y);
					}
				} else {
					points.push_back(glm::vec2(x,y));
				}
//				poly.addVertex(x,y);
			}
			if (uiC->pBool["poly"]) {
				poly.draw();
			} else {
				int pn = 0;
				for (auto & p : points) {
					float r = uiC->pEasy["raio"] + ofNoise(pn * uiC->pEasy["raioMult"] + a * uiC->pEasy["raioMultA"]) * uiC->pEasy["raioNoise"];
					if (r > 0) {
						ofDrawEllipse(p.x, p.y, r, r);
					}
					pn ++;
				}
			}
		}

		// densidade1
		// densidadeX1
		// densidadeA1

		// densidade2
		// densidadeX2
		// densidadeA2

		// densidade3
		// densidadeX3
		// densidadeA3

		// y
		// yX
		// yA
	}
};



struct sceneMinhoca : ofxScenes::sceneDmtr, ofxScenes::polarVec2 {
public:
	using sceneDmtr::sceneDmtr;
	// name = "minhoca";

	struct worm {
	public:
		#define NWORMS 40

		glm::vec2 pos;
		ofRectangle & _rectBounds;
		ofxMicroUI * ui = NULL;
//		glm::vec2 * _bounds = NULL;
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
		int index = 0;

		
		worm(ofRectangle & _r, ofxMicroUI * _uiC, int i) : _rectBounds(_r), ui(_uiC), index(i) {
//			pos = glm::vec2(ofRandom(0,1000), ofRandom(0,500));
			pos = glm::vec2(ofRandom(_rectBounds.x, _rectBounds.width),
							ofRandom(_rectBounds.y, _rectBounds.height));
			angulo = ofRandom(0,360);
			lineW = ofRandom(1,80);
			mag = ofRandom(1,9) * 3.0;
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
			for (int a=0; a<NWORMS; a+=1) {
				int index = (a + cursor) % NWORMS;
				poly.addVertex(ofPoint(corpo[index]));
			}
			ofSetLineWidth(lineW);
			
			if (ui != NULL) {
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
		for (int a=0; a<400; a++) {
			// worms.emplace_back()
			worms.push_back(worm(boundsRect, uiC, a));
		}
	}

	void draw() override {
		checkSetup();

        ofSetColor(ofxScenes::getColor(0, config->uiColors));

		for (auto & w : worms) {
			if (w.index < uiC->pInt["numero"]) {
				w.update();
				if (uiC->pBool["color"]) {
					float h = uiC->pEasy["hueStart"] + w.rand * uiC->pEasy["hueStep"];
					ofSetColor(ofColor::fromHsb(h, 255, 255));
				} else {
					ofSetColor(getCor(w.rand));
				}
				w.draw();
			}
		}
	}
};



struct scenePunchcard : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "punchcard";
	
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



struct sceneMicroscenes : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "microScenes";
		
	struct microScene {
	public:
		ofxMicroUI * uiColors;
		ofFbo fbo;
		ofFbo * fboOut;
		int s = 0;
		glm::vec2 pos;
		ofRectangle rect;
		
		ofImage images[15];
		
		microScene(ofxMicroUI * _ui, ofFbo * _fboOut) : uiColors(_ui), fboOut(_fboOut) {
			int w = int(ofRandom(1,3)) * 100;
			int h = int(ofRandom(1,3)) * 100;
			// cout << "begin " << w << " " << h << endl;
			

			fbo.allocate(w, h, GL_RGBA);
			fbo.begin();
			ofClear(0,255);
			fbo.end();
			fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
			
			// int x = ofRandom(0,fboOut->getWidth()/100) * 100;
			int y = ofRandom(0,fboOut->getHeight()/100) * 100;
			// pos = glm::vec2(x, y);

			pos = glm::vec2(int(ofRandom(0,136)) * 55 * 55, y);
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
            
//            ofSetColor();

            
            // XAXA RESOLVER
//            ofColor c = getCor(0);
//			ofClear(c);
            ofClear(ofColor(0));
            
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
			scenes.push_back(microScene(config->uiColors, fbo));
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
struct sceneWave : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "wave";
	
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



// este ta bugadão. testei no miaw e nada.
struct sceneBola2 : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "bola2";

	struct objeto {
	public:
        glm::vec2 pos;
		float raio;
		float angle;
		float mag;
		float index;

		objeto(glm::vec2 p, float r, float a, float m, float i) : pos(p), raio(r), angle(a), mag(m), index(i) {}

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
			glm::vec2 p1 = glm::vec2(r2x(0,mag), r2y(0,mag));
            glm::vec2 p2 = glm::vec2(r2x(0,mag), r2y(0,mag));
			float d = 0;
			float testAngle = 0;
			while (d < (raio*2+spacing)) {
				testAngle+=.1;
				p2 = glm::vec2(r2x(testAngle,mag), r2y(testAngle,mag));
                
//				d = p1.distance(p2);
                d = glm::distance(p1, p2);
			}
			int divisoes = (360.0 / testAngle);
			for (int a=0; a<divisoes; a++) {
				float ang = 360.0/(float)divisoes * a + 90;
				float x = r2x(ang, mag);
				float y = r2y(ang, mag);
				objetos.push_back(objeto(glm::vec2(x,y),raio, ang, mag,i));
				i++;
			}
		}
	}
};



struct sceneOndas : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio, ofxScenes::polarVec2, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "ondas";

	void draw() override {
		checkSetup();
		
		ofTranslate(middle);

		float scale = uiC->pEasy["scale"];
		ofScale(scale, scale);

		ofNoFill();
		for (int b=0; b<2; b++) {
			ofPushMatrix();
			float transX = uiC->pFloat["centroX" + ofToString(b)] + updown*
			uiC->pFloat["centroX" + ofToString(b) + "Audio"];

			float transY = uiC->pFloat["centroY" + ofToString(b)] + updown*
			uiC->pFloat["centroY" + ofToString(b) + "Audio"];

			float s = ofNoise(ofGetFrameNum() / 100.0, b * 10);
			float ss = ofNoise(ofGetFrameNum() / 75.0, b * 10);
			// float s = sin(ofGetFrameNum() / 100.0);
			// float ss = sin(ofGetFrameNum() / 75.0);

			transX += uiC->pEasy["offXAuto"] * s;
			transY += uiC->pEasy["offYAuto"] * ss;



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

		ofScale(1, 1);

	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneLatquad2 : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "latquad2";
	
	void draw() override {
		checkSetup();
		
		ofTranslate(middle);
		ofSetColor(getCor(0));

		float scale = uiC->pEasy["scale"];
		ofScale(scale, scale);

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
		ofScale(1.0, 1.0);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};





struct sceneMoire : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "moire";

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




struct sceneLatquad3 : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "latquad3";
	
	ofMesh mesh;
	ofImage spriteImage;

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



struct sceneLatquad : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "latquad";	
	
	void draw() override {
		checkSetup();
		ofTranslate(middle);

		float scale = uiC->pEasy["scale"];
		ofScale(scale, scale);

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

				// unused
				// float finalmag = c2m(xx,yy);
//				if (finalmag < ui->pFloat["raioCirculo"]) {
					ofDrawCircle(xx, yy, raio);
//				}
			}
		}

		ofScale(1.0, 1.0);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};





struct sceneRedes : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "redes";

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

		//unused
		// int numero = uiC->pInt["numero"];
		
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


struct sceneLeparc : public ofxScenes::sceneDmtr, ofxScenes::sceneIncrementa, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;
	// name = "leparc";
	
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



struct sceneRadial : public ofxScenes::sceneDmtr, ofxScenes::sceneIncrementa, ofxScenes::polar {
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




// not working yet
struct scenePlexus : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void draw() override {
		checkSetup();
		
		float tempo = ofGetElapsedTimef() * uiC->pFloat["tempoX"];
		float tempo2 = ofGetElapsedTimef() * uiC->pFloat["tempoY"];

		vector <glm::vec2> pontos;
		float margin = uiC->pFloat["margin"];

		for (int a=0; a<uiC->pEasy["numero"]; a++) {
			float qx = a * uiC->pFloat["qx"];
			float qy = a * uiC->pFloat["qy"];
			float x = (ofNoise(qx, tempo)*margin*2.0 - margin ) * fbo->getWidth() ;
			float y = (ofNoise(qy, tempo2)*margin*2.0 - margin ) * fbo->getHeight();
			pontos.push_back(glm::vec2(x, y));
		}

		float distance = uiC->pEasy["distance"];
		
//		cout << pontos.size() << endl;
		ofSetColor(getCor(0));

        int index = 0;
        ofNoFill();
		for (auto & p : pontos) {
//            cout << p.x << endl;
			for (auto & pp : pontos) {
				if (ABS(pp.x - p.x) < distance && ABS(pp.y - p.y) < distance) {
//					ofSetColor(index > numero *.5 ? ofColor(255,0,0,ui->pFloat["alpha"]) : ofColor(0,0,255,ui->pFloat["alpha"]));
					ofDrawLine(p, pp);
                    if (uiC->pBool["circles"]) {
                        ofDrawCircle(p.x, p.y, 20);
                        ofDrawCircle(pp.x, pp.y, 20);
                    }
				}
			}
			index++;
		}
	}
};


struct sceneRedes0 : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "redes0";
	
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






struct sceneSyntype : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "syntype";

	string loadedFile = "";

	//float scaleSyntype = 20.0;
//	float scaleSyntype = 0.03;
	float scaleSyntype = 32;

	vector <ofPolyline> polys;

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
		
		// 2020
		ofxMicroUI * uiColors = NULL;
		
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
							// cout << "close" << endl;
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
				// int c = 0;
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
//			cout << polylines.size() << endl;
            
			for (auto & p : polylines) {
//                ofSetColor(getCor(0));
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
                    //ofSetColor(ofxScenes::getColor(ofRandom(0,1), ofxScenes::config.uiColors));
                    
                    // xaxa getcolor
//                    ofSetColor(255);
//                    ofSetColor(getCor(ofRandom(0,1)));
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

	// float rand;
	void setup() override {
		frase1.uiColors = config->uiColors;
		// cout << "setup syntype" << endl;
		// rand = ofRandom(0,1);
//		frase1.scaleSyntype = &scaleSyntype;
	}
	
	void loadLetter(string fileName) {
		cout << "loadletter :: " << fileName << endl;
		// cout << "rand :: " << rand << endl;
		frase1.letrasMap = &letrasMap;
		letrasMap.clear();
		vector <string> linhas = ofxMicroUI::textToVector(fileName);
		int contagem = 0;
		for (auto & linha : linhas) {
//			cout << linha << endl;
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
						glm::vec2 coords =
						glm::vec2(
									ofToFloat(xy[0]) * 1.0,
									1.0-ofToFloat(xy[1]) * 1.0);
						largura = MAX(largura, coords.x);
						tempPolyline.addVertex(ofPoint(coords.x, coords.y));
						maxx = MAX(maxx,coords.x);
						minx = MIN(minx,coords.x);
					}
					
					// for (auto & v : tempPolyline.getVertices()) {
					// 	cout << v << endl;
					// }

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
		ofTranslate(uiC->pFloat["offsetX"], uiC->pFloat["offsetY"]);
		if (uiC->pBool["palavra"]) {

			// unused
			// float cursorX = uiC->pFloat["offX"] * 100.0;
			// float cursorY = uiC->pFloat["offY"] * 100.0;
			string frase = uiC->pString["frase"];
			ofNoFill();
			if (uiC->pBool["drawPolys"]) {
				frase1.drawPolys();
			}
			if (uiC->pBool["drawRounds"]) {
				frase1.drawRounds(uiC->pEasy["radius"]);
			}
			if (uiC->pBool["drawResampled"]) {
				frase1.drawResampled(uiC->pEasy["resampledSpace"], uiC->pEasy["radius"]);
				if (uiC->pBool["radius2"]) {
					frase1.drawResampled(uiC->pEasy["resampledSpace"], uiC->pEasy["radius2"]);
				}
			}
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		// cout << "uiEvents rand :: " << rand << endl;

		if (e.name == "scale") {
			frase1.scale = *e.f;
			cout << "syn scale " << frase1.scale << endl;
			frase1.reset();
		}
		else if (e.name == "entreletra") {
			frase1.entreletra = *e.f;
			frase1.reset();
			// settext ja vem reset embutido
		}

		else if (e.name == "frase") {
			if (*e.s != "") {
				frase1.setText(*e.s);
			}
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
			if (f != "/" && f != "" && loadedFile != f) {
				loadedFile = f;
				loadLetter(f);
				frase1.reset();
			}
		}
	}
};




// #include "sceneGirinos.h"

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
        
        girinoSet.fboRect = config->soft->fboRect;

//		ofRectangle fboRect = ofRectangle(0,0,fbo->getWidth(), fbo->getHeight());
        
        
        int margem = 100;
        girinoSet.boundsRect = ofRectangle(-margem, -margem,
                                             fbo->getWidth() + margem * 2,
                                             fbo->getHeight() + margem * 2);
        
        
//		girinoSet.fboRect = fboRect;
		
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








#ifdef USESVG
struct sceneSvg0 : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "svg0";

	ofxSVG svg;


	struct neonLight {
	public:
		glm::vec3 centroid;
		ofVboMesh mesh;
		float rot;
		float velRot;
		ofPath path;

		neonLight(ofMesh m, ofPath p) {
			path = p;
			path.setFilled(false);
			path.setUseShapeColor(false);
			//path.setStrokeWidth(5);

			
	//		centroid = m.getCentroid();
	//		mesh = m;
	//		for (auto & v : mesh.getVertices()) {
	//			v -= centroid;
	//		}
	//		rot = ofRandom(0,180);
	//		velRot = ofRandom(-2,2);
		}
		
		void update() {
			rot += velRot;
		}
		
		void drawRotate() {
			ofPushMatrix();
			ofTranslate(centroid);
			ofRotateYDeg(rot);
			mesh.drawFaces();
			
			
			
			ofPopMatrix();
		}
		
		void draw() {
			ofPushMatrix();
			ofTranslate(centroid);
			mesh.drawFaces();
			ofPopMatrix();
		}
		
		void drawPath() {
			path.draw();
		}
	};

	vector <neonLight> neonLights;
	
	void setup() override {
//		ofAddListener(uiC->uiEvent, this, &sceneSvg::uiEvents);
	}
	
	void draw() override {
		checkSetup();
		

		float multTimeX = incrementa("multTimeX");
		float multTimeY = incrementa("multTimeY");
		
		float anguloTime = incrementa("anguloTime");
		//ofGetElapsedTimef() * uiC->pFloat["anguloTime"])
		
		int numero = svg.getNumPath() * uiC->pFloat["numero"];
		float limite = uiC->pFloat["limite"];
		float velocity = (uiC->pFloat["velocity"] + updown * uiC->pFloat["velocityAudio"]);
		
		int vezes = uiC->pInt["vezes"];
		int index=0;
		
		float noiseW = uiC->pEasy["noisePos"] * fbo->getWidth();
		float noiseH = uiC->pEasy["noisePos"] * fbo->getHeight();
		
		
		ofPushMatrix();
		
		for (int b=0; b<vezes; b++) {
			for (int i = 0; i < numero; i++){
				if (ofNoise(i*.2, ofGetElapsedTimef()*velocity) < limite) {
					index ++;
					
					float qual = index / (float)(numero * vezes);
					//float qual = (i+b*vezes)/(float)(numero*vezes);
					float a = i + b*vezes;
					
					//cout << qual << endl;

					ofColor cor = getCor(fmod(qual*3, 1.0));
					ofSetColor(cor);
					
					
					// XAXA not used
//					float raio = uiC->pEasy["raio"] + qual*uiC->pEasy["raioMax"];
					float x = 0;
					float y = 0;
					
					if (uiC->pBool["noiseTranslate"]) {
//						x = ofNoise(ofGetElapsedTimef() * multTimeX, a+1) * noiseW - noiseW*.5;
//						y = ofNoise(ofGetElapsedTimef() * multTimeY, a*1.2+1) * noiseH - noiseH*.5;
						x = ofNoise( multTimeX * .01, a * uiC->pEasy["noiseAx"] +1) * noiseW - noiseW*.5;
						y = ofNoise( multTimeY * .01, a * uiC->pEasy["noiseAy"] +2) * noiseH - noiseH*.5;
					}
					
					
					if (uiC->pBool["translate"]) {
						//ofTranslate(fbo->getWidth() * .5 , fbo->getHeight() * .5);
						x += fbo->getWidth() * .5;
						y += fbo->getHeight() * .5;
					}
					
					


					
					bool wired = qual > uiC->pFloat["wired"];

					if (uiC->pBool["rewire"]) {
						if (wired) {
							ofNoFill();
						} else {
							ofFill();
						}
					}
					
					ofPath p = svg.getPathAt(i);
					p.setUseShapeColor(false);
					if (p.getStrokeWidth()) {
						p.setStrokeWidth(ui->pFloat["lineWidth"]);
					}

					if (uiC->pBool["rewire"]) {
						
						if (wired) {
							p.setFilled(false);
						} else {
							p.setFilled(true);
							p.setStrokeWidth(0);
						}
					}
					
					ofPushMatrix();
					ofTranslate(x,y);
					if (uiC->pBool["rotate"]) {
						float angulo = ofNoise(anguloTime * 0.01, a * uiC->pEasy["noiseAa"] + 1) * uiC->pEasy["anguloNoise"];
						ofRotateDeg(angulo);
					}
					
					ofPushStyle();
					float scale = (uiC->pEasy["scale"] + uiC->pEasy["scaleB"] * (b/(float)vezes));
					scale *= uiC->pEasy["scaleAll"];
					//cout << scale << endl;
					ofScale(scale, scale);
					
//							if (ui->pBool["useCairo"]) {
//								startCairoBlendingMode();
//							}
					
					p.draw();
					ofPopStyle();
					ofPopMatrix();
				}
			}
		}
		ofPopMatrix();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		cout << "UIEVENT" << endl;
		cout << e.name << endl;
			if (e.name == "svg") {
				if (*e.s != "") {
					string file = ((ofxMicroUI::dirList*)&e)->getFileName();
					cout << file << endl;
//					string file = uiC->getRadio(e.name)->getFullFileName();
					if (uiC->pString["loadedSvg"] != file && ofFile::doesFileExist(file)) {
						cout << "file loaded" << endl;
						uiC->pString["loadedSvg"] = file;
						svg.load(file);
						//meshes.clear();
						neonLights.clear();
		//				cout << "loading file svg :: " << file << endl;
		//				cout << "svg file loaded : number of paths = " << svg.getNumPath() << endl;
						for (auto a=0; a<svg.getNumPath(); a++) {
							ofPath p = svg.getPathAt(a);
							p.setFilled(false);
							p.setUseShapeColor(false);
							p.setStrokeWidth(uiC->pFloat["stroke"]);
							ofMesh m;
							//ofMesh m = p.getTessellation();
							neonLights.push_back(neonLight(m, p));
						}
						cout << neonLights.size() << endl;
					}
				}
			}
	}
};
#endif




#ifdef INSIDE

void setupScenesAll() {
	scenes.push_back(new sceneLines(&config, "lines"));
	scenes.push_back(new sceneLiner(&config, "liner"));

	scenes.push_back(new sceneMinhoca(&config, "minhoca"));
	scenes.push_back(new scenePunchcard(&config, "punchcard"));
	scenes.push_back(new sceneMicroscenes(&config, "microScenes"));
	// coke cymatics
	scenes.push_back(new sceneWave(&config, "wave"));
	scenes.push_back(new sceneBola2(&config, "bola2"));
	scenes.push_back(new sceneOndas(&config, "ondas"));
	scenes.push_back(new sceneLatquad(&config, "latquad"));
	scenes.push_back(new sceneLatquad2(&config, "latquad2"));
	scenes.push_back(new sceneLatquad3(&config, "latquad3"));
	scenes.push_back(new sceneMoire(&config, "moire"));

	// XAXA nome - rede ou redes
	scenes.push_back(new sceneRedes(&config, "redes"));
	scenes.push_back(new sceneRedes0(&config, "redes0"));
	scenes.push_back(new sceneLeparc(&config, "leparc"));
	scenes.push_back(new sceneRadial(&config, "radial"));
	scenes.push_back(new scenePlexus(&config, "plexus"));
	scenes.push_back(new sceneGirinos(&config, "girinos"));
	scenes.push_back(new scenePirose(&config, "pirose"));
	scenes.push_back(new sceneSyntype(&config, "syntype"));
}

#endif
