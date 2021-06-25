



struct sceneMilimetrado : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	struct sl {
		public:
		float space = 100;
		float line = 3;
		sl(float s, float l) : space(s), line(l) {}
	};

	vector <sl> sls = { 
		sl(50, 2), 
		// sl(50, 2), 
		sl(10,1) 
	};
	
	void setup() override {
	}
	
	void draw() override {
		checkSetup();
		ofSetColor(getCor(0));

		for (auto s : sls) {
			ofSetLineWidth(s.line);
			float space = s.space * uiC->pFloat["space"];
			for (float x=0; x<fbo->getWidth(); x+=space) {
				ofDrawLine(x, 0, x, fbo->getHeight());
			}
			for (float y=0; y<fbo->getWidth(); y+=space) {
				ofDrawLine(0, y, fbo->getWidth(), y);
			}
		}
	}
};



struct sceneModulo : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
	struct modulo {
		public:
		vector <ofRectangle> rects;
		vector <ofPolyline> lines;
		float qual;
		ofRectangle rect = ofRectangle(0,0,100,100);

		modulo(glm::vec2 pos, float m ) {
			rect = ofRectangle(0,0,m,m);
			qual = ofRandom(0,1);
			rect.x = pos.x;
			rect.y = pos.y;
			float raio = 6;
			rects = { 
				ofRectangle(rect.width * .25 - raio * .5, rect.height * .25 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .75 - raio * .5, rect.height * .25 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .75 - raio * .5, rect.height * .75 - raio * .5, raio, raio), 
				ofRectangle(rect.width * .25 - raio * .5, rect.height * .75 - raio * .5, raio, raio), 
			};
			for (int y =0; y<rect.height; y+=10) {
				{
					ofPolyline line;
					line.addVertex(glm::vec3(0,y,0));
					line.addVertex(glm::vec3(rect.width,y,0));
					lines.emplace_back(line);
				}
				{
					ofPolyline line;
					line.addVertex(glm::vec3(y,0,0));
					line.addVertex(glm::vec3(y,rect.width,0));
					lines.emplace_back(line);
				}
			}
		}

		void draw() {
			ofPushMatrix();
			ofTranslate(rect.x, rect.y);
			ofSetLineWidth(3);
			ofDrawLine(-5, 0, 5, 0);
			ofDrawLine(0, -5, 0, 5);
			ofSetLineWidth(1);
			ofNoFill();
			for (auto & r : rects) {
				ofDrawRectangle(r);
			}
			for (auto & l : lines) {
				l.draw();
			}
			ofPopMatrix();
		}
		

	};

	vector <modulo> modulos;
	
	void build() {
		cout << "build modulos" << endl;
		modulos.clear();
		int x = 0, y = 0;
		float size = uiC->pFloat["size"];
		size = MAX(20, size);
//        cout << size << endl;
		while (x < fbo->getWidth() * uiC->pFloat["fatorW"]) {
//            cout << x << endl;
			y = 0;
			while (y < fbo->getHeight()* uiC->pFloat["fatorH"]) {
				modulos.emplace_back(glm::vec2(x,y), size);
//                y += modulos.back().rect.width;
				y += size;
			}
//            x += modulos.back().rect.height;
			x += size;
		}
	}
	
	void setup() override {
		build();
	}
	
	void draw() override {
		for (auto & m : modulos) {
			ofColor cor = getCor(m.qual * uiC->pEasy["colorRange"]);
			cor.a = ofNoise(m.qual * uiC->pEasy["colorAlphaQual"], ofGetElapsedTimef() * uiC->pEasy["colorAlphaTempo"]) * 255.0;
			ofSetColor(cor);
			m.draw();
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "size") {
			build();
		}
	}
};


