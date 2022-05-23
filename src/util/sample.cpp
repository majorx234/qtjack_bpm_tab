#include "util/sample.h"

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "util/math.h"

#ifdef __WINDOWS__
#include <QtGlobal>
typedef qint64 int64_t;
typedef qint32 int32_t;
#endif
typedef std::ptrdiff_t SINT;
// LOOP VECTORIZED below marks the loops that are processed with the 128 bit SSE
// registers as tested with gcc 7.5 and the -ftree-vectorize -fopt-info-vec-optimized flags on
// an Intel i5 CPU. When changing, be careful to not disturb the vectorization.
// https://gcc.gnu.org/projects/tree-ssa/vectorization.html
// This also utilizes AVX registers when compiled for a recent 64-bit CPU
// using scons optimize=native.
// "SINT i" is the preferred loop index type that should allow vectorization in
// general. Unfortunately there are exceptions where "int i" is required for some reasons.

namespace {

#ifdef __AVX__
constexpr size_t kAlignment = 32;
#else
constexpr size_t kAlignment = 16;
#endif

// TODO() Check if uintptr_t is available on all our build targets and use that
// instead of size_t, we can remove the sizeof(size_t) check than
constexpr bool useAlignedAlloc() {
    // This will work on all targets and compilers.
    // It will return true bot 32 bit builds and false for 64 bit builds
    return alignof(max_align_t) < kAlignment &&
            sizeof(float*) == sizeof(size_t);
}

} // anonymous namespace

// static
float* SampleUtil::alloc(SINT size) {
    // To speed up vectorization we align our sample buffers to 16-byte (128
    // bit) boundaries on SSE builds and 32-byte (256 bit) on AVX builds so
    // that vectorized loops doesn't have to do a serial ramp-up before going
    // parallel.
    //
    // Pointers returned by malloc are aligned for the largest scalar type. On
    // most platforms the largest scalar type is long double (16 bytes).
    // However, on MSVC x86 long double is 8 bytes.
    // This can be tested via alignof(std::max_align_t)
    if (useAlignedAlloc()) {
#if defined(_MSC_VER)
        // On MSVC, we use _aligned_malloc to handle aligning pointers to 16-byte
        // boundaries.
        return static_cast<float*>(
                _aligned_malloc(sizeof(float) * size, kAlignment));
#elif defined(_GLIBCXX_HAVE_ALIGNED_ALLOC)
        std::size_t alloc_size = sizeof(float) * size;
        // The size (in bytes) must be an integral multiple of kAlignment
        std::size_t aligned_alloc_size = alloc_size;
        if (alloc_size % kAlignment != 0) {
            aligned_alloc_size += (kAlignment - alloc_size % kAlignment);
        }
        DEBUG_ASSERT(aligned_alloc_size % kAlignment == 0);
        return static_cast<float*>(std::aligned_alloc(kAlignment, aligned_alloc_size));
#else
        // On other platforms that might not support std::aligned_alloc
        // yet but where long double is 8 bytes this code allocates 16 additional
        // slack bytes so we can adjust the pointer we return to the caller to be
        // 16-byte aligned. We record a pointer to the true start of the buffer
        // in the slack space as well so that we can free it correctly.
        const size_t alignment = kAlignment;
        const size_t unaligned_size = sizeof(float) * size + alignment;
        void* pUnaligned = std::malloc(unaligned_size);
        if (pUnaligned == NULL) {
            return NULL;
        }
        // Shift
        void* pAligned = (void*)(((size_t)pUnaligned & ~(alignment - 1)) + alignment);
        // Store pointer to the original buffer in the slack space before the
        // shifted pointer.
        *((void**)(pAligned) - 1) = pUnaligned;
        return static_cast<float*>(pAligned);
#endif
    } else {
        // Our platform already produces aligned pointers (or is an exotic target)
        return static_cast<float*>(std::malloc(sizeof(float) * size));
    }
}

void SampleUtil::free(float* pBuffer) {
    // See SampleUtil::alloc() for details
    if (useAlignedAlloc()) {
#if defined(_MSC_VER)
        _aligned_free(pBuffer);
#elif defined(_GLIBCXX_HAVE_ALIGNED_ALLOC)
        std::free(pBuffer);
#else
        // Pointer to the original memory is stored before pBuffer
        if (!pBuffer) {
            return;
        }
        std::free(*((void**)((void*)pBuffer) - 1));
#endif
    } else {
        std::free(pBuffer);
    }
}

