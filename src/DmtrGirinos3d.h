
//ofxMicroUI * ui = &u.uis["ui"];

//namespace girinos3d {
#include "DmtrPolar.h"

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


	struct girino {
	public:
		girino() {}
		float rand;
		
		glm::vec3 pos;
		float qual;
		int cursor = 0;
		ofVec3f positions[60];
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
			glm::vec2 p = ofApp::p2c(glm::vec2(angulo, vel));
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

	void setupGirino() {
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

	void drawGirino() {
		
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



//}
