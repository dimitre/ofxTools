

static void calcNormals( ofMesh & mesh, bool bNormalize ){
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
		glm::vec3 e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
		glm::vec3 e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
//		glm::vec3 no = glm::cross(e2, e1);
		glm::vec3 no = glm::cross(e1, e2);
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
	
	map <string, float> incrementadorTemporal;
	float incrementa(string qual) {
		string uniqueId = uiC->uiName + qual;
		incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
		return incrementadorTemporal[uniqueId];
	}
	
	glm::vec2 middle;
	
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
		// ofSetColor(getColor(ofRandom(0,1),ui));
		// svg.draw();

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
	}

	void uiEvents(ofxMicroUI::element & e) override {
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





#ifdef USEASSIMP
struct sceneModel : public sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	string loadedFile = "";
	ofxAssimpModelLoader model;
	ofMesh mesh;

	
	void setup() override {
		model.drawFaces();
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(255);
		model.update();
		
		ofPushMatrix();
		ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
		ofRotateDeg(-0, 0, 1, 0);
		ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
		ofPushStyle();
		model.drawFaces();
		ofPopStyle();
//		model.draw();
		ofPopMatrix();

		
////		mesh = model.getCurrentAnimatedMesh(0);
//
//		ofxAssimpMeshHelper & meshHelper = model.getMeshHelper(0);
//
//		ofMultMatrix(model.getModelMatrix());
//		 ofMultMatrix(meshHelper.matrix);
//
//		 ofMaterial & material = meshHelper.material;
//		 if(meshHelper.hasTexture()){
//			 meshHelper.getTextureRef().bind();
//		 }
////		 material.begin();
////		 mesh.drawWireframe();
////		 material.end();
//		 if(meshHelper.hasTexture()){
//			 meshHelper.getTextureRef().unbind();
//		 }
		
//		ofPushMatrix();
//		model.drawFaces();
//		ofPopMatrix();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "model") {
			if (*e.s != "") {
				string file = ((ofxMicroUI::dirList*)&e)->getFileName();
				if (loadedFile != file && ofFile::doesFileExist(file)) {
					cout << file << endl;
					model.loadModel(file, false);
					model.setPosition(middle.x, middle.y , 0);
					model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
					model.playAllAnimations();
				}
			}
		}
	}
};

#endif




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

