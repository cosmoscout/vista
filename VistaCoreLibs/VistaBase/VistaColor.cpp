#include "VistaColor.h"
#include "VistaExceptionBase.h"

#include <algorithm>

//// Constructors

VistaColor::VistaColor(
	int iColor /*= WHITE */, EFormat eFormat /*= RGB*/ )
{
	SetValues(iColor, eFormat);
}

VistaColor::VistaColor(
	int i1, int i2, int i3, EFormat eFormat /*= RGBA*/ )
{
	SetValues(i1, i2, i3, eFormat);
}

VistaColor::VistaColor(
	float f1, float f2, float f3, EFormat eFormat /*= RGB*/ )
{
	SetValues(f1, f2, f3, eFormat);
}

VistaColor::VistaColor(
	double d1, double d2, double d3, EFormat eFormat /*= RGB*/ )
{
	SetValues(d1, d2, d3, eFormat);
}

VistaColor::VistaColor(
	int i1, int i2, int i3, int i4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(i1, i2, i3, i4, eFormat);
}

VistaColor::VistaColor(
	float f1, float f2, float f3, float f4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(f1, f2, f3, f4, eFormat);
}

VistaColor::VistaColor(
	double d1, double d2, double d3, double d4, EFormat eFormat /*= RGBA*/ )
{
	SetValues(d1, d2, d3, d4, eFormat);
}

VistaColor::VistaColor(const float afValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(afValues, eFormat);
}

VistaColor::VistaColor(
	const double adValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(adValues, eFormat);
}

VistaColor::VistaColor(const int aiValues[], EFormat eFormat /*= RGB*/)
{
	SetValues(aiValues, eFormat);
}

//// Setters

