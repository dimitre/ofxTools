struct sceneMuti : public sceneDmtr {
	using sceneDmtr::sceneDmtr;
	
	float updown = 0;
	float beat = 0;
	float audio = 0;
	
	ofBoxPrimitive box, box2;
	ofCylinderPrimitive cylinder, cylinder2;
	ofConePrimitive cone, cone2;
	ofSpherePrimitive sphere, sphere2;
	ofIcoSpherePrimitive icoSphere, icoSphere2;
	
	//--------------------------------------------------------------
	float r2f (ofPoint range, float mult, int steps) {
		if (steps > 0) {
			mult = int(mult*steps) / (float) steps;
		}
		return (range.y - range.x)*mult + range.x;
	}


	vector <string> tipos3d;
#ifdef USEASSIMP
	map <int,ofxAssimpModelLoader> modelsMap;
#endif
	
	void objeto3dneu(ofVec3f pos, float raio = 1, ofVec3f escala = ofVec3f(1,1,1), float qual=0, float indiceCor = 0) {
		of3dPrimitive * objeto;
//		cout << "objeto3dneu " << tipos3d.size() << endl;
		if (tipos3d.size() > 0) {
			int indiceObjeto = qual * tipos3d.size();
			string obj = tipos3d[indiceObjeto];
			escala *= raio;
			
			if (obj == "allmodels_3d" || obj == "model_3d") {
#ifdef USEASSIMP
				int indexModel = ui->pFloat["model"] * (modelsMap.size() - 1);
				if (obj == "allmodels_3d") {
					float qual2 = ofMap(qual, 1.0 - (1 / (float)tipos3d.size()), 1.0, 0.0, 1.0);
					indexModel = qual2 * (modelsMap.size());
				}

				ofxAssimpModelLoader * m;
				m = &modelsMap[indexModel];
				m->update();
				//modelsMap[indexModel].update();
				m->setPosition(pos.x,pos.y,pos.z);
				ofVec3f raio2 = escala * ui->pEasy["modelScale"];
				m->setScale(raio2.x, raio2.y, raio2.z);
				m->enableTextures();
				
				if (ui->pBool["point"]) {
					m->draw(OF_MESH_POINTS);
				}
				if (ui->pBool["faces"]) {
					m->draw(OF_MESH_FILL);
				}
				if (ui->pBool["wire"]) {
					ofPushStyle();
					if (uiColors->pBool["useCorWire"]) {
						ofSetColor(getCor(indiceCor, "corWire"));
					}
					m->draw(OF_MESH_WIREFRAME);
					ofPopStyle();
				}
#endif
			}
			else {
				objeto = &box;
				if (obj == "box")
					objeto = &box;
				else if (obj == "sphere")
					objeto = &sphere;
				else if (obj == "icosphere")
					objeto = &icoSphere;
				else if (obj == "cylinder")
					objeto = &cylinder;
				else if (obj == "cone")
					objeto = &cone;

				objeto->setScale(escala);
				objeto->setPosition(pos);
//				if (ui->pBool["faces"]) {
//				}
				if (ui->pBool["point"]) {
					objeto->drawVertices();
				}
				if (ui->pBool["wire"]) {
					ofPushStyle();
					
					// TODO - CORWIRE
//					if (uiColors->pBool["useCorWire"]) {
//						ofSetColor(getCor(indiceCor, "corWire"));
//					}
					objeto->drawWireframe();
					ofPopStyle();
				}
				else {
					objeto->drawFaces();
				}
				
			}
		}
	}
	
	void updateSolids() {
		tipos3d.clear();
		if (uiC->pBool["box_3d"])
			tipos3d.push_back("box");
		if (uiC->pBool["sphere_3d"])
			tipos3d.push_back("sphere");
		if (uiC->pBool["cylinder_3d"])
			tipos3d.push_back("cylinder");
		if (uiC->pBool["cone_3d"])
			tipos3d.push_back("cone");
		if (uiC->pBool["icosphere_3d"])
			tipos3d.push_back("icosphere");
		if (uiC->pBool["model_3d"])
			tipos3d.push_back("model_3d");
		if (uiC->pBool["allmodels_3d"])
			tipos3d.push_back("allmodels_3d");

	}
};



