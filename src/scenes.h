
struct sceneDmtr {
public:
	
//	friend class sceneConfig;
	
	#include "polar.h"
	
	bool isSetup = false;
	ofxMicroUI * u = NULL;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * ui = NULL;
	ofxMicroUI * uiColors = NULL;
	ofFbo * fbo = NULL;
	glm::vec2 middle;
	map <string, float> incrementadorTemporal;
	
	sceneConfig * config = NULL;
	
	sceneDmtr() { }

	sceneDmtr(sceneConfig * _c) : config(_c) {
		u = config->u;
		ui = &u->uis["ui"];
		uiC = &u->uis["scene"];
		uiColors = &u->uis["colors"];
		fbo = config->fbo;
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}
	
	sceneDmtr(ofxMicroUI * _u, ofFbo * _fbo) : u(_u), fbo(_fbo) {
		ui = &u->uis["ui"];
		uiC = &u->uis["scene"];
		uiColors = &u->uis["colors"];
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
	}
	
	// TODO
	float getFreq(int index) {
		return 0;
	}
	
	ofColor getCor(float n) {
		return getColor(n, uiColors);
	}


	float incrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
//		incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
		// 07/09/2020
		incrementadorTemporal[uniqueId] += uiC->pEasy[qual];
		return incrementadorTemporal[uniqueId];
	}

	void resetIncrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] = 0;
	}



	virtual void checkSetup() {
		if (!isSetup) {
			setup();
			isSetup = true;
		}
	}

	virtual void uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvent primitive" << endl;
	}
	
	virtual void setup() {
		isSetup = true;
	}

	virtual void update() {
		
	}
	
	virtual void draw() {
		checkSetup();
	}
};



#ifdef USESVG
struct sceneSvg : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	string loadedFile = "";

	// ofxSVG svg;
	// vector<ofPolyline> outlines;
	// ofFbo fboSvg;
	
	glm::vec2 positions[9] = { glm::vec2(0,0) };

	struct svgPoly {
		public:
		ofxSVG svg;
		vector<ofPolyline> outlines;
		float offX = 0;
		float offY = 0;

		void load(string fileName) {
			svg.load(fileName);
			outlines.clear();
			for (ofPath p: svg.getPaths()){
				// svg defaults to non zero winding which doesn't look so good as contours
				p.setPolyWindingMode(OF_POLY_WINDING_ODD);
				const vector<ofPolyline>& lines = p.getOutline();
				for(const ofPolyline & line: lines){
					outlines.push_back(line.getResampledBySpacing(1));
				}
			}
		}

		int getWidth() {
			return svg.getWidth();
		}

		int getHeight() {
			return svg.getHeight();
		}
	};

	svgPoly * _svg = NULL;
	map <string, svgPoly> svgMap;


