struct sceneImage : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	ofImage * i;
	ofTexture * tex;

	void setup() override {
		tex = &uiC->pImage["image"].getTexture();
		i = &uiC->pImage["image"];
	}

	void draw() override {
		if (i->isAllocated()) {
			ofSetColor(255);
			float scale = uiC->pEasy["scale"];
			float w = i->getWidth() * scale;
			float h = i->getHeight() * scale;
			ofTranslate(fbo->getWidth() * .5, fbo->getHeight() * .5);

			float x = -w*.5 + uiC->pEasy["offX"];
			float y = -h*.5 + uiC->pEasy["offY"];

			i->draw(x, y, w, h);
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (!e._settings->presetIsLoading && e._ui->uiIsCreated) {
			if (e.name == "resetOffset") {
				uiC->getSlider("offX")->set(0.0);
				uiC->getSlider("offY")->set(0.0);
				// resetIncrementa("rotTime");
			}
		}
	}
};

struct sceneNdi : public ofxScenes::sceneDmtr {
	using sceneDmtr::sceneDmtr;

#ifdef USENDI
	ofxNDIreceiver ndiReceiver; // NDI receiver
	ofTexture ndiTexture; // Texture to receive

	void setup() override {
		cout << ndiReceiver.GetNDIversion() << " (http://ndi.tv/)" << endl;
		ndiTexture.allocate(fbo->getWidth(), fbo->getHeight(), GL_RGBA);
	}

	void update() override {
//		cout << "update ndi" << endl;
		ndiReceiver.ReceiveImage(ndiTexture);
	}

	void draw0()  {
		ndiTexture.draw(0, 0, fbo->getWidth(), fbo->getHeight());
	}

	void draw() override {
		// update();
//		cout << "draw ndi" << endl;
		ofSetColor(255);
		// float scale = 1.0;
		float scale = uiC->pEasy["scale"];
		float w = ndiTexture.getWidth() * scale;
		float h = ndiTexture.getHeight() * scale;
		
		ofTranslate(fbo->getWidth() * .5, fbo->getHeight() * .5);

		float x = -w*.5 + uiC->pEasy["offX"];
		float y = -h*.5 + uiC->pEasy["offY"];

//		i->draw(x, y, w, h);

		ndiTexture.draw(x, y, w, h);
		
		if (uiC->pBool["debug"]) {
			cout << "DEBUG ----------" << endl;
			char name[256];
			int nsenders = ndiReceiver.GetSenderCount();
			for (int i = 0; i < nsenders; i++) {
				ndiReceiver.GetSenderName(name, 256, i);
				cout << "    Sender " << i << " [" << name << "]" << endl;
			}
			ndiTexture.draw(0, 0, fbo->getWidth(), fbo->getHeight());
		}
	}
#endif
};




struct sceneGen : public ofxScenes::sceneDmtr {
	using sceneDmtr::sceneDmtr;

	float pos = 0;

	void draw() {
		ofSetColor(255);
		float distancia = uiC->pEasy["distancia"];
		float largura = uiC->pEasy["largura"];
		float vel = uiC->pEasy["vel"];
		pos += vel;
		pos = fmod(pos, distancia);
		for (int x=-fbo->getWidth() * .5; x<fbo->getWidth* 1.5; x+=distancia) {
			ofDrawRectangle(pos, 0, largura, fbo->getHeight());
		}
	}
};