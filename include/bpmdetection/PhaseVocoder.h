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

#ifndef QM_DSP_PHASEVOCODER_H
#define QM_DSP_PHASEVOCODER_H

class FFTReal;

class PhaseVocoder  
{
public:
    PhaseVocoder(int size, int hop);
    virtual ~PhaseVocoder();

    /**
     * Given one frame of time-domain samples, FFT and return the
     * magnitudes, instantaneous phases, and unwrapped phases.
     *
     * src must have size values (where size is the frame size value
     * as passed to the PhaseVocoder constructor), and should have
     * been windowed as necessary by the caller (but not fft-shifted).
     *
     * mag, phase, and unwrapped must each be non-NULL and point to
     * enough space for size/2 + 1 values. The redundant conjugate
     * half of the output is not returned.
     */
    void processTimeDomain(const float *src,
                           float *mag, float *phase, float *unwrapped);

    /**
     * Given one frame of frequency-domain samples, return the
     * magnitudes, instantaneous phases, and unwrapped phases.
     *
     * reals and imags must each contain size/2+1 values (where size
     * is the frame size value as passed to the PhaseVocoder
     * constructor).
     *
     * mag, phase, and unwrapped must each be non-NULL and point to
     * enough space for size/2+1 values.
     */
    void processFrequencyDomain(const float *reals, const float *imags,
                                float *mag, float *phase, float *unwrapped);

    /**
     * Reset the stored phases to zero. Note that this may be
     * necessary occasionally (depending on the application) to avoid
     * loss of floating-point precision in the accumulated unwrapped
     * phase values as they grow.
     */
    void reset();

protected:
    void FFTShift(float *src);
    void getMagnitudes(float *mag);
    void getPhases(float *theta);
    void unwrapPhases(float *theta, float *unwrapped);

    int m_n;
    int m_hop;
    FFTReal *m_fft;
    float *m_time;
    float *m_imag;
    float *m_real;
    float *m_phase;
    float *m_unwrapped;
};

#endif