struct sceneUniversoNovo : public sceneMuti {
public:

	using sceneMuti::sceneMuti;

	// cena universo novo
	struct objNovo {
	public:
		float qual;
		float rand;
		float rand2;
		ofPoint pos;
		float x, y, z;
		float rx, ry, rz;
		float rotatex = 0;
		float rotatey = 0;
		float rotatez = 0;
        float tamanho = 0;

		objNovo() {
			rand = ofRandom(0,1);
			rand2 = ofRandom(0,1);
			rx = ofRandom(0,1);
			ry = ofRandom(0,1);
			rz = ofRandom(0,1);
		}
	};

	// 2016 pra cena UniversoNovo - achar melhor nome?
	vector <objNovo> objetosUni;
	// nomear melhor?
	vector <int> direcoes;

	void setup() override {}

	void draw() override {
		checkSetup();

//		cout << objetosUni.size() << endl;
		for (auto & u : objetosUni) {
			float qual = u.qual;
			// somar o audio aqui depois, ver frequencias.

			float vel = uiC->pEasy["velMin"] + uiC->pFloat["velRand"] * u.rand
			+ getFreq(qual) * uiC->pEasy["velAudio"];

			// pra orientacao usar o randNovo.
			if (u.rand > uiC->pEasy["velDirecao"]) {
				vel *= -1;
			}

			int orientacao = 0;
			if (direcoes.size() > 0) {
				orientacao = direcoes[u.rand2 * direcoes.size()];
				if (orientacao == 1) {
					u.x += vel;
				}
				else if (orientacao == 2) {
					u.y += vel;
				}
				else if (orientacao == 3) {
					u.z += vel;
				}

				if (u.x >  uiC->pInt["roomSizeX"])
					u.x -= uiC->pInt["roomSizeX"]*2;
				if (u.x < -uiC->pInt["roomSizeX"])
					u.x += uiC->pInt["roomSizeX"]*2;
				if (u.y >  uiC->pInt["roomSizeY"])
					u.y -= uiC->pInt["roomSizeY"]*2;
				if (u.y < -uiC->pInt["roomSizeY"])
					u.y += uiC->pInt["roomSizeY"]*2;
				if (u.z >  uiC->pInt["roomSizeZ"])
					u.z -= uiC->pInt["roomSizeZ"]*2;
				if (u.z < -uiC->pInt["roomSizeZ"])
					u.z += uiC->pInt["roomSizeZ"]*2;
			}

			ofPushMatrix();
			ofTranslate(u.x, u.y, u.z);
			if (orientacao == 1) {
				ofRotateZDeg(90);
			}
			else if (orientacao == 3) {
				ofRotateXDeg(90);
			}

			if (uiC->pFloat["randomAngle"]) {
				float angle = uiC->pFloat["randomAngle"] + uiC->pFloat["randomAngleAudio"] * updown;
				ofRotateDeg(angle, u.rx, u.ry, u.rz);
			}

			if (uiC->pFloat["autoRotateX"]) {
				u.rotatex += uiC->pFloat["autoRotateX"];
				ofRotateXDeg(u.rotatex);
			}
			if (uiC->pFloat["autoRotateY"]) {
				u.rotatey += uiC->pFloat["autoRotateY"];
				ofRotateYDeg(u.rotatey);
			}
			if (uiC->pFloat["autoRotateZ"]) {
				u.rotatez += uiC->pFloat["autoRotateZ"];
				ofRotateZDeg(u.rotatez);
			}


			ofRotateXDeg(uiC->pFloat["objRotateX"] + uiC->pFloat["objRotateXAudio"] * updown);
			ofRotateYDeg(uiC->pFloat["objRotateY"] + uiC->pFloat["objRotateYAudio"] * updown);
			ofRotateZDeg(uiC->pFloat["objRotateZ"] + uiC->pFloat["objRotateZAudio"] * updown);

			float raio = uiC->pEasy["raio"] + uiC->pEasy["raioAudio"] * updown;
			ofSetColor(getCor(u.rand));

			if (uiC->pBool["usaLinha"]) {
				float prof = uiC->pEasy["profundidade"] + getFreq(qual) * uiC->pEasy["profundidadeAudio"]+ u.rand*uiC->pEasy["profundidadeRand"];
				ofDrawLine(0, 0, 0, prof * raio);

			} else {
				ofPoint scale = ofPoint (
										 uiC->pEasy["altura"] + getFreq(qual) * uiC->pEasy["alturaAudio"] + u.rand*uiC->pEasy["alturaRand"],
										 uiC->pEasy["largura"] + getFreq(qual) * uiC->pEasy["larguraAudio"]+ u.rand*uiC->pEasy["larguraRand"],
										 uiC->pEasy["profundidade"] + getFreq(qual) * uiC->pEasy["profundidadeAudio"]+ u.rand*uiC->pEasy["profundidadeRand"]
										 );
				objeto3dneu(ofPoint(0,0,0), raio, scale, u.qual, u.rand2);
			}
			ofPopMatrix();
		}
	}

	
	// universoNovo
	void uiEvents(ofxMicroUI::element & e) override {
		
		if (e.name == "numeroObjetos" || e.name == "allPresets") {
			objetosUni.clear();
			for (int a=0; a<uiC->pInt["numeroObjetos"]; a++) {
				objNovo to = objNovo();
				to.x = ofRandom(-uiC->pInt["roomSizeX"], uiC->pInt["roomSizeX"]);
				to.y = ofRandom(-uiC->pInt["roomSizeY"], uiC->pInt["roomSizeY"]);
				to.z = ofRandom(-uiC->pInt["roomSizeZ"], uiC->pInt["roomSizeZ"]);
				to.qual = a/(float)uiC->pInt["numeroObjetos"];
				objetosUni.push_back(to);
			}
		}
		
		
		if (ofIsStringInString(e.name, "_3d") || e.name == "load") {
			updateSolids();
		}
		
		if (e.name == "direcaoX" || e.name == "direcaoY" || e.name == "direcaoZ" || e.name == "load") {
			direcoes.clear();
			
			if (uiC->pBool["direcaoX"]) {
				direcoes.push_back(1);
			}
			if (uiC->pBool["direcaoY"]) {
				direcoes.push_back(2);
			}
			if (uiC->pBool["direcaoZ"]) {
				direcoes.push_back(3);
			}
		}
	}
};



