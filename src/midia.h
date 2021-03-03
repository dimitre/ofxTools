// renomear de midia pra outra coisa?
    static float expand(float val, float mult = 0.0) {
        return ofClamp(ofMap(val, 0.0, 1.0, -mult, 1.0+mult), 0, 1);
    }

class ofxMidia { // : public ofBaseApp
public:

	ofFbo * fbo = NULL;
	string midiDevicesLastDev = "";
	ofxMicroUI * uiMidi = NULL;
	ofxMicroUI * uiNotes = NULL;
	ofxMicroUI * uiC = NULL;
    ofxMicroUI * u = NULL;
    
    
	struct configMidi {
	public:
        ofxMidiOut midiOut;

		ofFbo * fbo = NULL;
        ofxMicroUI * u = NULL;
		ofxMicroUI * uiC = NULL;
        ofxMicroUI * uiNotes = NULL;

        vector <int> scales;
        
		configMidi() {};
		configMidi(ofFbo * _fbo, ofxMicroUI * _u) : fbo(_fbo), u(_u) {
            uiC = &u->uis["sceneMidi"];
            uiNotes = &u->uis["notes"];
        }

		struct nota {
		public:
	//		uint64_t ofGetElapsedTimeMillis();
			uint64_t tempo;
			uint64_t tempoOff;
			float x, y;

			int note;
			int vel;
			uint64_t duration;
			uint64_t delay;
			bool played = true;
			bool end = true;
			
			nota(int n, uint64_t d, int v, uint64_t del = 0) : note(n), vel(v), duration(d), delay(del) { //, uint64_t d
//				tempo = ofGetElapsedTimeMillis() + del;
//                tempo = ofGetElapsedTimeMicros() + del;
//                tempoOff = tempo + duration;
                tempo = ofGetElapsedTimeMicros() + 1000 * del;
                tempoOff = tempo + duration * 1000;
				note = n;
				
				played = false;
				end = false;
			}
			nota() {
			}
		};
		
	//	vector <nota> notas = vector <nota> (1000);
		vector <nota> notas;
        
        void addNoteF(float n, float duration, float vel, float delay = 0) {
            if (scales.size()) {
//                cout << "----" << endl;
//                cout << n << endl;
//                cout << scales.size() << endl;
//                cout << n * scales.size() << endl;
                int note = scales[n * scales.size()];
                notas.emplace_back(note, duration, vel, delay);
            }
        }
        
        void update() {
            uint64_t tempo = ofGetElapsedTimeMicros();
            for (auto & n : notas) {
                if (!n.played) {
//                    if (n.tempo <= ofGetElapsedTimeMillis()) {
                    if (n.tempo <= tempo) {
                        midiOut.sendNoteOn(1, n.note, n.vel);
    //                    cout << "send note " << n.note << endl;
                        n.played = true;
                    }
                }
                else {
                    if (!n.end && n.tempoOff <= tempo) {
                        midiOut.sendNoteOff(1, n.note, n.vel);
    //                    cout << "send note off" << n.note << endl;
                        n.end = true;
                    }
                }
            }
        }

	} config;

    string & scene;

	ofxMidia(ofFbo * _f, ofxMicroUI * _u, string & s) : fbo(_f), u(_u), scene(s) {
		config = configMidi(fbo, u);
		uiMidi = &u->uis["midi"];
		uiNotes = &u->uis["notes"];
		uiC = &u->uis["sceneMidi"];
		ofAddListener(uiMidi->uiEvent, this, &ofxMidia::midiDevicesUIEvent);
		ofAddListener(uiNotes->uiEvent, this, &ofxMidia::notesUIEvent);
	}

	struct sceneMidi {
	public:
		bool isSetup = false;
		ofxMicroUI * uiC = NULL;
        ofxMicroUI * uiNotes = NULL;
        configMidi * config = NULL;
		string name = "";

		sceneMidi() {}
		sceneMidi(configMidi * _c, string n) : config(_c), name(n) {
			uiC = config->uiC;
            uiNotes = config->uiNotes;
            ofAddListener(uiC->uiEvent, this, &sceneMidi::uiEvents);
		}

		virtual void setup() {}
		virtual void update() {}
		virtual void bang() {}
		virtual void bang2() {}
		virtual void uiEvents(ofxMicroUI::element & e) {}