//#include "DmtrCairo.h"
	
	void setup() override {

		ofDirectory dir;
		dir.allowExt("svg");
		dir.listDir("_svg");
		dir.sort();
		cout << "SETUP SCENESVG =============" << endl;
		for (auto & d : dir) {
			string name = d.getFileName();
			svgMap[name].load("_svg/" + d.getFileName());
		}
	}
	
	void draw() override {
		checkSetup();
		if (uiC->pBool["middle"]) {
			ofTranslate(middle);
		}
		// ofSetColor(getColor(ofRandom(0,1),uiColors));
		// svg.draw();
		float scale = uiC->pEasy["scaleSvg"];
		if (scale > 0) {
		} else {
			scale = uiC->pEasy["scaleSvg"] = uiC->pFloat["scaleSvg"];
		}
		ofScale(scale, scale);
//		 cout << scale << endl;

		ofRotateDeg(uiC->pEasy["rot"]);
		// float offX = fmod(incrementa("velX"), svg.getWidth());
		// float offY = fmod(incrementa("velY"), svg.getHeight());
//		_svg->offX = fmod(incrementa("velX"), _svg->getWidth());
//		_svg->offY = fmod(incrementa("velY"), _svg->getHeight());

		
		_svg->offX = fmod(_svg->offX + uiC->pEasy["velX"], _svg->getWidth());
		_svg->offY = fmod(_svg->offY + uiC->pEasy["velY"], _svg->getHeight());

		

		ofTranslate(uiC->pEasy["offX"] + _svg->offX, uiC->pEasy["offY"] + _svg->offY);

		if (uiC->pBool["pattern"]) {
			
//			if (fboSvg.isAllocated()) {
//				cout << "allocated" << endl;
//			} else {
//				cout << "not allocated" << endl;
//			}
//			fboSvg.begin();
//			ofClear(ofColor(0,0));
//			ofClear(255, 0, 0);
			
//			beginCairo();
//			svgDraw();
//			ofDrawLine(0,0,300,300);
//			endCairo();
//			fboSvg.end();
			
//			ofSetColor(255);
			
//			float spaceX = uiC->pEasy["spaceX"];
//			float spaceY = uiC->pEasy["spaceY"];

//			ofRectangle rect = ofRectangle(0,0, fboSvg.getWidth() * scale, fboSvg.getHeight() * scale);

			for (int x=-1; x<1; x++) {
				for (int y=-1; y<1; y++) {
					float xx = x * _svg->getWidth() * scale;
					float yy = y * _svg->getHeight() * scale;
//					cout << xx << endl;
//					cout << yy << endl;
//					cout << "-----" << endl;
//					void ofTexture::drawSubsection(const ofRectangle &drawBounds, const ofRectangle &subsectionBounds)
//					fboSvg.getTexture().drawSubsection(rect, rect);
					
//					fboSvg.draw(xx, yy);
					ofPushMatrix();
					ofTranslate((x) * _svg->getWidth(), (y) * _svg->getHeight());
//					fbo.draw(0,0);
					svgDraw();
					ofPopMatrix();
				}
			}
		} else {
			svgDraw();
		}
//		ofScale(1.0, 1.0);
	}

	void svgDraw() {
		int i = 0;

		for (auto & p : _svg->outlines) {
			float n = ofNoise(ofGetElapsedTimef() * uiC->pFloat["tempoNoise"], i * uiC->pFloat["pathNoise"]);
			if (n > uiC->pEasy["drawLimite"]) {
				ofSetColor(ofMap(n, uiC->pEasy["drawLimite"], 1, 0, 255 ));
				ofSetColor(getColor(ofMap(n, uiC->pEasy["drawLimite"], 1, 0, 1 ), uiColors));
				p.draw();
			}
			i++;
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
//		cout << e.name << endl;
		if (e.name == "svg") {
			if (*e.s != "") {
				_svg = &svgMap[*e.s];
// 				string file = ((ofxMicroUI::dirList*)&e)->getFileName();
// 				if (loadedFile != file && ofFile::doesFileExist(file)) {
// 					cout << "SVG LOADED!" << file << endl;

// 					svg.load(file);
// 					fboSvg.allocate(svg.getWidth(), svg.getHeight(), GL_RGBA);
// //					setupCairo(svg.getWidth(), svg.getHeight());
					
// 					loadedFile = file;
// 					outlines.clear();
// 					for (ofPath p: svg.getPaths()){
// 						// svg defaults to non zero winding which doesn't look so good as contours
// 						p.setPolyWindingMode(OF_POLY_WINDING_ODD);
// 						const vector<ofPolyline>& lines = p.getOutline();
// 						for(const ofPolyline & line: lines){
// 							outlines.push_back(line.getResampledBySpacing(1));
// 						}
// 					}
// 				}
			}
		}
	}
};
#endif





