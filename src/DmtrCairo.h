#define SVGCAIRO 1

bool useCairo = false;

shared_ptr<ofBaseGLRenderer> opengl;
shared_ptr<ofCairoRenderer> cairo;
shared_ptr<ofCairoRenderer> cairoOut;
bool savingCairo = false;
ofTexture render;
bool cairoIsSetup = false;

void setupCairo(int w, int h) {
	cout << "setupCairo :: " << w << " x " << h << endl;
	opengl = ofGetGLRenderer();
	cairo = make_shared<ofCairoRenderer>();
	cairoOut = make_shared<ofCairoRenderer>();
	ofRectangle rect = ofRectangle(0, 0, w, h);
	cairo->setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, rect);
	cairoIsSetup = true;
}


void setupCairo(ofFbo * fbo) {
	int w = fbo->getWidth();
	int h = fbo->getHeight();
	cout << "setupCairo :: " << w << " x " << h << endl;
	opengl = ofGetGLRenderer();
	cairo = make_shared<ofCairoRenderer>();
	cairoOut = make_shared<ofCairoRenderer>();
	ofRectangle rect = ofRectangle(0, 0, w, h);
	cairo->setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, rect);
	cairoIsSetup = true;
}

string savingCairoFilename = "";
void beginCairo() {
	if (cairoIsSetup) {
		if (savingCairo) {
			cout << "SAVINGCAIRO!" << endl;
	#ifdef SVGCAIRO
			ofRectangle rect = ofRectangle(0,0,fbo->getWidth(), fbo->getHeight());
			savingCairoFilename = "_output/syntype_"+ofGetTimestampString()+".svg";
			cout << "SAVING " << savingCairoFilename << endl;
			cairoOut->setup(savingCairoFilename, ofCairoRenderer::SVG, false, false, rect);
			ofSetCurrentRenderer(cairoOut);
			ofGetCurrentRenderer()->setupGraphicDefaults();
			ofStyle style = ofGetCurrentRenderer()->getStyle();
			ofGetCurrentRenderer()->setStyle(style);
			cairo_set_miter_limit(cairoOut->getCairoContext(), 2);
			cairo_set_line_join(cairoOut->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
			cairo_set_line_cap(cairoOut->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
	#endif
		} else {
			
			ofSetCurrentRenderer(cairo);
			ofGetCurrentRenderer()->setupGraphicDefaults();
			ofStyle style = ofGetCurrentRenderer()->getStyle();
			ofGetCurrentRenderer()->setStyle(style);
			cairo_set_miter_limit(cairo->getCairoContext(), 2);
//			cairo_set_line_join(cairo->getCairoContext(), CAIRO_LINE_JOIN_ROUND); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
//			cairo_set_line_cap(cairo->getCairoContext(), CAIRO_LINE_CAP_ROUND); // ROUND SQUARE
			cairo_set_line_join(cairo->getCairoContext(), CAIRO_LINE_JOIN_BEVEL); //CAIRO_LINE_JOIN_ROUND //CAIRO_LINE_JOIN_BEVEL
			cairo_set_line_cap(cairo->getCairoContext(), CAIRO_LINE_CAP_BUTT); // ROUND SQUARE
			

			if (ui->pBool["cairoStroked"]) {
				static const double dashes[] = { 100.0,  /* ink */
					40.0,  /* skip */
					20.0,  /* ink */
					40.0   /* skip*/
				};
				int    ndash  = sizeof (dashes)/sizeof(dashes[0]);
				double offset = -50.0;
				cairo_set_dash(cairo->getCairoContext(), dashes, ndash, offset);
			} else {
				static const double nodash[] = { 0.0 };
				cairo_set_dash(cairo->getCairoContext(), nodash, 0, 0);
			}

			cairo->clearAlpha();
		}
		ofPushMatrix();
	}
}

void endCairo() {
	if (cairoIsSetup) {
		ofPopMatrix();
		ofSetCurrentRenderer(opengl, true);
		
		if (savingCairo) {
			render.loadData(cairoOut->getImageSurfacePixels());
			cairoOut->close();
			savingCairo = false;
			string resultado = ofSystem("open " + ofToDataPath(savingCairoFilename));

		} else {
			render.loadData(cairo->getImageSurfacePixels());
			ofSetColor(255);
			render.draw(0,0);
		}
	}
}


#ifdef USECAIROBLENDING
void startCairoBlendingMode() {
	if (cairoIsSetup) {
		string * s = &uiColors->pString["blend"];
		//#CAIRO_OPERATOR_CLEAR
		//#CAIRO_OPERATOR_SOURCE
		//	; 	#CAIRO_OPERATOR_OVER
		//	; 	#CAIRO_OPERATOR_IN
		//	; 	#CAIRO_OPERATOR_OUT
		//	; 	#CAIRO_OPERATOR_ATOP
		//	; 	#CAIRO_OPERATOR_DEST
		//	; 	#CAIRO_OPERATOR_DEST_OVER
		//	; 	#CAIRO_OPERATOR_DEST_IN
		//	; 	#CAIRO_OPERATOR_DEST_OUT
		//	; 	#CAIRO_OPERATOR_DEST_ATOP
		//	; 	#CAIRO_OPERATOR_XOR
		//	; 	#CAIRO_OPERATOR_ADD
		//	; 	#CAIRO_OPERATOR_SATURATE
		//	; 	#CAIRO_OPERATOR_MULTIPLY
		//	; 	#CAIRO_OPERATOR_SCREEN
		//	; 	#CAIRO_OPERATOR_OVERLAY
		//	; 	#CAIRO_OPERATOR_DARKEN
		//	; 	#CAIRO_OPERATOR_LIGHTEN
		//	; 	#CAIRO_OPERATOR_COLOR_DODGE
		//	; 	#CAIRO_OPERATOR_COLOR_BURN
		//	; 	#CAIRO_OPERATOR_HARD_LIGHT
		//	; 	#CAIRO_OPERATOR_SOFT_LIGHT
		//	; 	#CAIRO_OPERATOR_DIFFERENCE
		//	; 	#CAIRO_OPERATOR_EXCLUSION
		//	; 	#CAIRO_OPERATOR_HSL_HUE
		//	; 	#CAIRO_OPERATOR_HSL_SATURATION
		//	; 	#CAIRO_OPERATOR_HSL_COLOR
		//	; 	#CAIRO_OPERATOR_HSL_LUMINOSITY
		
		
		//CAIRO_OPERATOR_MULTIPLY //CAIRO_OPERATOR_SCREEN
		if (*s == "add") {
			cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_ADD);
		}
		
		else if (*s == "screen") {
			cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_SCREEN);
		}
		else if (*s == "multiply") {
			cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_MULTIPLY);
		}
		else if (*s == "subtract") {
			//cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_SCREEN);
		}
		
		else if (*s == "darken") {
			cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_DARKEN);
		}
		
		else if (*s == "lighten") {
			cairo_set_operator(cairo->getCairoContext(),CAIRO_OPERATOR_LIGHTEN);
		}
	}
}
#endif


//void setupCairo() {
//	opengl = ofGetGLRenderer();
//	cairo = make_shared<ofCairoRenderer>();
//	cairoOut = make_shared<ofCairoRenderer>();
//	ofRectangle rect = ofRectangle(0,0, fbo->getWidth(), fbo->getHeight());
//	cairo->setupMemoryOnly(ofCairoRenderer::IMAGE, false, false, rect);
//}
