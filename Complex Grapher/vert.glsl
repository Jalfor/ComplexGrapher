#version 330

#define PI 3.1415926535897932384626433832795

layout(location = 0) in vec2 position;
smooth out vec4 fColor;

uniform mat4 projMatrix;
uniform mat4 transform4d;
uniform int maxIter;
uniform float mandelbrotPower;
uniform int drawSet;

const float projectionConstant = 3.f;
const float bailoutRadius = 10000000.f;

vec2 mult(vec2 c1, vec2 c2)
{
	float real = c1.x * c2.x - c1.y * c2.y;
	float imag = c1.x * c2.y + c1.y * c2.x;

	vec2 result = vec2(real, imag);
	return result;
}

//Takes cNum^power
vec2 pow(vec2 cNum, float power)
{
	float cMod = length(cNum);
	float cArg = atan(cNum.y, cNum.x);

	float resultMod = pow(cMod, power);
	float resultArg = mod(cArg * power + PI, (2 * PI)) - PI;  //Keep arg within +-PI

	vec2 result = resultMod * vec2(cos(resultArg), sin(resultArg));
	return result;
}

vec2 sin(vec2 cNum)
{
	vec2 result = vec2(sin(cNum.x) * cosh(cNum.y), cos(cNum.x) * sinh(cNum.y));
	return result;
}

vec2 cos(vec2 cNum)
{
	vec2 result = vec2(cos(cNum.x) * cosh(cNum.y) - sin(cNum.x) * sinh(cNum.y));
	return result;
}

vec2 exp(vec2 cNum)
{
	float eToX = exp(cNum.x);
	vec2 eToIY = vec2(cos(cNum.y), sin(cNum.y));
	return eToX * eToIY;
}

vec4 getMandelbrotColor(vec2 c)
{
	vec4 mandelbrotColor;

	vec2 z = vec2(0.f, 0.f);  //For mandelbrot
	//vec2 z = c;

	bool inSet = true;

	for (int i = 0; i < maxIter; i++) {
		float fi = float(i);

		z = pow(z, mandelbrotPower) + c;
		//z = pow(z, mandelbrotPower) + vec2(-0.7f, 0.2);

		if (length(z) > 2.f) 
		{
			float contValue = fi - log(log(length(z)) / log(bailoutRadius)) / log(mandelbrotPower);
			
			mandelbrotColor.r = min(log(mandelbrotPower + 1) * 0.075f * contValue, 1.f);
			mandelbrotColor.g = min(log(mandelbrotPower + 1) * 0.05f * contValue, 1.f);
			mandelbrotColor.b = min(log(mandelbrotPower + 1) * 0.05f * exp(contValue), 1.f);

			inSet = false;
			break;
		}
	}

	if (inSet)
	{
		if (drawSet)
		{
			mandelbrotColor.r = 0.f;
			mandelbrotColor.g = 0.f;
			mandelbrotColor.b = 0.f;
		}
		
		else
		{
			mandelbrotColor.r = 1.f;
			mandelbrotColor.g = 1.f;
			mandelbrotColor.b = 1.f;
		}
	}

	return mandelbrotColor;
}

void main()
{
	//The Mandelbrot set is usually graphed from -2 to 2, but
	//position is from -1 to 1, hence * 2
	fColor = getMandelbrotColor(position * 3.f);

	//vec4 position4d = vec4(position, pow(position, 2.f));
	vec2 z = position;
	vec4 position4d = vec4(z,
$EQUATION$
	);
	position4d = transform4d * position4d;
	position4d *= projectionConstant / (projectionConstant + position4d[3]);
	position4d[3] = 1.f;

	vec4 position3d = projMatrix * position4d;
	gl_Position = position3d;
}