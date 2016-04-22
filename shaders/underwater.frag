/*
* Based off of Beneath the Sea by zel
* url https://www.shadertoy.com/view/4ljXWh
* 
* @var RockTex      rock.jpg
* @var NoiseTex0    noise0.png
* @var NoiseTex1    noise1.jpg
* @var time         current time
*
*/

#define TAU 6.28318530718
#define MAX_ITER 5

// Textures passed from main program
uniform sampler2D RockTex;
uniform sampler2D NoiseTex0;
uniform sampler2D NoiseTex1;

// Time given in main application
uniform float time;

// Resolution passed by main program
uniform vec2 resolution;

float speck(vec2 pos, vec2 uv, float radius)
{
    pos.y += 0.05;
    float color = distance(pos, uv);
    vec3 tex = texture2D(NoiseTex0, sin(vec2(uv)*10.1)).xyz;
    vec3 tex2 = texture2D(NoiseTex0, sin(vec2(pos)*10.1)).xyz;
    color = clamp((1.0 - pow(color * (5.0 / radius), pow(radius,0.9))), 0.0, 1.0);
    color *= clamp(mix(sin(tex.y)+0.1,cos(tex.x),0.5)*sin(tex2.x)+0.2,0.0,1.0);
    return color;
}

// Terrain base
vec3 caustic(vec2 uv)
{
    vec2 p = mod(uv*TAU, TAU)-250.0;

	vec2 i = vec2(p);
	float c = 1.0;
	float inten = .005;

	for (int n = 0; n < MAX_ITER; n++) 
	{
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
    
	c /= float(MAX_ITER);
	c = 1.17-pow(c, 1.4);
	vec3 color = vec3(pow(abs(c), 8.0));
    color = clamp(color + vec3(0.0, 0.35, 0.5), 0.0, 1.0);
    color = mix(color, vec3(1.0,1.0,1.0),0.3);
    
    return color;
}

// Use provided textures to add noise to shader for use in calculating where bubble specks should go
float noise(vec2 p)
{
   
    float height = mix(texture2D(NoiseTex0, p / 80.0, -100.0).x,1.0,0.85);
    float height2 = mix(texture2D(NoiseTex1, p / 700.0, -200.0).x,0.0,-3.5);
    
    return height2-height-0.179;
}

// Fractional Brownian Motion
float fBm(vec2 p) 
{
    float sum = 0.0;
    float amp = 1.0;
    
    for(int i = 0; i < 4; i++)
    {
        sum += amp * noise(p);
        amp *= 0.5;
        p *= 2.5;
    }
    return sum * 0.5 + 0.15;
}


vec3 raymarchTerrain(vec3 ro, vec3 rd, float tmin, float tmax)
{
    float t = tmin;
    vec3 res = vec3(-1.0);
    
    for (int i = 0; i < 110; i++) 
    {
        vec3 p = ro + rd * t;
        
        res = vec3(vec2(0.0, p.y - fBm(p.xz)), t);
        
        float d = res.y;
        
        if (d < (0.001 * t) || t > tmax)
        {
            break;
        }
        
        t += 0.5 * d;
    }
    
    return res;
}


vec3 getTerrainNormal(vec3 p) 
{
    float eps = 0.025;
    return normalize(vec3(fBm(vec2(p.x - eps, p.z)) - fBm(vec2(p.x + eps, p.z)),
                          2.0 * eps,
                          fBm(vec2(p.x, p.z - eps)) - fBm(vec2(p.x, p.z + eps))));
}

vec3 getSceneColor(vec3 ro, vec3 rd)
{
    vec3 skyColor = vec3(0.3, 1.0, 1.0);

    vec3 sunLightColor = vec3(1.7, 0.65, 0.65);
    vec3 skyLightColor = vec3(0.8, 0.35, 0.15);
    vec3 indLightColor = vec3(0.4, 0.3, 0.2);
    vec3 horizonColor = vec3(0.0, 0.05, 0.2);
    vec3 sunDirection = normalize(vec3(0.8, 0.8, 0.6));
    
    // background
    vec3 color = skyColor;
    float sky = 0.0;

    // terrain marching
    float tmin = 0.1;
    float tmax = 20.0;
    vec3 res = raymarchTerrain(ro, rd, tmin, tmax);
    
    vec3 colorBubble = vec3(0.0);
    float bubble = 0.0;
    bubble +=  speck(vec2(sin(time*0.32),cos(time)*0.2+0.1),rd.xy,-0.08*rd.z);
	bubble +=  speck(vec2(sin(1.0-time*0.39)+0.5,cos(1.0-time*0.69)*0.2+0.15),rd.xy,0.07*rd.z);   
	bubble +=  speck(vec2(cos(1.0-time*0.5)-0.5,sin(1.0-time*0.36)*0.2+0.1),rd.xy,0.12*rd.z);   
 	bubble +=  speck(vec2(sin(time*0.44)-1.0,cos(1.0-time*0.32)*0.2+0.15),rd.xy,-0.09*rd.z);   
 	bubble +=  speck(vec2(1.0-sin(1.0-time*0.6)-1.3,sin(1.0-time*0.82)*0.2+0.1),rd.xy,0.15*rd.z);

    colorBubble = bubble*vec3(0.2,0.7,1.0);
    if(rd.z < 0.1)
    {
        float y = 0.00;
        for(float x = 0.39; x < 6.28; x += 0.39)
        {
            vec3 height = texture2D(NoiseTex0, vec2(x)).xyz;
            y+= 0.03*height.x;
			bubble = speck(vec2(sin(time+x)*0.5+0.2,cos(time*height.z*2.1+height.x*1.7)*0.2+0.2),
            rd.xy,(cos(time+height.y*2.3+rd.z*-1.0)*-0.01+0.25));
            colorBubble += bubble*vec3(-0.1*rd.z,-0.5*rd.z,1.0);
        }
    }    
         
    float t = res.z;
    
    if (t < tmax) 
    {
        vec3 pos = ro + rd * t;
        vec3 nor;
        
        // add bumps
        nor = getTerrainNormal(pos);
        nor = normalize(nor + 0.5 * getTerrainNormal(pos * 8.0));
 
        float sun = clamp(dot(sunDirection, nor), 0.0, 1.0);
        sky = clamp(0.5 + 0.5 * nor.y, 0.0, 1.0);
        vec3 diffuse=mix(texture2D(RockTex, vec2(pos.x*pow(pos.y,0.01),pos.z*pow(pos.y,0.01))).xyz,vec3(1.0,1.0,1.0),clamp(1.1-pos.y,0.0,1.0));

        diffuse *= caustic(vec2(mix(pos.x,pos.y,0.2),mix(pos.z,pos.y,0.2))*1.1);
        vec3 lightColor = 1.0 * sun * sunLightColor;

        lightColor += 0.7 * sky * skyLightColor;

        color *= 0.8 * diffuse*lightColor;

        // fog
        color = mix(color, horizonColor, 1.0 - exp(-0.3 *pow(t,1.0) ));
    }
    else 
    {
        sky = clamp(0.8 * (1.0 - 0.8 * rd.y), 0.0, 1.0);
        color = sky * skyColor;

        // horizon
        color = mix(color, horizonColor, pow(1.0 - pow(rd.y,4.0), 20.0));       
    }
    
    // special effects
    color += colorBubble;

    // gamma correction
    vec3 gamma = vec3(0.46);
    return pow(color, gamma);
}



/********************* JellyFish Addition ************************/
#define PI				3.14159265359
#define PIH				(PI/2.0)
#define PI2				(PI*2.0)
#define DRAW_RANGE		50.0
vec3	MOD_OFFSET		= vec3(16.0, 32.0, 16.0);
vec3	MOD_INV_OFFSET	= 1.0 / MOD_OFFSET;
#define OFFSET_Y		4.0


mat3 RotX(float a)								{float s = sin(a); float c = cos(a); return mat3(1,0,0,0,c,s,0,-s,c);}
mat3 RotY(float a)								{float s = sin(a); float c = cos(a); return mat3(c,0,-s,0,1,0,s,0,c);}
mat3 RotZ(float a)								{float s = sin(a); float c = cos(a); return mat3(c,s,0,-s,c,0,0,0,1);}
float Sub(float from, float what)				{return max(from, -what);}
float Add(float a, float b)						{return min(a, b);}
float Intersect(float a, float b)				{return max(a, b);}


float PropulsionTime(float s, float offset)		{return time*0.5 + sin((time+offset*PI)*2.0 + PI*0.1)*s;}
float BreathScale(float y, float offset)		{return sin(y * PI * 0.8 + (time+offset*PI) * 2.0) * 0.5 + 0.5;}


vec4 Rnd4(float i)								{return sin(vec4(123.8456, 64.146543, 992.12343, 1235.01023) * i);}

float ModRand(vec3 p)
{
	p = (p + MOD_OFFSET*8.0) / MOD_OFFSET;
	p = sin(floor(p)*946.1235) * 0.5 + 0.5;
	return sin(dot(p, vec3(123.1123, 234.123, 67.112))) * 0.5 + 0.5;
}

float JellyBody(vec3 p, float inOffset)
{
	p.y					*= 0.7 + inOffset*0.2;
	
	// Z plugged into X on purpose, this makes the shape not perfect from both sides
	p.z					+= (sin(p.x * PI * 4.0)*0.5 + 0.5) * 0.06;
	p.x					+= (cos(p.z * PI * 4.0)*0.5 + 0.5) * 0.06;
	
	float breath		= pow(0.2 + BreathScale(p.y, inOffset)*0.8, 0.2);
	p.xz				*= 0.5 + breath*0.5;
	
	float sphereOuter	= (length(p) - 1.0);
	float sphereInner	= (length(p) - 0.95);
	float hallowSphere	= Sub(sphereOuter, sphereInner);
	float bodyLength	= (p.y+0.5 + (1.0-clamp(breath-0.5,0.0,1.0)) * 0.4);
	float halfSpere		= Sub(hallowSphere, bodyLength);

	return halfSpere;
}



float JellyTentacle(vec3 p, float inTentacleLength, float inAnimSpeed, float inOffset)
{	
	float tentacleScalePos	= clamp(abs(p.y) / inTentacleLength, 0.0, 1.0);
	
	float tentacleMod		= pow(tentacleScalePos, 1.5) * PI2 - PropulsionTime(0.15, inOffset) * inAnimSpeed;
	float tentacleModifierX	= cos(tentacleMod)*0.4;
	float tentacleModifierY	= cos(tentacleMod + 12.02343)*0.4;
	p.x						+=(tentacleScalePos * tentacleModifierX) * 2.0;
	p.z						+=(tentacleScalePos * tentacleModifierY) * 2.0;
	
	float tentacleThickness	= mix(0.15, 0.01, tentacleScalePos);
	
	p.y						= abs(p.y + inTentacleLength*0.5) - inTentacleLength * 0.5;

	float cylinder			= max(length(p.xz) - tentacleThickness, p.y);
	return cylinder;
	}


float JellyFish(vec3 p, float inOffset)
{
	
	vec4 rnd	= Rnd4(1.0 + inOffset);

	float d		= 1000.0;
	
	d			= min(d, JellyBody(p, inOffset));	
	d			= min(d, JellyTentacle(p + vec3(rnd.x, 0, rnd.y)* 0.2, 6.0+rnd.z*4.0, 1.0+rnd.w*0.5, inOffset));
	d			= min(d, JellyTentacle(p + vec3(rnd.w, 0, rnd.x)* 0.2, 6.0+rnd.y*4.0, 1.0+rnd.z*0.5, inOffset));
	d			= min(d, JellyTentacle(p + vec3(rnd.z, 0, rnd.w)* 0.2, 6.0+rnd.x*4.0, 1.0+rnd.y*0.5, inOffset));
	d			= min(d, JellyTentacle(p + vec3(rnd.y, 0, rnd.z)* 0.2, 6.0+rnd.w*4.0, 1.0+rnd.x*0.5, inOffset));

	return d;	
}

vec2 Scene(vec3 p)
{
	// Create an offset
	float rnd		= ModRand(p);
	
	p.y				-=PropulsionTime(0.3, rnd * PI);
	
	// Instantiate
	p				= mod(p, MOD_OFFSET)-MOD_OFFSET*0.5;
	p.y				+=rnd * OFFSET_Y;
	
	float d			= JellyFish(p, rnd);

	return vec2(d, length(p.xz));
}

vec2 GetDepth(vec3 p, vec3 o)
{
	vec2 d	= vec2(1000.0, 0.0);
	float z = 0.0;
	
	for(int i=0; i<46; i++)
	{
		d = Scene(p + o*z);
		if(d.x < 0.01)
			continue;
		z += max(0.01, d.x*0.8);
	}
	
	if(z > DRAW_RANGE)
		z = 0.0;
	
	return vec2(z, d.y);
}


vec3 Normal(vec3 p, float d)
{	
	vec3 n;
	vec2 dn = vec2(d, 0.0);
	n.x	= Scene(p + dn.xyy).x - Scene(p - dn.xyy).x;
	n.y	= Scene(p + dn.yxy).x - Scene(p - dn.yxy).x;
	n.z	= Scene(p + dn.yyx).x - Scene(p - dn.yyx).x;
	return normalize(n);
	}


vec3 GetRayDir(vec2 inTC, vec2 inAspectRatio)
{
	return normalize(vec3((-0.5 + inTC)*0.8 * inAspectRatio, 1.0));
}

	
void main()
{
    // Setup background and terrain
    vec2 p = (-resolution.xy + 2.0 * gl_FragCoord.xy) / resolution.y;

    vec3 eye = vec3(0.0, 1.25, 1.5);
    vec2 rot = 6.2831 * (vec2(-0.05 + time * 0.01, 0.0 - sin(time * 0.5) * 0.01) + vec2(1.0, 0.0));
    eye.yz = cos(rot.y) * eye.yz + sin(rot.y) * eye.zy * vec2(-1.0, 1.0);
    eye.xz = cos(rot.x) * eye.xz + sin(rot.x) * eye.zx * vec2(1.0, -1.0);

    vec3 ro = eye;
    vec3 ta = vec3(0.5, 1.0, 0.0);

    vec3 cw = normalize(ta - ro);
    vec3 cu = normalize(cross(vec3(0.0, 1.0, 0.0), cw));
    vec3 cv = normalize(cross(cw, cu));
    mat3 cam = mat3(cu, cv, cw);

    vec3 rd = cam * normalize(vec3(p.xy, 1.0));
    vec3 col = getSceneColor(ro, rd);
    
    
    // Add in jellyfish
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    // Construct simple ray
	vec2 aspect		= vec2(resolution.x/resolution.y, 1.0);
	vec3 rayOrigin	= vec3(-8.0, -8.0, -0.0);
	vec3 rayDir		= GetRayDir(uv, aspect);
	
	// Transform camera orientation
	mat3 ymat		= RotY(PI2 * 0.55);
	mat3 xmat		= RotX(PI2 * 0.5);
	mat3 zmat		= RotZ(PI2 * 0.6);
	mat3 mat		= zmat * ymat * xmat;
	rayOrigin		= mat * rayOrigin;
	rayDir			= mat * rayDir;
	
	// Light
	vec3 lightDir	= rayOrigin;
	
	vec2 result		= GetDepth(rayOrigin, rayDir);
	if(result.x > 0.0)
	{
		vec3 p				= rayOrigin + rayDir * result.x;
		vec3 n				= Normal(p, 0.01);
		float liFacing		= max(dot(n, normalize(lightDir)), 0.0);
		float liUp			= n.y;
		
		float expScale		= 0.0;//pow(clamp(result.y-0.2, 0.0, 1.0), 4.0);
		float depthFog		= pow(1.0 - result.x / DRAW_RANGE, 2.0);
		
		// Fake skin / sub surface scatting
		vec3 color1			= vec3(0.95,0.6,0.25);
		vec3 color2			= vec3(0.95,0.2,0.1);
		float highLight 	= 1.0 - liFacing;
		vec3 jellyColor		= mix(color2, color1, highLight);
		vec3 colorFacing	= mix(col, jellyColor * depthFog, pow(depthFog, 0.6));
		vec3 colorUp		= vec3(liUp);
		col				= colorFacing + colorUp*pow(depthFog, 2.0) * 0.8;
	}
	
    
    gl_FragColor = vec4(col, 1.0);
}

