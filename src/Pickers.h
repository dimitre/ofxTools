//#include "DmtrPolar.h"

class picker {
public:
	
	bool on = false;
	ofxMicroUI * _ui = NULL;
	
	ofColor color = ofColor(0);
	ofColor lastColor = ofColor(0);
	glm::vec2 pos;
	glm::vec2 posPicker;

	float raio = 6;
	float diametro = 12;
	ofRectangle rect = ofRectangle(0,0,12,12);
	
	int index;
	int note;
	string name = "";
	
	picker() {}
	picker(glm::vec2 p, int i = 0) : pos(p), index(i) {}
	
	ofPixels * _pixels = NULL;
	
	ofRectangle recta, rectf;
	glm::vec2 posFloat;
	glm::vec2 labelPos;
	
	
	void setPos(glm::vec2 p, ofRectangle & ra, ofRectangle & rf) {
		if (p != posFloat || ra != recta || rf != rectf) {
			recta = ra;
			rectf = rf;
			posFloat = p;
			pos = posFloat * glm::vec2(rectf.width, rectf.height);
			posPicker = posFloat * glm::vec2(recta.width, recta.height);
			rect.x = pos.x - rect.width/2;
			rect.y = pos.y - rect.width/2;
			labelPos.x = pos.x - rect.width /2 - 2;
			labelPos.y = rect.y + 12 + rect.height + 4;
		}
	}

	void getColor() {
		lastColor = color;
		if (_pixels != NULL) {
			color = _pixels->getColor(pos.x, pos.y);
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) {
	}

	void draw() {
		ofFill();
		ofSetColor(color);
		ofDrawRectangle(rect);
		ofNoFill();
		ofSetColor(255, 0, 70);
		ofDrawRectangle(rect);
		ofFill();
		drawLabel();
	}
	
	void drawLabel() {
		if (_ui != NULL) {
			_ui->_settings->drawLabel(name, labelPos);
		}
	}
};
