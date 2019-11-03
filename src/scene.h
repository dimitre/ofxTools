map <string, float> incrementadorTemporal;
float incrementa(string qual) {
	string uniqueId = uiC->uiName + qual;
	incrementadorTemporal[uniqueId] += uiC->pFloat[qual];
	return incrementadorTemporal[uniqueId];
}

ofColor getCor(float qual) {
	return ofColor(255, ui->pEasy["alpha"]);
}



void drawMesh(ofMesh * m) {
	if (ui->pString["draw"] == "wire") {
		m->drawWireframe();
	}
	if (ui->pString["draw"] == "faces") {
		m->drawFaces();
	}
	if (ui->pString["draw"] == "points") {
		//cout << "aqui " << endl;
		glPointSize(ui->pEasy["pointSize"]);
//		glDisable(GL_POINT_SMOOTH);
		m->draw(OF_MESH_POINTS);
	}
}

bool checkIs3d() {
	if (scene == "ocean" || scene == "3d" || scene == "galaxia") {
		return true;
	} else {
		return false;
	}
}

void begin3d() {
	if (checkIs3d()) {
		beginCam_3d();
		beginShader("shaders3d");
	}
}

void end3d() {
	if (checkIs3d()) {
		endShader("shaders3d");
		endCam_3d();
	}
}


float updown = 0;


struct rede {
public:
	
	int w = 6400;
	int h = 256;
	
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
			pontos.push_back(glm::vec2(x, y));
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
	for (int a=0; a<150; a++) {
//		redes.push_back(rede(a));
		redes.emplace_back(a);
	}
}

void drawScene(string scene) {
	
	if (scene == "punchCard") {
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
		
		for (auto & r : redes) {
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
			for (int a=0; a<dif; a++) {
				redes.emplace_back(a);
			}
		}
	}
	
	else if (scene == "3d") {
//		begin3d();
		ofSpherePrimitive sphere;
		sphere.set(uiC->pEasy["raio"], uiC->pInt["resolution"]);
		float aresta = uiC->pEasy["aresta"];
		float max = uiC->pInt["max"] * 0.5;
		for (float x=-max; x<=max; x++) {
			for (float y=-max; y<=max; y++) {
				for (float z=-max; z<=max; z++) {
					ofPushMatrix();
					ofTranslate(x*aresta, y*aresta, z*aresta);
					sphere.drawWireframe();
					ofPopMatrix();
				}
			}
		}
		//ofDrawSphere(0,0,400);
//		end3d();
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

		for (int a=0; a<numero; a++) {
			float qx = a * uiC->pFloat["qx"];
			float qy = a * uiC->pFloat["qy"];
			float x = ofNoise(qx, tempo) * fbo->getWidth();
			float y = ofNoise(qy, tempo2) * fbo->getHeight();
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
