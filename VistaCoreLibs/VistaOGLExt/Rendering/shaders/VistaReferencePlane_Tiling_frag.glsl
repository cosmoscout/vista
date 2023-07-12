varying vec2 var_v2Position;

uniform vec4	u_v4TileColor;
uniform vec4	u_v4LineColor;

uniform float	u_fLineWidth;
uniform float	u_fTileWidth;
uniform float	u_fTileLength;

float g_fTileWidthInv  = 1.0 / u_fTileWidth;
float g_fTileLengthInv = 1.0 / u_fTileLength;

void main(void)
{    
    // Determines the line width.
    vec4 v4Line;
    v4Line.x = step(var_v2Position.x, 1.0 - 0.5*(u_fLineWidth * g_fTileWidthInv )  );
    v4Line.y = step(var_v2Position.y, 1.0 - 0.5*(u_fLineWidth * g_fTileLengthInv ) );
    v4Line.z = 1.0 - step(var_v2Position.x, 0.5*(u_fLineWidth * g_fTileWidthInv )  );
    v4Line.w = 1.0 - step(var_v2Position.y, 0.5*(u_fLineWidth * g_fTileLengthInv ) );
	
	float f = length(v4Line);
	
	// Mix the according color.
    vec4 color = mix(u_v4LineColor, u_v4TileColor, v4Line.x*v4Line.y*v4Line.z*v4Line.w );
    //vec4 color = vec4(line, 0.0, 1.0);
    
    // Output the color.
    gl_FragColor = color;
}