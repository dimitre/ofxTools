class artnetDmtr {
public:
	// https://art-net.org.uk/how-it-works/streaming-packets/artdmx-packet-definition/
	// connect, saber se ta conectaod
			  const std::string head = "Art-Net";

	unsigned char dataPacket[530];
	unsigned char data[512];
	unsigned char dataBlackout[512];
	// unsigned char data[4][512];
	
	std::shared_ptr<ofxUDPManager> udpRef = std::shared_ptr<ofxUDPManager>(new ofxUDPManager);
	string ip;

	int index = 0; //
	bool isSetup = false;
	unsigned int universe = 0;
	int universeStart = 0;


	void setup(string i, int u = 0) {

		ip = i;
		universeStart = u;

		cout << "setup ip = " << ip << ", universeStart = " << universeStart << endl;
		unsigned int size = head.length();
		for (auto & c : dataPacket) {
			c = 0;
		}
		for (auto & c : dataBlackout) {
			c = 0;
		}

		for (unsigned int a=0; a<size; a++) {
			dataPacket[a] = head[a];
		}
		static constexpr short OP_OUTPUT = 0x5000;
		dataPacket[size] = 0;//end string
		// 8
		dataPacket[size + 1] = (OP_OUTPUT & 0xff);
		dataPacket[size + 2] = (OP_OUTPUT >> 8) & 0xff;
		dataPacket[size + 3] = 0;//protocol version high byte
		dataPacket[size + 4] = 14;//protocol version low byte
		unsigned int seq = 0;
		dataPacket[size + 5] = seq;//sequence no - disable sequence(0)
		//13
		dataPacket[size + 6] = 0;//The physical input port from which DMX512 - information only
		dataPacket[14] = universe;
//			dataPacket[15] = universe;
		dataPacket[15] = 0;
		// 0x200 = 512 em hex. msb no 16 e lsb no 17
		dataPacket[16] = 0x02; //
		dataPacket[17] = 0x00;
		// dataPacket[16] = 0x00; //
		// dataPacket[17] = 0x02;
//            dataPacket[0] = head;
		
		if (connect()) {
			isSetup = true;
			cout <<  ">>>>>>>>> conn ok " << ip << endl ;
		}
		else {
			cout <<  ">>>>>>>>> conn not ok " << ip << endl;
		}

	}
	

	artnetDmtr() {}

	artnetDmtr (string i, int u = 0) : ip(i), universeStart(u) {
		// setup();
	}

	~artnetDmtr() {
		memcpy(&dataPacket[18] , &dataBlackout[0], 512);
		for (int a=0; a<4; a++) {
			sendToUniverse(a);
		}
		udpRef->Close();
	}

	void setUniverse(unsigned int universe) {
		// fazer bit shift se precisar mais tarde
		int uni = universe;
		// cout << "setUniverse index " << index << " : uni " << uni << " " << ip << endl;
		dataPacket[14] = uni;
	}
	
	void send() {
		if (isSetup) {
			udpRef->Send((const char *)dataPacket, 530);
		}
	}

	void sendToUniverse(uint8_t universe) {
		setUniverse(universe);
		send();
	}


	void sendDataToUniverse(const void * dataInput, unsigned int universe, size_t size = 512) {
		setUniverse(universe);
		// memcopy to data at pos 18.
		//memcpy(void *destination, const void *source, size_t number_of_bytes)
		// cout << &dataInput << endl;
		// cout << dataInput << endl;
		cout << &dataPacket[18] << endl;
		cout << dataPacket[18] << endl;
		// cout << *dataPacket[18] << endl;
		cout << "----" << endl;
		memcpy(&dataPacket[18], dataInput, size);
		send();
	}

	void dataToPacket(size_t size = 512) {
		memcpy(&dataPacket[18] , &data[0], size);
	}

	void random() {
//            cout << "send " << endl;
		for (int a=0; a<512; a++) {
			data[a] = ofClamp(ofRandom(-1000,255), 0, 255);
		}
		dataToPacket();
	}

	void ciclo() {
		int saida = ofClamp(ofMap(sin(ofGetElapsedTimef()*2), -1, 1, -255, 355), 0, 255);
		for (int a=0; a<512; a++) {
			data[a] = saida;
		}
		dataToPacket();
	}
	
	bool connect() {
		// cout << ip << endl;
		udpRef->Create();
		udpRef->SetTTL(1);
		udpRef->SetNonBlocking(true);
		bool saida = udpRef->Connect(ip.c_str(), 6454); // artnet port
		return saida;
	}
};


uint8_t data[512];
artnetDmtr arts[9];
// vector <artnetDmtr> arts;

void setupArts() {
	ofSetWindowPosition(20,20);
	for (auto & c : data) {
		c = 0;
	}

	int index = 0;
	for (auto & l : ofxMicroUI::textToVector("artnet.txt")) {
		if (l != "") {
			vector <string> cols = ofSplitString(l , " ");
			arts[index].setup(cols[0], ofToInt(cols[1]));
			arts[index].index = index;
			index++;
		}
		// arts.emplace_back(cols[0], ofToInt(cols[1]));
	}
}


void drawArts() {
	for (auto & c : arts) {
		int maxUni = 4;
		int channel = 499; // 500 no dmx
		if (uiC->pString["mode"] == "mesa") {
			for (int i=0; i<8; i++) {
				c.data[channel + i] = uiC->pInt["c_" + ofToString(i)];
			}
			c.dataToPacket();
			for (int a=0; a<4;a++) {
				// c.sendDataToUniverse(&data, a, 512);
				c.sendToUniverse(a);
			}
		}
		else if (uiC->pString["mode"] == "cycle") {
			data[channel + 0] = 255;
			// dimmer, rgb
			data[channel + 1] = ofClamp(ofMap(sin(ofGetElapsedTimef() * 4 * uiC->pEasy["cycleSpeed"]), -1, 1, -400, 300), 0, 255);
			data[channel + 2] = ofClamp(ofMap(sin(ofGetElapsedTimef() * 3 * uiC->pEasy["cycleSpeed"]), -1, 1, -400, 300), 0, 255);
			data[channel + 3] = ofClamp(ofMap(sin(ofGetElapsedTimef() * 2 * uiC->pEasy["cycleSpeed"]), -1, 1, -400, 300), 0, 255);

			for (int a=0; a<maxUni;a++) {
				c.sendDataToUniverse(data, a , 512);
			}
		}
		if (uiC->pString["mode"] == "random") {
			c.random();
			for (int a=0; a<maxUni;a++) {
				c.sendToUniverse(a);
			}
		}
		else if (uiC->pString["mode"] == "ciclo") {
			c.ciclo();
			for (int a=0; a<maxUni;a++) {
				c.sendToUniverse(a);
			}
		}
	}
}
