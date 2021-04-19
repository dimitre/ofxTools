
#define USETYPE 1

struct sceneStocks : public ofxScenes::sceneType, ofxScenes::sceneIncrementa {
public:
	using sceneType::sceneType;
	// using sceneDmtrBasic::sceneDmtrBasic;

	int screen = 0;

	struct entry {
		public:
		float val;
		float valNorm;
		float diff;
		float diffNorm;
		ofColor cor;
		entry(float v) : val(v) {}
	};

	struct view {
		public:

		int index = 0;
		int jump = 2;
		int numero = 100;
		bool isLoaded = false;

		float media = 0;
        
        string loadedUrl = "";

		vector <entry> entriesAll;
		vector <entry> entries;

		string symbol = "SPY"; // TSLA ETSY
		string range = "1hour";

		view() {}
		view(int i) : index(i) {
			ofRegisterURLNotification(this);
		}
		
		// ofColor corUp = ofColor(0,0,255);
		// ofColor corDown = ofColor(255,0,0);

		void norm() {
			if (entriesAll.size()) {
				entries.clear();
				for (int a=1; a<numero+1; a++) {
					int index = a*jump;
					if (index > 0) {
						entries.push_back(entriesAll[index]);
					}
				}
				std::reverse(entries.begin(),entries.end());

				for (int a=1; a<entries.size(); a++) {
					entries[a].diff = entries[a].val - entries[a-1].val;
				}
				auto max = *std::max_element( entries.begin(), entries.end(), []( const entry &a, const entry &b ) { return a.val < b.val; });
				auto min = *std::min_element( entries.begin(), entries.end(), []( const entry &a, const entry &b ) { return a.val < b.val; });
				auto maxDiff = *std::max_element( entries.begin()+1, entries.end(), []( const entry &a, const entry &b ) { return a.diff < b.diff; });
				auto minDiff = *std::min_element( entries.begin()+1, entries.end(), []( const entry &a, const entry &b ) { return a.diff < b.diff; });
				
				//cout << "max & min " << max.val << " -- " << min.val << endl;;
				for (auto & e : entries) {
					e.valNorm = ofMap(e.val, min.val, max.val, 0, 1);
					e.diffNorm = ofMap(e.diff, minDiff.diff, maxDiff.diff, 0, 1);
					// e.cor = corDown.getLerped(corUp, e.diffNorm);
				}

				// media = entries[entries.size()-1].valNorm - entries[entries.size()-1-cols].valNorm ;
				media = entries[entries.size()-1].valNorm - entries[1].valNorm ;

			}
		}
		
		void recolor() {
			for (auto & e : entries) {
				// e.cor = corDown.getLerped(corUp, e.diffNorm);
			}
		}

		void stringToJson(string json) {
			if (json != "") {
				ofJson js = ofJson::parse(json);
				for (auto & k : js) {
					entriesAll.emplace_back(k["close"]);
				}
				isLoaded = true;
			} else {
	//            cout << "empty json" << endl;
			}
		}

		bool haveToLoad = false;
		void load() {
			// cout << "load for screen index " << index << endl;
			haveToLoad = true;
		}

		void loadData() {
            cout << "loadData for screen index " << index << endl;
			cout << ofGetFrameNum() << endl;
			haveToLoad = false;
			entriesAll.clear();

			bool loadLocal = false;

			if (loadLocal) {
				ofBuffer file = ofBufferFromFile("_json/temp.json");

				string json = file.getData();
				stringToJson(json);
			} else {
				cout << "json loading " << symbol << " : " << range << endl;
				string url = "https://financialmodelingprep.com/api/v3/historical-chart/" +range + "/" + symbol + "?apikey=562c36be10fce020d33f378be7ddc4bb";

//				cout << url << endl;
                loadedUrl = url;
				int id = ofLoadURLAsync(url);
			}
		}
		
		void urlResponse(ofHttpResponse & resp) {
//            cout << "-----------" << endl;
//			cout << "YESSS RESPONSE" << endl;
//			cout << resp.request.name << endl;
//			cout << resp.status << endl;

			if (resp.status==200 && resp.request.name == loadedUrl) { // && resp.request.name == "async_req"
				cout << "json loaded! index=" << index << endl;
				string json = resp.data.getText();
				
				string fileName = "_cache/" + ofToString(index) + ".json";
				ofFile myTextFile(fileName,ofFile::WriteOnly);
				myTextFile << json;
				myTextFile.close();
//                    cout << "cache written to file " << fileName << endl;
				
				stringToJson(json);
				norm();

			} else {
//              cout << response.status << " " << response.error << endl;
//              if (response.status != -1) loading = false;
			}
		}
	};

