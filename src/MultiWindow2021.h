
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

	ofGLFWWindowSettings settings;
	settings.resizable = true;
	settings.setSize(WINDOWWIDTH, WINDOWHEIGHT);
	settings.windowMode = OF_WINDOW;
	//settings.numSamples = 8;
	//settings.setPosition(ofVec2f(80,80));
	//settings.setGLVersion(3, 2);

	auto guiWindow = ofCreateWindow(settings);
	auto mainApp = make_shared<ofApp>();
	int monitorCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

	cout << glfwGetVersionString() << endl;
	cout << "Screens Found: " << monitorCount << endl;
	
	for (int a=0; a<monitorCount; a++) {
		string output;
		output += "display # " + ofToString(a) + " : " + glfwGetMonitorName(monitors[a]) + "\r";

		float scaleX, scaleY;
		glfwGetMonitorContentScale(monitors[a], &scaleX, &scaleY); // We take the second monitor
		output += "contentscale : " + ofToString(scaleX) + " x " + ofToString(scaleY) + "\r";

//		glViewport(0, 0, width, height);

		const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[a]);
		glm::vec2 size = glm::vec2(desktopMode->width, desktopMode->height);
		output += "size: " + ofToString(size.x) + " x " + ofToString(size.y) + "\r";

		// not primary display, second, third
		if (a > 0) {
			settings.setSize (size.x, size.y);

			int posx; int posy;
			glfwGetMonitorPos(monitors[a], &posx, &posy);
			output += "position: " + ofToString(posx) + " x " + ofToString(posy);
			settings.setPosition(glm::vec2(posx, posy));

			settings.windowMode = OF_FULLSCREEN;
//			settings.resizable = false;=-
			settings.shareContextWith = guiWindow;
			newWindow = ofCreateWindow(settings);

			if (a==1) {
				ofAddListener(newWindow->events().draw, mainApp.get(), &ofApp::drawSecondWindow1);
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