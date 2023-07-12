#include "VistaEaseCurve.h"

VistaEaseCurve::VistaEaseCurve()
    : m_eType(LINEAR) {
}

VistaEaseCurve::~VistaEaseCurve() {
}

double VistaEaseCurve::GetValue(double t) const {
  double dValue = -1;
  switch (m_eType) {
  case LINEAR:
    dValue = t;
    break;
  case QUADRATIC_IN:
    dValue = GetInValue(t, GetQuadraticValue);
    break;
  case QUADRATIC_OUT:
    dValue = GetOutValue(t, GetQuadraticValue);
    break;
  case QUADRATIC_IN_OUT:
    dValue = GetInOutValue(t, GetQuadraticValue);
    break;
  case QUADRATIC_OUT_IN:
    dValue = GetOutInValue(t, GetQuadraticValue);
    break;
  case CUBIC_IN:
    dValue = GetInValue(t, GetCubicValue);
    break;
  case CUBIC_OUT:
    dValue = GetOutValue(t, GetCubicValue);
    break;
  case CUBIC_IN_OUT:
    dValue = GetInOutValue(t, GetCubicValue);
    break;
  case CUBIC_OUT_IN:
    dValue = GetOutInValue(t, GetCubicValue);
    break;
  case SIN_IN:
    dValue = GetInValue(t, GetSinValue);
    break;
  case SIN_OUT:
    dValue = GetOutValue(t, GetSinValue);
    break;
  case SIN_IN_OUT:
    dValue = GetInOutValue(t, GetSinValue);
    break;
  case SIN_OUT_IN:
    dValue = GetOutInValue(t, GetSinValue);
    break;
  case CIRC_IN:
    dValue = GetInValue(t, GetCircValue);
    break;
  case CIRC_OUT:
    dValue = GetOutValue(t, GetCircValue);
    break;
  case CIRC_IN_OUT:
    dValue = GetInOutValue(t, GetCircValue);
    break;
  case CIRC_OUT_IN:
    dValue = GetOutInValue(t, GetCircValue);
    break;
  }
  return dValue;
}

void VistaEaseCurve::SetType(eEaseCurveType val) {
  m_eType = val;
}

VistaEaseCurve::eEaseCurveType VistaEaseCurve::GetType() const {
  return m_eType;
}

double VistaEaseCurve::GetInValue(double t, ValueFn pValueFn) {
  return (*pValueFn)(t);
}

double VistaEaseCurve::GetOutValue(double t, ValueFn pValueFn) {
  return 1.0 - (*pValueFn)(1.0 - t);
}

double VistaEaseCurve::GetInOutValue(double t, ValueFn pValueFn) {
  if (t < 0.5) {
    return 0.5 * (*pValueFn)(2.0 * t);
  } else {
    return 0.5 + 0.5 * (1.0 - (*pValueFn)(1.0 - (t - 0.5) * 2.0));
  }
}

double VistaEaseCurve::GetOutInValue(double t, ValueFn pValueFn) {
  if (t < 0.5) {
    return 0.5 * (1.0 - (*pValueFn)(1.0 - 2.0 * t));
  } else {
    return 0.5 + 0.5 * ((*pValueFn)(2.0 * (t - 0.5)));
  }
}

double VistaEaseCurve::GetQuadraticValue(double t) {
  return t * t;
}

double VistaEaseCurve::GetCubicValue(double t) {
  return t * t * t;
}

double VistaEaseCurve::GetSinValue(double t) {
  return 1.0 + std::sin(t * (double)Vista::Pi * 0.5 - (double)Vista::Pi * 0.5);
}

double VistaEaseCurve::GetCircValue(double t) {
  return 1.0 - sqrt(1.0 - t * t);
}