struct sceneImage : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	ofImage * i;

	// TabNine::Config
	ofTexture * tex;
	void setup() override {
		tex = &uiC->pImage["image"].getTexture();
		i = &uiC->pImage["image"];
	}

	void drawTexture() {
		tex->bind();
		//GL_MIRRORED_REPEAT GL_CLAMP_TO_EDGE GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ofMesh mesh;
		glm::vec3 vertices[4] = {
			glm::vec3(0, 0, 0),
			glm::vec3(fbo->getWidth(), 0, 0),
			glm::vec3(0, fbo->getHeight(), 0),
			glm::vec3(fbo->getWidth(), fbo->getHeight(), 0)
		};

		float tw = uiC->pFloat["tw"];
		float th = uiC->pFloat["th"];
		mesh.addIndices( {0,1,2,1,2,3} );

		glm::vec3 texCoords[4] = {
			glm::vec3(0, 0, 0),
			glm::vec3(tw, 0, 0),
			glm::vec3(0, th, 0),
			glm::vec3(tw, th, 0)
		};

		for (auto & v : vertices) {
			mesh.addVertex(v);
		}
		for (auto & t : texCoords) {
			mesh.addTexCoord(t);
		}
		for (int a=0; a<4; a++) {
			mesh.addVertex(vertices[a]);
		}

		mesh.draw();
		tex->unbind();
	}
	
	vector <string> indices = { "", "2" };

	
	void draw() override {
		checkSetup();

		ofSetColor(255);
		if (uiC->pBool["useTexture"]) {
			drawTexture();
		}
		else {
			for (string & index : indices) {
				i = index == "" ? &uiC->pImage["image"] : &uiC->pImage["image2"];
				if (uiC->pBool["ok" + index] && i->isAllocated()) {
					// string index = "";
					float scale = uiC->pEasy["scale" + index];
					float iw = i->getWidth() * scale;
					float ih = i->getHeight() * scale;
					float iwMargin = iw * uiC->pEasy["margin" + index];
					float ihMargin = ih * uiC->pEasy["margin" + index];

					// TODO UPDATE
					int vx = fbo->getWidth() / iwMargin;
					int vy = fbo->getHeight() / ihMargin;
					int total = vx * vy;
					int imageNumber = 0;
					float rot = uiC->pEasy["rot"+ index] + incrementa("rotTime"+ index);

					float offX = fmod(incrementa("velX" + index), iwMargin) - iwMargin *.5;
					float offY = fmod(incrementa("velY" + index), ihMargin) - ihMargin *.5;

					for (int x = -1; x<= (vx+1); x++) {
						for (int y = -1; y<= (vy+1); y++) {
							if ((x+y)%2 || !uiC->pBool["impar"]) {
								ofSetColor(getColor(imageNumber/(float)total, uiColors));
								ofPushMatrix();
								ofTranslate(
									x * iwMargin + iwMargin * .5 + offX, 
									y * ihMargin + ihMargin * .5 + offY
								);
								ofRotateDeg(rot);
								uiC->pImage["image"+ index].draw(-iw*.5, -ih * .5, iw, ih);
								ofPopMatrix();
								imageNumber++;
							}
						}
					}
				}
			}
		}

	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "resetRot") {
			resetIncrementa("rotTime");
		}
		else if (e.name == "resetRot2") {
			resetIncrementa("rotTime2");
		}
	}
};


struct sceneVideo : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	int vw, vh;
	int vx, vy, total;
	
	ofVideoPlayer video;
	string loadedFile = "";

	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		video.update();
		// video.draw();

		float rot = uiC->pFloat["rot"] + incrementa("rotTime");

		vw = video.getWidth() * uiC->pEasy["cropX"];
		vh = video.getHeight()* uiC->pEasy["cropY"];
		// cout << vw << endl;

		vx = fbo->getWidth() / vw;
		vy = fbo->getHeight() / vh;
		total = vx * vy;

		int index = 0;
		for (int x =0; x<=vx; x++) {
			for (int y =0; y<=vy; y++) {
				ofSetColor(getColor(index/(float)total, uiColors));
				ofPushMatrix();
				ofTranslate(x * vw + vw*.5, y * vh + vh * .5);
				ofRotateDeg(rot);
				video.draw(-vw*.5, -vh * .5);
				ofPopMatrix();
				index++;
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "resetRot") {
			resetIncrementa("rotTime");
		}
		if (e.name == "video") {
			string f = ((ofxMicroUI::dirList*)&e)->getFileName();
			//string f = uiL->getFileFullPath(e.name);
			if (f != "" && loadedFile != f) {
				loadedFile = f;
				video.load(f);
				video.play();
				vw = video.getWidth();
				vh = video.getHeight();

				vx = fbo->getWidth() / vw;
				vy = fbo->getHeight() / vh;
				total = vx * vy;
			}
		}
	}
};



struct sceneConfetti : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	struct confetti {
		public:
			int index;
			ofRectangle * rect = NULL;
			float * vel = NULL;
			float * velX = NULL;
			glm::vec2 pos;
			float mult;
			ofRectangle objeto;
			glm::vec3 rot, rotVel;

			ofVboMesh mesh;
			ofPlanePrimitive plane;
		
		float raio, raio2;