struct sceneBars : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;
	
    struct config {
    public:
        double width = 20;
        double vel = 1;
        config (float w, float v) : width(w), vel(v) {}
    };
    
    struct settings {
    public:
        float minW = 20;
        float maxW = 140;
        
        config configs[6] = {
            config(20,-4),
            config(40,-4),
            config(40,2),
            config(80,-4),
            config(160,-8),
            config(320,8),
        };
    } set;
    

    
	struct bar {
		public:
        
        settings * _set = NULL;
		ofRectangle rect;
		vector <ofRectangle> rects;
		int w = 5;
		float pos = 0;
        double vel;
        double velFactor = 1;
		float qual = 0;
        bar(ofRectangle r, settings * s) : rect(r), _set(s) {
            setup();
        }
		bar(ofRectangle r) : rect(r) {
            setup();
		}
        

        

        
        void setup() {
            

            
            
            qual = ofRandom(0,1);
            vel = ofRandom(-4, 4);
            if (_set != NULL) {
                w = ofRandom(_set->minW, _set->maxW);
            } else {
                w = ofRandom(20,140);
            }
            
            // novo modo
            int i = ofRandom(0,6);
            w = _set->configs[i].width;
            vel = _set->configs[i].vel;
            
            if (ofRandom(0,10) > 5) {
                vel *= -1;
            }
//            rect.height = ;
            // cout << rect << endl;
            int x = -rect.width * .25;
            while (x < rect.width*1.25) {
                rects.emplace_back(ofRectangle(x, rect.y, w, rect.height));
                x += w * 2;
            }
        }

		void draw() {
			pos += vel * velFactor;
			ofPushMatrix();
			ofTranslate(fmod(pos, w*2) + rect.x, rect.y);
			// ofDrawRectangle(rect);
			for (auto & r : rects) {
				ofDrawRectangle(r);
			}
			ofPopMatrix();
		}
	};

	vector<bar> bars;
	float nextJump = 0;

	void build() {
        if (uiC->pBool["seed"]) {
            ofSeedRandom(uiC->pInt["seed"]);
        }
		int y=0;
        float heightFactor = uiC->pFloat["height"];
        if (heightFactor < .5) {
            heightFactor = .5;
        }
		while (y < fbo->getHeight()) {
            int h = ofRandom(10,200);
			bars.emplace_back(ofRectangle(0,y,fbo->getWidth(), h), &set);
//			y+= bars.back().rect.height;
            y+= h * heightFactor;
		}
//		cout << "setup bars size = " << bars.size() << endl;
	}

	void setup() override {
		build();
	}

	void draw() override {
		if (uiC->pBool["rebuild"]) {
			if (ofGetElapsedTimef() > nextJump) {
				nextJump = ofGetElapsedTimef() + uiC->pFloat["rebuildTime"];
				bars.clear();
				build();
			}
		}
		for (auto & b : bars) {
			ofSetColor(getCor(b.qual));
			b.draw();
		}
	}

    void calcVelFactor() {
        int frames = 0;
        for (auto & c : set.configs) {
            frames = MAX(frames, ABS(c.width/(c.vel*uiC->pFloat["vel"])));
    //                cout << ABS(width/vel) << endl;
        }
        cout << "------- BARS -------";
        cout << frames << endl;
        cout << "------- BARS -------";
    }
    
    
	void uiEvents(ofxMicroUI::element & e) override {
        
        if (e.name == "vel") {
            calcVelFactor();
        }
        if (e.name == "minW") {
            set.minW = *e.i;
        }
        else if (e.name == "maxW") {
            set.maxW = *e.i;
        }
        if (e.name == "vel") {
            for (auto & b : bars) {
                b.velFactor = *e.f;
            }
        }
		if (e.name == "build") {
			build();
		}
		else if (e.name == "clear") {
			bars.clear();
		}
		else if (e.name == "rebuild" || e.name == "seed") {
			bars.clear(); 
			build();
		}
	}
};



struct sceneBars2 : public ofxScenes::sceneDmtr {
public:
    using sceneDmtr::sceneDmtr;
    vector <ofRectangle> rects;