struct sceneRect : public sceneMuti {
public:
	
	using sceneMuti::sceneMuti;
	
	// provisorio
	bool isPoligonos = false;
	
	


	struct objetoEase {
	public:
		bool on = false;
		unsigned int index = 0;
		
		float x = 0;
		float y = 0;
		float scale = 1;
		
		float largura = 0;
		float altura = 0;
		// ispoligono
		float raio = 0;

		
		float rand = 0;
		int	freq = 0;
		float tamanho = 0;
		
	};

	objetoEase objetos[1600];
	int angulosTodos[4] = {0,0,0,0};
	bool updateObjects = true;
	ofMesh mesh;
	
	void setupObjetos() {
		unsigned int index = 0;
		for (auto & o : objetos) {
			o.index = index;
			index++;
		}
	}

	void setup() override {
		setupObjetos();
	}
	
	void setPoligonos() {
		isPoligonos = true;

	}

	void update() override {
		audio =
		config->fft->updown;
		updown =
		config->fft->updown;
		
		if (updateObjects) {
			updateObjects = false;
			
			//cout << "updating Objects" << endl;

			float linhastotal = floor(uiC->pFloat["objetos"] + updown*uiC->pFloat["objetosAudio"] + beat*uiC->pFloat["objetosBeat"]);

			uiC->pFloat["espacoDistributex"] = fbo->getWidth() / linhastotal;
			uiC->pFloat["espacoDistributey"] = fbo->getHeight() / linhastotal;
			
	//		for (int a = 0; a<1600; a++) {
			for (int a = 0; a<=linhastotal; a++) {
				objetoEase & o = objetos[a];
				//for (int a = 0; a<linhastotal; a++) {
				o.rand = ofRandom(1);
				
				float altura =
				uiC->pEasy["altura"] +
				uiC->pEasy["alturaAudio"]  +
				uiC->pEasy["alturaRand"] * objetos[a].rand;
				
				float largura =
				uiC->pEasy["largura"] +
				uiC->pEasy["larguraAudio"]  +
				uiC->pEasy["larguraRand"] * objetos[a].rand;
				
				// xaxa
                o.tamanho = MAX(altura, largura);
				
				if (uiC->pString["verticalAlign"] == "vrandom") {
					o.x = ofRandom(-o.tamanho, fbo->getWidth() + o.tamanho);
				}
				else if (uiC->pString["verticalAlign"] == "vregular") {
					o.x = a * uiC->pFloat["espacoDistributex"] + uiC->pFloat["espacoDistributex"] / 2.0;
				}
				else if (uiC->pString["verticalAlign"] == "valign") {
					o.x = fbo->getWidth() / 2;
				}
				
				if (uiC->pString["horizontalAlign"] == "hrandom") {
					o.y = ofRandom(-o.tamanho, fbo->getHeight() + o.tamanho);
				}
				else if (uiC->pString["horizontalAlign"] == "hregular") {
					o.y = a * uiC->pFloat["espacoDistributey"] + uiC->pFloat["espacoDistributey"] / 2.0;
				}
				else if (uiC->pString["horizontalAlign"] == "halign") {
					o.y = fbo->getHeight() / 2;
				}
			}
		}
	}
	

