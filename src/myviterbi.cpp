//
// File: myviterbi.cpp
//
// Code generated for Simulink model 'myviterbi'.
//
// Model version                  : 1.17
// Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
// C/C++ source code generated on : Mon Aug  7 12:01:00 2017
//
// Target selection: ert.tlc
// Embedded hardware selection: Intel->x86-64 (Linux 64)
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "myviterbi.h"

extern int32_T ACS_fxp_s32_s32(const int32_T numStates, int32_T pTempMetric[],
  const int32_T alpha, const int32_T pBMet[], int32_T pStateMet[], uint32_T
  pTbState[], uint32_T pTbInput[], const int32_T pTbPtr[], const uint32_T
  pNxtSt[], const uint32_T pEncOut[], const int32_T maxValue);
int32_T ACS_fxp_s32_s32(const int32_T numStates, int32_T pTempMetric[], const
  int32_T alpha, const int32_T pBMet[], int32_T pStateMet[], uint32_T pTbState[],
  uint32_T pTbInput[], const int32_T pTbPtr[], const uint32_T pNxtSt[], const
  uint32_T pEncOut[], const int32_T maxValue)
{
  int32_T minstate;
  int32_T stateIdx;
  int32_T renorm;
  int32_T inpIdx;
  int32_T nextStateIdx;
  int32_T tmpIdx;
  int32_T q0;
  int32_T qY;

  // S-Function (scomviterbi2): '<Root>/Viterbi Decoder'
  renorm = maxValue;
  for (stateIdx = 0; stateIdx < numStates; stateIdx++) {
    pTempMetric[stateIdx] = maxValue;
  }

  for (stateIdx = 0; stateIdx < numStates; stateIdx++) {
    q0 = pStateMet[stateIdx];
    for (inpIdx = 0; inpIdx < alpha; inpIdx++) {
      qY = inpIdx * numStates + stateIdx;
      nextStateIdx = (int32_T)pNxtSt[qY];
      qY = pBMet[pEncOut[qY]];
      if ((q0 < 0) && (qY < MIN_int32_T - q0)) {
        qY = MIN_int32_T;
      } else if ((q0 > 0) && (qY > MAX_int32_T - q0)) {
        qY = MAX_int32_T;
      } else {
        qY += q0;
      }

      if (qY < pTempMetric[nextStateIdx]) {
        tmpIdx = numStates * pTbPtr[0U] + nextStateIdx;
        pTbState[tmpIdx] = (uint32_T)stateIdx;
        pTbInput[tmpIdx] = (uint32_T)inpIdx;
        pTempMetric[nextStateIdx] = qY;
        if (pTempMetric[nextStateIdx] < renorm) {
          renorm = pTempMetric[nextStateIdx];
        }
      }
    }
  }

  // Update (and renormalize) state metrics, then find
  //  minimum metric state for start of traceback
  minstate = 0;
  for (stateIdx = 0; stateIdx < numStates; stateIdx++) {
    q0 = pTempMetric[stateIdx];
    if ((q0 >= 0) && (renorm < q0 - MAX_int32_T)) {
      qY = MAX_int32_T;
    } else if ((q0 < 0) && (renorm > q0 - MIN_int32_T)) {
      qY = MIN_int32_T;
    } else {
      qY = q0 - renorm;
    }

    pStateMet[stateIdx] = qY;
    if (qY == 0) {
      minstate = stateIdx;
    }
  }

  // End of S-Function (scomviterbi2): '<Root>/Viterbi Decoder'
  return minstate;
}

