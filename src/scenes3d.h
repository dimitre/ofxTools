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




struct sceneOcean : public ofxScenes::sceneDmtr, ofxScenes::sceneIncrementa  {
public:
	using sceneDmtr::sceneDmtr;
	// name = "ocean";

	ofVboMesh mesh;
	int width = 105;
	float multiplicador = 0.4;
	
	struct param {
	public:
		float signal = 1;
		float noiseMult;
		float multY;
		float easy;
		float ny;
		float my;
		string i;
		bool ok = false;
//		param();
	};
	
	param params[4];
	float hashValuesX[200];
	float updown = 0;
	
	void addNormals() {
		for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(glm::vec3(0,0,0));
	}
	
	void setupParams() {
		for (int a=0; a<3; a++) {
			string i = ofToString(a+1);
			params[a].i = i;
			params[a].signal = a%2 ? -1 : 1;
		}
	}
	
	void setup() override {
		setupParams();
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
	}
	
	float shaper(float in, float inMin, float inMax, float outMin, float outMax, float shaper){
		// (1) convert to pct (0-1)
		float pct = ofMap (in, inMin, inMax, 0, 1, true);
		// raise this number to a power
		pct = powf(pct, shaper);
		float out = ofMap(pct, 0,1, outMin, outMax, true);
		return out;
	}

	void draw() override {
		checkSetup();
		for (auto & p : params) {
			p.ok = uiC->pBool["ok" + p.i];
			if (p.ok) {
				p.noiseMult = uiC->pEasy["noiseMult" + p.i];
				p.multY = uiC->pEasy["multY" + p.i];
				p.easy = incrementa("multiVel" + p.i) + uiC->pFloat["velManual" + p.i];
			}
		}
		
		float offXTime = incrementa("offXTime");
		float offYTime = incrementa("offYTime");
//		for (int x=0; x<width; x++) {
//			hashValuesX[x] = 0;
//			for (auto & p : params) {
//				hashValuesX[x]  += x * p.noiseMult + offXTime;
//			}
//		}
		
		
		for (int y=0; y<width; y++) {
			for (auto & p : params) {
				if (p.ok) {
					p.ny = y * p.noiseMult + offYTime;
					p.my = multiplicador * p.multY;
				}
			}

			for (int x=0; x<width; x++) {
				int index = x + y*width;
				glm::vec3 tmpVec = mesh.getVertex(index);
				
				// mesmo no X fazer hashtable se quiser.
				tmpVec.y = 0;
				for (auto & p : params) {
					if (p.ok) {
						tmpVec.y += (ofNoise(x * p.noiseMult + offXTime, p.ny, p.easy)-.5) * p.my;
//						tmpVec.y += (ofNoise(hashValuesX[x], p.ny, p.easy)-.5) * p.my;
					}
				}
				
				if (uiC->pBool["shaper"]) {
					tmpVec.y = shaper(tmpVec.y, 0, 400, 0, 400, uiC->pEasy["shaper"]);
				}
				mesh.setVertex(index, tmpVec);
//				mesh.getVerticesPointer()[index].y = tmpVec.y;
			}
		}
		
		if (uiC->pBool["normals"]) {
			if (!mesh.getNormals().size()) {
				addNormals();
			}
			calcNormals(mesh, uiC->pBool["normalize"], uiC->pBool["winding"]);
		}

        drawMeshStatic(&mesh, ui);
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
//		cout << "wow uievent here" << endl;
		if (e.name == "invert") {
			if (!e._settings->presetIsLoading) {
				cout << "OCEAN INVERT" << endl;
				float noiseMult1 = uiC->pFloat["noiseMult1"];
				float multY1 = uiC->pFloat["multY1"];
				float multiVel1 = uiC->pFloat["multiVel1"];
				
				uiC->set("noiseMult1", uiC->pFloat["noiseMult2"]);
				uiC->set("multY1", uiC->pFloat["multY2"]);
				uiC->set("multiVel1", uiC->pFloat["multiVel2"]);

				uiC->set("noiseMult2", noiseMult1);
				uiC->set("multY2", multY1);
				uiC->set("multiVel2", multiVel1);
			}
			
		}
		if (e.name == "normals") {
			if (*e.b) {
				addNormals();
			} else {
				mesh.clearNormals();
			}
		}
	}
};




//
//};