// static
void SampleUtil::applyGain(float* pBuffer, float gain,
        SINT numSamples) {
    if (gain == 1.0f) {
        return;
    }
    if (gain == 0.0f) {
        clear(pBuffer, numSamples);
        return;
    }

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pBuffer[i] *= gain;
    }
}

// static
void SampleUtil::applyRampingGain(float* pBuffer, float old_gain,
        float new_gain, SINT numSamples) {
    if (old_gain == 1.0f && new_gain == 1.0f) {
        return;
    }
    if (old_gain == 0.0f && new_gain == 0.0f) {
        clear(pBuffer, numSamples);
        return;
    }

    const float gain_delta = (new_gain - old_gain)
            / float(numSamples / 2);
    if (gain_delta != 0) {
        const float start_gain = old_gain + gain_delta;
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples / 2; ++i) {
            const float gain = start_gain + gain_delta * i;
            // a loop counter i += 2 prevents vectorizing.
            pBuffer[i * 2] *= gain;
            pBuffer[i * 2 + 1] *= gain;
        }
    } else {
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples; ++i) {
            pBuffer[i] *= old_gain;
        }
    }
}

// static
void SampleUtil::applyAlternatingGain(float* pBuffer, float gain1,
        float gain2, SINT numSamples) {
    // This handles gain1 == 1.0f && gain2 == 1.0f as well.
    if (gain1 == gain2) {
        applyGain(pBuffer, gain1, numSamples);
        return;
    }

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples / 2; ++i) {
        pBuffer[i * 2] *= gain1;
        pBuffer[i * 2 + 1] *= gain2;
    }
}


void SampleUtil::applyRampingAlternatingGain(float* pBuffer,
        float gain1, float gain2,
        float gain1Old, float gain2Old, SINT numSamples) {
    if (gain1 == gain1Old && gain2 == gain2Old){
        applyAlternatingGain(pBuffer, gain1, gain2, numSamples);
        return;
    }

    const float gain1Delta = (gain1 - gain1Old)
            / float(numSamples / 2);
    if (gain1Delta != 0) {
        const float start_gain = gain1Old + gain1Delta;
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples / 2; ++i) {
            const float gain = start_gain + gain1Delta * i;
            pBuffer[i * 2] *= gain;
        }
    } else {
        // not vectorized: vectorization not profitable.
        for (int i = 0; i < numSamples / 2; ++i) {
            pBuffer[i * 2] *= gain1Old;
        }
    }

    const float gain2Delta = (gain2 - gain2Old)
            / float(numSamples / 2);
    if (gain2Delta != 0) {
        const float start_gain = gain2Old + gain2Delta;
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples / 2; ++i) {
            const float gain = start_gain + gain2Delta * i;
            pBuffer[i * 2 + 1] *= gain;
        }
    } else {
        // not vectorized: vectorization not profitable.
        for (int i = 0; i < numSamples / 2; ++i) {
            pBuffer[i * 2 + 1] *= gain2Old;
        }
    }
}

// static
void SampleUtil::add(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        SINT numSamples) {
    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pDest[i] += pSrc[i];
    }
}

// static
void SampleUtil::addWithGain(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        float gain, SINT numSamples) {
    if (gain == 0.0f) {
        return;
    }

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pDest[i] += pSrc[i] * gain;
    }
}

void SampleUtil::addWithRampingGain(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        float old_gain, float new_gain,
        SINT numSamples) {
    if (old_gain == 0.0f && new_gain == 0.0f) {
        return;
    }

    const float gain_delta = (new_gain - old_gain)
            / float(numSamples / 2);
    if (gain_delta != 0) {
        const float start_gain = old_gain + gain_delta;
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples / 2; ++i) {
            const float gain = start_gain + gain_delta * i;
            pDest[i * 2] += pSrc[i * 2] * gain;
            pDest[i * 2 + 1] += pSrc[i * 2 + 1] * gain;
        }
    } else {
        // note: LOOP VECTORIZED.
        for (int i = 0; i < numSamples; ++i) {
            pDest[i] += pSrc[i] * old_gain;
        }
    }
}