		confetti(int i, ofRectangle * r, float * v, float * vx) : index(i), rect(r), vel(v), velX(vx) {
			mult = ofRandom(.1, 2.0);
			pos = glm::vec2(ofRandom(rect->x, rect->width), ofRandom(rect->y, rect->height));
			// float raio = mult*11.0;
			// objeto = ofRectangle(-raio, -raio, raio*2.0, raio*2.0);
			raio = mult*11.0;
//			raio2 = raio * ofRandom(.2, .8);
			raio2 = raio * .33;
			objeto = ofRectangle(-raio*.5, -raio2*.5, raio*2, raio2*2.0);


			plane.set(raio*2, raio2*2);
			plane.setResolution(5, 5);
			mesh = plane.getMesh();

			rot = glm::vec3(
				ofRandom(0,180),
				ofRandom(0,180),
				ofRandom(0,180)
			);

			rotVel = glm::vec3(
				ofRandom(0, 3),
				ofRandom(0, 3),
				ofRandom(0, 3)
			);
		}

		void draw() {
			rot += rotVel;
			float soma = *vel * mult;
			float somaX = *velX * mult;
			pos.y += soma;
			pos.x += somaX;
			if (pos.y > rect->height) {
				pos.y = rect->y;
			}
			else if (pos.y < rect->y) {
				pos.y = rect->height;
			}

			if (pos.x > rect->width) {
				pos.x = rect->x;
			}
			else if (pos.x < rect->x) {
				pos.x = rect->width;
			}


		}
	};

	vector <confetti> confettis;

	int margem = 100;
	ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);
	void setup() override {
		for (auto a=0; a<3600; a++) {
			confettis.push_back(confetti(a, &boundsRect, &vel, &velX));
		}
	}
	
	float vel = 0;
	float velX = 0;

	void draw() override {
		checkSetup();
		vel = uiC->pEasy["vel"];
		velX = uiC->pEasy["velX"];
		ofSetColor(255);
		float scale = uiC->pEasy["scale"];
		for (auto & c : confettis) {
			if (c.index < uiC->pEasy["numero"]) {
				if (uiC->pInt["colorMode"] == 0) {
					ofSetColor(getColor(c.mult, uiColors));
				} else if (uiC->pInt["colorMode"] == 1) {
					ofSetColor(getColor(ofRandom(0,1), uiColors));
				} else if (uiC->pInt["colorMode"] == 2) {
					float n = ofNoise(incrementa("tempoColor"), c.mult, c.pos.x * .1);
					ofSetColor(getColor(n, uiColors));
				}
				
				c.draw();
				
				ofPushMatrix();
				ofTranslate(c.pos);
				ofRotateXDeg(c.rot.x);
				ofRotateYDeg(c.rot.y);
				ofRotateZDeg(c.rot.z);

				 if (uiC->pBool["circle"]) {
					ofDrawCircle(0, 0, c.raio*scale, c.raio*scale);
				 } else {
					c.mesh.draw();
				 }

				ofPopMatrix();
			}
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneToma : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	struct coisa {
		public:
			//ofConePrimitive::ofConePrimitive(float radius, float height, int radiusSegments, int heightSegments, int capSegments=2, ofPrimitiveMode mode=OF_PRIMITIVE_TRIANGLE_STRIP)
			ofConePrimitive cone = ofConePrimitive(12, 10, 24, 2);

			// ofBoxPrimitive::ofBoxPrimitive(float width, float height, float depth, int resWidth=2, int resHeight=2, int resDepth=2)
			ofBoxPrimitive box = ofBoxPrimitive(10, 10, 10);

			//ofSpherePrimitive::ofSpherePrimitive(float radius, int res, ofPrimitiveMode mode=OF_PRIMITIVE_TRIANGLE_STRIP)
			ofSpherePrimitive sphere = ofSpherePrimitive(5, 48);

			// ofCylinderPrimitive::ofCylinderPrimitive(float radius, float height, int radiusSegments, int heightSegments, int capSegments=2, bool bCapped=true, ofPrimitiveMode mode=OF_PRIMITIVE_TRIANGLE_STRIP)
			ofCylinderPrimitive cylinder = ofCylinderPrimitive(10, 10, 24, 2);

			int tipo;
			ofColor cor;
			glm::vec3 pos;
			glm::vec3 rot;
			void draw() {
				ofSetColor(cor);
				ofPushMatrix();
				ofTranslate(pos);
				ofRotateXDeg(rot.x);
				ofRotateYDeg(rot.y);
				ofRotateZDeg(rot.z);
				if (tipo == 0) {
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*3)), 0, 1, 5, 0);
					rot.y += 2.5;
					cone.draw();
				}
				else if (tipo == 1) {	
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*5)), 0, 1, 5, 0);
					// rot.y = ofMap(sin(ofGetElapsedTimef()*.2),0, 1, 0, 90);
					rot.x += .3;
					rot.y += .4;
					rot.z += .5;
					box.draw();
				}
				else if (tipo == 2) {
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*4)), 0, 1, 5, 0);
					sphere.draw();
				}
				else if (tipo == 3) {
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*6)), 0, 1, 5, 0);

					cylinder.draw();
				}
				ofPopMatrix();
			}

			coisa(int t, ofColor c, glm::vec3 p) : tipo(t), cor(c), pos(p) {};
	};

	float dist = 30;
	vector <coisa> coisas = { 
		coisa(0, ofxMicroUI::stringHexToColor("#fedc61"), glm::vec3(0, 0, 0)),
		coisa(1, ofxMicroUI::stringHexToColor("#ae3fac"), glm::vec3(dist, 0, 0)),
		coisa(2, ofxMicroUI::stringHexToColor("#453fb0"), glm::vec3(-dist, 0, 0)),
		coisa(3, ofxMicroUI::stringHexToColor("#008d54"), glm::vec3(0, 0, -dist)) 
	};

	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		for (auto & c : coisas) {
			c.draw();
		}
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
		ofSetColor(getColor(0, uiColors));
		ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