    struct bar {
    public:
        float h;
        ofRectangle rect;
        bar(ofRectangle r) : rect(r) {}
        
        void draw() {
            ofDrawRectangle(rect);
        }
    };
    
    vector <bar> bars;
    
    void build()  {
        float y =0;
        
        while (y < fbo->getHeight()) {
            float w = ofRandom(20,140);
            float h = ofRandom(20,140);
            float x = 0;
            while (x < fbo->getWidth()) {
                bars.emplace_back(ofRectangle(x, y, w, h));
                x += w * 2;
            }
            y += h;
        }
    }
    
    void setup() override {
        build();
    }
    void draw() override {
        ofFloatColor c = getCor(0);
        for (auto & b : bars) {
//            c.a = ofNoise(ofGetElapsedTimef() , b.rect.x + b.rect.y) * 255.0;
            c.a = ofMap(sin(
                            ofGetElapsedTimef() * uiC->pFloat["alphaTime"] +
                            ofDegToRad(ofGetFrameNum() * uiC->pFloat["alphaFrames"]) +
                            b.rect.x * uiC->pFloat["alphaX"] +
                            b.rect.y * uiC->pFloat["alphaY"]
                            ), -1, 1, 0, 1);
            ofSetColor(c);
            b.draw();
        }
    }
    
    void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "build") {
            build();
        }
        if (e.name == "clear") {
            bars.clear();
        }
    }
};




struct sceneImages : public ofxScenes::sceneDmtr {
public:
	using sceneDmtr::sceneDmtr;

	ofImage bg;
	vector <ofImage> images;

	struct img {
		public:
		ofImage image;
		glm::vec2 pos;
		int index = 0;
		img(ofImage i, int in) : image(i), index(in) {
			pos = glm::vec2(ofRandom(0,2000), ofRandom(0,2000));
		}
		void draw() {
//			image.draw(pos.x, pos.y);
			pos.x = ofNoise(ofGetElapsedTimef() * 0.02, index * 10) * 1420;
			pos.y = ofNoise(ofGetElapsedTimef() * 0.04, index * 11) * 980;
//            cout << pos << endl;
			image.draw(pos.x, pos.y, 400, 400);
		}
	};

	vector <img> imgs;

	void setup() override {
		bg.load("bg.jpg");
		ofDirectory dir;
		dir.allowExt("jpg");
		dir.allowExt("gif");
		dir.allowExt("jpeg");
		dir.listDir("_images");
		dir.sort();
		int index = 0;
		for (auto & d : dir) {
			ofImage i;
			i.load("_images/"+d.getFileName());
			imgs.emplace_back(i, index);
			index++;
		}
		
		for (auto & d : dir) {
			ofImage i;
			i.load("_images/"+d.getFileName());

			imgs.emplace_back(i, index);
			index++;
		}
	}
	
	map <string, ofBlendMode> blendModes = {
		{ "alpha", OF_BLENDMODE_ALPHA },
		{ "add", OF_BLENDMODE_ADD },
		{ "subtract", OF_BLENDMODE_SUBTRACT },
		{ "screen", OF_BLENDMODE_SCREEN },
		{ "multiply", OF_BLENDMODE_MULTIPLY },
		// { "darken", OF_BLENDMODE_DARKEN },
		// { "lighten", OF_BLENDMODE_LIGHTEN },
	};

	void draw() override {
		ofSetColor(255);
		bg.draw(0,0, 1920, 1080);
		ofEnableBlendMode(blendModes[uiC->pString["blend"]]);
		for (auto & i : imgs) {
			i.draw();
		}
	}
};


struct sceneStripes : public ofxScenes::sceneDmtr {
	public:
	using sceneDmtr::sceneDmtr;
	ofFbo fbo;
	void setup() override {
		fbo.allocate(3000, 200, GL_RGBA);
	}