// static
void SampleUtil::add2WithGain(float* __restrict__ pDest,
        const float* __restrict__ pSrc1, float gain1,
        const float* __restrict__ pSrc2, float gain2,
        SINT numSamples) {
    if (gain1 == 0.0f) {
        addWithGain(pDest, pSrc2, gain2, numSamples);
        return;
    } else if (gain2 == 0.0f) {
        addWithGain(pDest, pSrc1, gain1, numSamples);
        return;
    }

    // note: LOOP VECTORIZED.
    for (int i = 0; i < numSamples; ++i) {
        pDest[i] += pSrc1[i] * gain1 + pSrc2[i] * gain2;
    }
}

// static
void SampleUtil::add3WithGain(float* pDest,
        const float* __restrict__ pSrc1, float gain1,
        const float* __restrict__ pSrc2, float gain2,
        const float* __restrict__ pSrc3, float gain3,
        SINT numSamples) {
    if (gain1 == 0.0f) {
        add2WithGain(pDest, pSrc2, gain2, pSrc3, gain3, numSamples);
        return;
    } else if (gain2 == 0.0f) {
        add2WithGain(pDest, pSrc1, gain1, pSrc3, gain3, numSamples);
        return;
    } else if (gain3 == 0.0f) {
        add2WithGain(pDest, pSrc1, gain1, pSrc2, gain2, numSamples);
        return;
    }

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pDest[i] += pSrc1[i] * gain1 + pSrc2[i] * gain2 + pSrc3[i] * gain3;
    }
}

// static
void SampleUtil::copyWithGain(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        float gain, SINT numSamples) {
    if (gain == 1.0f) {
        copy(pDest, pSrc, numSamples);
        return;
    }
    if (gain == 0.0f) {
        clear(pDest, numSamples);
        return;
    }

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pDest[i] = pSrc[i] * gain;
    }

    // OR! need to test which fares better
    // copy(pDest, pSrc, iNumSamples);
    // applyGain(pDest, gain);
}

// static
void SampleUtil::copyWithRampingGain(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        float old_gain,
        float new_gain,
        SINT numSamples) {
    if (old_gain == 1.0f && new_gain == 1.0f) {
        copy(pDest, pSrc, numSamples);
        return;
    }
    if (old_gain == 0.0f && new_gain == 0.0f) {
        clear(pDest, numSamples);
        return;
    }

    const float gain_delta = (new_gain - old_gain)
            / float(numSamples / 2);
    if (gain_delta != 0) {
        const float start_gain = old_gain + gain_delta;
        // note: LOOP VECTORIZED only with "int i" (not SINT i)
        for (int i = 0; i < numSamples / 2; ++i) {
            const float gain = start_gain + gain_delta * i;
            pDest[i * 2] = pSrc[i * 2] * gain;
            pDest[i * 2 + 1] = pSrc[i * 2 + 1] * gain;
        }
    } else {
        // note: LOOP VECTORIZED.
        for (SINT i = 0; i < numSamples; ++i) {
            pDest[i] = pSrc[i] * old_gain;
        }
    }

    // OR! need to test which fares better
    // copy(pDest, pSrc, iNumSamples);
    // applyRampingGain(pDest, gain);
}

// static
void SampleUtil::convertS16ToFloat32(float* __restrict__ pDest,
        const short int* __restrict__ pSrc, SINT numSamples) {
    // SAMPLE_MIN = -32768 is a valid low sample, whereas SAMPLE_MAX = 32767
    // is the highest valid sample. Note that this means that although some
    // sample values convert to -1.0, none will convert to +1.0.
    DEBUG_ASSERT(-SHRT_MIN >= SHRT_MAX);
    const float kConversionFactor = SHRT_MIN * -1.0f;
    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples; ++i) {
        pDest[i] = float(pSrc[i]) / kConversionFactor;
    }
}

//static
void SampleUtil::convertFloat32ToS16(short int* pDest, const float* pSrc,
        SINT numSamples) {
    // We use here -SHRT_MIN for a perfect round trip with convertS16ToFloat32
    // +1.0 is clamped to 32767 (0.99996942)
    DEBUG_ASSERT(-SHRT_MIN >= SHRT_MAX);
    const float kConversionFactor = SHRT_MIN * -1.0f;
    // note: LOOP VECTORIZED only with "int i" (not SINT i)
    for (int i = 0; i < numSamples; ++i) {
        pDest[i] = static_cast<short int>(math_clamp(pSrc[i] * kConversionFactor,
                static_cast<float>(SHRT_MIN),
                static_cast<float>(SHRT_MAX)));
    }
}