	void draw() override {
		checkSetup();
	
		update();
		
		// UPDOWN
		if (uiC->pFloat["audioSolidoContorno"]) {
			updown > uiC->pEasy["audioSolidoContorno"] ? ofFill() : ofNoFill();
		}

		ofSetRectMode(OF_RECTMODE_CENTER);
		
		float nObjetos = uiC->pFloat["objetos"]
		+ uiC->pFloat["objetosAudio"] * audio
		+ uiC->pFloat["objetosBeat"] * beat;
		
		float contagem = 0;
		for (auto & o : objetos) {
			if (o.scale > 0) {
				contagem ++;
			}
		}
			
		for (auto & o : objetos) {
			o.on = o.index < nObjetos;
			if (o.on && o.scale < 1) {
				o.scale += uiC->pFloat["easeIn"];
			}
			
			if (!o.on && o.scale > 0) {
				o.scale -= uiC->pFloat["easeOut"];
			}
			
			if (o.on && o.scale > 0) {
				
				int angulo = uiC->pString["angulos"] == "random" ? o.rand * 360.0 : angulosTodos[int(o.rand * 4.0)];
				
				float velocidade =
				uiC->pFloat["velo"] +
				uiC->pFloat["veloMicro"] +
				audio * uiC->pFloat["veloAudio"] +
				uiC->pFloat["veloRand"] * o.rand
				;
				
				if (isPoligonos) {
					o.raio = uiC->pEasy["raio"] + uiC->pEasy["raioAudio"] * audio + uiC->pEasy["raioBeat"]*beat;
//					float raioRand = 0;
					if (uiC->pFloat["raioRandom"]) {
						o.raio += uiC->pEasy["raioRandom"] * o.rand;
					}

					o.tamanho = o.raio;
				}
				else
				{
				
//					float altura =
//					uiC->pEasy["altura"] +
//					uiC->pEasy["alturaAudio"] * audio +
//					beat * uiC->pFloat["alturaBeat"] +
//					uiC->pEasy["alturaRand"] * o.rand +
//					uiC->pEasy["alturaFreq"] * getFreq(o.freq)
//					;
//
//					float largura =
//					uiC->pEasy["largura"] +
//					uiC->pEasy["larguraAudio"] * audio +
//					beat * uiC->pFloat["larguraBeat"] +
//					uiC->pEasy["larguraRand"] * o.rand +
//					uiC->pEasy["larguraFreq"] * getFreq(o.freq)
//					;

					o.altura =
					uiC->pEasy["altura"] +
					beat * uiC->pFloat["alturaBeat"] +
					audio * uiC->pEasy["alturaAudio"]  +
					uiC->pEasy["alturaRand"] * o.rand;
					
					o.largura =
					uiC->pEasy["largura"] +
					audio * uiC->pEasy["larguraAudio"]  +
					beat * uiC->pFloat["larguraBeat"] +
					uiC->pEasy["larguraRand"] * o.rand;
					
					// o.tamanho = MAX(maxaltura, maxlargura)/2.0;
					o.tamanho = MAX(o.altura, o.largura);
				}

				// marca metade das possibilidades como indo, metade como voltando. da pra resortear isso em algum lado.
				// lindo isso.
				
				//cout << velocidade << endl;
				
				if (velocidade != 0) {
					if (uiC->pBool["twoway"] && o.rand > .5) {
						velocidade *= -1;
					}
					//velocidade = uiC->pBool["2way"] ? velocidade * o.vaivolta : velocidade;
					if (uiC->pBool["inverteMovimento"]) velocidade *= -1;
					o.x += r2x(angulo, velocidade);
					o.y += r2y(angulo, velocidade);
					
					if (o.x > fbo->getWidth() + o.tamanho)
						o.x =  - o.tamanho;
					else if (o.x < - o.tamanho)
						o.x = fbo->getWidth() + o.tamanho;
					
					if (o.y > fbo->getHeight() + o.tamanho)
						o.y = - o.tamanho;
					else if (o.y < - o.tamanho)
						o.y = fbo->getHeight() + o.tamanho;
				}
				
				// metodo shiny brilhante
				
				
				//				ofColor estacor = getCor(a/(float)nObjetos);
				ofColor estacor = getCor(o.index/contagem);
				if (uiC->pBool["cintilante"]) {
					estacor = getCor(o.index/(float)nObjetos);
				}
				
				if (isPoligonos) {
					int lados = MAX(1,uiC->pInt["numeroDeLados"]);
					float ang =
					uiC->pInt["angulo"] + uiC->pInt["anguloAudio"] * audio + uiC->pInt["anguloBeat"]*beat + 45 ;
					float numLinhas = uiC->pBool["audioAfetaNLinhas"] ?
					int((float)uiC->pInt["numero de linhas"] * updown) % (uiC->pInt["numeroDeLinhas"]) : uiC->pInt["numeroDeLinhas"];
					
					float raiofinal = o.raio; //raioPoli + raioRand;

					// first push
					ofPushMatrix();
					ofTranslate(o.x, o.y);

					
					float jitter = uiC->pEasy["jitter"] + uiC->pEasy["jitterAudio"]*audio + uiC->pEasy["jitterBeat"]*beat;
					
					for (int b=0; b < numLinhas; b++) {
						int distancia = (float)raiofinal - (float)uiC->pInt["distanciaEntreLinhas"]*(float)b;
						if (distancia > 0)
						{
							float jitterx, jittery = 0;
							vector <glm::vec2> points;
							for (int c=0; c < lados; c++) {
								int angulo = c * 360.0 / lados + ang;
								if (jitter)
								{
									jitterx = (ofNoise(o.index + 1, b + 1, c + 1)-.5) * jitter;
									jittery = (ofNoise(o.index * 1.5 + 1, b*1.5 + 1, c*1.5 + 1)-.5) * jitter;
								}
								
								if (uiC->pBool["pointSprite"]) {
									mesh.addColor(estacor);
									mesh.addVertex(ofPoint(r2x(angulo,distancia) + jitterx + o.x, r2y(angulo,distancia) + jittery + o.y));
								} else {
									points.push_back(glm::vec2(r2x(angulo,distancia) + jitterx, r2y(angulo,distancia) + jittery));
								}
							}
							
							estacor = getCor (o.index/(float)nObjetos + b/(float)numLinhas);
							ofSetColor(estacor);

							if (uiC->pString["mode"] == "fill") {
								ofFill();
								ofBeginShape();
								ofVertices(points);
								ofEndShape(true);
							}
							if (uiC->pString["mode"] == "lineBlack") {
								ofFill();
								ofSetColor(0);
								ofBeginShape();
								ofVertices(points);
								ofEndShape(true);
							}
							if (uiC->pString["mode"] == "lineBlack" || uiC->pString["mode"] == "line") {
								ofNoFill();
								ofBeginShape();
								ofVertices(points);
								ofEndShape(true);
							}
						}
					}
					ofPopMatrix();
				}
				
				else {
					if (uiC->pBool["pointSprite"]) {
						mesh.addVertex(ofPoint(o.x, o.y));
						mesh.addColor(estacor);
					} else {
						ofSetColor(estacor);
						ofPushMatrix();
						ofTranslate(o.x, o.y);
						float w = o.largura * o.scale;
						float h = o.altura * o.scale;
						ofRotateDeg(angulo);
						ofDrawRectangle(0,0, w,h);
						ofPopMatrix();
					}
				}
			}
		}
		ofSetRectMode(OF_RECTMODE_CORNER);
	}

	
	void uiEvents(ofxMicroUI::element & e) override {
		
		if (e.name == "load" || (e.name == "Beat!" && uiC->pBool["randomNoBeat"])) {
			updateObjects = true;
		}
		if (e.name == "horizontalAlign" || e.name == "verticalAlign" || e.name == "objetos") {
			updateObjects = true;
		}
		// string nome = e.name;

		// trocar isso pra e.tag == "rectParams"
//		if ((*cena == "rect" || *cena == "poligonos")) {
//			if (nome == "load" || (nome == "Beat!" && uiC->pBool["randomNoBeat"])) {
//				updateObjects = true;
//			}
//			if (nome == "horizontalAlign" || nome == "verticalAlign" || nome == "objetos") {
//				updateObjects = true;
//			}
//		}
//
//		if (nome == "video") {
//			if (uiC->pString["video"] != "") {
//				vid.load(((radio*)uiC->getElement("video"))->getFullFileName());
//				vid.setVolume(0);
//				vid.play();
//			}
//		}
		
		if (e.name == "angulos") {
			if (*e.s == "horiz") {
				angulosTodos[0] = 0;
				angulosTodos[1] = 0;
				angulosTodos[2] = 0;
				angulosTodos[3] = 0;
			}
			else if (*e.s == "vert") {
				angulosTodos[0] = 90;
				angulosTodos[1] = 90;
				angulosTodos[2] = 90;
				angulosTodos[3] = 90;
			}
			else if (*e.s == "diagonal") {
				angulosTodos[0] = 45;
				angulosTodos[1] = 45;
				angulosTodos[2] = 45;
				angulosTodos[3] = 45;
			}
			else if (*e.s == "xis") {
				angulosTodos[0] = 45;
				angulosTodos[1] = 135;
				angulosTodos[2] = 45;
				angulosTodos[3] = 135;
			}
			else if (*e.s == "cruz") {
				angulosTodos[0] = 0;
				angulosTodos[1] = 90;
				angulosTodos[2] = 0;
				angulosTodos[3] = 90;
			}
			else if (*e.s == "asterisco") {
				angulosTodos[0] = 0;
				angulosTodos[1] = 45;
				angulosTodos[2] = 90;
				angulosTodos[3] = 135;
			}
		}
	}
};


