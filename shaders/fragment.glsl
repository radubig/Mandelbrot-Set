#version 440 core

precision highp float;

layout(location = 0) out vec4 color;

uniform int iter;
uniform double zoom;
uniform dvec2 screenSize;
uniform dvec2 screenOffset;
uniform sampler1D tex;
uniform float freq;
uniform float UVoffset;

double n = 0;
double C = 4.0;

double IterationsNumber(dvec2 coord)
{
    dvec2 last = dvec2(0.0, 0.0);
    for(int i=1; i<=iter; i++)
    {
        dvec2 z;
        z.x = (last.x * last.x) - (last.y * last.y ) + coord.x;
        z.y = (2.0 * last.x * last.y) + coord.y;
        if((z.x * z.x) + (z.y * z.y) > C)
            break;
        last = z;
        n+=1;
    }
    return n;
}

void main()
{
    dvec2 coord = dvec2(gl_FragCoord.xy);
    double t = IterationsNumber((coord - screenSize/2)/zoom - screenOffset);
    if(t==iter) color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    else
        color = texture(tex, float(t) / freq + UVoffset);
}
