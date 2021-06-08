struct elastica {
public:
	float raio, peso, peso2;
	ofNode node;

	glm::vec3 pos, mPos, lastPos;
	glm::vec3 vel, acel, acelEasy;
	
	float life = 10.0f;
	float maxLife = 10.0f;
	bool dead = false;
	
	std::vector<glm::vec3> ribbon{150};
	float startAlpha = 0.75f;
	int ribbonStep = 2;
	
	ofColor color;
	ofMesh mesh;
	float thickness = 1.5;

	void randomize() {
		pos = ofPoint(ofRandom(-100,100), ofRandom(-20,20), ofRandom(-40,40));
		for(auto & v : ribbon){
			v = pos;
		}
	}
	
	elastica(const ofColor& c) {
		peso = ofRandom(100,300);
		peso2 = ofRandom(200,400);
		randomize();
		life = ofRandom(2,15);
		maxLife = life;
		color =  c;

		mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	}
	
	void setPos(const glm::vec3 & p ){
		pos = p;
		for(auto & v : ribbon){
			v = pos;
		}
	}
	
	void setRibbonSize(int size) {
		ribbon.clear();
		for( int i = 0; i < size; i++){
			ribbon.push_back(pos);
		}
	}
	
	void setRibbonStep(int step){
		ribbonStep = step;
	}
	
	void updatePos(glm::vec3 p) {
		mPos = p;
		raio = peso/15.0 + 2;
		acel = (p-pos)/glm::vec3(peso, peso, peso);
		update();
	}
	
	void update() {
		acelEasy = (acel - acelEasy) / 10.0f;
		vel += acelEasy - (vel/peso2);
		node.setPosition(lastPos);
		pos += vel;
		node.lookAt(pos);
		lastPos = pos;
		
		ribbon[0] = pos;
		for( int i = ribbon.size() -1; i > 0; i-- ){
			ribbon[i] = ribbon[i-1];
		}

		life -= 0.08f;
		
		if( life < 0 ){
			dead = true;
		}
		
		if( life < 4 ){
			thickness *= (life / 4.0f);
		}
	}
	
	void friccao() {
		if (vel.x > .001 && vel.y > .001) {
			vel *= 0.9999;
		} else {
			vel = ofPoint(0,0);
		}
		if (acel.x > .001 && acel.y > .001) {
			acel *= 0.9;
		} else {
			acel = ofPoint(0,0);
		}
	}
	
	void draw() {
		if( ribbonStep * 2 > ribbon.size()){
			return;
		}
		
		mesh.clear();
		for(unsigned int i = 1; i < ribbon.size(); i++){
			//find this point and the next point
			glm::vec3 thisPoint = ribbon[i-1];
			glm::vec3 nextPoint = ribbon[i];
			
			//get the direction from one to the next.
			//the ribbon should fan out from this direction
			glm::vec3 direction = (nextPoint - thisPoint);
			
			//get the distance from one point to the next
			float distance = direction.length();
			
			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			// glm::vec3 unitDirection = direction.getNormalized();
			glm::vec3 unitDirection = glm::normalize(direction);
			
			
			//find both directions to the left and to the right
			// glm::vec3 toTheLeft = unitDirection.getRotated(-90, glm::vec3(0,0,1));
			// glm::vec3 toTheRight = unitDirection.getRotated(90, glm::vec3(0,0,1));

						//find both directions to the left and to the right
			glm::vec3 toTheLeft =  glm::rotate(unitDirection, -90.f, glm::vec3(0,0,1));
			glm::vec3 toTheRight = glm::rotate(unitDirection, 90.f, glm::vec3(0,0,1));
			
			//use the map function to determine the distance.
			//the longer the distance, the narrower the line.
			//this makes it look a bit like brush strokes
			
			float n = 1 -  float( i ) / float( ribbon.size() );
			
			float thick = ofMap(i, 1, ribbon.size(), thickness, thickness/2.0);
			
			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
//            glm::vec3 leftPoint = thisPoint+toTheLeft* thickness * n;
//            glm::vec3 rightPoint = thisPoint+toTheRight* thickness * n;
			glm::vec3 leftPoint = thisPoint+toTheLeft* thick;
			glm::vec3 rightPoint = thisPoint+toTheRight* thick;
			
			//add these points to the triangle strip
			mesh.addVertex(glm::vec3(leftPoint.x, leftPoint.y, leftPoint.z));
			mesh.addVertex(glm::vec3(rightPoint.x, rightPoint.y, rightPoint.z));
			
			float alpha = ofMap(i, 1, ribbon.size(), 255, 0);
			auto c = ofColor(color.r, color.g , color.b, alpha );
			
			mesh.addColor( c );
			mesh.addColor( c );
		}
		mesh.draw();
	}
};


struct sceneElastica : public ofxScenes::sceneDmtr, ofxScenes::polar {
public:
	using sceneDmtr::sceneDmtr;

	vector <elastica> elasticas;

	void setup() override {

	}

	void draw() override {
        cout << elasticas.size() << endl;
		for (auto & e : elasticas) {
			if (ofGetMousePressed()) {
				e.update();
				e.friccao();
			} else {
				// e.updatePos(p);
				//e.updatePos(p2);
			}
			
			//		e.node.transformGL();
			//		m.draw();
			//		e.node.restoreTransformGL();
			
			e.draw();
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
		if (e.name == "randomize") {
			for (auto & e : elasticas) {
				e.randomize();
			}
		}
		else if (e.name == "peso" || e.name == "pesoRand") {
			for (auto & e : elasticas) {
				e.peso = ofRandom(uiC->pFloat["peso"], uiC->pFloat["peso"] + uiC->pFloat["pesoRand"]);
			}
		}
		else if (e.name == "peso2" || e.name == "peso2Rand") {
			for (auto & e : elasticas) {
				e.peso2 = ofRandom(uiC->pFloat["peso2"], uiC->pFloat["peso2"] + uiC->pFloat["peso2Rand"]);
			}
		}

		else if (e.name == "numero") {
			elasticas.clear();
			for (int a=0; a<uiC->pInt["numero"]; a++) {
				// ofColor c;
				// if( chosenColors.size()> 0 ){
				// 	auto index =  int( ofRandom( chosenColors.size() -1 ) );
				// 	c = chosenColors[index % chosenColors.size() ];
				// }
				ofColor c = getCor(0);
				elasticas.push_back(elastica( c ));
			}
			//		elasticas.assign(uiC->pInt["numero"], elastica(ms[ofRandom(0,3)]));
		}

		else if (e.name == "ribbonAlpha") {
			for(auto& elas : elasticas){
				elas.startAlpha = *e.f;
			}
		}
		else if (e.name == "ribbonSize") {
			
			for(auto& elas : elasticas){
				elas.setRibbonSize( *e.i );
			}
			
		}
		else if (e.name == "ribbonStep") {
			for(auto& elas : elasticas){
				elas.setRibbonStep( *e.i );
			}
		}
	}

};
