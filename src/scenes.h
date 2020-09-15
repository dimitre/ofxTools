

static void calcNormals( ofMesh & mesh, bool bNormalize, bool mode){
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
		glm::vec3 e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
		glm::vec3 e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
		// depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped.
		glm::vec3 no = mode ? glm::cross(e2, e1) : glm::cross(e1, e2);
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

static void drawMeshStatic(ofVboMesh * m, ofxMicroUI * ui) {
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
	#include "polar.h"
	
	bool isSetup = false;
	ofxMicroUI * u = NULL;
	ofxMicroUI * uiC = NULL;
	ofxMicroUI * ui = NULL;
	ofxMicroUI * uiColors = NULL;
	ofFbo * fbo = NULL;
	float updown = 0.0;
	glm::vec2 middle;
	map <string, float> incrementadorTemporal;

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
	
	ofColor getCor(float n) {
		return getColor(n, uiColors);
	}

	sceneDmtr() { }

	sceneDmtr(ofxMicroUI * _u, ofFbo * _fbo) : u(_u), fbo(_fbo) {
		ui = &u->uis["ui"];
		uiC = &u->uis["scene"];
		uiColors = &u->uis["colors"];
		middle = glm::vec2(fbo->getWidth() * .5, fbo->getHeight() * .5);
		ofAddListener(uiC->uiEvent, this, &sceneDmtr::uiEvents);
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
	
	virtual void draw() {
		checkSetup();
	}
};



#ifdef USESVG
struct sceneSvg : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	ofxSVG svg;
	string loadedFile = "";
	vector<ofPolyline> outlines;

	void setup() override {

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
			ofScale(scale, scale);
		}
//		 cout << scale << endl;
		
		int i = 0;
		for (auto & p : outlines) {
			float n = ofNoise(ofGetElapsedTimef() * uiC->pFloat["tempoNoise"], i * uiC->pFloat["pathNoise"]);
			if (n > uiC->pEasy["drawLimite"]) {
			// if (ofRandom(0,1) > .5) {
				ofSetColor(ofMap(n, uiC->pEasy["drawLimite"], 1, 0, 255 ));
				
				ofSetColor(getColor(ofMap(n, uiC->pEasy["drawLimite"], 1, 0, 1 ), uiColors));
				p.draw();
			}
			i++;
		}
//		ofScale(1.0, 1.0);
	}

	void uiEvents(ofxMicroUI::element & e) override {
//		cout << e.name << endl;
		if (e.name == "svg") {
			if (*e.s != "") {
				string file = ((ofxMicroUI::dirList*)&e)->getFileName();
				cout << file << endl;
				if (loadedFile != file && ofFile::doesFileExist(file)) {
					svg.load(file);
					loadedFile = file;
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
		if (i->isAllocated()) {
			ofSetColor(255);
			if (uiC->pBool["useTexture"]) {
				drawTexture();
			}
			else {
				for (string & index : indices) {
					i = index == "" ? &uiC->pImage["image"] : &uiC->pImage["image2"];
					// string index = "";
					float scale = uiC->pEasy["scale" + index];
					float iw = i->getWidth() * scale;
					float ih = i->getHeight() * scale;
					int vx = fbo->getWidth() / iw;
					int vy = fbo->getHeight() / ih;
					int total = vx * vy;
					int i = 0;
					float rot = uiC->pEasy["rot"+ index] + incrementa("rotTime"+ index);
					for (int x =0; x<=vx; x++) {
						for (int y =0; y<=vy; y++) {
							ofSetColor(getColor(i/(float)total, uiColors));
							ofPushMatrix();
							ofTranslate(x * iw + iw*.5, y * ih + ih * .5);
							ofRotateDeg(rot);
							uiC->pImage["image"+ index].draw(-iw*.5, -ih * .5, iw, ih);
							ofPopMatrix();
							i++;
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

		confetti(int i, ofRectangle * r, float * v, float * vx) : index(i), rect(r), vel(v), velX(vx) {
			mult = ofRandom(.1, 2.0);
			pos = glm::vec2(ofRandom(rect->x, rect->width), ofRandom(rect->y, rect->height));
			// float raio = mult*11.0;
			// objeto = ofRectangle(-raio, -raio, raio*2.0, raio*2.0);
			float raio = mult*11.0;
			float raio2 = raio * ofRandom(.2, .8);
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

			ofPushMatrix();
			ofTranslate(pos);
			ofRotateXDeg(rot.x);
			ofRotateYDeg(rot.y);
			ofRotateZDeg(rot.z);

			// if (uiC->pBool["mesh"]) {
				mesh.draw();
			// } else {
				// ofDrawRectangle(objeto);
			// }

			ofPopMatrix();
		}
	};

	vector <confetti> confettis;

	int margem = 100;
	ofRectangle boundsRect = ofRectangle(-margem, -margem, fbo->getWidth() + margem, fbo->getHeight() + margem);
	void setup() override {
		for (auto a=0; a<2600; a++) {
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
		for (auto & c : confettis) {
			if (uiC->pInt["colorMode"] == 0) {
				ofSetColor(getColor(c.mult, uiColors));
			} else if (uiC->pInt["colorMode"] == 1) {
				ofSetColor(getColor(ofRandom(0,1), uiColors));
			} else if (uiC->pInt["colorMode"] == 2) {
				float n = ofNoise(incrementa("tempoColor"), c.mult, c.pos.x * .1);
				ofSetColor(getColor(n, uiColors));
			}
			c.draw();
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
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*1.5)), 0, 1, 5, 0);
					rot.y += 3;
					cone.draw();
				}
				else if (tipo == 1) {	
					pos.y = ofMap(abs(sin(ofGetElapsedTimef()*2)), 0, 1, 5, 0);
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