struct scenePoligonos : public sceneRect {
public:

	using sceneRect::sceneRect;
	void setup() override {
		setPoligonos();
		isPoligonos = true;
		setupObjetos();
//		cout << "this is setup scene poligonos" << endl;
	}
};



struct sceneTemp : public sceneMuti {
public:

	using sceneMuti::sceneMuti;
	void setup() override {
	}

	void draw() override {

	}	
};


struct sceneGrid : public sceneMuti {
public:

	
	float beat = 0;
	
	using sceneMuti::sceneMuti;
	void setup() override {
	}

	void draw() override {
//		ofTranslate(fbo->getWidth() * .5, fbo->getHeight() * .5);
		int limitX = uiC->pEasy["nObjetosX"] + uiC->pEasy["nObjetosXAudio"] * updown;
		int limitY = uiC->pEasy["nObjetosY"] + uiC->pEasy["nObjetosYAudio"] * updown;
		int limitZ = uiC->pEasy["nObjetosZ"] + uiC->pEasy["nObjetosZAudio"] * updown;
		
		float objetosDistancia = uiC->pEasy["objetosDistancia"]
		+ uiC->pEasy["objetosDistanciaAudio"] * updown
		+ uiC->pEasy["objetosDistanciaBeat"] * beat;
		
		int contagem = 0;
		float space = objetosDistancia;
		float totalObjetos = (limitX * 2.0 +1)*(limitY * 2.0 + 1) * (limitZ * 2.0+1);
		if (totalObjetos == 0)
			totalObjetos = 1.0;
		
		//cout << beat << endl;

		for (int a=-limitX; a<=limitX; a++) {
			for (int b=-limitY; b<=limitY; b++) {
				for (int c=-limitZ; c<=limitZ; c++) {
					float qual = ofNoise(a,b,c);

					float objetosRaio = uiC->pEasy["raio"]
					+ uiC->pEasy["raioAudio"] * r2f(ofPoint(0,1), getFreq(qual), uiC->pEasy["steps"])
					+ uiC->pEasy["raioBeat"] * beat
					;
					
					// GRID SCENE
					ofPoint scale = ofPoint (
						 objetosRaio * (uiC->pEasy["profundidade"] +
										uiC->pEasy["profundidadeAudio"] * r2f(ofPoint(0,1), getFreq(qual), uiC->pEasy["steps"])) +
										uiC->pEasy["profundidadeBeat"] * beat
						 ,
						 objetosRaio * (uiC->pEasy["altura"] +
										uiC->pEasy["alturaAudio"] * r2f(ofPoint(0,1), getFreq(qual), uiC->pEasy["steps"])) +
										uiC->pEasy["alturaBeat"] * beat
						 ,
						 objetosRaio * (uiC->pEasy["largura"] +
										uiC->pEasy["larguraAudio"] * r2f(ofPoint(0,1), getFreq(qual), uiC->pEasy["steps"])) +
										uiC->pEasy["larguraBeat"] * beat
						 );

					float indiceCor = contagem++/(float)totalObjetos;
					//						ofSetColor(ui->getCor(indiceCor));
					ofSetColor(getCor(indiceCor));
					//cout << ui->getCor(indiceCor) << endl;
					// xaxa
					//ofSetColor(255);
					ofPushMatrix();


					float x = space * (float)a + uiC->pEasy["randomPos"] * (ofNoise(a*10.0 + .1, b, c + .1)-.5);
					float y = space * (float)b + uiC->pEasy["randomPos"] * (ofNoise(b*10.0 + .1, a, c + .1)-.5);
					float z = space * (float)c + uiC->pEasy["randomPos"] * (ofNoise(c*10.0 + .1, a, b + .1)-.5);

					ofTranslate(ofPoint(x,y,z));
					if (uiC->pFloat["randomAngle"] || uiC->pFloat["randomAngleAudio"]) {
						ofRotateDeg((uiC->pEasy["randomAngle"] + uiC->pEasy["randomAngleAudio"] * getFreq(qual)) * ofNoise(a,b,c)
								 + uiC->pEasy["randomAngleBeat"] * beat
								 
								 ,1,1,1);
					}
					objeto3dneu(ofPoint(0,0,0), 1.0, scale, qual, indiceCor);
					
					//objeto3d(ofPoint(0,0,0), 1.0, scale, indiceCor);

					ofPopMatrix();
				}
			}
		}
	}
	
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (ofIsStringInString(e.name, "_3d") || e.name == "load") {
			updateSolids();
		}
	}
};


void setupScenesMuti() {
//	scenes.push_back(new sceneRect(u, fbo));
	scenes.push_back(new sceneRect(&config));
	scenesMap["rect"] = scenes.back();

//	scenes.push_back(new scenePoligonos(u, fbo));
	scenes.push_back(new scenePoligonos(&config));
	scenesMap["poligonos"] = scenes.back();

	scenes.push_back(new sceneUniversoNovo(u, fbo));
	scenesMap["universoNovo"] = scenes.back();

	scenes.push_back(new sceneGrid(u, fbo));
	scenesMap["grid"] = scenes.back();

}
