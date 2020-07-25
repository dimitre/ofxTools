
float updown = 0;

map <string, float> incrementadorTemporal;
float incrementa(string qual) {
	string uniqueId = uiC->uiName + qual;
	incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
	return incrementadorTemporal[uniqueId];
}

#ifdef USECOLOR
ofColor getCor(float qual) {
	ofColor c = ui->pColor["cor"].getLerped(ui->pColor["cor2"], qual);
	c.a = ui->pEasy["alpha"];
	return c;
	//return ofColor(ui->pColor["cor"], ui->pEasy["alpha"]);
}
#else
ofColor getCor(float qual) {
	return ofColor(255, ui->pEasy["alpha"]);
}
#endif


void drawMesh(ofMesh * m) {
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



//OLD SCENES

struct objeto {
public:
	float raio;
	float rand;
	ofPoint pos;
	int corIndice;
	objeto() {}
};

vector <objeto> objetos;
ofBoxPrimitive box;
ofCylinderPrimitive cylinder;
ofConePrimitive cone;
ofSpherePrimitive sphere;
ofIcoSpherePrimitive icoSphere;

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

void drawPoeira() {
	int index = 0;
	for (auto & p : poeiras) {
		ofSetColor(getCor(p.qual));
		p.draw();
		if (index++ > uiC->pInt["numero"]) {
			break;
		}
	}
}



void begin3d() {
	if (checkIs3d()) {
		beginCam_3d();
		beginIlumina_3d();
		beginMaterial_3d();
		beginShader("shaders3d");
	}
}

void end3d() {
	if (checkIs3d()) {
		endShader("shaders3d");
		endMaterial_3d();
		endIlumina_3d();
		endCam_3d();
	}
}



struct rede {
public:
	
	int w = 3840;
	int h = 160;
	
	vector <glm::vec2> pontos;
	glm::vec2 pos, vel;
	float tempo, tempo2;
	int a;
	float qx, qy;
	int numero;
	int raio;
	rede(int aa) : a(aa) {
		tempo = ofRandom(0, .2);
		tempo2 = ofRandom(0, .2);
		float maxVel = 0.7;
		vel = glm::vec2(ofRandom(-maxVel, maxVel), ofRandom(-maxVel, maxVel));
		pos = glm::vec2(ofRandom(0,w), ofRandom(0,h));
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
		if (pos.x > (w + margem)) {
			pos.x = -margem;
		}
		else if (pos.x < -margem) {
			pos.x = w+margem;
		}

		if (pos.y > (h + margem)) {
			pos.y = -margem;
		}
		else if (pos.y < -margem) {
			pos.y = h+margem;
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


void setupScene() {
	int n = 400;
	poeiras.reserve(n);
	for (int a=0; a<n; a++) {
		poeiras.emplace_back(a/(float)n);
	}

	redes.reserve(150);
	for (int a=0; a<150; a++) {
		redes.emplace_back(a);
	}
}

// scene = 3d
//ofSpherePrimitive sphere;


//static ofxMicroUI * uiStatic = &u.uis["ui"];

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



bool checkIs3d() {
	// futuro = regexp
	glShadeModel(uiLuz->pBool["shadeFlat"] ? GL_FLAT : GL_SMOOTH);
	
	if (
		scene == "box" ||
		scene == "ocean" || scene == "3d" || scene == "galaxia"  ||
		scene == "solidos" || scene == "poeira" || scene == "novelo" ||
		scene == "uva"
		) {
		return true;
	} else {
		return false;
	}
}


void drawScene(string scene) {
	
	if (uiBpm->pBool["bpm"]) {
		updown = bpm.getPercent();
		if (uiBpm->pString["wave"] == "sin") {
			updown = sin(bpm.getPercent() * 3.1415926) *0.5 + .5;
//			cout << updown << endl;
		}
	} else {
		updown = fft.updown;
	}
	
	if (scene == "poeira") {
		drawPoeira();
	}
	
	else if (scene == "box") {
		float aresta = uiC->pEasy["aresta"] + updown * uiC->pEasy["arestaAudio"];
		ofDrawBox(aresta);
	}
	
	else if (scene == "image") {
		uiC->pImage["image"].draw(0,0);
	}
	
	else if (scene == "rect") {
		float audio = fmod(ofGetElapsedTimef() * .5, 1.0);
		float w = uiC->pFloat["w"] * fbo->getWidth();
		float h = uiC->pFloat["h"] * fbo->getWidth();
		float x = (uiC->pFloat["x"] + uiC->pFloat["xAudio"]*audio) * fbo->getWidth();
		float y = uiC->pFloat["y"] * fbo->getWidth();
		float qualCor = audio * uiC->pFloat["qualCor"];
		ofSetColor(getCor(qualCor));
		ofDrawRectangle(x, y, w, h);
	}
	
	else if (scene == "uva") {
		ofSetColor(getCor(0));
		uvinha.draw();
	}
	
	else if (scene == "novelo") {
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
	
	else if (scene == "pulse") {
		ofColor cor = getCor(0);
		if (!uiC->pBool["solid"]) {
			cor.a = ofMap(sin(ofGetElapsedTimef() * uiC->pFloat["multTime"]), -1, 1, 0, 255);
		}
		ofSetColor(cor);
		ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
	}
	
	else if (scene == "punchCard") {
		float aresta = uiC->pFloat["aresta"];
		float margem = 10;
		ofSetColor(255);
		for (int x=0; x<100; x++) {
			for (int y=0; y<8; y++) {
				float noise = ofNoise(x, y, ofGetElapsedTimef() * uiC->pFloat["velNoise"]);
				if (noise > uiC->pFloat["minNoise"]) {
					int raio = noise * 2 + 2;
					ofDrawCircle(x*aresta + margem, y*aresta + margem + uiC->pFloat["offY"], raio);
				}
			}
		}
	}
	
	else if (scene == "redes") {
		int numero = uiC->pInt["numero"];
		
		int index = 0;
		for (auto & r : redes) {
			float qual = index/(float)redes.size();
			index ++;
			ofSetColor(getCor(qual));
			r.update();
			r.draw();
		}
		
		if (numero < redes.size()) {
			int dif = redes.size() - numero;
//			cout << dif << endl;
			redes.erase(redes.begin(), redes.begin() + numero);
		}
		
		if (numero > redes.size()) {
			int dif = numero - redes.size();
//			cout << dif << endl;
			int n = redes.size();
			for (int a=0; a<dif; a++) {
				redes.emplace_back(a + n);
			}
		}
	}
	

	
	else if (scene == "3d") {
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
						sphere.set(raio, res);
						ofMesh m = sphere.getMesh();
						float aa = aresta + uiC->pEasy["arestaNoise"] * nn;
						ofTranslate(x*aa, y*aa, z*aa);
						//sphere.drawWireframe();
						drawMesh(&m);
						ofPopMatrix();
					}
					index++;
				}
			}
		}
	}
	
	else if (scene == "solidos") {
		int numero = uiC->pInt["numero"] + updown * (float)uiC->pInt["numeroAudio"];
		for (int a=0; a<numero; a++ ) {
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
			ofVec3f posicao = objetos[a].pos * (uiC->pEasy["fatorDistancia"] + uiC->pFloat["fatorDistanciaAudio"] * updown);
			objeto->setScale(raio);
			objeto->setPosition(posicao);
			
			if (ui->pBool["wire"]) {
				objeto->drawWireframe();
			}
			if (ui->pBool["faces"]) {
				objeto->drawFaces();
			}
			if (ui->pBool["point"]) {
				objeto->getMesh().draw(OF_MESH_POINTS);
			}
		}
	}
	
	
	
	else if (scene == "galaxia") {
//		begin3d();
		
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
						float r = raio/2;
						glm::vec3 ponto = glm::vec3(xx,yy,zz);
						glm::vec3 ponto2 = glm::vec3(xx + raio,yy + raio,zz);
						
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
				drawMesh(&mesh);
				ofPopMatrix();
			}
			ofPopMatrix();
		}
//		end3d();
	}
	if (scene == "ocean") {

//		begin3d();
		//ofRotate(-55.0 + sin(ofGetFrameNum()/100.0)*10.0, 1, 0, 0);
		ofVboMesh mesh;
		int width = 70;
		float multiplicador = 80.0;
		
		for (int x=0; x<width; x++) {
			for (int z=0; z<width; z++) {
				float y = ofNoise(x/10.0, z/10.0, ofGetFrameNum() ) * multiplicador * 12.0;
				ofVec3f pos = ofVec3f(x*multiplicador - width/2*multiplicador, y, z*multiplicador - width/2*multiplicador);
//				meshEq.addVertex(pos);
				mesh.addVertex(pos);
			}
		}
//		meshEq.disableColors();
		
		for (int x=0; x<width-1; x++) {
			for (int z=0; z<width-1; z++) {
//				meshEq.addIndex(x+z*width);				// 0
//				meshEq.addIndex((x+1)+z*width);			// 1
//				meshEq.addIndex(x+(z+1)*width);			// 10
//				meshEq.addIndex((x+1)+z*width);			// 1
//				meshEq.addIndex((x+1)+(z+1)*width);		// 11
//				meshEq.addIndex(x+(z+1)*width);			// 10
				
				mesh.addIndex(x+z*width);				// 0
				mesh.addIndex((x+1)+z*width);			// 1
				mesh.addIndex(x+(z+1)*width);			// 10
				mesh.addIndex((x+1)+z*width);			// 1
				mesh.addIndex((x+1)+(z+1)*width);		// 11
				mesh.addIndex(x+(z+1)*width);			// 10
			}
		}
		
		
		int total = width * width;
		float noiseDiv = uiC->pEasy["noiseDiv"] + uiC->pEasy["noiseDivAudio"] * updown;
		float noiseDiv2 = uiC->pEasy["noiseDiv2"] + uiC->pEasy["noiseDiv2Audio"] * updown;
		
		float noiseMult1 = uiC->pEasy["noiseMult1"] + uiC->pEasy["noiseMult1Audio"] * updown;
		float noiseMult2 = uiC->pEasy["noiseMult2"] + uiC->pEasy["noiseMult2Audio"] * updown;
		
		float multY = uiC->pEasy["multY"] + uiC->pEasy["multYAudio"] * updown;
		float multY2 = uiC->pEasy["multY2"] + uiC->pEasy["multY2Audio"] * updown;
		
		float easy1 = ofGetElapsedTimef() * uiC->pEasy["multiVel1"];
		float easy2 = ofGetElapsedTimef() * uiC->pEasy["multiVel2"];
		float offXTime = ofGetElapsedTimef() * uiC->pFloat["offXTime"];
		float offYTime = ofGetElapsedTimef() * uiC->pFloat["offYTime"];
		ofSetColor(getCor(0));
		//cout << multY << endl;
		//cout << width << endl;
		for (int y=0; y<width; y++) {
			for (int x=0; x<width; x++) {
				int index = x + y*width;
				ofVec3f tmpVec = mesh.getVertex(index);
				
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
		if (!uiC->pBool["esferas"]) {
			mesh.disableColors();
			drawMesh(&mesh);
		}
//		end3d();

	}
	
	
	else if (scene == "plexus") {
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
		for (auto & p : pontos) {
			for (auto & pp : pontos) {
				if (ABS(pp.x - p.x) < distance && ABS(pp.y - p.y) < distance) {
					
					ofSetColor(index > numero *.5 ? ofColor(255,0,0,ui->pFloat["alpha"]) : ofColor(0,0,255,ui->pFloat["alpha"]));
					ofDrawLine(p, pp);
				}
			}
			index++;
		}
	}
	
	else if (scene == "redes0") {
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
	
	else if (scene == "radial3") {
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
			
			float alphaTime = 		incrementa("alphaTime");
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
	
	else if (scene == "leparc2") {
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
	
	else if (scene == "circles") {
		for (int a=1; a<4; a++) {
			string id = ofToString(a);
			if (uiC->pBool["circle" + id]) {
				ofSetColor(
					uiC->pFloat["r" + id],
					uiC->pFloat["g" + id],
					uiC->pFloat["b" + id]);
				ofDrawCircle(
					uiC->pFloat["x" + id],
					uiC->pFloat["y" + id],
					uiC->pFloat["radius" + id]);
			}
		}
	}
	
	else if (scene == "lines") {
		for (int a=0; a<100; a++) {
			float x = ofRandom(0,fbo->getWidth());
			float y = ofRandom(0, fbo->getHeight());
			ofDrawLine(0,0,x,y);
		}
	}
	
	

}




//
//else if (*cena == "3d") {
//	ofSetColor(255, ui->pFloat["alpha"]);
//	beginCam_3d();
//	//test();
//	ofSpherePrimitive sphere;
//	sphere.set(uiC->pEasy["raio"], uiC->pInt["resolution"]);
//	float aresta = uiC->pEasy["aresta"];
//	for (int x=-1; x<=1; x++) {
//		for (int y=-1; y<=1; y++) {
//			for (int z=-1; z<=1; z++) {
//				ofPushMatrix();
//				ofTranslate(x*aresta, y*aresta, z*aresta);
//				sphere.drawWireframe();
//				ofPopMatrix();
//			}
//		}
//	}
//	endCam_3d();
//}