// static
unsigned long SampleUtil::sumAbsPerChannel(float* pfAbsL,
        float* pfAbsR, const float* pBuffer, SINT numSamples) {
    float fAbsL = 0.0f;
    float fAbsR = 0.0f;
    float clippedL = 0;
    float clippedR = 0;

    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numSamples / 2; ++i) {
        float absl = fabs(pBuffer[i * 2]);
        fAbsL += absl;
        clippedL += absl > 1.0f ? 1 : 0;
        float absr = fabs(pBuffer[i * 2 + 1]);
        fAbsR += absr;
        // Replacing the code with a bool clipped will prevent vetorizing
        clippedR += absr > 1.0f ? 1 : 0;
    }

    *pfAbsL = fAbsL;
    *pfAbsR = fAbsR;
    unsigned long clipping = SampleUtil::NO_CLIPPING;
    if (clippedL > 0) {
        clipping |= SampleUtil::CLIPPING_LEFT;
    }
    if (clippedR > 0) {
        clipping |= SampleUtil::CLIPPING_RIGHT;
    }
    return clipping;
}

// static
void SampleUtil::copyClampBuffer(float* __restrict__ pDest,
        const float* __restrict__ pSrc, SINT iNumSamples) {
    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < iNumSamples; ++i) {
        pDest[i] = clampSample(pSrc[i]);
    }
}

// static
void SampleUtil::interleaveBuffer(float* __restrict__ pDest,
        const float* __restrict__ pSrc1,
        const float* __restrict__ pSrc2,
        SINT numFrames) {
    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numFrames; ++i) {
        pDest[2 * i] = pSrc1[i];
        pDest[2 * i + 1] = pSrc2[i];
    }
}

// static
void SampleUtil::deinterleaveBuffer(float* __restrict__ pDest1,
        float* __restrict__ pDest2,
        const float* __restrict__ pSrc,
        SINT numFrames) {
    // note: LOOP VECTORIZED.
    for (SINT i = 0; i < numFrames; ++i) {
        pDest1[i] = pSrc[i * 2];
        pDest2[i] = pSrc[i * 2 + 1];
    }
}

// static
void SampleUtil::linearCrossfadeBuffersOut(
        float* __restrict__ pDestSrcFadeOut,
        const float* __restrict__ pSrcFadeIn,
        SINT numSamples) {
    // __restrict__ unoptimizes the function for some reason.
    const float cross_inc = 1.0f
            / float(numSamples / 2);
    // note: LOOP VECTORIZED only with "int i" (not SINT i)
    for (int i = 0; i < numSamples / 2; ++i) {
        const float cross_mix = cross_inc * i;
        pDestSrcFadeOut[i * 2] *= (1.0f - cross_mix);
        pDestSrcFadeOut[i * 2] += pSrcFadeIn[i * 2] * cross_mix;
    }
    // note: LOOP VECTORIZED only with "int i" (not SINT i)
    for (int i = 0; i < numSamples / 2; ++i) {
        const float cross_mix = cross_inc * i;
        pDestSrcFadeOut[i * 2 + 1] *= (1.0f - cross_mix);
        pDestSrcFadeOut[i * 2 + 1] += pSrcFadeIn[i * 2 + 1] * cross_mix;
    }
}

// static
void SampleUtil::linearCrossfadeBuffersIn(
        float* __restrict__ pDestSrcFadeIn,
        const float* __restrict__ pSrcFadeOut,
        SINT numSamples) {
    // __restrict__ unoptimizes the function for some reason.
    const float cross_inc = 1.0f / float(numSamples / 2);
    /// note: LOOP VECTORIZED only with "int i" (not SINT i)
    for (int i = 0; i < numSamples / 2; ++i) {
        const float cross_mix = cross_inc * i;
        pDestSrcFadeIn[i * 2] *= cross_mix;
        pDestSrcFadeIn[i * 2] += pSrcFadeOut[i * 2] * (1.0f - cross_mix);
    }
    // note: LOOP VECTORIZED only with "int i" (not SINT i)
    for (int i = 0; i < numSamples / 2; ++i) {
        const float cross_mix = cross_inc * i;
        pDestSrcFadeIn[i * 2 + 1] *= cross_mix;
        pDestSrcFadeIn[i * 2 + 1] += pSrcFadeOut[i * 2 + 1] * (1.0f - cross_mix);
    }
}

