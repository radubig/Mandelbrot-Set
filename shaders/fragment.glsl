#version 440 core

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

double NormalizedIteration(dvec2 coord)
{
    double x = 0, y = 0, aux;
    while(x*x + y*y <= C && n < iter)
    {
        aux = x*x - y*y + coord.x;
        y = 2 * x * y + coord.y;
        x = aux;
        n += 1;
    }
    if(n < iter)
    {
        float log_zn = log(float(x*x + y*y)) * 0.5;
        float nu = log(log_zn / log(2)) / log(2);
        n = floor(n + 1 - nu);
    }

    return n;
}

void main()
{
    vec4 cl1, cl2;
    dvec2 coord = dvec2(gl_FragCoord.xy);
    double t = IterationsNumber((coord - screenSize/2)/zoom - screenOffset);
    //double t = NormalizedIteration((coord - screenSize * 0.5)/zoom - screenOffset);
    if(t==iter) color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    else
        /*cl1 = texture(tex, floor(float(t)) / iter + UVoffset);
        cl2 = texture(tex, floor(float(t)+1) / iter + UVoffset);
        color = mix(cl1, cl2, float(t) - floor(float(t)));*/
        //color = texture(tex, 1.0 / float(t));
        color = texture(tex, float(t) / freq + UVoffset);
}
