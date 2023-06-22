#pragma once

#include <span>
#include <cstdint>
#include <vector>
#include <ttf/ttf.h>

// We pretend to render at 72pt always.
struct Vm {
  enum TtfInstructions {
    SetFreeAndProjToCoordAxis = 0x00, // -01
    SetProjToCoordAxis = 0x02, // -03
    SetFreeToCoordAxis = 0x04, // -05
    SetProjToLine = 0x06, // -07
    SetFreeToLine = 0x08, // -09
    SetProjFromStack = 0x0A,
    SetFreeFromStack = 0x0B,
    GetProj = 0x0C,
    GetFree = 0x0D,
    SetFreeToProj = 0x0E,
    MovePointToIntersection = 0x0F,
    SetRefN = 0x10, // -12
    SetZoneN = 0x13, // -15
    SetAllZone = 0x16,
    SetLoop = 0x17,
    RoundGrid = 0x18,
    RoundHalfGrid = 0x19,
    SetMinDistance = 0x1A,
    Else = 0x1B,
    Jump = 0x1C,
    SetCVTCutIn = 0x1D,
    SetSingleWidthCutIn = 0x1E,
    SetSingleWidth = 0x1F,
    DuplicateStackTop = 0x20,
    PopStack = 0x21,
    ClearStack = 0x22,
    SwapTopTwo = 0x23,
    GetStackSize = 0x24,
    CopyIndexedElementToStack = 0x25,
    MoveIndexedToTopOfStack = 0x26,
    AlignPoints = 0x27,
    UntouchPoint = 0x29,
    LoopAndCall = 0x2A,
    CallFunc = 0x2B,
    FuncDef = 0x2C,
    FuncEnd = 0x2D,
    MoveDirectAbsPoint = 0x2E, // -2F
    InterpolateUntouchedPointsThroughOutline = 0x30, // -31
    ShiftPointByLastPoint = 0x32, // -33
    ShiftContourByLastPoint = 0x34, // -35
    ShiftZoneByLastPoint = 0x36, // -37
    ShiftPointByPixel = 0x38,
    InterpolatePointByLastRelativeStretch = 0x39,
    MoveStackIndirectRelativePoint = 0x3A, // -3B
    AlignRelativePoint = 0x3C,
    RoundDoubleGrid = 0x3D,
    MoveIndirectAbsPoint = 0x3E, // -3F
    PushNBytes = 0x40,
    PushNWords = 0x41,
    WriteStore = 0x42,
    ReadStore = 0x43,
    WriteCVTInPixel = 0x44,
    ReadCVT = 0x45,
    GetCoordinateProjectedOnProj = 0x46, // -47
    SetCoordFromStackWithProjAndFree = 0x48,
    MeasureDistance = 0x49, // -4A
    MeasurePixelsPerEm = 0x4B,
    MeasurePointSize = 0x4C,
    SetAutoFlipOn = 0x4D,
    SetAutoFlipOff = 0x4E,
    DebugCall = 0x4F,
    LessThan = 0x50,
    LessThanOrEqual = 0x51,
    GreaterThan = 0x52,
    GreaterThanOrEqual = 0x53,
    Equal = 0x54,
    NotEqual = 0x55,
    Odd = 0x56,
    Even = 0x57,
    IfTest = 0x58,
    EndIf = 0x59,
    LogicalAnd = 0x5A,
    LogicalOr = 0x5B,
    LogicalNot = 0x5C,
    DeltaExceptionP1 = 0x5D,
    SetDeltaBase = 0x5E,
    SetDeltaShift = 0x5F,
    Add = 0x60,
    Subtract = 0x61,
    Divide = 0x62,
    Multiply = 0x63,
    Absolute = 0x64,
    Negate = 0x65,
    Floor = 0x66,
    Ceiling = 0x67,
    Round = 0x68, // -6B
    NoRound = 0x6C, // -6F
    WriteCVTInFUnits = 0x70,
    DeltaExceptionP2 = 0x71,
    DeltaExceptionP3 = 0x72,
    DeltaExceptionC1 = 0x73,
    DeltaExceptionC2 = 0x74,
    DeltaExceptionC3 = 0x75,
    RoundSuper = 0x76,
    RoundSuper45 = 0x77,
    JumpRelative = 0x78,
    JumpRelativeInv = 0x79,
    RoundOff = 0x7A,
    RoundUpToGrid = 0x7C,
    RoundDownToGrid = 0x7D,
    SetAngleWeight = 0x7E,
    AdjustAngle = 0x7F,
    FlipPoint = 0x80,
    FlipRangeOn = 0x81,
    FlipRangeOff = 0x82,
    ScanConversionControl = 0x85,
    SetDualProjToLine = 0x86, // -87
    GetInfo = 0x88,
    InstrDef = 0x89,
    RollTopThreeStackElements = 0x8A,
    Maximum = 0x8B,
    Minimum = 0x8C,
    ScanType = 0x8D,
    InstrControl = 0x8E,
    GetVariation = 0x91,
    PushBytes = 0xB0, // -B7
    PushWords = 0xB8, // -BF
    MoveDirectRelativePoint = 0xC0, // -DF
    MoveIndirectRelativePoint = 0xE0, // -FF
  };
  enum class RoundingState {
    UpToGrid = 4,
    Off = 5,
    Super = 6,
    Super45 = 7,
  };
  Vm(uint16_t units_per_em, std::span<const uint8_t> maxp, std::span<const uint8_t> prep, std::span<const uint8_t> fpgm, std::span<const uint8_t> cvtt);
  void SkipAcrossIf();
  
  std::vector<const uint8_t*> callstack;
  std::vector<uint32_t> stack;
  std::vector<uint32_t> store;
  std::vector<uint32_t> cvt;
  std::vector<const uint8_t*> functions;

  const uint8_t* curInstr = 0;

  RoundingState roundState = RoundingState::Super;
  vec2 proj = vec2(1, 0);
  vec2 free = vec2(1, 0);
  vec2 dual_proj;
  uint32_t refpoint[3] = {0, 0, 0};
  uint8_t zonepoint[3] = {1, 1, 1};
  int loop = 1;
  int delta_base = 9;
  int delta_shift = 3;
  float minimum_distance = 1;
  bool GridFitInhibited = false;
  bool IgnoreCvtProgram = false;
  float cut_in = 16;
  float single_width_cut_in = 0;
  float single_width_value = 0;
  bool auto_flip = false;
  uint16_t units_per_em;
  uint32_t superRound = 0x48;

  float from_214(uint32_t val);
  uint32_t to_214(float f);
  uint32_t FUnitsToPixels(uint32_t v);
  void runProgram(std::span<const uint8_t> instructions);
  size_t nextInstructionSize();
  void runOneInstruction();
};

