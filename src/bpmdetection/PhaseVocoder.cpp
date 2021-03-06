/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP Library

    Centre for Digital Music, Queen Mary, University of London.
    This file 2005-2006 Christian Landone, copyright 2013 QMUL.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "bpmdetection/PhaseVocoder.h"
#include "maths/FFT.h"
#include "maths/MathUtilities.h"
#include <math.h>

#include <cassert>

#include <iostream>
using std::cerr;
using std::endl;

PhaseVocoder::PhaseVocoder(int n, int hop) :
    m_n(n),
    m_hop(hop)
{
    m_fft = new FFTReal(m_n);
    m_time = new float[m_n];
    m_real = new float[m_n];
    m_imag = new float[m_n];
    m_phase = new float[m_n/2 + 1];
    m_unwrapped = new float[m_n/2 + 1];

    for (int i = 0; i < m_n/2 + 1; ++i) {
        m_phase[i] = 0.0;
        m_unwrapped[i] = 0.0;
    }

    reset();
}

PhaseVocoder::~PhaseVocoder()
{
    delete[] m_unwrapped;
    delete[] m_phase;
    delete[] m_real;
    delete[] m_imag;
    delete[] m_time;
    delete m_fft;
}

void PhaseVocoder::FFTShift(float *src)
{
    const int hs = m_n/2;
    for (int i = 0; i < hs; ++i) {
        float tmp = src[i];
        src[i] = src[i + hs];
        src[i + hs] = tmp;
    }
}

void PhaseVocoder::processTimeDomain(const float *src,
                                     float *mag, float *theta,
                                     float *unwrapped)
{
    for (int i = 0; i < m_n; ++i) {
        m_time[i] = src[i];
    }
    FFTShift(m_time);
    m_fft->forward(m_time, m_real, m_imag);
    getMagnitudes(mag);
    getPhases(theta);
    unwrapPhases(theta, unwrapped);
}

void PhaseVocoder::processFrequencyDomain(const float *reals, 
                                          const float *imags,
                                          float *mag, float *theta,
                                          float *unwrapped)
{
    for (int i = 0; i < m_n/2 + 1; ++i) {
        m_real[i] = reals[i];
        m_imag[i] = imags[i];
    }
    getMagnitudes(mag);
    getPhases(theta);
    unwrapPhases(theta, unwrapped);
}

void PhaseVocoder::reset()
{
    for (int i = 0; i < m_n/2 + 1; ++i) {
        // m_phase stores the "previous" phase, so set to one step
        // behind so that a signal with initial phase at zero matches
        // the expected values. This is completely unnecessary for any
        // analytical purpose, it's just tidier.
        float omega = (2 * M_PI * m_hop * i) / m_n;
        m_phase[i] = -omega;
        m_unwrapped[i] = -omega;
    }
}

void PhaseVocoder::getMagnitudes(float *mag)
{       
    for (int i = 0; i < m_n/2 + 1; i++) {
        mag[i] = sqrt(m_real[i] * m_real[i] + m_imag[i] * m_imag[i]);
    }
}

void PhaseVocoder::getPhases(float *theta)
{
    for (int i = 0; i < m_n/2 + 1; i++) {
        theta[i] = atan2(m_imag[i], m_real[i]);
    }   
}

void PhaseVocoder::unwrapPhases(float *theta, float *unwrapped)
{
    for (int i = 0; i < m_n/2 + 1; ++i) {

        float omega = (2 * M_PI * m_hop * i) / m_n;
        float expected = m_phase[i] + omega;
        float error = MathUtilities::princarg(theta[i] - expected);

        unwrapped[i] = m_unwrapped[i] + omega + error;

        m_phase[i] = theta[i];
        m_unwrapped[i] = unwrapped[i];
    }
}

