
//float scaleSyntype = 20.0;
float scaleSyntype = 0.03;

vector <ofPolyline> polys;
ofxMicroUI * uiL = uiC;

// armazena um contorno de uma letra
struct letra {
public:
	float largura;
	float altura;
	vector <ofPolyline> polylines; // mais de um traço por letra
};

// armazena todas as letras de uma fonte.
map <string, letra> letrasMap;


class fraseSyntype {
public:
	fraseSyntype() {}
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
					
					//cout << "---" << endl;

					if (p.getVertices()[0] == p.getVertices()[p.getVertices().size()-1])
					{
						poly.close();
						cout << "close" << endl;
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
			int c = 0;
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
//		cout << -cursorX/2 << endl;
//		cout << -alturaTotalFrase/2 << endl;
//		cout << polylines.size() << endl;
//		cout << "======" << endl;
		for (auto & p : polylines) {
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



void setupSyntype() {
	frase1.scaleSyntype = &scaleSyntype;
}

void loadLetter(string fileName) {
	cout << "loadletter :: " << fileName << endl;

	frase1.letrasMap = &letrasMap;

	letrasMap.clear();
	vector <string> linhas = u.textToVector(fileName);
	int contagem = 0;
	for (auto & linha : linhas) {
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
					ofPoint coords =
					ofPoint(
								ofToFloat(xy[0]) * 1.0,
								1.0-ofToFloat(xy[1]) * 1.0);
					largura = MAX(largura, coords.x);
					float x = coords.x;
					float y = coords.y;
					tempPolyline.addVertex(ofPoint(x, y));
					//cout << ofPoint(x, y) << endl;
					maxx = MAX(maxx,x);
					minx = MIN(minx,x);
				}

				letrasMap[letra].polylines.push_back(move(tempPolyline));
			}
			letrasMap[letra].largura = largura;
			contagem++;
		}
	}
}

void desenhaSyntype2() {
	
	if (uiL->pBool["palavra"]) {
		float cursorX = uiL->pFloat["offX"] * 100.0;
		float cursorY = uiL->pFloat["offY"] * 100.0;
		string frase = uiL->pString["frase"];
		ofNoFill();
		if (uiL->pBool["drawPolys"]) {
			frase1.drawPolys();
		}
		if (uiL->pBool["drawRounds"]) {
			frase1.drawRounds(uiL->pEasy["radius"]);
		}
		if (uiL->pBool["drawResampled"]) {
			frase1.drawResampled(uiL->pEasy["resampledSpace"], uiL->pEasy["radius"]);
			if (uiL->pBool["radius2"]) {
				frase1.drawResampled(uiL->pEasy["resampledSpace"], uiL->pEasy["radius2"]);
			}
		}
	}
}


void syntypeUIEvent(ofxMicroUI::element & e) {
	//cout << e.name << endl;
	if (e.name == "scale") {
		frase1.scale = *e.f;
		cout << frase1.scale << endl;
		frase1.reset();
	}
	else if (e.name == "entreletra") {
		frase1.entreletra = *e.f;
		frase1.reset();
		// settext ja vem reset embutido
	}

	else if (e.name == "frase") {
		frase1.setText(*e.s);
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
		if (f != "/" && f != "" && uiL->pString["loadedLetter"] != f) {
			uiL->pString["loadedLetter"] = f;
			loadLetter(f);
			frase1.reset();
		}
	}
}













#ifdef DMTRUI
void syntypeUIEvent(uiEv e) {
	//cout << e.name << endl;
	if (e.name == "scale") {
		frase1.scale = e.f;
		cout << frase1.scale << endl;
		frase1.reset();
	}
	else if (e.name == "entreletra") {
		frase1.entreletra = e.f;
		frase1.reset();
		// settext ja vem reset embutido
	}

	else if (e.name == "frase") {
		frase1.setText(e.s);
		// settext ja vem reset embutido
	}
	else if (e.name == "offX") {
		frase1.offX = e.f;
		frase1.reset();
	}
	else if (e.name == "offY ") {
		frase1.offY = e.f;
		frase1.reset();
	}

	
	else if (e.name == "letra") {
		string f = uiL->getFileFullPath(e.name);
		if (f != "" && uiL->pString["loadedLetter"] != f) {
			uiL->pString["loadedLetter"] = f;
			loadLetter(f);
			frase1.reset();
		}
	}
}
#endif