// TENTEI PORTAR mas nao funciona ainda nao sei porque
struct sceneRibbon : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	ofImage * ribbon1;
	ofImage * ribbon2;
	
	void setup() override {
		ofDisableArbTex();
		//	ribbon1.loadImage("ribbon.jpg");
		// ribbon1.loadImage("ribbon_texture1.tiff");
		// ribbon2.loadImage("ribbon_texture2.tiff");
	}
	
	void draw() override {
		checkSetup();

		ribbon1 = &uiC->pImage["tex1"];
		ribbon2 = &uiC->pImage["tex2"];
		ribbon1->getTexture().setTextureWrap( GL_REPEAT, GL_REPEAT );
		ribbon2->getTexture().setTextureWrap( GL_REPEAT, GL_REPEAT );

		ofMesh mesh2;
//		mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		
		int ww = uiC->pEasy["maxX"];
		int hh = uiC->pEasy["maxY"];
		for (int y=0; y<hh; y++) {
			for (int x=0; x<ww; x++) {

				ofVec3f ponto = ofVec3f(x, y, 0);

				// cout << ponto << endl;
				ofVec3f ponto2 = ponto.getRotated(y * uiC->pEasy["multZ"], ofVec3f(0,0,1));
				ofVec3f ponto3 = ponto2.getRotated(y * uiC->pEasy["multY"], ofVec3f(0,1,0));
				ofVec3f ponto4 = ponto3.getRotated(y * uiC->pEasy["multX"], ofVec3f(1,0,0));

				mesh2.addVertex(ponto4 * uiC->pEasy["multiplicador"]);
				mesh2.addTexCoord(ofVec2f(y, x / (float)ww));

				mesh2.addIndex(x+y*ww);				// 0
				if (x<ww-1)
					mesh2.addIndex((x+1)+y*ww);			// 1
				if (y<hh-1)
					mesh2.addIndex(x+(y+1)*ww);			// 10

				if (x<ww-1)
					mesh2.addIndex((x+1)+y*ww);			// 1
				if (x<ww-1 && y<hh-1)
					mesh2.addIndex((x+1)+(y+1)*ww);		// 11
				if (y<hh-1)
					mesh2.addIndex(x+(y+1)*ww);			// 10
				


			}
		}
		
		mesh2.enableTextures();

		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		if (uiC->pBool["useTex"]) {
			ribbon1->getTexture().bind();
		}
		if (uiC->pString["draw"] == "wire") {
			mesh2.drawWireframe();
		}
		if (uiC->pString["draw"] == "faces") {
			mesh2.drawFaces();
		}
		if (uiC->pString["draw"] == "points") {
			mesh2.draw(OF_MESH_POINTS);
		}
		if (uiC->pBool["useTex"]) {
			ribbon1->getTexture().unbind();
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		
		
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		if (uiC->pBool["useTex"]) {
			ribbon2->getTexture().bind();
		}

		if (uiC->pString["draw"] == "wire") {
			mesh2.drawWireframe();
		}
		if (uiC->pString["draw"] == "faces") {
			mesh2.drawFaces();
		}
		if (uiC->pString["draw"] == "points") {
			mesh2.draw(OF_MESH_POINTS);
		}

		if (uiC->pBool["useTex"]) {
			ribbon2->getTexture().bind();
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);		
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneNav : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	

	struct nav {
		public:

		glm::vec3 pos = glm::vec3(0,0,0);
		glm::vec3 vertices[20] = { glm::vec3(0,0,0) };
		
//		ofMesh mesh;
		nav() {
//			mesh.setMode(OF_PRIMITIVE_LINES);
//			mesh.disableColors();
		}

		int cursor = 0;

		ofPolyline poly;
		
		void draw() {
	        glm::vec4 Position = glm::vec4(pos, 1.0f);
			float angleX = ofDegToRad(ofRandom(-1, 1));
			glm::mat4 Rot = glm::rotate(glm::mat4(1.0f), (angleX), glm::vec3(1.0, 0.0, 0.0));
			float angleY = ofDegToRad(ofRandom(-1, 1));
			glm::mat4 RotY = glm::rotate(glm::mat4(1.0f), (angleX), glm::vec3(0, 1.0, 0.0));
			float angleZ = ofDegToRad(ofRandom(-1, 1));
			glm::mat4 RotZ = glm::rotate(glm::mat4(1.0f), (angleX), glm::vec3(0.0, 0.0, 1.0));

       		glm::mat4 Model = glm::translate( glm::mat4(1.0f), glm::vec3(0, 0.0f, 1.0f));
			glm::vec4 Transformed = Position * Model * Rot * RotY * RotZ;
			pos = glm::vec3(Transformed) / Transformed[3];
			vertices[cursor%20] = pos;
//			cout << pos << endl;
//
			poly.clear();
////			mesh.clear();
			for (int a=0; a<20; a++) {
				int index = (a + cursor) % 20;
				poly.addVertex(vertices[index]);
			}
////			if (cursor > 20)
//			{
////				mesh.drawWireframe();
				poly.draw();
//			}
			cursor ++;
		}
	};

	nav navs[35] = { nav() };

	void setup() override {
	}
	
	void draw() override {
		checkSetup();

		ofSetColor(getColor(0, uiColors));
		for (auto & n : navs) {
			n.draw();
		}
		// ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};





struct sceneRandom : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	int margem = 500;
	// boundsrect quadrado aqui.
	ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem*2, fbo->getWidth() + margem*2);
	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();

		ofTranslate(fbo->getWidth()* .5 , fbo->getHeight()* .5);
		ofRotateDeg(uiC->pEasy["rot"]);

		float tx = incrementa("tempoX") / 100.0;
		float ty = incrementa("tempoY") / 100.0;
		ofSetRectMode(OF_RECTMODE_CENTER);
		if (uiC->pBool["noise"]) {
			for (int a=0; a<uiC->pFloat["numero"]; a++) {
				float w = ofNoise(a * uiC->pEasy["noiseWA"], tx) * fbo->getWidth() * uiC->pEasy["w"];
				float h = ofNoise(a * uiC->pEasy["noiseHA"], ty) * fbo->getHeight() * uiC->pEasy["h"];
				float x = -fbo->getWidth()* .5 + boundsRect.x + ofNoise(a * uiC->pEasy["noiseXA"], tx) * boundsRect.width;
				float y = -fbo->getHeight()* .5 + boundsRect.x + ofNoise(a * uiC->pEasy["noiseYA"], ty) * boundsRect.height;
				ofSetColor(getColor(a * uiC->pEasy["aColor"] + uiC->pEasy["offColor"], uiColors));
				ofDrawRectangle(x,y,w,h);
			}
		} else {
			for (int a=0; a<uiC->pFloat["numero"]; a++) {
				float x = -fbo->getWidth()* .5 + ofRandom(boundsRect.x, boundsRect.width);
				float y = -fbo->getHeight()* .5 + ofRandom(boundsRect.x, boundsRect.width);
				float w = ofRandom(0,fbo->getWidth()*.5);
				float h = ofRandom(0,fbo->getHeight()*.5);
				ofSetColor(getColor(ofRandom(0,1), uiColors));
				ofDrawRectangle(x, y, w, h);
			}
		}
		ofSetRectMode(OF_RECTMODE_CORNER);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};




struct sceneText : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofTrueTypeFont font;
	ofFbo fboText;

	void setup() override {
		fboText.allocate(300,600, GL_RGBA);
		fboText.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		font.load("_ui/mono08_55.ttf", 7);
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(getColor(0, uiColors));
		// ofDrawBitmapString(uiC->pText["text"], 20, 20);
		fboText.begin();
		ofClear(0,0);
		
		string s = uiC->pBool["upper"] ? ofToUpper(uiC->pText["text"]) : uiC->pText["text"];
		uiC->pFont["font"].drawString(s, 4, 16);
//		font.drawString(uiC->pText["text"], 4, 16);
		fboText.end();

		int vezes = uiC->pInt["vezes"];
		fboText.draw(0,0,fboText.getWidth() * vezes, fboText.getHeight() * vezes);


		// cout << uiC->pText["text"] << endl;
		// ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "fontSize") {
			ofxMicroUI::fontList * _f = ((ofxMicroUI::fontList*)uiC->getElement("font"));
			if (_f != NULL) {
				((ofxMicroUI::fontList*)uiC->getElement("font"))->size = *e.i;
				((ofxMicroUI::fontList*)uiC->getElement("font"))->reload();
			}
			// font.load("_ui/mono08_55.ttf", *e.i);
		}
	}
};