void VistaColor::SetValues( int i1, int i2, int i3, EFormat eFormat /*= RGB */ )
{
	float a3fValues[3] = { 
		i1 / 255.f, 
		i2 / 255.f, 
		i3 / 255.f };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues(
	float f1, float f2, float f3, EFormat eFormat /*= RGB */ )
{
	float a3fValues[3] = { f1, f2, f3 };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues( double d1, double d2, double d3, EFormat eFormat
	/*= RGB */ )
{
	float a3fValues[3] = { 
		static_cast<float>(d1),
		static_cast<float>(d2),
		static_cast<float>(d3) };
	SetValues(a3fValues, eFormat);
}

void VistaColor::SetValues(
	int i1, int i2, int i3, int i4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = {
		i1 / 255.f,
		i2 / 255.f,
		i3 / 255.f,
		i4 / 255.f };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	float f1, float f2, float f3, float f4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = { f1, f2, f3, f4 };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	double d1, double d2, double d3, double d4, EFormat eFormat /*= RGBA*/ )
{
	float a4fValues[4] = { 
		static_cast<float>(d1),
		static_cast<float>(d2),
		static_cast<float>(d3),
		static_cast<float>(d4) };
	SetValues(a4fValues, eFormat);
}

void VistaColor::SetValues(
	const float afValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
		m_a4fValues[0] = afValues[0];
		m_a4fValues[1] = afValues[1];
		m_a4fValues[2] = afValues[2];
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[0] = afValues[0];
		m_a4fValues[1] = afValues[1];
		m_a4fValues[2] = afValues[2];
		m_a4fValues[3] = afValues[3];
		break;
	case HSL:
		HSLtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = 1.f;
		break;
	case HSLA:
		HSLtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = afValues[3];
		break;
	case HSV:
		HSVtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = 1.f;
		break;
	case HSVA:
		HSVtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = afValues[3];
		break;
	case LAB:
		LabtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = 1.f;
		break;
	case LABA:
		LabtoRGB(afValues, m_a4fValues);
		m_a4fValues[3] = afValues[3];
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(
	const double adValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3] = {
				static_cast<float>(adValues[0]),
				static_cast<float>(adValues[1]),
				static_cast<float>(adValues[2])
				};
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			float a4fValues[4] = {
				static_cast<float>(adValues[0]),
				static_cast<float>(adValues[1]),
				static_cast<float>(adValues[2]),
				static_cast<float>(adValues[3])
			};
			SetValues(a4fValues, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(int iColor, EFormat eFormat /*= FORMAT_RGB*/ )
{
	float a4fDecomp[4] = {
		((iColor & 0xFF000000) >> 24) / 255.0f,
		((iColor & 0x00FF0000) >> 16) / 255.0f,
		((iColor & 0x0000FF00) >> 8) / 255.0f,
		 (iColor & 0x000000FF) / 255.0f };

	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3] = { a4fDecomp[1], a4fDecomp[2], a4fDecomp[3] };
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			SetValues(a4fDecomp, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetValues(
	const int aiValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3] = {
				aiValues[0] / 255.f,
				aiValues[1] / 255.f,
				aiValues[2] / 255.f,
			};
			SetValues(a3fValues, eFormat);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			float a4fValues[4] = {
				aiValues[0] / 255.f,
				aiValues[1] / 255.f,
				aiValues[2] / 255.f,
				aiValues[3] / 255.f,
			};
			SetValues(a4fValues, eFormat);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::SetRed(float fR)
{
	m_a4fValues[0] = fR;
}

void VistaColor::SetGreen(float fG)
{
	m_a4fValues[1] = fG;
}

void VistaColor::SetBlue(float fB)
{
	m_a4fValues[2] = fB;
}

void VistaColor::SetAlpha(float fA)
{
	m_a4fValues[3] = fA;
}

void VistaColor::SetHue(float fHue)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[0] = fHue;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetHSLSaturation(float fSaturation)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[1] = fSaturation;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetLightness(float fLightness)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[2] = fLightness;
	HSLtoRGB(a3fHSL, m_a4fValues);
}


void VistaColor::SetHSVSaturation(float fSaturation)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[1] = fSaturation;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

void VistaColor::SetValue(float fValue)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[2] = fValue;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

//// Getters
int VistaColor::GetValues( EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			return (static_cast<int>(a3fValues[0] * 255.f) << 16)
				+ (static_cast<int>(a3fValues[1] * 255.f) << 8)
				+ static_cast<int>(a3fValues[2] * 255.f);
		}
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			return (static_cast<int>(a4fValues[0] * 255.f) << 24)
				+ (static_cast<int>(a4fValues[1] * 255.f) << 16)
				+ (static_cast<int>(a4fValues[2] * 255.f) << 8)
				+ static_cast<int>(a4fValues[3] * 255.f);
		}
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues(
	int aiValues[], EFormat eFormat /*= FORMAT_RGB*/) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			aiValues [0] = static_cast<int>(a3fValues[0] * 255.f);
			aiValues [1] = static_cast<int>(a3fValues[1] * 255.f);
			aiValues [2] = static_cast<int>(a3fValues[2] * 255.f);
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			aiValues [0] = static_cast<int>(a4fValues[0] * 255.f);
			aiValues [1] = static_cast<int>(a4fValues[1] * 255.f);
			aiValues [2] = static_cast<int>(a4fValues[2] * 255.f);
			aiValues [3] = static_cast<int>(a4fValues[3] * 255.f);
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues( 
	float afValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
		afValues[0] = m_a4fValues[0];
		afValues[1] = m_a4fValues[1];
		afValues[2] = m_a4fValues[2];
		break;
	case RGBA:
		afValues[0] = m_a4fValues[0];
		afValues[1] = m_a4fValues[1];
		afValues[2] = m_a4fValues[2];
		afValues[3] = m_a4fValues[3];
		break;
	case HSL:
		{
			float a3fValues[3];
			RGBtoHSL(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
		}
		break;
	case HSLA:
		{
			float a3fValues[3];
			RGBtoHSL(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
			afValues[3] = m_a4fValues[3];
		}
		break;
	case HSV:
		{
			float a3fValues[3];
			RGBtoHSV(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
		}
		break;
	case HSVA:
		{
			float a3fValues[3];
			RGBtoHSV(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
			afValues[3] = m_a4fValues[3];
		}
		break;
	case LAB:
		{
			float a3fValues[3];
			RGBtoLab(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
		}
		break;
	case LABA:
		{
			float a3fValues[3];
			RGBtoLab(m_a4fValues, a3fValues);
			afValues[0] = a3fValues[0];
			afValues[1] = a3fValues[1];
			afValues[2] = a3fValues[2];
			afValues[3] = m_a4fValues[3];
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::GetValues( 
	double adValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
	case HSV:
	case HSL:
	case LAB:
		{
			float a3fValues[3];
			GetValues(a3fValues, eFormat);
			adValues [0] = (double)a3fValues[0];
			adValues [1] = (double)a3fValues[1];
			adValues [2] = (double)a3fValues[2];
		}
		break;
	case RGBA:
	case HSVA:
	case HSLA:
	case LABA:
		{
			float a4fValues[4];
			GetValues(a4fValues, eFormat);
			adValues [0] = (double)a4fValues[0];
			adValues [1] = (double)a4fValues[1];
			adValues [2] = (double)a4fValues[2];
			adValues [3] = (double)a4fValues[3];
		}
		break;
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

float VistaColor::GetRed() const
{
	return m_a4fValues[0];
}

float VistaColor::GetGreen() const
{
	return m_a4fValues[1];
}

float VistaColor::GetBlue() const
{
	return m_a4fValues[2];
}

float VistaColor::GetAlpha() const
{
	return m_a4fValues[3];
}

float VistaColor::GetHue() const
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	return a3fHSL[0];
}

float VistaColor::GetHSLSaturation() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	float fL = (fMax + fMin) / 2.f; // Lightness

	float fS;

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		fS = 0.f;
	}
	else //Chromatic data...
	{
		if (fL < .5f)
			fS = fDelta / (fMax + fMin); // Saturation
		else
			fS = fDelta / (2.f - fMax - fMin); // Saturation
	}

	return fS;
}

float VistaColor::GetLightness() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB

	return (fMax + fMin) / 2.f;
}

float VistaColor::GetHSVSaturation() const
{
	// @todo: implement direct calculation of the saturation
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[1];
}

float VistaColor::GetValue() const
{
	// @todo: implement direct calculation of the value
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[2];
}

//// Operations

VistaColor VistaColor::Mix( const VistaColor& colEnd, 
	float fFraction /*= .f5*/, EFormat eFormat /* = VistaColor::RGBA */ ) const
{
	float fOwn = 1.0f - fFraction;
	switch (eFormat)
	{
	case RGBA:
		return VistaColor( 
			fOwn * m_a4fValues[0] + fFraction * colEnd.m_a4fValues[0],
			fOwn * m_a4fValues[1] + fFraction * colEnd.m_a4fValues[1],
			fOwn * m_a4fValues[2] + fFraction * colEnd.m_a4fValues[2],
			fOwn * m_a4fValues[3] + fFraction * colEnd.m_a4fValues[3],
			RGBA);
	case RGB:
		return VistaColor( 
			fOwn * m_a4fValues[0] + fFraction * colEnd.m_a4fValues[0],
			fOwn * m_a4fValues[1] + fFraction * colEnd.m_a4fValues[1],
			fOwn * m_a4fValues[2] + fFraction * colEnd.m_a4fValues[2],
			RGB);
	case HSVA:
		return VistaColor( 
			fOwn * GetHue() + fFraction * colEnd.GetHue(),
			fOwn * GetHSVSaturation() + fFraction * colEnd.GetHSVSaturation(),
			fOwn * GetValue() + fFraction * colEnd.GetValue(),
			fOwn * m_a4fValues[3] + fFraction * colEnd.m_a4fValues[3],
			HSVA);
	case HSV:
		return VistaColor( 
			fOwn * GetHue() + fFraction * colEnd.GetHue(),
			fOwn * GetHSVSaturation() + fFraction * colEnd.GetHSVSaturation(),
			fOwn * GetValue() + fFraction * colEnd.GetValue(),
			HSV);
	case HSLA:
		return VistaColor( 
			fOwn * GetHue() + fFraction * colEnd.GetHue(),
			fOwn * GetHSLSaturation() + fFraction * colEnd.GetHSLSaturation(),
			fOwn * GetLightness() + fFraction * colEnd.GetLightness(),
			fOwn * m_a4fValues[3] + fFraction * colEnd.m_a4fValues[3],
			HSLA);
	case HSL:
		return VistaColor( 
			fOwn * GetHue() + fFraction * colEnd.GetHue(),
			fOwn * GetHSLSaturation() + fFraction * colEnd.GetHSLSaturation(),
			fOwn * GetLightness() + fFraction * colEnd.GetLightness(),
			HSL);
	case LAB:
	{
		float a3Temp[3], a3Temp2[3];
		RGBtoLab(m_a4fValues, a3Temp);
		RGBtoLab(&colEnd[0], a3Temp2);
		float aColorMix[3] = {
			fOwn * a3Temp[0] + fFraction * a3Temp2[0],
			fOwn * a3Temp[1] + fFraction * a3Temp2[1],
			fOwn * a3Temp[2] + fFraction * a3Temp2[2]
		};
		VistaColor colRes;
		LabtoRGB(&aColorMix[0], &colRes[0]);
		colRes[3] = 1.f;
		return colRes;
	}
	case LABA:
	{
		float a3Temp[3], a3Temp2[3];
		RGBtoLab(m_a4fValues, a3Temp);
		RGBtoLab(&colEnd[0], a3Temp2);
		float aColorMix[3] = {
			fOwn * a3Temp[0] + fFraction * a3Temp2[0],
			fOwn * a3Temp[1] + fFraction * a3Temp2[1],
			fOwn * a3Temp[2] + fFraction * a3Temp2[2]
		};
		VistaColor colRes;
		LabtoRGB(&aColorMix[0], &colRes[0]);
		colRes[3] = fOwn * m_a4fValues[3] + fFraction * colEnd[3];
		return colRes;
	}
	default:
		VISTA_THROW("Unknown color mode", 1);
	}
}

void VistaColor::ClampValues()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_a4fValues[i] < 0.f) m_a4fValues[i] = 0.f;
		if (m_a4fValues[i] > 1.f) m_a4fValues[i] = 1.f;
	}
}

VistaColor& VistaColor::AddRGB(const VistaColor &oOther)
{
	m_a4fValues[0] += oOther.m_a4fValues[0];
	m_a4fValues[1] += oOther.m_a4fValues[1];
	m_a4fValues[2] += oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::SubtractRGB(const VistaColor &oOther)
{
	m_a4fValues[0] -= oOther.m_a4fValues[0];
	m_a4fValues[1] -= oOther.m_a4fValues[1];
	m_a4fValues[2] -= oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::MultiplyRGB(const float fScale)
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	return (*this);
}

VistaColor& VistaColor::DivideRGB(const float fScale)
{
	m_a4fValues[0] /= fScale;
	m_a4fValues[1] /= fScale;
	m_a4fValues[2] /= fScale;
	return (*this);
}

//// static methods

float VistaColor::HueToRGB(float f1, float f2, float fHue)
{
	if (fHue < 0.f)
		fHue += 1.f;
	else if (fHue > 1.f)
		fHue -= 1.f;

	float fReturn;

	if ((6.f * fHue) < 1.f)
		fReturn = f1 + (f2 - f1) * 6.f * fHue;
	else if ((2.f * fHue) < 1.f)
		fReturn = f2;
	else if ((3.f * fHue) < 2.f)
		fReturn = f1 + (f2 - f1) * ((2.f / 3.f) - fHue) * 6.f;
	else
		fReturn = f1;

	return fReturn;
}

void VistaColor::HSLtoRGB(const float a3fHSL[3], float a3fRGB[3])
{
	float fH, fS, fL;
	fH = a3fHSL[0];
	fS = a3fHSL[1];
	fL = a3fHSL[2];

	if( fS == 0.0f )
	{
		// saturation == 0 so use set rgb values to luminance value
		a3fRGB[0] = fL;
		a3fRGB[1] = fL;
		a3fRGB[2] = fL;
	}
	else
	{
		float f2;

		if (fL < .5f)
		{
			f2 = fL * (1.f + fS);
		}
		else
		{
			f2 = (fL + fS) - (fS * fL);
		}

		float f1 = 2.f * fL - f2;

		a3fRGB[0] = HueToRGB(f1, f2, fH + (1.f/3.f));
		a3fRGB[1] = HueToRGB(f1, f2, fH);
		a3fRGB[2] = HueToRGB(f1, f2, fH - (1.f/3.f));
	}
}

void VistaColor::RGBtoHSL(const float a3fRGB[3], float a3fHSL[3])
{
	float fR, fG, fB;
	fR = a3fRGB[0];
	fG = a3fRGB[1];
	fB = a3fRGB[2];

	float fMin = std::min(std::min(fR, fG), fB);    //Min. value of RGB
	float fMax = std::max(std::max(fR, fG), fB);    //Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	a3fHSL[2] = (fMax + fMin) / 2.f; // Luminance

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		a3fHSL[0] = 0.f;	// Hue
		a3fHSL[1] = 0.f;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (a3fHSL[2] < .5f)
			a3fHSL[1] = fDelta / (fMax + fMin); // Saturation
		else
			a3fHSL[1] = fDelta / (2.f - fMax - fMin); // Saturation

		float deltaR = (((fMax - fR) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaG = (((fMax - fG) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaB = (((fMax - fB) / 6.f) + (fDelta / 2.f)) / fDelta;

		if (fR == fMax )
			a3fHSL[0] = deltaB - deltaG; // Hue
		else if (fG == fMax)
			a3fHSL[0] = (1.f / 3.f) + deltaR - deltaB; // Hue
		else if (fB == fMax)
			a3fHSL[0] = (2.f / 3.f) + deltaG - deltaR; // Hue

		if (a3fHSL[0] < 0.f)
			a3fHSL[0] += 1.f; // Hue
		else if (a3fHSL[0] > 1.f)
			a3fHSL[0] -= 1.f; // Hue
	}
}

void VistaColor::HSLtoHSV(const float a3fHSL[3], float a3fHSV[3])
{
	a3fHSV[0] = a3fHSL[0];
	float fL = a3fHSL[2] * 2.f;
	float fS = a3fHSL[1] * ((fL <= 1.f) ? fL : 2.f - fL);
	a3fHSV[2] = (fL + fS) / 2.f;
	if (fL + fS > 0.f)
	{
		a3fHSV[1] = (2.f * fS) / (fL + fS);
	}
	else
	{
		a3fHSV[1] = 0.f;
	}
}

void VistaColor::HSVtoHSL(const float a3fHSV[3], float a3fHSL[3])
{
	a3fHSL[0] = a3fHSV[0];
	float fL = (2.f - a3fHSV[1]) * a3fHSV[2];
	a3fHSL[1] = a3fHSV[1] * a3fHSV[2];
	if (fL == 2.f)
	{
		a3fHSL[1] = 0.f;
	}
	else if (fL > 0.f)
	{
		a3fHSL[1] /= (fL <= 1.f) ? fL : (2.f - fL);
	}
	else
	{
		a3fHSL[1] = 0.f;
	}
	a3fHSL[2] = fL / 2.f;
}

void VistaColor::HSVtoRGB(const float a3fHSV[3], float a3fRGB[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	HSVtoHSL(a3fHSV, a3fHSL);
	HSLtoRGB(a3fHSL, a3fRGB);
}

void VistaColor::RGBtoHSV(const float a3fRGB[3], float a3fHSV[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	RGBtoHSL(a3fRGB, a3fHSL);
	HSLtoHSV(a3fHSL, a3fHSV);
}

void VistaColor::LabtoRGB(const float a3fLAB[3], float a3fRGB[3])
{
	// RGB -> XYZ
	float Y = a3fLAB[0] * .0086207f + 0.1379310f;
	float X = a3fLAB[1] * .002f + Y;
	float Z = a3fLAB[2] * -0.005f + Y;

	X = X > 0.2068966f ? std::pow(X, 3.f) : X * .1284185f - .0177129f;
	Y = a3fLAB[0] > 8.f ? std::pow(Y, 3.f) : a3fLAB[0] * .0011070f;
	Z = Z > 0.2068966f ? std::pow(Z, 3.f) : Z * .1284185f - .0177129f;

	// normalized XYZ -> linear sRGB (in 0...1)
	a3fRGB[0] =  3.0803420f * X - 1.5373990f * Y - 0.5429430f * Z;
	a3fRGB[1] = -0.9211784f * X + 1.8759300f * Y + 0.0452484f * Z;
	a3fRGB[2] =  0.0528813f * X - 0.2040112f * Y + 1.1511299f * Z;

	// linear sRGB -> gamma-compressed sRGB (in 0...1)
	a3fRGB[0] = a3fRGB[0] > .0031308f ? std::pow(a3fRGB[0], .4166667f) * 1.055f - .055f : a3fRGB[0] * 12.92f;
	a3fRGB[1] = a3fRGB[1] > .0031308f ? std::pow(a3fRGB[1], .4166667f) * 1.055f - .055f : a3fRGB[1] * 12.92f;
	a3fRGB[2] = a3fRGB[2] > .0031308f ? std::pow(a3fRGB[2], .4166667f) * 1.055f - .055f : a3fRGB[2] * 12.92f;
}

void VistaColor::RGBtoLab(const float a3fRGB[3], float a3fLAB[3])
{
	float r = a3fRGB[0] > 0.04045f ? std::pow((a3fRGB[0] + 0.055f) / 1.055f, 2.4f) : a3fRGB[0] / 12.92f;
	float g = a3fRGB[1] > 0.04045f ? std::pow((a3fRGB[1] + 0.055f) / 1.055f, 2.4f) : a3fRGB[1] / 12.92f;
	float b = a3fRGB[2] > 0.04045f ? std::pow((a3fRGB[2] + 0.055f) / 1.055f, 2.4f) : a3fRGB[2] / 12.92f;

	//Observer. = 2°, Illuminant = D65
	float x = r * .4338954f + g * .3762424f + b * .1898622f;
	float y = r * .2126729f + g * .7151522f + b * .0721750f;
	float z = r * .0177524f + g * .1094656f + b * .8727819f;

	x = x > 0.008856f ? std::pow(x, .33333333f) : (7.787037f * x) + .137931f;
	y = y > 0.008856f ? std::pow(y, .33333333f) : (7.787037f * y) + .137931f;
	z = z > 0.008856f ? std::pow(z, .33333333f) : (7.787037f * z) + .137931f;

	a3fLAB[0] = (116.f * y) - 16.f;
	a3fLAB[1] = 500.f * (x - y);
	a3fLAB[2] = 200.f * (y - z);
}

bool VistaColor::GetColorFromName( const std::string& sColorName, VistaColor& oResult )
{
	std::string sClearName = sColorName;
	std::transform( sClearName.begin(), sClearName.end(), sClearName.begin(), ::toupper );
	std::replace( sClearName.begin(), sClearName.end(), ' ', '_' );

	if( sClearName == "AQUAMARINE" )
		oResult = AQUAMARINE;
	else if( sClearName == "BLACK" )
		oResult = BLACK;
	else if( sClearName == "BLUE" )
		oResult = BLUE;
	else if( sClearName == "BRICK_RED" )
		oResult = BRICK_RED;
	else if( sClearName == "BROWN" )
		oResult = BROWN;
	else if( sClearName == "CORAL" )
		oResult = CORAL;
	else if( sClearName == "CYAN" )
		oResult = CYAN;
	else if( sClearName == "FOREST_GREEN" )
		oResult = FOREST_GREEN;
	else if( sClearName == "GOLD" )
		oResult = GOLD;
	else if( sClearName == "GRAY" )
		oResult = GRAY;
	else if( sClearName == "GREEN" )
		oResult = GREEN;
	else if( sClearName == "GREEN_YELLOW" )
		oResult = GREEN_YELLOW;
	else if( sClearName == "INDIGO" )
		oResult = INDIGO;
	else if( sClearName == "KHAKI" )
		oResult = KHAKI;
	else if( sClearName == "LAWN_GREEN" )
		oResult = LAWN_GREEN;
	else if( sClearName == "LIME" )
		oResult = LIME;
	else if( sClearName == "MAGENTA" )
		oResult = MAGENTA;
	else if( sClearName == "MAROON" )
		oResult = MAROON;
	else if( sClearName == "NAVY" )
		oResult = NAVY;
	else if( sClearName == "OLIVE" )
		oResult = OLIVE;
	else if( sClearName == "ORANGE" )
		oResult = ORANGE;
	else if( sClearName == "ORANGE_RED" )
		oResult = ORANGE_RED;
	else if( sClearName == "PINK" )
		oResult = PINK;
	else if( sClearName == "PURPLE" )
		oResult = PURPLE;
	else if( sClearName == "SKY_BLUE" )
		oResult = SKY_BLUE;
	else if( sClearName == "RED" )
		oResult = RED;
	else if( sClearName == "ROYAL_BLUE" )
		oResult = ROYAL_BLUE;
	else if( sClearName == "SALMON" )
		oResult = SALMON;
	else if( sClearName == "VIOLET" )
		oResult = VIOLET;
	else if( sClearName == "WHITE" )
		oResult = WHITE;
	else if( sClearName == "YELLOW" )
		oResult = YELLOW;
	else if( sClearName == "LIGHT_BLUE" )
		oResult = LIGHT_BLUE;
	else if( sClearName == "LIGHT_GRAY" )
		oResult = LIGHT_GRAY;
	else if( sClearName == "LIGHT_GREEN" )
		oResult = LIGHT_GREEN;
	else if( sClearName == "LIGHT_ORANGE" )
		oResult = LIGHT_ORANGE;
	else if( sClearName == "LIGHT_PINK" )
		oResult = LIGHT_PINK;
	else if( sClearName == "LIGHT_SKY_BLUE" )
		oResult = LIGHT_SKY_BLUE;
	else if( sClearName == "LIGHT_STEEL_BLUE" )
		oResult = LIGHT_STEEL_BLUE;
	else if( sClearName == "DARK_BLUE" )
		oResult = DARK_BLUE;
	else if( sClearName == "DARK_CYAN" )
		oResult = DARK_CYAN;
	else if( sClearName == "DARK_GREEN" )
		oResult = DARK_GREEN;
	else if( sClearName == "DARK_MAGENTA" )
		oResult = DARK_MAGENTA;
	else if( sClearName == "DARK_RED" )
		oResult = DARK_RED;
	else if( sClearName == "DARK_OLIVE" )
		oResult = DARK_OLIVE;
	else if( sClearName == "DARK_ORANGE" )
		oResult = DARK_ORANGE;
	else if( sClearName == "DARK_ORCHID" )
		oResult = DARK_ORCHID;
	else if( sClearName == "DARK_SALMON" )
		oResult = DARK_SALMON;
	else if( sClearName == "DARK_TURQUOISE" )
		oResult = DARK_TURQUOISE;
	else if( sClearName == "VISTA_BLUE" )
		oResult = VISTA_BLUE;
	else if( sClearName == "RWTH_BLUE" )
		oResult = RWTH_BLUE;
	else if( sClearName == "RWTH_BLACK" )
		oResult = RWTH_BLACK;
	else if( sClearName == "RWTH_MAGENTA" )
		oResult = RWTH_MAGENTA;
	else if( sClearName == "RWTH_YELLOW" )
		oResult = RWTH_YELLOW;
	else if( sClearName == "RWTH_PETROL" )
		oResult = RWTH_PETROL;
	else if( sClearName == "RWTH_TURQUOISE" )
		oResult = RWTH_TURQUOISE;
	else if( sClearName == "RWTH_GREEN" )
		oResult = RWTH_GREEN;
	else if( sClearName == "RWTH_MAY_GREEN" )
		oResult = RWTH_MAY_GREEN;
	else if( sClearName == "RWTH_ORANGE" )
		oResult = RWTH_ORANGE;
	else if( sClearName == "RWTH_RED" )
		oResult = RWTH_RED;
	else if( sClearName == "RWTH_BORDEAUX" )
		oResult = RWTH_BORDEAUX;
	else if( sClearName == "RWTH_VIOLET" )
		oResult = RWTH_VIOLET;
	else if( sClearName == "RWTH_PURPLE" )
		oResult = RWTH_PURPLE;
	else
		return false;
	return true;

}