// comparar com a anterior e remover se for o caso.
struct sceneOcean0 : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	ofMesh mesh;
	int width = 110;
	float multiplicador = 0.4;
	
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



struct sceneNovelo : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// name = "novelo";

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



struct scene3d : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// xaxa revisar se é isso mesmo?
	// name = "3d";
	
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

						if (uiC->pBool["bundinha"]) {
							ofTranslate(-raio*0.65, 0);
							drawMeshStatic(&m, ui);
							ofTranslate(raio*1.3, 0);
                            drawMeshStatic(&m, ui);
						}
						else {
                            drawMeshStatic(&m, ui);
						}
						ofPopMatrix();
					}
					index++;
				}
			}
		}
	}
};






struct sceneSolidos : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// name = "solidos";
	
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
			// int corIndice = 0;
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



struct scenePulsar : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// name = "pulsar";

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
				
				float z = 0;
				if (!ui->pBool["pulsarEq"]) {
					z = ofNoise(ofGetFrameNum() * uiC->pEasy["frameDiv"], x * uiC->pEasy["xDiv"]) * multY * 30.0;
				}
				
				// else {
					// xaxa todo, implementar pointer pro fft.
//					int i = x * fft.FFTanalyzer.nAverages / width;
//					z = fft.FFTanalyzer.peaks[i] * multY;
				// }
				
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



struct sceneLuan : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "luan";

	ofVboMesh ceu;
	ofVboMesh ceus[3];
	int densidade[3] = { 1, 5, 10 };
	

	void setup() override {
		ceu.setMode(OF_PRIMITIVE_POINTS);
		float limite = 75;
		
		for (int c=0; c<3; c++) {
			for (int a=0; a<12500; a+=densidade[c]) {
				for (int b=0; b<10; b++) {
					float x = ofRandom(-limite, limite);
					float y = ofRandom(-limite, limite);
	//				ceu.addVertex(glm::vec3(x, y, -a));
					ceus[c].addVertex(glm::vec3(x, y, -a));
				}
			}
		}
	}
	
	float zPos = 0;
	
	
	// fazer com shader : https://forum.openframeworks.cc/t/glpointsize-effecting-individual-points-ofmesh/26494
	
	//https://stackoverflow.com/questions/4597020/how-can-i-vary-the-point-size-in-opengl-glbegingl-points
	void draw() override {
		checkSetup();
//		ofSetColor(255);
		
//		ceu.drawVertices();
		ofDisableArbTex();
		ofTranslate(0,0,zPos);
		zPos -= uiC->pEasy["vel"];
		int i=1;
		for (auto & c : ceus) {
			ofSetColor(uiC->pColorEasy["cor" + ofToString(i)]);
//			glPointSize(ui->pEasy["pointSize"] * densidade[i-1] );
			glPointSize(uiC->pEasy["pointSpriteSize" + ofToString(i)] );
//			cout << uiC->pEasy["pointSpriteSize" + ofToString(i)]  << endl;

//			ofEnablePointSprites();
//			ofDisableDepthTest();
			uiC->pImage["pointSprite"+ ofToString(i)].getTexture().bind();
//			glBegin(GL_POINTS);
            drawMeshStatic(&c, ui);
//			glEnd();
			uiC->pImage["pointSprite"+ ofToString(i)].getTexture().unbind();


			i++;
		}
//		ceu.drawWireframe();
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
	}
};






struct sceneGalaxia : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// name = "galaxia";

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




struct sceneBox : public ofxScenes::sceneDmtr, ofxScenes::sceneAudio {
public:
	using sceneDmtr::sceneDmtr;
	// name = "box";
	ofPlanePrimitive plane;

	void setup() override {
		plane.set(20, 10);   ///dimensions for width and height in pixels
		plane.setPosition(0, 0, 0); /// position in x y z
		plane.setResolution(20, 10); /// this resolution (as columns and rows) is enough
	}

	void draw() override {
//        cout << "draw box scene" << endl;
		checkSetup();
		float aresta = uiC->pEasy["aresta"] + updown * uiC->pEasy["arestaAudio"];
		if (uiC->pBool["plane"]) {
			plane.drawWireframe();
		}
		if (uiC->pBool["box"]) {
			ofDrawBox(aresta);
		}
	}
};



struct sceneGridbox : public ofxScenes::sceneDmtr {
public:
    using sceneDmtr::sceneDmtr;
    // name = "gridbox";

