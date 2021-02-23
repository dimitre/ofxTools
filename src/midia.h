class ofxMidia : public ofBaseApp {
public:
	ofxMidiOut midiOut;
	string midiDevicesLastDev = "";
	ofxMicroUI * uiMidi = NULL;
	ofxMicroUI * uiNotes = NULL;
	ofxMicroUI * uiC = NULL;


	void setUI(ofxMicroUI * u) {
		
//		midi.setUI(&u.uis["midi"]);
//		midi.setUINotes(&u.uis["notes"]);

		uiMidi = &u->uis["midi"];
		uiNotes = &u->uis["notes"];
		uiC = &u->uis["sceneMidi"];
		
		ofAddListener(uiMidi->uiEvent, this, &ofxMidia::midiDevicesUIEvent);
		ofAddListener(uiNotes->uiEvent, this, &ofxMidia::notesUIEvent);
	}
	
	void setUINotes(ofxMicroUI * u) {
		uiNotes = u;
		ofAddListener(uiNotes->uiEvent, this, &ofxMidia::notesUIEvent);
	}

	// renomear pra midiDevicesSetup
	
	void midiDevicesUIList() {
		string vals = ofJoinString(midiOut.getOutPortList(), "|");
		vector <string> lines;
		uiMidi->futureLines.push_back("radioPipeNoLabel	midiPort	"+vals);
	}
	
	void setup() {
		ofAddListener(ofEvents().update, this, &ofxMidia::onUpdate);
		midiDevicesUIList();

		uiMidi->saveMode = ofxMicroUI::MASTER;
		uiMidi->loadMode = ofxMicroUI::MASTER;
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
			tempo = ofGetElapsedTimeMillis() + del;
			tempoOff = tempo + d;
			note = n;
			
			played = false;
			end = false;
		}
		nota() {
		}
	};
	
