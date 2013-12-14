uniform sampler2D framedata;
uniform float time;
varying vec4 texCoords;

void main( void )
{
  gl_FragColor = vec4(1.0,0.2,gl_FragCoord.z,1.0);
}