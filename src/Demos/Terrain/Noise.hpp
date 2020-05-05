#include "Common.h"

// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 glm::uvec3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))

glm::vec3 hash33(glm::vec3 p)
{
	glm::uvec3 q = glm::uvec3(glm::ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z)*UI3;
	return -1.0f + 2.0f * glm::vec3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

// Gradient noise by iq (modified to be tileable)
float gradientNoise(glm::vec3 x, float freq)
{
    // grid
    glm::vec3 p = floor(x);
    glm::vec3 w = fract(x);
    
    // quintic interpolant
    glm::vec3 u = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);

    // gradients
    glm::vec3 ga = hash33(glm::mod(p + glm::vec3(0., 0., 0.), freq));
    glm::vec3 gb = hash33(glm::mod(p + glm::vec3(1., 0., 0.), freq));
    glm::vec3 gc = hash33(glm::mod(p + glm::vec3(0., 1., 0.), freq));
    glm::vec3 gd = hash33(glm::mod(p + glm::vec3(1., 1., 0.), freq));
    glm::vec3 ge = hash33(glm::mod(p + glm::vec3(0., 0., 1.), freq));
    glm::vec3 gf = hash33(glm::mod(p + glm::vec3(1., 0., 1.), freq));
    glm::vec3 gg = hash33(glm::mod(p + glm::vec3(0., 1., 1.), freq));
    glm::vec3 gh = hash33(glm::mod(p + glm::vec3(1., 1., 1.), freq));
    
    // projections
    float va = glm::dot(ga, w - glm::vec3(0., 0., 0.));
    float vb = glm::dot(gb, w - glm::vec3(1., 0., 0.));
    float vc = glm::dot(gc, w - glm::vec3(0., 1., 0.));
    float vd = glm::dot(gd, w - glm::vec3(1., 1., 0.));
    float ve = glm::dot(ge, w - glm::vec3(0., 0., 1.));
    float vf = glm::dot(gf, w - glm::vec3(1., 0., 1.));
    float vg = glm::dot(gg, w - glm::vec3(0., 1., 1.));
    float vh = glm::dot(gh, w - glm::vec3(1., 1., 1.));
	
    // interpolation
    return va + 
           u.x * (vb - va) + 
           u.y * (vc - va) + 
           u.z * (ve - va) + 
           u.x * u.y * (va - vb - vc + vd) + 
           u.y * u.z * (va - vc - ve + vg) + 
           u.z * u.x * (va - vb - ve + vf) + 
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}


// Fbm for Perlin noise based on iq's blog
float perlinfbm(glm::vec3 p, float freq, int octaves)
{
    float G = exp2(-.85f);
    float amp = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    return noise;
}

//----------------------------------------------------------------------------------------
float Hash(glm::vec2 p, float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return glm::fract(sin(glm::dot(p, glm::vec2(27.16898f, 38.90563f))) * 5151.5473453f);
}

//----------------------------------------------------------------------------------------
float Noise(glm::vec2 p, float scale )
{
	glm::vec2 f;
	
	p *= scale;

	
	f = glm::fract(p);		// Separate integer from fractional
    p = glm::floor(p);
	
    f = f*f*(3.0f-2.0f*f);	// Cosine interpolation approximation
	
    float res = glm::mix(glm::mix(Hash(p, 				 scale),
						Hash(p + glm::vec2(1.0, 0.0), scale), f.x),
					glm::mix(Hash(p + glm::vec2(0.0, 1.0), scale),
						Hash(p + glm::vec2(1.0, 1.0), scale), f.x), f.y);
    return res;
}