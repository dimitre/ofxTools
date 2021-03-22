
shared_ptr<ofAppBaseWindow> newWindow;
vector <shared_ptr<ofAppBaseWindow> > allWindows;

#include "GLFW/glfw3.h"

//========================================================================
int main( ){
#ifndef WINDOWWIDTH
#define WINDOWWIDTH 1420
#endif

#ifndef WINDOWHEIGHT
#define WINDOWHEIGHT 840
#endif

//	ofTargetPlatform {
//	  OF_TARGET_OSX, OF_TARGET_WINGCC, OF_TARGET_WINVS, OF_TARGET_IOS,
//	  OF_TARGET_ANDROID, OF_TARGET_LINUX, OF_TARGET_LINUX64, OF_TARGET_LINUXARMV6L,
//	  OF_TARGET_LINUXARMV7L
//	}
	
	// WINDOW GUI
	ofGLFWWindowSettings settings;

#ifdef OPENGL3
	settings.setGLVersion(3, 2); //we define the OpenGL version we want to use
#endif

	settings.resizable = true;
	settings.setSize(WINDOWWIDTH, WINDOWHEIGHT);
	settings.windowMode = OF_WINDOW;

//	ofGLFWWindowSettings settings;
	// settings.numSamples = 8;
	//settings.setPosition(ofVec2f(80,80));
	//settings.setGLVersion(3, 2);

	auto guiWindow = ofCreateWindow(settings);
	auto mainApp = make_shared<ofApp>();

	//#ifdef TARGET_LINUX
	// mainApp->gui = guiApp;
	// mainApp->window = mainWindow;
	// mainApp->gl = dynamic_pointer_cast<ofBaseGLRenderer>(mainWindow->renderer());
	
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	

//    cout << "Multiwindow manager start" << endl;
//	#ifdef TARGET_OSX
//		cout << "TARGET_OSX" << endl;
//	#endif
//
//	#ifdef TARGET_LINUX
//		cout << "TARGET_LINUX" << endl;
//	#endif
//
//	#ifdef TARGET_LINUX64
//		cout << "TARGET_LINUX64" << endl;
//	#endif

	cout << glfwGetVersionString() << endl;
	cout << "Screens Found: " << monitorCount << endl;
	
	for (int a=0; a<monitorCount; a++) {
		string output;
		output += "display # " + ofToString(a) + " : " + glfwGetMonitorName(monitors[a]) + "\r";

		float scaleX, scaleY;
		glfwGetMonitorContentScale(monitors[a], &scaleX, &scaleY); // We take the second monitor
		output += "contentscale : " + ofToString(scaleX) + " x " + ofToString(scaleY) + "\r";

		// nao funciona pois eh sobre uma janela especifica
//		int fbX, fbY;
//		glfwGetFramebufferSize(monitors[a], &fbX, &fbY);
//		cout << "glfwGetFramebufferSize : " << fbX << " x " << fbY << "\r";
		
		int xM; int yM;
		glfwGetMonitorPos(monitors[a], &xM, &yM);
		glm::vec2 pos = glm::vec2(xM, yM);
		output += "position: " + ofToString(pos.x) + " x " + ofToString(pos.y) + "\r";
//		glViewport(0, 0, width, height);
		
		const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[a]);
		glm::vec2 size = glm::vec2(desktopMode->width, desktopMode->height);
		output += "size: " + ofToString(size.x) + " x " + ofToString(size.y) ; //+ "\r"
		// output << "size:" << size << endl;

		if (a > 0) {
			settings.setSize (size.x, size.y);
			settings.setPosition(pos);
			settings.windowMode = OF_FULLSCREEN;
//			settings.resizable = false;=-
			settings.shareContextWith = guiWindow;
			newWindow = ofCreateWindow(settings);

			if (a==1) {
				ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow1);
//                ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::soft.drawSecondWindow1);
			}
#ifdef MULTIPLE
			else if (a==2) {
				ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow2);
			}
			else if (a==3) {
				ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow3);
			}
			else if (a==4) {
				ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow4);
			}
#endif
			allWindows.emplace_back(newWindow);
		}
		
		ofxMicroUI::messageBox(output);
	}

	ofRunApp(guiWindow, mainApp);
	ofRunMainLoop();
}