	void build() {
        
		fbo.begin();
		ofClear(0,255);
		// ofSetColor(252, 186, 3);
		ofSetColor(255);
		int w = 250;
		int h = fbo.getHeight();
		ofPushMatrix();
		ofTranslate(-w*2,0);
		for (int x = -w*2; x<fbo.getWidth() + w*2; x+=w*2) {
			ofBeginShape();
			ofVertex(0, 0);
			ofVertex(w, 0);
			ofVertex(w+h, h);
			ofVertex(h, h);
			ofVertex(0, 0);
			ofEndShape();
			ofTranslate(w*2,0);
		}
		ofPopMatrix();

	glEnable(GL_BLEND);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glLogicOp(GL_INVERT);
	glEnable(GL_COLOR_LOGIC_OP);
	// glBlendFunc(GL_DST_COLOR,GL_ONE_MINUS_SRC_COLOR);

		ofSetColor(255);
		cout << uiC->pInt["sujeira"] << endl;
		for (int a=0; a<uiC->pInt["sujeira"]; a++) {
			float x = ofRandom(0, fbo.getWidth());
			float y = ofRandom(0, fbo.getHeight());
			// cout << x << endl;
			// cout << y << endl;
			float raio = ofRandom(-4,12);
			ofDrawCircle(x, y, raio);
		}

		glDisable(GL_COLOR_LOGIC_OP);

		fbo.end();
	}

	void uiEvents(ofxMicroUI::element & e) override {
		build();
	}

	void draw() override {
		ofSetColor(3, 86, 252);
		
		ofDrawRectangle(0,0,config->fbo->getWidth(),config->fbo->getHeight());
		ofSetColor(255);

		// ofSetColor(0);
		// ofDrawRectangle(0,400,fbo.getWidth(), fbo.getHeight());

		ofSetColor(252, 186, 3);
		fbo.draw(0,400);
	}
};




struct sceneRipple : public ofxScenes::sceneDmtr {
	using sceneDmtr::sceneDmtr;

	struct gota {
		public:
		glm::vec2 pos;
		float raio = 10;
		float vel = 2;

		gota(glm::vec2 p) : pos(p) {
		}

		void update() {
			raio += vel;
		}
		void draw() {
			ofDrawCircle(pos.x, pos.y, raio);
		}
	};

	struct emitter {
	public:
		glm::vec2 pos;
		emitter(glm::vec2 p) : pos(p) {
		}

		vector <gota> gotas;

		float nextJump = 0;
		void update() {
			if (ofGetElapsedTimef() > nextJump) {
				nextJump = ofGetElapsedTimef() + 1;
				gotas.emplace_back(gota(pos));
			}
			for (auto & g : gotas) {
				g.update();
			}	
		}

		void draw() {
			ofDrawRectangle(pos.x - 4, pos.y - 4, 8, 8);
			for (auto & g : gotas) {
				g.draw();
			}
		}
	};

	vector <emitter> emitters;
	glm::vec2 wh; 
	ofFbo f;

	void setup() override {
		wh = glm::vec2(fbo->getWidth(), fbo->getHeight());
		// cout << " ||||||| setup emitter" << endl;
		// f.allocate(wh.x * 3, wh.y * 3, GL_RGBA);
		f.allocate(wh.x * 2, wh.y * 2, GL_RGBA);
		for (int a=0; a<20; a++) {
			glm::vec2 pos = glm::vec2(ofRandom(0,wh.x), ofRandom(0,wh.y));
			emitters.emplace_back(pos);
		}
	}

	void drawFbo() {
		f.begin();
		ofClear(0,255);
		ofSetColor(0,255,0);
		ofDrawRectangle(0,0,100,100);
		ofPushMatrix();
		// ofTranslate(wh.x, wh.y);
		ofSetColor(255,0,0);
		ofDrawRectangle(0,0,wh.x, wh.y);

		ofNoFill();
		ofSetColor(255);
		for (auto & e : emitters) {
			// cout << e.pos << endl;
			e.update();
			e.draw();
		}
		ofPopMatrix();
		f.end();
	}