// Model step function
void myviterbiModelClass::step()
{
  int32_T i;
  int32_T j;
  int32_T doBM;
  uint32_T input;
  int32_T bitIdx;
  uint32_T intVal;
  uint8_T rtb_BittoIntegerConverter[3024];
  int32_T bMetric[4];
  int32_T tempMetric[64];
  int32_T bMetric_0;

  // S-Function (scomviterbi2): '<Root>/Viterbi Decoder' incorporates:
  //   Inport: '<Root>/In1'

  j = 0;
  for (i = 0; i < 48384; i++) {
    if (rtConstP.ViterbiDecoder_PuncVec[i % 4]) {
      rtDW.preDecodVect[i] = rtU.In1[j];
      j++;
    }
  }

  for (j = 0; j < 24192; j++) {
    bitIdx = (j << 1) + 1;

    // Branch Metric Computation
    for (i = 0; i < 4; i++) {
      bMetric_0 = 0;
      doBM = 0;
      if (rtDW.expEraPuncVect[bitIdx]) {
        doBM = 1;
      }

      if (doBM != 0) {
        doBM = rtDW.preDecodVect[bitIdx];
        if ((i & 1) != 0) {
          doBM = 1 - rtDW.preDecodVect[bitIdx];
        }

        bMetric_0 = doBM;
      }

      doBM = 0;
      if (rtDW.expEraPuncVect[bitIdx - 1]) {
        doBM = 1;
      }

      if (doBM != 0) {
        doBM = rtDW.preDecodVect[bitIdx - 1];
        if ((int32_T)((uint32_T)i >> 1) != 0) {
          doBM = 1 - rtDW.preDecodVect[bitIdx - 1];
        }

        bMetric_0 += doBM;
      }

      bMetric[i] = bMetric_0;
    }

    // State Metric Update
    intVal = (uint32_T)ACS_fxp_s32_s32(64, tempMetric, 2, bMetric,
      rtDW.stateMetric, rtDW.tbState, rtDW.tbInput, &rtDW.tbPtr,
      rtConstP.ViterbiDecoder_StateVec, rtConstP.ViterbiDecoder_OutputVec, 32767);

    // Traceback decoding
    bitIdx = rtDW.tbPtr;
    input = 0U;
    for (i = 0; i < 65; i++) {
      input = rtDW.tbInput[(bitIdx << 6) + intVal];
      intVal = rtDW.tbState[(bitIdx << 6) + intVal];
      if (bitIdx > 0) {
        bitIdx--;
      } else {
        bitIdx = 64;
      }
    }

    rtDW.ViterbiDecoder[j] = ((input & 1U) != 0U);

    // Increment (mod TbDepth) the traceback index and store
    if (rtDW.tbPtr < 64) {
      rtDW.tbPtr++;
    } else {
      rtDW.tbPtr = 0;
    }
  }

  // End of S-Function (scomviterbi2): '<Root>/Viterbi Decoder'

  // S-Function (scominttobit): '<Root>/Bit to Integer Converter'
  // Bit to Integer Conversion
  bitIdx = 0;
  for (i = 0; i < 3024; i++) {
    intVal = 0U;
    for (j = 0; j < 8; j++) {
      // Input bit order is MSB first
      intVal <<= 1U;
      intVal |= rtDW.ViterbiDecoder[bitIdx];
      bitIdx++;
    }

    rtb_BittoIntegerConverter[i] = (uint8_T)intVal;
  }

  // End of S-Function (scominttobit): '<Root>/Bit to Integer Converter'

  // Outport: '<Root>/Out1'
  memcpy(&rtY.Out1[0], &rtb_BittoIntegerConverter[0], 3024U * sizeof(uint8_T));
}

// Model initialize function
void myviterbiModelClass::initialize()
{
  {
    int32_T i;
    int32_T idx;

    // InitializeConditions for S-Function (scomviterbi2): '<Root>/Viterbi Decoder' 
    // Set state metric for all zeros state equal to zero and all other state metrics equal to max values 
    rtDW.stateMetric[0U] = 0;
    for (i = 0; i < 63; i++) {
      rtDW.stateMetric[i + 1] = 32767;
    }

    // Set traceback memory to zero
    // Fill in the expanded puncture pattern that will be used as
    //  the erasure vector while decoding.
    idx = 0;
    for (i = 0; i < 48384; i++) {
      rtDW.expEraPuncVect[i] = rtConstP.ViterbiDecoder_PuncVec[idx];
      idx++;
      if (idx >= 4) {
        idx = 0;
      }
    }

    // End of InitializeConditions for S-Function (scomviterbi2): '<Root>/Viterbi Decoder' 
  }
}

// Constructor
myviterbiModelClass::myviterbiModelClass()
{
}

// Destructor
myviterbiModelClass::~myviterbiModelClass()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL * myviterbiModelClass::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