//	vector <nota> notas = vector <nota> (1000);
	vector <nota> notas;


	void onUpdate(ofEventArgs &data) {
//		cout << "update midi" << endl;
//		midiOut.sendNoteOn(m.channel, m.nota, vel);
		update();
		for (auto & n : notas) {
			if (!n.played) {
				if (n.tempo <= ofGetElapsedTimeMillis()) {
					midiOut.sendNoteOn(1, n.note, n.vel);
//					cout << "send note " << n.note << endl;
					n.played = true;
				}
			}
			else {
				if (!n.end && n.tempoOff <= ofGetElapsedTimeMillis()) {
					midiOut.sendNoteOff(1, n.note, n.vel);
//					cout << "send note off" << n.note << endl;
					n.end = true;
				}
			}
		}
	}

	float expand(float val, float mult = 0.0) {
		return ofClamp(ofMap(val, 0.0, 1.0, -mult, 1.0+mult), 0, 1);
	}


	void midi2() {
		cout << "!!! MIDI2" << endl;
		cout << uiC->pInt["notas"] << endl;

		for (auto & e : uiC->pInt) {
			cout << e.first << endl;
		}

		for (int a=0; a<uiC->pInt["notas"]; a++) {
			cout << a << endl;
			float noisenota = expand(ofNoise(uiC->pFloat["notaNoiseMult"] * a + uiC->pFloat["notaNoiseStart"]), uiC->pFloat["notaNoiseExpand"]);
			// noisenota = ofClamp(noisenota, 0.0, 1.0);
			float noise = noisenota*scales.size();
			int note = scales[int(noise)];

			float d = expand(ofNoise(uiC->pFloat["durationNoiseMult"] * a + uiC->pFloat["durationNoiseStart"]), uiC->pFloat["durationNoiseExpand"]);
			int duration = ofMap(d, 0, 1, uiC->pInt["minDuration"], uiC->pInt["maxDuration"]);
			// int duration = ofRandom(uiC->pInt["minDuration"], uiC->pInt["maxDuration"]);

			float v = expand(ofNoise(uiC->pFloat["velNoiseMult"] * a + uiC->pFloat["velNoiseStart"]), uiC->pFloat["velNoiseExpand"]);
			int vel = ofMap(v, 0, 1, uiC->pInt["minVel"], uiC->pInt["maxVel"]);

			float del = expand(ofNoise(uiC->pFloat["delayNoiseMult"] * a + uiC->pFloat["delayNoiseStart"]), uiC->pFloat["delayNoiseExpand"]);
			int delay = ofMap(del, 0, 1, uiC->pInt["minDelay"], uiC->pInt["maxDelay"]);

			cout << d << " : " << v << " : " << del << endl;
			// int delay = ofRandom(uiC->pInt["minDelay"], uiC->pInt["maxDelay"]);
			notas.emplace_back(note, duration, vel, delay);			
		}
	}

	//random apenas
    void test() {
		cout << "!!! TEST" << endl;
		for (int a=0; a<uiC->pInt["notas"]; a++) {
//			int note = ofRandom(uiNotes->pInt["minNote"], uiNotes->pInt["maxNote"]);
			
			int note = scales[ofRandom(0, scales.size()-1)];
			// note duration velocity delay
			int duration = ofRandom(uiC->pInt["minDuration"], uiC->pInt["maxDuration"]);
			int vel = ofRandom(uiC->pInt["minVel"], uiC->pInt["maxVel"]);
			int delay = ofRandom(uiC->pInt["minDelay"], uiC->pInt["maxDelay"]);
			
//			notas.emplace_back(note, ofRandom(30,1500), ofRandom(0,100), ofRandom(0,2000));
			notas.emplace_back(note, duration, vel, delay);
			//, ofGetElapsedTimeMillis() + ofRandom(0,2000)
		}
    }
	
	
	ofFbo * fbo = NULL;
	
	void draw() {
		for (auto & n : notas) {
			float raio = n.vel * .5;
			n.x = ofMap(n.delay, uiC->pInt["minDelay"],  uiC->pInt["maxDelay"], 0, fbo->getWidth());
			n.y = ofMap(n.note, uiNotes->pInt["minNote"], uiNotes->pInt["maxNote"], fbo->getHeight(), 0);
			if (n.played) {
				ofFill();
			} else {
				ofNoFill();
			}
			ofDrawCircle(n.x, n.y, raio);
		}

		ofNoFill();
		ofBeginShape();
		for (auto & n : notas) {
			ofVertex(n.x, n.y);
		}
		ofEndShape();

	}
	
	float nextJump = 0;
	
	vector <int> willErase;
    void update() {
		willErase.clear();
		for (int a=notas.size()-1; a>=0; a--) {
//			cout << a << endl;
			if (notas[a].end) {
				willErase.push_back(a);
			}
		}
		
		for (auto w : willErase) {
			notas.erase(notas.begin() + w);
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
				if (midiOut.isOpen()) {
					midiOut.closePort();
				}
				cout << "midiOut open :: " << dev << endl;
				midiOut.openPort(dev);
				midiDevicesLastDev = dev;
			}
		}
	}
	
	void notesUIEvent(ofxMicroUI::element & e) {
//		cout << "notesUIEvent " << e.name << endl;
		if (e.tag == "scale") {
			// booleano has to update notes
			updateNotas();
		}
	}

	vector <int> scales;
	
	void updateNotas() {
		scales.clear();
		for (int o=0; o<10; o++) {
			for (int a=0; a<12; a++) { // cromatica
//				string nome = "n" + ui->pString["cell"] + "_" + ofToString(a);
				string nome = "n1_" + ofToString(a);
				float valor = uiNotes->pFloat[nome];
//				if (valor) {
					int index = a + 12 * o;
				if (index > uiNotes->pInt["minNote"] && index < uiNotes->pInt["maxNote"]) {
						for (int n=0; n<(valor * 10.0); n++) {
							scales.push_back(index);
						}
					}
//				}
			}
		}
//		cout << "updateNotas" << endl;
//		cout << scales.size() << endl;
	}

};