struct sceneTextFile : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofTrueTypeFont font;

	void setup() override {
		font.load("_ui/mono08_55.ttf", 7);
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(255);
		string s = uiC->pBool["upper"] ? ofToUpper(uiC->pText["text"]) : uiC->pText["text"];
		uiC->pFont["font"].drawString(s, uiC->pInt["offX"], uiC->pInt["offY"]);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "fontSize") {
			ofxMicroUI::fontList * _f = ((ofxMicroUI::fontList*)uiC->getElement("font"));
			if (_f != NULL) {
				((ofxMicroUI::fontList*)uiC->getElement("font"))->size = *e.i;
				((ofxMicroUI::fontList*)uiC->getElement("font"))->reload();
			}
		}
	}
};


struct sceneNo : public sceneDmtr {
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








struct sceneGrad : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofMesh fundoMesh;

	void setup() override {
		fundoMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	}
	
	void draw() override {
		checkSetup();
		int paletteSize = ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getPaletteSize();
		fundoMesh.clear();
		for (int p=0; p<=paletteSize; p++) {
			float y = ofMap(p, 0, paletteSize, 0, fbo->getHeight());
			ofColor color = ((ofxMicroUI::colorPalette*)uiColors->getElement("colorPalette"))->getColorByIndex(p);
			// cout << y << endl;
			fundoMesh.addVertex(glm::vec3(0, y, 0));
			fundoMesh.addColor(color);
			fundoMesh.addVertex(glm::vec3(fbo->getWidth(), y, 0));
			fundoMesh.addColor(color);
		}
		// fundoMesh.addVertex(glm::vec3(0, 0, 0));
		// fundoMesh.addColor(uiColors->pColorEasy["bg"]);
		// fundoMesh.addVertex(glm::vec3(fbo->getWidth(), 0, 0));
		// fundoMesh.addColor(uiColors->pColorEasy["bg"]);
		// fundoMesh.addVertex(glm::vec3(0, fbo->getHeight(), 0));
		// fundoMesh.addColor(uiColors->pColorEasy["bg2"]);
		// fundoMesh.addVertex(glm::vec3(fbo->getWidth(), fbo->getHeight(), 0));
		// fundoMesh.addColor(uiColors->pColorEasy["bg2"]);
		fundoMesh.draw();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};

struct sceneClaquete : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	ofFbo fboText;

	void setup() override {
		fboText.allocate(100,100, GL_RGBA);

		fboText.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	}
	
	void draw() override {
		checkSetup();
//		ofBackground(255,0,0);
		
		fboText.begin();
		ofClear(0,0);
		ofSetColor(getColor(ofRandom(0,1), uiColors));
		ofSetColor(getColor(0, uiColors));
//		cout <<  << endl;
		ofDrawBitmapString("OK Dmtr.org\n" + uiC->pString["text"], 4, 18);
		fboText.end();
		
		int vezes = uiC->pInt["vezes"];
		fboText.draw(0,0,fboText.getWidth() * vezes,fboText.getHeight() * vezes);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
//		cout << "text " << e.name << endl;
		if (e.name == "text") {

		}

	}
};





struct sceneSvank : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(getColor(0, uiColors));
		ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};


