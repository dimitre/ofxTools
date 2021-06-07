struct featureBloom : virtual public microFeature {
	using microFeature::microFeature;

	map <string, ofBlendMode> blendMap = {
		{ "add", OF_BLENDMODE_ADD },
		{ "alpha", OF_BLENDMODE_ALPHA },
		{ "screen", OF_BLENDMODE_SCREEN },
		{ "multiply", OF_BLENDMODE_MULTIPLY },
		{ "subtract", OF_BLENDMODE_SUBTRACT }
	};

	string frag = R"V0G0N(
//#version 150	
#version 120
//precision highp float;
uniform sampler2DRect tex0;
varying vec2 texcoord0;

uniform bool horizontal;
uniform float scale;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    
    //https://github.com/Jam3/glsl-fast-gaussian-blur

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  color += texture2D(image, uv) * 0.29411764705882354;
  color += texture2D(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture2D(image, uv - (off1 / resolution)) * 0.35294117647058826;
  return color;
}

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture2D(image, uv) * 0.2270270270;
  color += texture2D(image, uv + (off1 / resolution)) * 0.3162162162;
  color += texture2D(image, uv - (off1 / resolution)) * 0.3162162162;
  color += texture2D(image, uv + (off2 / resolution)) * 0.0702702703;
  color += texture2D(image, uv - (off2 / resolution)) * 0.0702702703;
  return color;
}

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.411764705882353) * direction;
  vec2 off2 = vec2(3.2941176470588234) * direction;
  vec2 off3 = vec2(5.176470588235294) * direction;
  color += texture2D(image, uv) * 0.1964825501511404;
  color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
  color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
  return color;
}
    
    
void main (void) 
{ 		
	// vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec2 tex_offset = vec2(1.0 * 2.0, 1.0 * 2.0);
	vec2 xy = gl_FragCoord.xy * scale;
	vec4 result = texture2DRect(tex0, xy).rgba * weight[0];
	if(horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture2DRect(tex0, xy + vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
			result += texture2DRect(tex0, xy - vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture2DRect(tex0, xy + vec2(0.0, tex_offset.y * i)).rgba * weight[i];
			result += texture2DRect(tex0, xy - vec2(0.0, tex_offset.y * i)).rgba * weight[i];
		}
	}
	gl_FragColor = vec4(result); //, texture2DRect(tex0, xy).a
} 
)V0G0N";

	string vert = R"V0G0N(
varying vec2 texcoord0;

void main()
{
	gl_Position = ftransform();
	texcoord0 = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
}
)V0G0N";
	ofFbo fbos[12];
	ofFbo fbos2[12];
	ofFbo fboIn;
	ofShader shader;
	ofFbo * f = &fbos[0];
    int maxFbos = 0;

	void setup() override {
		shader.setupShaderFromSource( GL_VERTEX_SHADER, vert );
		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, frag );
		shader.bindDefaults();
		shader.linkProgram();
		float w = soft->fboFinal->getWidth();
		float h = soft->fboFinal->getHeight();
//        int depth = GL_RGBA32F_ARB; //GL_RGBA
//        int depth = GL_RGBA16F_ARB; //GL_RGBA
        int depth = GL_RGBA; //
		for (int a=0; a<11; a++) {
            if (w > 2 && h > 2) {
                fbos[a].allocate(w, h, depth);
                fbos2[a].allocate(w, h, depth);
                maxFbos = a;
            }
			// fbosB[a].allocate(w, h, GL_RGBA);
			w *= .5;
			h *= .5;
		}
		use = &ui->pBool["bloom"];
	}

	void begin() override {
		if (isOk()) {
			f->begin();
			ofClear(0,0);
		}
	}
	
	void end() override {
		if (isOk()) {
			f->end();
            
            // reduce the size from big to small
			int iteracoes = MIN(maxFbos, ui->pInt["iteracoes"]);
            ofSetColor(255, ui->pFloat["opacity"]);
			for (int a=1; a<=iteracoes; a++) {
				ofFbo * fo = &fbos[a-1];
				ofFbo * f1 = &fbos[a]; // menorzinho
				f1->begin();
				ofClear(0,0);
				fo->draw(0,0,f1->getWidth(),f1->getHeight());
				f1->end();
			}
            
			ofEnableBlendMode(blendMap[ui->pString["blend"]]);

            // blurring the smaller into the bigger
			for (int a=iteracoes; a>0; a--) {
				ofFbo * fo = &fbos[a-1];
                ofFbo * fo2 = &fbos2[a-1];
				ofFbo * f1 = &fbos[a]; // menorzinho
//                ofFbo * f12 = &fbos[a]; // menorzinho
                fo2->begin(); // original style
                ofClear(0,0);
                shader.begin();
                shader.setUniform1i("horizontal", 1);
                shader.setUniform1f("scale", 0.5);
                
                // reduz imagem aqui. o certo seria fazer um ping pong.
                f1->draw(0,0,fo->getWidth(), fo->getHeight());
//                f12->draw(0,0,fo->getWidth(), fo->getHeight());
                shader.end();
                fo2->end();


                fo->begin(); // original style
//                ofClear(0,0);
                shader.begin();
                shader.setUniform1i("horizontal", 0);
                shader.setUniform1f("scale", 1);
                fo2->draw(0,0);
                shader.end();
                fo->end();
                
                
//                fo2->begin();
//                ofClear(0,0);
//                fo2->end();
			}

            ofSetColor(255);
			f->draw(0,0);
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		}
	}

	void drawFbos() {
		int offy = 0;
		float fator = .2;
		for (auto & f : fbos) {
			int w = f.getWidth() * fator;
			int h = f.getHeight() * fator;
			f.draw(1000,offy,w,h);
			offy += h + 5;
		}
	}

	void uiEvents(ofxMicroUI::element & e) override {
	}
};
