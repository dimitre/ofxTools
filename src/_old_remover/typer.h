vector <string> frases;
// TYPER vars
ofTrueTypeFont font;


void setupTyper() {
	cout << "setupTyper" << endl;
	font.load("_type/Simetrica-Light_20190719T113225.otf", 40);
	frases = ofxMicroUI::textToVector("frases.txt");
	for (auto & f : frases) {
		cout << f << endl;
	}
}

string ofUTF16DecToUtf8Char(int input)
{
	std::stringstream ss;
	ss<< hex<<input;
	unsigned short myVar;
	sscanf(ss.str().c_str(),"%hx",&myVar);

	wchar_t  in = (wchar_t) myVar;
	string out;
	unsigned int codepoint = 0;
	if (in >= 0xd800 && in <= 0xdbff)
		codepoint = ((in - 0xd800) << 10) + 0x10000;
	else
	{
		if (in >= 0xdc00 && in <= 0xdfff)
			codepoint |= in - 0xdc00;
		else
			codepoint = in;

		if (codepoint <= 0x7f)
			out.append(1, static_cast<char>(codepoint));
		else if (codepoint <= 0x7ff)
		{
			out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else if (codepoint <= 0xffff)
		{
			out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else
		{
			out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		codepoint = 0;
	}
	return out;
}



//ofxMicroUI *uiC  = &u.uis["scene"];

int tempo = 0;
int subTexto = 0;
float nextCharTime = 0;

//setlocale(LC_ALL,locale.c_str())


void drawTyper() {
	string texto = "";
	if (frases.size() > 0) {
		texto = frases[uiC->pInt["frase"] % frases.size()];
	}

	Poco::UTF8Encoding utf8Encoding;
		if (uiC->pBool["upper"]) {
	//		texto = ofToUpper(texto, "en_US.iso885915");
			texto = ofToUpper(texto, "pt_BR.UTF-8");
		}
	string utf8String(texto);

	uiC->getInspector("texto")->set(texto);
	

	Poco::TextIterator it(utf8String, utf8Encoding);
	Poco::TextIterator end(utf8String);

	int numero = ofNoise(ofGetElapsedTimef()*uiC->pFloat["multNumberTime"]) * 10;

	string textoOutput;
	string textoOutput2;
	
	if (subTexto < texto.size() * uiC->pFloat["numbersDuration"]) {
		float qual = subTexto / (float) texto.size();
		if (ofGetElapsedTimef() > nextCharTime) {
			subTexto ++;
			nextCharTime = ofGetElapsedTimef() + uiC->pFloat["charSeconds"] + ofRandom(0,uiC->pFloat["charSecondsRand"]);
		}

		int contagem = 0;
		while (it != end) {
			// anima desenhando na tela.
			if (subTexto > contagem) {
				// coloca o tempo do proximo caracter a ser desenhado
				if (ofGetElapsedTimef() > nextCharTime) {
					subTexto ++;
					nextCharTime = ofGetElapsedTimef() + uiC->pFloat["charSeconds"] + ofRandom(0,uiC->pFloat["charSecondsRand"]);
				}

				string s = ofUTF16DecToUtf8Char(*it);
				if (ofNoise(contagem, ofGetElapsedTimef() * uiC->pFloat["tempoRandomChar"])>uiC->pFloat["numbersProbability"]
					&& s != " "
					&& s != "\r"
					&& s != "\n"
					)
				{
					s = ofToString(int(ofRandom(0,9)));
					int numero = ofNoise(contagem, ofGetElapsedTimef() * uiC->pFloat["tempoNumero"])*10.0;
					s = ofToString(int(numero));
				}
				textoOutput += s;
			}
			++it;
			contagem++;
		}

	}

	else {
		textoOutput = texto;
	}
	
	ofSetColor(255);
	//XAXA
	//ofSetColor(getCor(0,"cor1"));
	font.drawString(textoOutput, uiC->pInt["textoX"], uiC->pInt["textoY"]);

	if (uiC->pBool["debugNumber"]) {
		font.drawString(ofToString(numero), uiC->pInt["textoX"], uiC->pInt["textoY"] + 100);
	}
}


string fontFile = "";
int fontSize = 20;

void typerUIEvent(ofxMicroUI::element & e) {
	
	if (e.name == "loadPreset" || e.name == "resetTempo" || e.name == "begin") {
		subTexto = 0;
	}
	else if (e.name == "end") {
		subTexto = 500;
	}
	else if (e.name == "fontSize") {
		fontSize = *e.i;
		typerLoadFont();
	}
	else if (e.name == "type") {
		fontFile = ((ofxMicroUI::dirList*)&e)->getFileName();
		typerLoadFont();
	}
}

void typerLoadFont() {
	if (fontFile != "") {
		cout << fontFile << endl;
		font.load(fontFile, fontSize, true, true);
	}
}

