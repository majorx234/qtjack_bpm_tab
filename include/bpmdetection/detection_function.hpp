/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    QM DSP Library

    Centre for Digital Music, Queen Mary, University of London.
    This file 2005-2006 Christian Landone.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef QM_DSP_DETECTIONFUNCTION_H
#define QM_DSP_DETECTIONFUNCTION_H

#include "maths/MathUtilities.h"
#include "maths/MathAliases.h"
#include "bpmdetection/PhaseVocoder.h"
#include "bpmdetection/Window.h"

#define DF_HFC (1)
#define DF_SPECDIFF (2)
#define DF_PHASEDEV (3)
#define DF_COMPLEXSD (4)
#define DF_BROADBAND (5)

struct DFConfig{
    int stepSize; // DF step in samples
    int frameLength; // DF analysis window - usually 2*step. Must be even!
    int DFType; // type of detection function ( see defines )
    float dbRise; // only used for broadband df (and required for it)
    bool adaptiveWhitening; // perform adaptive whitening
    float whiteningRelaxCoeff; // if < 0, a sensible default will be used
    float whiteningFloor; // if < 0, a sensible default will be used
};

class DetectionFunction
{
public:
    float* getSpectrumMagnitude();
    DetectionFunction( DFConfig config );
    virtual ~DetectionFunction();

    /**
     * Process a single time-domain frame of audio, provided as
     * frameLength samples.
     */
    float processTimeDomain(const float* samples);

    /**
     * Process a single frequency-domain frame, provided as
     * frameLength/2+1 real and imaginary component values.
     */
    float processFrequencyDomain(const float* reals, const float* imags);

private:
    void whiten();
    float runDF();

    float HFC(int length, float* src);
    float specDiff(int length, float* src);
    float phaseDev(int length, float *srcPhase);
    float complexSD(int length, float *srcMagnitude, float *srcPhase);
    float broadband(int length, float *srcMagnitude);

private:
    void initialise( DFConfig Config );
    void deInitialise();

    int m_DFType;
    int m_dataLength;
    int m_halfLength;
    int m_stepSize;
    float m_dbRise;
    bool m_whiten;
    float m_whitenRelaxCoeff;
    float m_whitenFloor;

    float* m_magHistory;
    float* m_phaseHistory;
    float* m_phaseHistoryOld;
    float* m_magPeaks;

    float* m_windowed; // Array for windowed analysis frame
    float* m_magnitude; // Magnitude of analysis frame ( frequency domain )
    float* m_thetaAngle;// Phase of analysis frame ( frequency domain )
    float* m_unwrapped; // Unwrapped phase of analysis frame

    Window<float> *m_window;
    PhaseVocoder* m_phaseVoc;   // Phase Vocoder
};

#endif
