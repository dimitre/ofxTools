float map(float in, float inMin, float inMax, float outMin, float outMax, float shaper){
	// (1) convert to pct (0-1)
	float pct = ofMap (in, inMin, inMax, 0, 1, true);
	// raise this number to a power
	pct = powf(pct, shaper);
	float out = ofMap(pct, 0,1, outMin, outMax, true);
	return out;
}
