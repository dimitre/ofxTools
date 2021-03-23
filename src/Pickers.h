//#include "DmtrPolar.h"

#define NEUE 1
#ifdef NEUE
class picker {
public:
    picker() {}
    picker(glm::vec2 p, int i=0, ofFbo * f = NULL, ofxMicroUI * u = NULL) : pos(p), index(i), _fbo(f), _ui(u) {
        setup();
    }
    glm::vec2 pos;
    int index = 0;
    ofFbo * _fbo = NULL;
    ofxMicroUI * _ui = NULL;
    
    string name = "";

    bool changed = false;
    glm::vec2 labelPos;
    ofRectangle rect = ofRectangle(0,0,12,12);
    ofRectangle rect2 = ofRectangle(0,0,12,12);

    ofFbo fbo;
    ofPixels pixels;
    ofColor color = ofColor(0);
    ofColor lastColor = ofColor(0);

    void setPos(glm::vec2 p) {
        pos = p;
        rect.x = pos.x - 6;
        rect.y = pos.y - 6;
        labelPos.x = rect.x - rect.width *.5 - 2;
        labelPos.y = rect.y + rect.height + 16;
    }
    
    void setup() {
        int wh = 1;
        fbo.allocate(wh, wh, GL_RGB);
        fbo.begin();
        ofClear(0,255);
        fbo.end();
        setPos(pos);
    }
    
    float dist (ofColor & c1, ofColor & c2) {
        return ((c1.r - c2.r) + (c1.g - c2.g) + (c1.b - c2.b))*.33;
    }
    
    float distColor() {
//        return dist(lastColor, color);
        cout << color << endl;
        cout << lastColor << endl;
        cout << ABS(color.r - lastColor.r + color.g - lastColor.g + color.b - lastColor.b) << endl;
        return ABS(color.r - lastColor.r);
//        cout << lastColor << endl;
//        cout << color << endl;
    }
    
    unsigned int delta = 0;
    unsigned int minDelta = 0;
    bool deltaChanged = false;

    
    void update() {
        if (_fbo != NULL) {
            ofSetColor(255);
            fbo.begin();
            ofClear(0,255);
            _fbo->draw(-pos.x, -pos.y);
            fbo.end();
            fbo.readToPixels(pixels);
            color = pixels.getColor(0, 0);
            changed = lastColor != color;
            
//            if (changed)
            {
                delta = ABS(color.r - lastColor.r + color.g - lastColor.g + color.b - lastColor.b);
            }
            
            deltaChanged = delta > minDelta;
            lastColor = color;
        } else {
            cout << "pickers, _fbo input is null" << endl;
        }
    }
    
    void draw() {
        ofFill();
        ofSetColor(color);
        ofDrawRectangle(rect);
        ofNoFill();
        ofSetColor(deltaChanged ? ofColor(255) : ofColor(255, 0, 70));
        ofDrawRectangle(rect);
        ofFill();
        drawLabel();
    }
    
    void draw(ofRectangle & r) {
        rect2.x = ofMap(rect.x, 0, _fbo->getWidth(), r.x, r.x+r.getWidth());
        rect2.y = ofMap(rect.y, 0, _fbo->getHeight(), r.y, r.y+r.getHeight());
        
        labelPos.x = rect2.x - rect2.width *.5 - 2;
        labelPos.y = rect2.y + rect2.height + 16;

        ofFill();
        ofSetColor(color);
        ofDrawRectangle(rect2);
        ofNoFill();
        ofSetColor(deltaChanged ? ofColor(255) : ofColor(255, 0, 70));
        ofDrawRectangle(rect2);
        ofFill();
        drawLabel();
    }
    
    void drawLabel() {
        if (_ui != NULL) {
            string label = name + "\n" + ofToString(delta);
            _ui->_settings->drawLabel(label, labelPos);
        }
    }
};


#else
class picker {
public:
	
    int index;
    string name = "";
    //position with fboFinal zoom
    glm::vec2 pos;

    
    picker() {}
    picker(glm::vec2 p, int i = 0) : pos(p), index(i) {
        setup();
    }
    
	bool on = true;
	ofxMicroUI * _ui = NULL;
	ofRectangle * _rect = NULL;
	
	// 2021
	ofFbo * _fbo = NULL;
	ofFbo fbo;
	ofPixels pixels;
	
	ofColor color = ofColor(0);
	ofColor lastColor = ofColor(0);
	// position float
	glm::vec2 posFloat;
	
	
	// position picker
	glm::vec2 posPicker;
	glm::vec2 labelPos;

    float raio = 6;
	float diametro = 12;
	ofRectangle rect = ofRectangle(0,0,12,12);
	
	int note;
	
//	ofPixels * _pixels = NULL;
//	ofRectangle recta, rectf;
	
	void setUI(ofxMicroUI * u ) {
		_ui = u;
		ofAddListener(_ui->uiEvent, this, &picker::uiEvents);
	}
	
	void setup() {
		int wh = 1;
		fbo.allocate(wh, wh, GL_RGB);
		fbo.begin();
		ofClear(0,255);
		fbo.end();
//		pixels.allocate(wh, wh, OF_IMAGE_COLOR_ALPHA);
	}
	
	void uiEvents(ofxMicroUI::element & e) {
		on = _ui->pString["mode"] != "" && _ui->pString["mode"] != "no";
		haveToUpdate = true;
	}
	
	bool haveToUpdate = true;
	
	void updatePosition() {
		if (on && _ui != NULL) {
			
//			cout << "updateposition, picker is on" << endl;
			posFloat.x = _ui->pFloat["picker_x"];
			posFloat.y = _ui->pFloat["picker_y"];
			posPicker.x = posFloat.x * _fbo->getWidth();
			posPicker.y = posFloat.y * _fbo->getHeight();

			pos.x = posFloat.x * _rect->width + _rect->x;
			pos.y = posFloat.y * _rect->height + _rect->y;
			rect.x = pos.x - rect.width/2;
			rect.y = pos.y - rect.width/2;
			labelPos.x = pos.x - rect.width /2 - 2;
			labelPos.y = rect.y + 12 + rect.height + 4;
		}
	}
	
	void updateFromFbo() {
		if (haveToUpdate) {
			updatePosition();
			haveToUpdate = false;
		}
		
		if (_fbo != NULL) {
			ofSetColor(255);
			fbo.begin();
			ofClear(0,255);
			_fbo->draw(-posPicker.x, -posPicker.y);
			fbo.end();
			fbo.readToPixels(pixels);
			lastColor = color;
			color = pixels.getColor(0, 0);
		} else {
			cout << "pickers, _fbo input is null" << endl;
		}
	}

	void draw() {
		if (on) {
//			cout << "draw picker " << rect << endl;
			ofFill();
			ofSetColor(color);
			ofDrawRectangle(rect);
			ofNoFill();
			ofSetColor(255, 0, 70);
			ofDrawRectangle(rect);
			ofFill();
			drawLabel();
		}
		else {
//			cout << "draw picker is off " << rect << endl;
		}
	}
	
	void drawLabel() {
		if (_ui != NULL) {
			_ui->_settings->drawLabel(name, labelPos);
		}
	}
};
#endif