    void draw() override {
       cout << "draw gridbox scene" << endl;

        ofSetColor(255);
        // ofSetLineWidth(uiC->pEasy["linewidth"]);
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
        
        float total = uiC->pInt["nx"] * uiC->pInt["ny"];
        
        for (int a=0; a<uiC->pInt["nx"]; a++) {
            for (int b=0; b<uiC->pInt["ny"]; b++) {
                float x = ofMap(a, 0, uiC->pInt["nx"], -limite, limite);
                float y = ofMap(b, 0, uiC->pInt["ny"], -limite, limite);
                if (uiC->pBool["color"]) {
                    float hue = fmod(numero*uiC->pEasy["hueMult"] + uiC->pEasy["hue"], 255);
                    ofSetColor(ofColor::fromHsb(hue, uiC->pEasy["sat"], 255));
                } else {
                    float n = (float)numero / total;
//                    cout << n << endl;
                    ofSetColor(ofxScenes::getColor(n, config->uiColors));
					ofSetColor(getCor(n));
                }
                ofDrawBox(x, 0, y, w, h, d);
                numero ++;
            }
        }
    }
};




struct scenePoeira : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "poeira";

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




// festa da uva mirabilis. portado pra ca mas nao testado .
struct sceneUva : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "uva";

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
	}  uvinha = uva(0, ui->pString["draw"]);
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(getCor(0));
		uvinha.draw();
	}
};