	void update() override {
		drawFbo();

		// for (auto & e : emitters) {
		// }

		// cout << "update" << endl;

	}

	void draw() override {
		// cout << "ripple draw" << endl;
		ofSetColor(255);
		// pool.draw(-wh.x  + uiC->pInt["offX"], -wh.y + uiC->pInt["offY"]);

		f.draw( uiC->pInt["offX"],  uiC->pInt["offY"]);
		// ofSetColor(255, 127);
		// ofEnableBlendMode(OF_BLENDMODE_ADD);
		// f.getTexture().drawSubsection(0, 0, wh.x, wh.y, wh.x, wh.y, wh.x, wh.y);
		// f.getTexture().drawSubsection(0, 0, wh.x, wh.y, wh.x, 0, -wh.x, wh.y);

		// void ofTexture::drawSubsection(float x, float y, float w, float h, float sx, float sy, float sw, float sh)
	}
};





#ifdef USEDELAUNAY

struct sceneDelaunay : public ofxScenes::sceneDmtr { //, ofxScenes::sceneType
	public:
	using sceneDmtr::sceneDmtr;
	ofTrueTypeFont font;
	ofTrueTypeFont * type = NULL;
	vector <ofPath> paths;
	vector <ofPolyline> lines;
	ofColor color = ofColor(255);
	
	struct letra {
	public:
		ofPath path;
		vector <ofPolyline> lines;
		ofxMicroUI * uiC = NULL;
		float qual;
		ofColor color = ofColor(255);

		// se eu for tridimensionalizar.
		ofMesh front;
		ofMesh back;
		ofMesh middle;

		// se eu for delaunayzar
		ofRectangle rect;
		ofFbo fbo;

		
		letra (ofPath p, ofxMicroUI * u) : path(p), uiC(u) {
			qual = ofRandom(0,1);
			path.setPolyWindingMode(OF_POLY_WINDING_ODD);
			const vector<ofPolyline>& allLines = path.getOutline();
			lines = allLines;
			rect = lines[0].getBoundingBox();
			for (auto & l : lines) {
				rect.growToInclude(l.getBoundingBox());
			}

			fbo.allocate(rect.width, rect.height, GL_RGBA);
			fbo.begin();
			ofClear(0, 255);
			ofSetColor(255);
			ofPushMatrix();
			ofTranslate(-rect.x, -rect.y);
			path.draw();
			fbo.end();
			// cout << rect << endl;


			front = path.getTessellation();
			back = path.getTessellation();
		}

		void draw() {
			ofSetColor(color);

			for (auto & l : lines) {
				auto line = l.getResampledBySpacing(uiC->pEasy["spacing"]);
				if (uiC->pBool["smooth"]) {
					line = line.getSmoothed(uiC->pEasy["smooth"]);
				}

				for (auto & v : line.getVertices()) {
					if (uiC->pBool["node"]) {
						for (auto & vv : line.getVertices()) {
							if (vv != v) {
								if (uiC->pBool["vibra"]) {
									color.a = ofClamp(ofRandom(-255,255), 0, 255);
									ofSetColor(color);
								}
								ofDrawLine(vv, v);
							}
						}
					}
					if (uiC->pBool["circle"]) {
						ofDrawCircle(v.x, v.y, uiC->pEasy["radius"]);
//                        ofDrawCircle(v.x, v.y, 6);
					}
				}
				if (uiC->pBool["drawMesh"]) {
					front.draw();
				}

				if (uiC->pBool["drawLine"]) {
					line.draw();
				}
			}
		}
	};

	vector <letra> letras;
	glm::vec2 offset;

