//  Fragment shader

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

float length2(vec2 p) { return dot(p, p); }

float noise(vec2 p){
	return fract(sin(fract(sin(p.x) * (43.13311)) + p.y) * 31.0011);
}

float worley(vec2 p) {
	float d = 1e30;
	for (int xo = -1; xo <= 1; ++xo) {
		for (int yo = -1; yo <= 1; ++yo) {
			vec2 tp = floor(p) + vec2(xo, yo);
			d = min(d, length2(p - tp - vec2(noise(tp))));
		}
	}
	return 3.0*exp(-4.0*abs(2.0*d - 1.0));
}

float fworley(vec2 p) {
	return sqrt(sqrt(sqrt(
		0.5 * // light
		worley(p*5. + .3) *
		sqrt(worley(p * 50. + 0.9 )) *
		sqrt(sqrt(worley(p * -10. + 9.3))))));
}

void main() {
	vec2 uv = gl_FragCoord.xy ;
	
	float t = fworley(uv * resolution.xy / 1000.0) ;
	gl_FragColor = vec4(pow(t,0.4),0.7, 0.2, 1.0) * (1.2 - t) * 2.;
}