// static
void SampleUtil::mixStereoToMono(float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        SINT numSamples) {
    const float mixScale = 1.0f
            / (1.0f + 1.0f);
    // note: LOOP VECTORIZED
    for (SINT i = 0; i < numSamples / 2; ++i) {
        pDest[i * 2] = (pSrc[i * 2] + pSrc[i * 2 + 1]) * mixScale;
        pDest[i * 2 + 1] = pDest[i * 2];
    }
}

// static
void SampleUtil::mixStereoToMono(float* pBuffer, SINT numSamples) {
    const float mixScale = 1.0f / (1.0f + 1.0f);
    // note: LOOP VECTORIZED
    for (SINT i = 0; i < numSamples / 2; ++i) {
        pBuffer[i * 2] = (pBuffer[i * 2] + pBuffer[i * 2 + 1]) * mixScale;
        pBuffer[i * 2 + 1] = pBuffer[i * 2];
    }
}

// static
void SampleUtil::doubleMonoToDualMono(float* pBuffer, SINT numFrames) {
    // backward loop
    SINT i = numFrames;
    // not vectorized: vector version will never be profitable.
    while (0 < i--) {
        const float s = pBuffer[i];
        pBuffer[i * 2] = s;
        pBuffer[i * 2 + 1] = s;
    }
}

// static
void SampleUtil::copyMonoToDualMono(float* __restrict__ pDest,
        const float* __restrict__ pSrc, SINT numFrames) {
    // forward loop
    // note: LOOP VECTORIZED
    for (SINT i = 0; i < numFrames; ++i) {
        const float s = pSrc[i];
        pDest[i * 2] = s;
        pDest[i * 2 + 1] = s;
    }
}

// static
void SampleUtil::addMonoToStereo(float* __restrict__ pDest,
        const float* __restrict__ pSrc, SINT numFrames) {
    // forward loop
    // note: LOOP VECTORIZED
    for (SINT i = 0; i < numFrames; ++i) {
        const float s = pSrc[i];
        pDest[i * 2] += s;
        pDest[i * 2 + 1] += s;
    }
}

// static
void SampleUtil::stripMultiToStereo(
        float* pBuffer,
        SINT numFrames,
        int numChannels) {
    DEBUG_ASSERT(numChannels > 2);
    // forward loop
    for (SINT i = 0; i < numFrames; ++i) {
        pBuffer[i * 2] = pBuffer[i * numChannels];
        pBuffer[i * 2 + 1] = pBuffer[i * numChannels + 1];
    }
}

// static
void SampleUtil::copyMultiToStereo(
        float* __restrict__ pDest,
        const float* __restrict__ pSrc,
        SINT numFrames,
        int numChannels) {
    DEBUG_ASSERT(numChannels > 2);
    // forward loop
    for (SINT i = 0; i < numFrames; ++i) {
        pDest[i * 2] = pSrc[i * numChannels];
        pDest[i * 2 + 1] = pSrc[i * numChannels + 1];
    }
}


// static
void SampleUtil::reverse(float* pBuffer, SINT numSamples) {
    for (SINT j = 0; j < numSamples / 4; ++j) {
        const SINT endpos = (numSamples - 1) - j * 2 ;
        float temp1 = pBuffer[j * 2];
        float temp2 = pBuffer[j * 2 + 1];
        pBuffer[j * 2] = pBuffer[endpos - 1];
        pBuffer[j * 2 + 1] = pBuffer[endpos];
        pBuffer[endpos - 1] = temp1;
        pBuffer[endpos] = temp2;
    }
}

// static
void SampleUtil::copyReverse(float* __restrict__ pDest,
        const float* __restrict__ pSrc, SINT numSamples) {
    for (SINT j = 0; j < numSamples / 2; ++j) {
        const int endpos = (numSamples - 1) - j * 2;
        pDest[j * 2] = pSrc[endpos - 1];
        pDest[j * 2 + 1] = pSrc[endpos];
    }
}