struct sceneGirinos3d : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	// name = "girinos3d";

	struct girinoSettings {
	public:
		bool circuit2;
		glm::vec3 dimensions = glm::vec3(800, 480, 480);
		int minX, minY, maxX, maxY, largura, altura;
		int minZ, maxZ, profundidade;
		int margem = -50;
		bool drawRect = false;
		bool circuit = true;
		bool drawText = false;
		float incrementador,
				densidade, fatorQual, anguloMult,
				densidade2, fatorQual2, anguloMult2;
		
		void setDimensions(glm::vec3 d) {
			dimensions = d;
			minX = 		margem;
			minY = 		margem;
			minZ = 		margem;
			maxX = 		dimensions.x - margem;
			maxY = 		dimensions.y - margem;
			maxZ = 		dimensions.z - margem;
			largura = 	dimensions.x - margem*2;
			altura = 	dimensions.y - margem*2;
			profundidade = 	dimensions.z - margem*2;
		}

	} girinoSet, girinoSet2;


    static glm::vec2 p2c (glm::vec2 pos) {
        return glm::vec2(pos.y * cos(ofDegToRad(pos.x)), pos.y * sin(ofDegToRad(pos.x)));
    }

	struct girino {
	public:
		girino() {}
		float rand;
		
		glm::vec3 pos;
		float qual;
		int cursor = 0;
		glm::vec3 positions[60];
		int posSize = 0;
		float angulo;
		float theta;
		float vel = 2;
		
		girinoSettings * _settings = NULL;
		
		// border
		int minX, minY, maxX, maxY, largura, altura;
		int minZ, maxZ, profundidade;

		
		girino(float q, girinoSettings & _s) : qual(q) {
			_settings = &_s;
			//fazer posicao negativa no inicio
			//dimensions = p;
			pos = glm::vec3(ofRandom(0,_settings->dimensions.x),
							ofRandom(0,_settings->dimensions.y),
							ofRandom(0,_settings->dimensions.z)
						);
			rand = ofRandom(0,1);
			//int n = ;
			posSize = ofRandom(30,60);
			//positions.assign(n, ofPoint());
			angulo = ofRandom(0,360);
			theta = ofRandom(0,360);
		}
		
		void addX(float v) {
			pos.x += v;
			for (auto & p : positions) { p.x += v; }
		}
		
		void addY(float v) {
			pos.y += v;
			for (auto & p : positions) { p.y += v; }
		}
		
		void addZ(float v) {
			pos.z += v;
			for (auto & p : positions) { p.z += v; }
		}

		void preUpdate() {
			if (_settings != NULL) {
				
				// todo : pos.z
				angulo = fmod(ofNoise(
								   (pos.x)/_settings->densidade,
								   (pos.y)/_settings->densidade,
								   //(pos.z)/_settings->densidade,
								   qual * _settings->fatorQual,
								   _settings->incrementador
								) * 360.0 * _settings->anguloMult, 360.0);
				if (_settings->densidade2) {
					angulo = fmod(ofNoise(
									(pos.x)/_settings->densidade2,
									(pos.y)/_settings->densidade2,
									//(pos.z)/_settings->densidade2,
									qual * _settings->fatorQual2,
									_settings->incrementador
								) * 360.0 * _settings->anguloMult2, 360.0);
				}
			}
		}
		
		void update() {
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
			
			// XAXA p2c atualizar isso
			
			//pos += glm::euclidean(glm::vec2(vel, angulo));
			glm::vec2 p = p2c(glm::vec2(angulo, vel));
			pos += glm::vec3(p.x, p.y, p.x + p.y);
			
			if (pos.x > _settings->maxX) {  addX(-_settings->largura);  }
			else if (pos.x < _settings->minX) {  addX(_settings->largura);   }
			if (pos.y > _settings->maxY) {  addY(-_settings->altura);   }
			else if (pos.y < _settings->minY) {  addY(_settings->altura);    }
			if (pos.z > _settings->maxZ) {  addZ(-_settings->altura);   }
			else if (pos.z < _settings->minZ) {  addZ(_settings->altura);    }

			positions[cursor] = pos;
			cursor ++ ;
			if (cursor >= posSize) {
				cursor -= posSize;
			}
		}
		
		void draw() {
			//update();
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
				ofDrawBitmapString(s, pos.x, pos.y, pos.z);
			}
		}
	};



	class girinosThread : public ofThread {
		public:
		bool doUpdate = true;
		vector <girino> girinosDraw, girinos, girinosMiddle;
		
		girinosThread() {
			girinosDraw.reserve(3000);
			girinos.reserve(3000);
			girinosMiddle.reserve(3000);
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
		gi.lock();
		gi.girinos.emplace_back(qual,girinoSet);
		gi.unlock();
	}

	void setup() override {
		girinoSet.setDimensions(glm::vec3(fbo->getWidth(), fbo->getHeight(), fbo->getHeight()));
		girinoSet2.setDimensions(glm::vec3(fbo->getWidth(), fbo->getHeight(), fbo->getHeight()));
		gi.startThread();
		int max = 200;
		for (int a=0; a<max; a++) {
			float qual = a/(float)max;
			createGirino(qual);
		}
	}


	float incrementador = 0;
	bool changed = true;
	
	void draw() override {
		checkSetup();
		
		if (ui->pBool["numeroWaves"]) {
			float s = ofMap(sin(ofGetElapsedTimef() * 3), -1, 1, 100, 3000);
			ui->getSlider("numero")->set(int(s));
		}
		
		
		ofSetLineWidth(ui->pFloat["lineWidth"]);
		if (ui->pBool["rastro"]) {
			ofSetColor(0,ui->pFloat["rastro"]);
			ofDrawRectangle(0,0,fbo->getWidth(),fbo->getHeight());
		} else {
			ofClear(0,255);
		}

		ofNoFill();
		ofEnableAlphaBlending();
		if (ui->pString["blend"] == "no") {
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		}
		else if (ui->pString["blend"] == "add") {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
		}
		else if (ui->pString["blend"] == "screen") {
			ofEnableBlendMode(OF_BLENDMODE_SCREEN);
		}

		incrementador += 		ui->pFloat["perFrame"];
		ofSetColor(255, ui->pFloat["alpha"]);
		
		gi.draw();
		gi.doUpdate = true;
		
		if (gi.girinos.size() < ui->pInt["numero"]) {
			int numero = ui->pInt["numero"] - gi.girinos.size();
			for (int a=0; a<numero; a++) {
				createGirino(ofRandom(0,1));
			}
		}
		else if (gi.girinos.size() > ui->pInt["numero"]) {
			int numero = gi.girinos.size() - ui->pInt["numero"];
			gi.lock();
			for (int a=0; a<numero; a++) {
				gi.girinos.erase(gi.girinos.begin());
			}
			gi.unlock();
		}
		
		float halfPosX = fbo->getWidth() * .5;
		for (auto & g : gi.girinos) {
			g._settings = g.pos.x < halfPosX ? &girinoSet : &girinoSet2;
		}
		
		gi.lock();
			girinoSet.incrementador = incrementador;
			girinoSet2.incrementador = incrementador;
			girinoSet.densidade = 		ui->pEasy["densidade"];
			girinoSet.fatorQual = 		ui->pEasy["fatorQual"];
			girinoSet.anguloMult =		ui->pEasy["anguloMult"];
			girinoSet.densidade2 = 		ui->pEasy["densidade2"];
			girinoSet.fatorQual2 = 		ui->pEasy["fatorQual2"];
			girinoSet.anguloMult2 =		ui->pEasy["anguloMult2"];
			girinoSet.circuit = ui->pBool["circuit"];
			girinoSet.circuit2 = ui->pBool["circuit90"];
			girinoSet.drawRect = ui->pBool["drawRect"];
			girinoSet.drawText = ui->pBool["drawText"];
			
			
			
			girinoSet2.densidade = 		ui->pEasy["densidade3"];
			girinoSet2.fatorQual = 		ui->pEasy["fatorQual3"];
			girinoSet2.anguloMult =		ui->pEasy["anguloMult3"];
			girinoSet2.densidade2 = 		ui->pEasy["densidade4"];
			girinoSet2.fatorQual2 = 		ui->pEasy["fatorQual4"];
			girinoSet2.anguloMult2 =		ui->pEasy["anguloMult4"];
			girinoSet2.circuit = ui->pBool["circuit2"];
			girinoSet2.circuit2 = ui->pBool["circuit290"];
			girinoSet2.drawRect = ui->pBool["drawRect"];
			girinoSet2.drawText = ui->pBool["drawText"];
		gi.unlock();
		
		if (changed)
		{
			gi.lock();
			int index = 0;
			for (auto & g : gi.girinos) {
				g.qual = index/(float)gi.girinos.size();
				index++;
				g.vel = ui->pEasy["vel"] + g.rand * ui->pEasy["velRand"];
			}
			changed = false;
			gi.unlock();
		}
	}
};