	void setup() override {
		int size = 290;
		font.load("_type/DINNextLTPro-Black.ttf", size, true, true, true);
		type = &font;

		bool vflip = true; // OF flips y coordinate in the default perspective, 
		bool filled = true;
		string texto = "IDEIAS";
		ofRectangle rect = type->getStringBoundingBox(texto, 0, 0);
		offset = middle - glm::vec2(rect.width, -rect.height) * .5;
		paths = type->getStringAsPoints(texto, vflip, filled);
		for (auto & p : paths) {
			letras.emplace_back(p, uiC);
		}

		int index = 0;
		for (auto & l : letras) {
			l.qual = index / (float) letras.size();
			index++; 
		}
	}

	void draw() override {
		ofTranslate(offset);
		ofNoFill();


//		ofSetColor(255);

		for (auto & l : letras) {
			if (l.qual <= uiC->pEasy["letters"]) {
				float qual = fmod(l.qual + ofNoise(ofGetElapsedTimef()), 1.0);
				// cout << qual << endl;
				// ofSetColor(getCor(qual));
				l.color = getCor(qual);
				l.draw();
			}
		}
		// for (auto & p : paths) {
		// 	p.setFilled(false);
		// 	p.setStrokeColor(ofColor(255));
		// 	p.setStrokeWidth(4);
		// 	p.draw();
		// }
	}
};
#endif






struct sceneSorteio : public ofxScenes::sceneDmtr { //, ofxScenes::sceneType
public:
    using sceneDmtr::sceneDmtr;

    vector <ofRectangle> rects = {
        ofRectangle(768, 128, 512, 384),
        ofRectangle(1280, 128, 512, 384),
        ofRectangle(1792, 128, 512, 384),
        ofRectangle(2304, 128, 512, 384),
    };
    
    vector <ofRectangle> rectsMargem = rects;
    
    float margem = 12;
    void setup() override {
        for (auto & r : rectsMargem) {
            r.x += margem *.5;
            r.y += margem *.5;
            r.width -= margem;
            r.height -= margem;
        }
        
    }
    
    int indexRect = 0;
    float endTime = 0;
    void draw() override {
        ofNoFill();
        ofSetColor(getCor(0));
        ofSetLineWidth(margem);
        if (ofGetElapsedTimef() < endTime) {
            int nframes = uiC->pInt["nframes"];
            if (ofGetFrameNum() % nframes == 0) {
                int lastFrame = indexRect;
                while (indexRect == lastFrame) {
                    indexRect = ofRandom(0,4);
                }
            }
            ofDrawRectangle(rectsMargem[indexRect]);
        } else {
            if ((ofGetFrameNum()/2) % 2 == 0) {
                ofDrawRectangle(rectsMargem[uiC->pInt["vencedor"]]);
            }
        }
    }
    
    void uiEvents(ofxMicroUI::element & e) override {
        if (e.name == "reset") {
            endTime = ofGetElapsedTimef() + uiC->pInt["segundos"];
        }
    }
};



struct sceneLikeDislike : public ofxScenes::sceneDmtr {
public:
    using sceneDmtr::sceneDmtr;
    ofImage l, d;
    
    void setup() override {
        l.load("_img/like.png");
        d.load("_img/dis.png");
    }
    
    void draw() override {
        int interval = uiC->pInt["interval"];
        float x = 0;
        bool dislike = true;
        ofSetColor(255);
        while (x < fbo->getWidth()) {
            dislike ^= 1;
            float y = 0;
//            float yy = fmod(y + incrementa("velY"), fbo->getHeight());
            while (y < fbo->getHeight()) {
                ofPushMatrix();
//                float xx = fmod(x + incrementa("velX"), fbo->getWidth());
                ofTranslate(x, y);
                ofImage * i = dislike ? &l : &d;
                float alpha = ofNoise(x * uiC->pFloat["qual"], y * uiC->pFloat["qual"], ofGetElapsedTimef() * uiC->pFloat["alphaTime"]) * uiC->pFloat["maxAlpha"] + uiC->pFloat["minAlpha"];
                alpha = ofClamp(alpha, 0, 255);
                ofSetColor(255, alpha);
                i->draw(0,0);
                ofPopMatrix();
                y += interval;
            }
            x += interval;
        }
    }
};
