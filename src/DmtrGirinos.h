#include "polar.h"

bool girinoChanged = false;
void girinoUIEvent(ofxMicroUI::element & e) {
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

void setupGirino() {
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

void drawGirino() {
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