	view views[3] { 
		view(0),
		view(1),
		view(2),
	};

	void setup() override {
        sceneType::setup();
	}

	void draw() override {
		view * v = &views[screen];

		if (v->haveToLoad) {
			v->loadData();
		}
		
		fbo = config->fbo;
		screen = config->screen;
		string symbol = views[screen].symbol;
		
		// float cols = uiC->pEasy["cols"];
		float cols = views[screen].numero;
		float spacing = uiC->pFloat["spacing_" + ofToString(screen)] * fbo->getWidth() / cols;
		// float ny = fbo->getHeight() / spacing * uiC->pFloat["linesMult"];

		// cout << "screen : " << screen << endl;
		// cout << fbo->getWidth() << endl;
		float ny = fbo->getHeight() / spacing * uiC->pFloat["linesMult_" + ofToString(screen)];
		float seno = sin(ofGetElapsedTimef() * 2.0);
		float rotx = uiC->pEasy["rotX"] + incrementa("rotXauto"); //uiC->pFloat["rotXauto"] * seno;
		float roty = uiC->pEasy["rotY"] + incrementa("rotYauto"); //uiC->pFloat["rotYauto"] * seno;
		float rotz = uiC->pEasy["rotZ"] + incrementa("rotZauto"); //uiC->pFloat["rotZauto"] * seno;

		vector <entry> entries = views[screen].entries;

		if (uiC->pBool["wireframe"]) {
			ofNoFill();
		} else {
			ofFill();
		}

		if (entries.size()) {
			float mediaOut = uiC->pBool["testMedia"] ? uiC->pFloat["media"] : v->media;
			float mediaHue = ofMap(mediaOut, -1, 1, uiC->pFloat["downHue"], uiC->pFloat["upHue"]);

			for (int x=0; x<cols; x++) {
				int index = entries.size() - cols + x -1;
				if (index < entries.size()) {
					// ofSetColor(entries[index].cor);
					float hueRange = ofMap(x, 0, cols, 0, uiC->pFloat["hueRange"]);
					ofSetColor(ofColor::fromHsb(mediaHue + hueRange, 255, 255));
					// float my = uiC->pEasy["colsMin"] + entries[index].valNorm * uiC->pEasy["colsMult"];
					float my = ofMap(entries[index].valNorm, 0, 1, uiC->pEasy["linesMin"], ny);
					for (int y=0; y<my; y++) {
						ofPushMatrix();
						ofTranslate((x-cols*.5)*spacing + fbo->getWidth()*uiC->pEasy["offX_"+ofToString(screen)], fbo->getHeight()*uiC->pEasy["offY_"+ofToString(screen)] + y*spacing);
						ofRotateXDeg(rotx);
						ofRotateYDeg(roty);
						ofRotateZDeg(rotz);
						// ofSetColor(getColorRange(y/12.0));

						ofDrawBox(spacing * uiC->pEasy["raioX"], spacing * uiC->pEasy["raioY"], spacing * uiC->pEasy["raioZ"]);
						ofPopMatrix();
					}
				}
			}
		} else {

		}
	}


	void drawTexto() {
		ofSetColor(255);
		// string s = views[screen].symbol + " " + views[screen].range;
		if (type != NULL && type->isLoaded())
		{
			// int tx = uiC->pInt["textoX_"+ ofToString(screen)];
			// int ty = uiC->pInt["textoY_"+ ofToString(screen)];
			int tx = uiC->pInt["textoX"];
			int ty = uiC->pInt["textoY"];
			if (uiC->pBool["drawAsShapes"]) {
				type->drawStringAsShapes(text, tx, ty);
			} else {
				type->drawString(text, tx, ty);
			}
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.tag == "numero") {
			int index = ofToInt(ofSplitString(e.name, "_")[1]);
			views[index].numero = *e.i;
		}
		else if (e.tag == "range") {
//			cout << e.name << endl;
			if (ofIsStringInString(e.name, "range_")) {
				if (e.s != NULL) {
					int index = ofToInt(ofSplitString(e.name, "_")[1]);
					views[index].range = *e.s;
					views[index].load();
				}
			}
		}
		else if (e.tag == "symbol") {
			if (ofIsStringInString(e.name, "symbol_")) {
				if (e.s != NULL) {
					int index = ofToInt(ofSplitString(e.name, "_")[1]);
					views[index].symbol = *e.s;
					views[index].load();
				}
			}
		}

		if (e.name == "jump") {
			for (auto & v : views) { 
				if (*e.i != v.jump) {
					v.jump = *e.i; 
					v.norm(); 
				}
			}
		}

		text = views[screen].symbol + " " + views[screen].range;

        sceneType::uiEvents(e);
	}
};
