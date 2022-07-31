uniform sampler2D colormap;
uniform sampler2D colormap2;
uniform float time;
uniform vec2 playercoord;
uniform float zoom;
uniform vec2 resolution;

void main() {
	vec2 xy = (gl_FragCoord.xy / resolution / zoom);
	vec4 col;
	xy.y = mod((1 - floor(mod(xy.x + playercoord.x, 2))) * (1.0 - xy.y - playercoord.y) + floor(mod(xy.x + playercoord.x, 2)) * (xy.y + playercoord.y), 1);
	//xy.y = mod(1 - xy.y - playercoord.y, 1);
	//col = (texture2D(colormap, vec2(mod(xy.x + playercoord.x, 1), xy.y))*3/4 + texture2D(colormap2, vec2(mod(xy.x + playercoord.x, 1), xy.y))/4);
	col = texture2D(colormap, vec2(mod(xy.x + playercoord.x, 1), xy.y));
	gl_FragColor = col;
	}