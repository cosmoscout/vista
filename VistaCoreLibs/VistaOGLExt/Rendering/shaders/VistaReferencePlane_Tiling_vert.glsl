varying vec2 var_v2Position;

void main(void)
{
    var_v2Position = gl_Vertex.xy;
    gl_Position    = ftransform();
}