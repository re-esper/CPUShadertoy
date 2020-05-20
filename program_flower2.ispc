// https://www.shadertoy.com/view/4lSXzR

//yibojiang, 2015-8-30
#define pi 3.1415926
#define flyCount 40

float pingPong(float v) {

  const float amplitude = 1.;
  const float t = pi * 2.0;
  float k = 4.0*amplitude / t;
  float r = mod( v  , t);
  float d = floor(v / (0.5 * t) );
  return mix(k * r - amplitude ,  amplitude * 3. - k * r , mod(d, 2.0));

}

float getRad(vec2 q) {
  return atan(q.y, q.x);
}

vec2 hash(vec2 p)
{
  p = vec2( dot(p, vec2(127.1, 311.7)),
            dot(p, vec2(269.5, 183.3)) );
  return -1. + 2.*fract(sin(p) * 53758.5453123);
}

vec2 noise(vec2 tc) {
  return hash(tc);
}

float firefly(vec2 p, float size) {
  return smoothstep(0.0, size, length(p) );

}

const float pow1 = 1.0;
const float flySpeed = 0.1;
const float duration = 1.0;

vec3 drawFlower(vec3 col, vec2 p, vec2 flowerP, float t, float count, float ratio)
{

  vec2 q = p - flowerP - vec2( pow1 * 0.008 * cos(3.0*iTime) , pow1 * 0.008 * sin(3.0*iTime) ) ;
  vec2 rootP = p - flowerP - vec2( pow1 * 0.02 * cos(3.0*iTime) * p.y , -0.48 + pow1 * 0.008 * sin(3.0*iTime) );
  q.y *= ratio;
  //draw stem
  float width = 0.01;
  float h = 0.5;
  float w = 0.0005;

  col = mix(vec3(0.5, 0.7, 0.4), col,
            1.0 - (1.0 - smoothstep(h, h + width, abs(rootP.y)))
            * (1.0 - smoothstep(w, w + width, abs(rootP.x - 0.1 * sin(4.0 * rootP.y + pi * 0.35)))));

  //draw flower
  vec3 flowerCol = mix(vec3(0.7, 0.7, 0.2), vec3(0.7, 0.9, 0.7), smoothstep(0.0, 1.0, length(q) * 10.0));

  float r = 0.1 + 0.05 * ( pingPong( getRad( q ) * count  + 2.*q.x * (t - duration)  )  );

  col = mix(flowerCol, col, smoothstep(r, r + 0.02,  length(q)));

  //draw buds
  float r1 = 0.04;
  vec3 budCol = mix (vec3(.3, .4, 0.), vec3(.9, .8, 0.), length(q) * 10.0);
  col = mix(budCol, col, smoothstep(r1, r1 + 0.01,  length(q)));
  return col;
}

inline void mainImage(vec4& fragColor, vec2 fragCoord)
{
  float t = duration * (1. + sin(3.0 * iTime ) );

  vec2 p = fragCoord.xy / iResolution.xy;

  float ratio = iResolution.y / iResolution.x;

  vec2 uv = p;
  uv.y *= ratio;

  //vec2 flowerP = ;

  //sky
  vec3 col = mix( vec3(0.1, 0.6, 0.5), vec3(0.2, 0.1, 0.2), sqrt(p.y) * .6 );
  col = mix(drawFlower(col, p, vec2(0.618, 0.518), t, 7.0, ratio), col, 1.0 - smoothstep(0.0, 0.01, length(p)));
  col = mix(drawFlower(col, p, vec2(0.418, 0.418), t*4.0, 6.0, ratio), col, 1.0 - smoothstep(0.0, 0.01, length(p)));
  col = mix(drawFlower(col, p, vec2(0.818, 0.318), t*2.0, 8., ratio), col, 1.0 - smoothstep(0.0, 0.01, length(p)));

  for (int ii = 0; ii < flyCount; ii++) {
    float i = float(ii);
    float seed = i / float(flyCount);
    float t1 = 1.0*(1. + sin((noise(vec2(seed))).x * iTime));
    vec2 fireflyP = uv - vec2((noise(vec2(seed))).x + (noise(vec2(seed))).y * t1 * flySpeed, (noise(vec2(seed))).y + (noise(vec2(seed))).y * t1 * flySpeed);
    float fly = firefly( fireflyP, 0.002 + 0.008 * seed );
    vec3 flyCol = mix(vec3(0.1, 0.9, 0.1) * t1, vec3(0.0), fly );
    col += flyCol;
  }
  fragColor = vec4(col, 0.);
}