		virtual void draw() {
			for (auto & n : config->notas) {
				float raio = n.vel * .5;
				n.x = ofMap(n.delay, uiC->pInt["minDelay"],  uiC->pInt["maxDelay"], 0, config->fbo->getWidth());
				n.y = ofMap(n.note, uiNotes->pInt["minNote"], uiNotes->pInt["maxNote"], config->fbo->getHeight(), 0);
				if (n.played) {
					ofFill();
				} else {
					ofNoFill();
				}
				ofDrawCircle(n.x, n.y, raio);
			}

			ofNoFill();
			ofBeginShape();
			for (auto & n : config->notas) {
				ofVertex(n.x, n.y);
			}
			ofEndShape();
		}
	};

	vector <sceneMidi *> scenes;
	map <string, sceneMidi *> scenesMap;

	void setUINotes(ofxMicroUI * u) {
		uiNotes = u;
		ofAddListener(uiNotes->uiEvent, this, &ofxMidia::notesUIEvent);
	}

	// renomear pra midiDevicesSetup
	void midiDevicesUIList() {
		string vals = ofJoinString(config.midiOut.getOutPortList(), "|");
		vector <string> lines;
		uiMidi->futureLines.push_back("radioPipeNoLabel	midiPort	"+vals);
	}
	
	void setup() {
		ofAddListener(ofEvents().update, this, &ofxMidia::onUpdate);
		midiDevicesUIList();
		uiMidi->saveMode = ofxMicroUI::MASTER;
		uiMidi->loadMode = ofxMicroUI::MASTER;

		for (auto & s : scenes) {
			cout << ":: adding " << s->name << endl;
			scenesMap[s->name] = s;
			scenesMap[s->name]->setup();
		}
	}
	
	void onUpdate(ofEventArgs &data) {
		// nao sei se precisa update aqui.
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				scenesMap[scene]->update();
			}
		}
		update();
        // checa as notas se foram tocadas ou nao
        config.update();
	}


	void bang() {
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				scenesMap[scene]->bang();
			} else {
				cout << "Bang! scene not found " << scene << endl;
			}
		}
	}

	void bang2() {
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
				scenesMap[scene]->bang2();
			} else {
				cout << "Bang! scene not found " << scene << endl;
			}
		}
	}	
	
	void draw() {
//		ofSetLineWidth(ui->pEasy["lineWidth"]);
		if (scene != "" && scene != "_") {
			if ( scenesMap.find(scene) != scenesMap.end() ) {
//				ofSetColor(getColor(0, uiColors));
				scenesMap[scene]->draw();
			} else {
				cout << "scene not found " << scene << endl;
			}
		}
	}
	
	float nextJump = 0;
	
	vector <int> willErase;
    void update() {
        
        if (scene != "" && scene != "_") {
            if ( scenesMap.find(scene) != scenesMap.end() ) {
                scenesMap[scene]->update();
            }
        }
        
        // passar muita coisa aqui pra dentro do config
		willErase.clear();
		for (int a = config.notas.size()-1; a>=0; a--) {
//			cout << a << endl;
			if (config.notas[a].end) {
				willErase.push_back(a);
			}
		}
		
		for (auto w : willErase) {
			config.notas.erase(config.notas.begin() + w);
		}
		
		if (ofGetElapsedTimef() > nextJump) {
//			cout << notas.size() << endl;
			nextJump = ofGetElapsedTimef() + 1.0;
		}
    }

	void midiDevicesUIEvent(ofxMicroUI::element & e) {
		if (e.name == "midiPort") {
			string dev = *e.s;
			if (midiDevicesLastDev != dev) {
				if (config.midiOut.isOpen()) {
					config.midiOut.closePort();
				}
				cout << "midiOut open :: " << dev << endl;
                config.midiOut.openPort(dev);
				midiDevicesLastDev = dev;
			}
		}
	}
	
	void notesUIEvent(ofxMicroUI::element & e) {
		if (e.tag == "scale") {
			updateNotas();
		}
	}
	
	void updateNotas() {
		config.scales.clear();
		for (int o=0; o<10; o++) {
			for (int a=0; a<12; a++) { // cromatica
//				string nome = "n" + ui->pString["cell"] + "_" + ofToString(a);
				string nome = "n1_" + ofToString(a);
				float valor = uiNotes->pFloat[nome];
//				if (valor) {
					int index = a + 12 * o;
				if (index > uiNotes->pInt["minNote"] && index < uiNotes->pInt["maxNote"]) {
						for (int n=0; n<(valor * 10.0); n++) {
							config.scales.push_back(index);
						}
					}
//				}
			}
		}
	}
};