#ifdef INSIDE
void setupScenes3d() {
		// 3d
	scenes.push_back(new sceneOcean(&config, "ocean"));
	scenes.push_back(new sceneGridbox(&config, "gridbox"));
	scenes.push_back(new sceneSolidos(&config, "solidos"));
	scenes.push_back(new scenePulsar(&config, "pulsar"));
	scenes.push_back(new sceneLuan(&config, "luan"));
	scenes.push_back(new sceneBox(&config, "box"));
	scenes.push_back(new sceneNovelo(&config, "novelo"));
	//xaxa
	scenes.push_back(new scene3d(&config, "3d"));
	scenes.push_back(new sceneGalaxia(&config, "galaxia"));
	scenes.push_back(new sceneGirinos3d(&config, "girinos3d"));
#ifdef USEASSIMP
	scenes.push_back(new sceneModel(&config, "model"));
#endif
	scenes.push_back(new scenePoeira(&config, "poeira"));
}

#endif




struct sceneVbo : public virtual ofxScenes::sceneDmtrBasic {
	public:
	using sceneDmtrBasic::sceneDmtrBasic;

	ofVboMesh mesh;
	int max = 400;

	vector <ofPrimitiveMode> modes = { 
		OF_PRIMITIVE_TRIANGLES, OF_PRIMITIVE_TRIANGLE_STRIP, OF_PRIMITIVE_TRIANGLE_FAN, OF_PRIMITIVE_LINES, OF_PRIMITIVE_LINE_STRIP, OF_PRIMITIVE_LINE_LOOP, OF_PRIMITIVE_POINTS
	};

	void create() {
		mesh.clear();
		float lon = ofRandom(0,360);
		float lat = ofRandom(0,360);
		float rad = 300;
		ofNode node;
		for (int a=0; a<uiC->pInt["numero"]; a++) {
			lon += ofRandom(-uiC->pFloat["randLon"], uiC->pFloat["randLon"]);
			lat += ofRandom(-uiC->pFloat["randLat"], uiC->pFloat["randLat"]);
			rad += ofRandom(-uiC->pFloat["randRad"], uiC->pFloat["randRad"]);
			node.orbitDeg(lon, lat, rad, glm::vec3(0,0,0));
			mesh.addVertex(node.getPosition());

			// mesh.addVertex(glm::vec3(
			// 		ofRandom(-max, max),
			// 		ofRandom(-max, max),
			// 		ofRandom(-max, max)
			// 	));				
		}
		mesh.setMode(modes[uiC->pInt["mode"]]);
	}

	void setup() override {
		create();
	}

	void draw() override{
		ofSetColor(255);
		mesh.draw();
	}

	void uiEvents(ofxMicroUI::element & e) override {
		create();
	}
};
