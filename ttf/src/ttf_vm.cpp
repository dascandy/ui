#include "ttf_vm.h"

Vm::Vm(uint16_t units_per_em, std::span<const uint8_t> maxp, std::span<const uint8_t> prep, std::span<const uint8_t> fpgm, std::span<const uint8_t> cvtt)
: units_per_em(units_per_em)
{
  Bini::reader maxpr(maxp);
  maxpr.skip(18);
  uint16_t storageSize = maxpr.read16be();
  uint16_t maxFunc = maxpr.read16be();
  uint16_t maxInstr = maxpr.read16be();
  store.resize(storageSize);
  functions.resize(maxFunc);
  (void)maxInstr;

  Bini::reader rcvt(cvtt);
  while (rcvt.sizeleft()) {
    cvt.push_back((int16_t)rcvt.read16be());
  }

  fprintf(stderr, "running fpgm\n");
  runProgram(fpgm);
  fprintf(stderr, "fpgm done, running prep\n");
  runProgram(prep);
  fprintf(stderr, "prep done\n");
}

float Vm::from_214(uint32_t val) {
  return ((int16_t)val) / 16384.0f;
}
uint32_t Vm::to_214(float f) {
  if (f < -2) f = -2.0f;
  if (f > 1.99995f) f = 1.99995f;
  int16_t v = int16_t(f * 16384 + 0.5f);
  return (uint16_t)v;
}
uint32_t Vm::FUnitsToPixels(uint32_t v) {
  return v * 72 / units_per_em;
}
uint32_t Vm::RoundValue(uint32_t val) {
  uint32_t period;
  uint32_t phase;
  uint32_t threshold;
  switch(superRound & 0xC0) {
    case 0x00:
      period = 32;
      break;
    case 0x40:
      period = 64;
      break;
    case 0x80:
      period = 128;
      break;
    default:
      throw std::runtime_error("Invalid rounding period in super rounding mode");
  }
  switch(superRound & 0x30) {
    case 0x00:
      phase = 0;
      break;
    case 0x10:
      phase = period / 4;
      break;
    case 0x20:
      phase = period / 2;
      break;
    case 0x30:
      phase = 48;
      break;
  }
  threshold = period / 8;
  switch(superRound & 0x0F) {
    case 0: threshold = period - 1; break;
    case 1: threshold *= -3; break;
    case 2: threshold *= -2; break;
    case 3: threshold *= -1; break;
    case 4: threshold = 0; break;
    case 5: threshold *= 1; break;
    case 6: threshold *= 2; break;
    case 7: threshold *= 3; break;
    case 8: threshold *= 4; break;
    case 9: threshold *= 5; break;
    case 10: threshold *= 6; break;
    case 11: threshold *= 7; break;
    case 12: threshold *= 8; break;
    case 13: threshold *= 9; break;
    case 14: threshold *= 10; break;
    case 15: threshold *= 11; break;
  }

  switch(roundState) {
  case Off:
    return val;
  case UpToGrid:
    return (val + 63) & 0xFFFFFFC0;
  case Super:
    
    return;
  case Super45:
    return;
  }
}

void Vm::runProgram(std::span<const uint8_t> instructions) {
  const uint8_t* end = instructions.data() + instructions.size();
  curInstr = instructions.data();
  while (curInstr != end) {
    runOneInstruction();
  }
}

void Vm::SkipAcrossIf() {
  ++curInstr;
  while (*curInstr != EndIf) {
    if (*curInstr == IfTest) {
      SkipAcrossIf();
    }
    ++curInstr;
  }
}

size_t Vm::nextInstructionSize() {
  switch(*curInstr) {
  case PushBytes:
  case PushWords:
    return 2;
  case PushBytes+1:
  case PushWords+1:
    return 3;
  case PushBytes+2:
  case PushWords+2:
    return 4;
  case PushBytes+3:
  case PushWords+3:
    return 5;
  case PushBytes+4:
  case PushWords+4:
    return 6;
  case PushBytes+5:
  case PushWords+5:
    return 7;
  case PushBytes+6:
  case PushWords+6:
    return 8;
  case PushBytes+7:
  case PushWords+7:
    return 9;
  case PushNBytes:
    return curInstr[1] + 2;
  case PushNWords:
    return curInstr[1] * 2 + 2;
  default:
    return 1;
  }
}

void Vm::runOneInstruction() {
  fprintf(stderr, "stack [");
  for (auto& e : stack) {
    fprintf(stderr, " %d, ", e);
  }
  fprintf(stderr, "]      callstack [ ");
  for (auto& e : callstack) {
    fprintf(stderr, " %p, ", (const void*)e);
  }
  fprintf(stderr, "]\n");
  switch(*curInstr) {
  case PushNBytes:
  {
    ++curInstr;
    uint8_t count = *curInstr++;
    fprintf(stderr, "%s %d\n", "PushNBytes", count);
    for (size_t n = 0; n < count; n++) {
      stack.push_back(*curInstr++);
    }
    curInstr--;
  }
    break;
  case PushNWords:
  {
    ++curInstr;
    uint8_t count = *curInstr++;
    fprintf(stderr, "%s %d\n", "PushNWords", count);
    for (size_t n = 0; n < count; n++) {
      uint16_t val = (curInstr[0] * 256 + curInstr[1]);
      curInstr += 2;
      stack.push_back(int16_t(val));
    }
    curInstr--;
  }
    break;

  case PushBytes:
  case PushBytes+1:
  case PushBytes+2:
  case PushBytes+3:
  case PushBytes+4:
  case PushBytes+5:
  case PushBytes+6:
  case PushBytes+7:
  {
    uint8_t count = *curInstr++ - PushBytes + 1;
    fprintf(stderr, "%s %d\n", "PushBytes", count);
    for (size_t n = 0; n < count; n++) {
      stack.push_back(*curInstr++);
    }
    curInstr--;
  }
    break;
  case PushWords:
  case PushWords+1:
  case PushWords+2:
  case PushWords+3:
  case PushWords+4:
  case PushWords+5:
  case PushWords+6:
  case PushWords+7:
  {
    uint8_t count = *curInstr++ - PushWords + 1;
    fprintf(stderr, "%s %d\n", "PushWords", count);
    for (size_t n = 0; n < count; n++) {
      uint16_t val = (curInstr[0] * 256 + curInstr[1]);
      curInstr += 2;
      stack.push_back(int16_t(val));
    }
    curInstr--;
  }
    break;

  case ReadStore:
  {
    fprintf(stderr, "%s\n", "ReadStore");
    uint32_t location = stack.back(); stack.pop_back();
    stack.push_back(store[location]);
  }
    break;
  case WriteStore:
  {
    fprintf(stderr, "%s\n", "WriteStore");
    uint32_t value = stack.back(); stack.pop_back();
    uint32_t location = stack.back(); stack.pop_back();
    store[location] = value;
  }
    break;

  case WriteCVTInPixel:
  {
    fprintf(stderr, "%s\n", "WriteCVTInPixel");
    uint32_t value = stack.back(); stack.pop_back();
    uint32_t location = stack.back(); stack.pop_back();
    cvt[location] = value;
  }
    break;
  case WriteCVTInFUnits:
  {
    fprintf(stderr, "%s\n", "WriteCVTInFUnits");
    uint32_t value = stack.back(); stack.pop_back();
    uint32_t location = stack.back(); stack.pop_back();
    cvt[location] = FUnitsToPixels(value);
  }
    break;
  case ReadCVT:
  {
    fprintf(stderr, "%s\n", "ReadCVT");
    uint32_t location = stack.back(); stack.pop_back();
    stack.push_back(cvt[location]);
  }
    break;

  case SetFreeAndProjToCoordAxis:
    fprintf(stderr, "%s\n", "SetFreeAndProjToCoordAxis");
    free = proj = vec2(0, 1);
    break;
  case SetFreeAndProjToCoordAxis+1:
    fprintf(stderr, "%s\n", "SetFreeAndProjToCoordAxis+1");
    free = proj = vec2(1, 0);
    break;
  case SetProjToCoordAxis:
    fprintf(stderr, "%s\n", "SetProjToCoordAxis");
    proj = vec2(0, 1);
    break;
  case SetProjToCoordAxis+1:
    fprintf(stderr, "%s\n", "SetProjToCoordAxis+1");
    proj = vec2(1, 0);
    break;
  case SetFreeToCoordAxis:
    fprintf(stderr, "%s\n", "SetFreeToCoordAxis");
    free = vec2(0, 1);
    break;
  case SetFreeToCoordAxis+1:
    fprintf(stderr, "%s\n", "SetFreeToCoordAxis+1");
    free = vec2(1, 0);
    break;
  case SetProjToLine:
    fprintf(stderr, "%s\n", "SetProjToLine");
    std::abort();
  case SetProjToLine+1:
    fprintf(stderr, "%s\n", "SetProjToLine+1");
    std::abort();
  case SetFreeToLine:
    fprintf(stderr, "%s\n", "SetFreeToLine");
    std::abort();
  case SetFreeToLine+1:
    fprintf(stderr, "%s\n", "SetFreeToLine+1");
    std::abort();
  case SetFreeToProj:
    fprintf(stderr, "%s\n", "SetFreeToProj");
    free = proj;
    break;
  case SetDualProjToLine: // parallel
  case SetDualProjToLine+1: // perpendicular
    fprintf(stderr, "%s\n", "SetDualProjToLine");
    std::abort();
//      dual_proj = xxx;
//      proj = xxx;
    break;
  case SetProjFromStack:
  {
    fprintf(stderr, "%s\n", "SetProjFromStack");
    uint32_t y = stack.back();
    stack.pop_back();
    uint32_t x = stack.back();
    stack.pop_back();
    proj = vec2(from_214(x), from_214(y));
  }
    break;
  case SetFreeFromStack:
  {
    fprintf(stderr, "%s\n", "SetFreeFromStack");
    uint32_t y = stack.back();
    stack.pop_back();
    uint32_t x = stack.back();
    stack.pop_back();
    free = vec2(from_214(x), from_214(y));
  }
    break;
  case GetProj:
    fprintf(stderr, "%s\n", "GetProj");
    stack.push_back(to_214(proj.x));
    break;
  case GetFree:
    fprintf(stderr, "%s\n", "GetFree");
    stack.push_back(to_214(proj.y));
    break;

  case SetRefN:
    fprintf(stderr, "%s\n", "SetRef0");
    refpoint[0] = stack.back(); stack.pop_back();
    break;
  case SetRefN+1:
    fprintf(stderr, "%s\n", "SetRef1");
    refpoint[1] = stack.back(); stack.pop_back();
    break;
  case SetRefN+2:
    fprintf(stderr, "%s\n", "SetRef2");
    refpoint[2] = stack.back(); stack.pop_back();
    break;
  case SetZoneN:
    fprintf(stderr, "%s\n", "SetZone0");
    zonepoint[0] = stack.back(); stack.pop_back();
    break;
  case SetZoneN+1:
    fprintf(stderr, "%s\n", "SetZone1");
    zonepoint[1] = stack.back(); stack.pop_back();
    break;
  case SetZoneN+2:
    fprintf(stderr, "%s\n", "SetZone2");
    zonepoint[2] = stack.back(); stack.pop_back();
    break;
  case SetAllZone:
    fprintf(stderr, "%s\n", "SetAllZone");
    zonepoint[0] = zonepoint[1] = zonepoint[2] = stack.back(); stack.pop_back();
    break;

  case RoundHalfGrid:
    fprintf(stderr, "%s\n", "RoundHalfGrid");
    roundState = RoundingState::Super;
    superRound = 0x08;
    break;
  case RoundGrid:
    fprintf(stderr, "%s\n", "RoundGrid");
    roundState = RoundingState::Super;
    superRound = 0x48;
    break;
  case RoundDoubleGrid:
    fprintf(stderr, "%s\n", "RoundDoubleGrid");
    roundState = RoundingState::Super;
    superRound = 0x88;
    break;
  case RoundDownToGrid:
    fprintf(stderr, "%s\n", "RoundDownToGrid");
    roundState = RoundingState::Super;
    superRound = 0x40;
    break;
  case RoundUpToGrid:
    fprintf(stderr, "%s\n", "RoundUpToGrid");
    roundState = RoundingState::UpToGrid;
    break;
  case RoundOff:
    fprintf(stderr, "%s\n", "RoundOff");
    roundState = RoundingState::Off;
    break;

  case RoundSuper:
    fprintf(stderr, "%s\n", "RoundSuper");
    roundState = RoundingState::Super;
    superRound = stack.back(); stack.pop_back();
    break;
  case RoundSuper45:
    fprintf(stderr, "%s\n", "RoundSuper45");
    roundState = RoundingState::Super45;
    superRound = stack.back(); stack.pop_back();
    break;

  case SetLoop:
    loop = stack.back(); stack.pop_back();
    fprintf(stderr, "%s %d\n", "SetLoop", loop);
    break;

  case SetMinDistance:
    minimum_distance = stack.back(); stack.pop_back();
    fprintf(stderr, "%s\n", "SetMinDistance");
    break;

  case InstrControl:
  {
    uint32_t selector = stack.back(); stack.pop_back();
    uint32_t value = stack.back(); stack.pop_back();
    fprintf(stderr, "%s %x %x\n", "InstrControl", selector, value);
    switch(selector) {
    case 1:
      GridFitInhibited = (value == 1);
      break;
    case 2:
      IgnoreCvtProgram = (value == 1);
      break;
    case 3:
      // cleartype stuff. Ignore.
      break;
    default:
      std::abort();
    }
  }
    break;

  case ScanConversionControl: // funny, we don't do scan conversion. At all.
    fprintf(stderr, "%s\n", "ScanConversionControl");
    stack.pop_back();
    break;
  case ScanType:  // Nope.
    fprintf(stderr, "%s\n", "ScanType");
    stack.pop_back();
    break;

  case SetCVTCutIn:
    fprintf(stderr, "%s\n", "SetCVTCutIn");
    cut_in = stack.back(); stack.pop_back();
    break;
  case SetSingleWidthCutIn:
    fprintf(stderr, "%s\n", "SetSingleWidthCutIn");
    single_width_cut_in = stack.back(); stack.pop_back();
    break;
  case SetSingleWidth:
    fprintf(stderr, "%s\n", "SetSingleWidth");
    single_width_value = FUnitsToPixels(stack.back()); stack.pop_back();
    break;

  case SetAutoFlipOn:
    fprintf(stderr, "%s\n", "SetAutoFlipOn");
    auto_flip = true;
    break;
  case SetAutoFlipOff:
    fprintf(stderr, "%s\n", "SetAutoFlipOff");
    auto_flip = false;
    break;

  case SetAngleWeight: // obsolete instruction
    fprintf(stderr, "%s\n", "SetAngleWeight");
    stack.pop_back();
    break;
  case AdjustAngle: // obsolete instruction
    fprintf(stderr, "%s\n", "AdjustAngle");
    stack.pop_back();
    break;
    
  case SetDeltaBase:
    fprintf(stderr, "%s\n", "SetDeltaBase");
    delta_base = stack.back(); stack.pop_back();
    break;
  case SetDeltaShift:
    fprintf(stderr, "%s\n", "SetDeltaShift");
    delta_shift = stack.back(); stack.pop_back();
    break;

  case GetCoordinateProjectedOnProj:
    fprintf(stderr, "%s\n", "GetCoordinateProjectedOnProj");
    // project coordinate onto something somehow, with current position of p
    std::abort();
  case GetCoordinateProjectedOnProj+1:
    fprintf(stderr, "%s\n", "GetCoordinateProjectedOnProj");
    // project coordinate onto something somehow, with original position of p
    std::abort();
    
  case SetCoordFromStackWithProjAndFree:
    fprintf(stderr, "%s\n", "SetCoordFromStackWithProjAndFree");
  {
    uint32_t value = stack.back(); stack.pop_back();
    uint32_t p = stack.back(); stack.pop_back();
    (void)p;
    (void)value;

    // Moves point p from its current position along the freedom_vector so that its component along the projection_vector becomes the value popped off the stack.
    std::abort();
  }
    break;

  case MeasureDistance: // in grid-fitted outline
    fprintf(stderr, "%s\n", "MeasureDistance");
  case MeasureDistance+1: // in original outline
    fprintf(stderr, "%s\n", "MeasureDistance");
    std::abort();
  case MeasurePixelsPerEm:
    fprintf(stderr, "%s\n", "MeasurePixelsPerEm");
    // This instruction pushes the number of pixels per em onto the stack. Pixels per em is a function of the resolution of the rendering device and the current point size and the current transformation matrix. This instruction looks at the projection_vector and returns the number of pixels per em in that direction.
    stack.push_back(0x42);
    break;
  case MeasurePointSize:
    fprintf(stderr, "%s\n", "MeasurePointSize");
    // Pushes the current point size onto the stack.
    // Measure point size can be used to obtain a value which serves as the basis for choosing whether to branch to an alternative path through the instruction stream. It makes it possible to treat point sizes below or above a certain threshold differently
    stack.push_back(72);
    break;

  case FlipPoint:
    fprintf(stderr, "%s\n", "FlipPoint");
  {
    uint32_t p = stack.back(); stack.pop_back();
    (void)p;
    std::abort();
  }
    break;
  case FlipRangeOn:
    fprintf(stderr, "%s\n", "FlipRangeOn");
  {
    uint32_t highp = stack.back(); stack.pop_back();
    uint32_t lowp = stack.back(); stack.pop_back();
    (void)highp;
    (void)lowp;
    std::abort();
  }
    break;
  case FlipRangeOff:
    fprintf(stderr, "%s\n", "FlipRangeOff");
  {
    uint32_t highp = stack.back(); stack.pop_back();
    uint32_t lowp = stack.back(); stack.pop_back();
    (void)highp;
    (void)lowp;
    std::abort();
  }
    break;

  // Shift point p by the same amount that the reference point has been shifted. Point p is shifted along the freedom_vector so that the distance between the new position of point p and the current position of point p is the same as the distance between the current position of the reference point and the original position of the reference point.
  // NOTE: Point p is shifted from its current position, not its original position. The distance that the reference point has shifted is measured between its current position and the original position.
  case ShiftPointByLastPoint:
  case ShiftPointByLastPoint+1:
  case ShiftContourByLastPoint:
  case ShiftContourByLastPoint+1:
  case ShiftZoneByLastPoint:
  case ShiftZoneByLastPoint+1:
  case ShiftPointByPixel:
    fprintf(stderr, "%s\n", "ShiftSomething");
    std::abort();
    break;

  case MoveStackIndirectRelativePoint:
  case MoveStackIndirectRelativePoint+1:
    fprintf(stderr, "%s\n", "MoveStackIndirectRelativePoint");
    std::abort();
  case MoveDirectAbsPoint:
  case MoveDirectAbsPoint+1:
    fprintf(stderr, "%s\n", "MoveDirectAbsPoint");
    std::abort();
  case MoveIndirectAbsPoint:
  case MoveIndirectAbsPoint+1:
    fprintf(stderr, "%s\n", "MoveIndirectAbsPoint");
    std::abort();

  case MoveDirectRelativePoint+0:
  case MoveDirectRelativePoint+1:
  case MoveDirectRelativePoint+2:
  case MoveDirectRelativePoint+3:
  case MoveDirectRelativePoint+4:
  case MoveDirectRelativePoint+5:
  case MoveDirectRelativePoint+6:
  case MoveDirectRelativePoint+7:
  case MoveDirectRelativePoint+8:
  case MoveDirectRelativePoint+9:
  case MoveDirectRelativePoint+10:
  case MoveDirectRelativePoint+11:
  case MoveDirectRelativePoint+12:
  case MoveDirectRelativePoint+13:
  case MoveDirectRelativePoint+14:
  case MoveDirectRelativePoint+15:
  case MoveDirectRelativePoint+16:
  case MoveDirectRelativePoint+17:
  case MoveDirectRelativePoint+18:
  case MoveDirectRelativePoint+19:
  case MoveDirectRelativePoint+20:
  case MoveDirectRelativePoint+21:
  case MoveDirectRelativePoint+22:
  case MoveDirectRelativePoint+23:
  case MoveDirectRelativePoint+24:
  case MoveDirectRelativePoint+25:
  case MoveDirectRelativePoint+26:
  case MoveDirectRelativePoint+27:
  case MoveDirectRelativePoint+28:
  case MoveDirectRelativePoint+29:
  case MoveDirectRelativePoint+30:
  case MoveDirectRelativePoint+31:
    fprintf(stderr, "%s\n", "MoveDirectRelativePoint");
    std::abort();
    break;

  case MoveIndirectRelativePoint+0:
  case MoveIndirectRelativePoint+1:
  case MoveIndirectRelativePoint+2:
  case MoveIndirectRelativePoint+3:
  case MoveIndirectRelativePoint+4:
  case MoveIndirectRelativePoint+5:
  case MoveIndirectRelativePoint+6:
  case MoveIndirectRelativePoint+7:
  case MoveIndirectRelativePoint+8:
  case MoveIndirectRelativePoint+9:
  case MoveIndirectRelativePoint+10:
  case MoveIndirectRelativePoint+11:
  case MoveIndirectRelativePoint+12:
  case MoveIndirectRelativePoint+13:
  case MoveIndirectRelativePoint+14:
  case MoveIndirectRelativePoint+15:
  case MoveIndirectRelativePoint+16:
  case MoveIndirectRelativePoint+17:
  case MoveIndirectRelativePoint+18:
  case MoveIndirectRelativePoint+19:
  case MoveIndirectRelativePoint+20:
  case MoveIndirectRelativePoint+21:
  case MoveIndirectRelativePoint+22:
  case MoveIndirectRelativePoint+23:
  case MoveIndirectRelativePoint+24:
  case MoveIndirectRelativePoint+25:
  case MoveIndirectRelativePoint+26:
  case MoveIndirectRelativePoint+27:
  case MoveIndirectRelativePoint+28:
  case MoveIndirectRelativePoint+29:
  case MoveIndirectRelativePoint+30:
  case MoveIndirectRelativePoint+31:
    fprintf(stderr, "%s\n", "MoveIndirectRelativePoint");
    std::abort();
    break;

  case AlignRelativePoint:
    fprintf(stderr, "%s\n", "AlignRelativePoint");
    std::abort();

  case MovePointToIntersection:
    fprintf(stderr, "%s\n", "MovePointToIntersection");
    std::abort();
  case AlignPoints:
    fprintf(stderr, "%s\n", "AlignPoints");
    std::abort();
  case InterpolatePointByLastRelativeStretch:
    fprintf(stderr, "%s\n", "InterpolatePointByLastRelativeStretch");
    std::abort();
  case UntouchPoint:
    fprintf(stderr, "%s\n", "UntouchPoint");
    std::abort();
  case InterpolateUntouchedPointsThroughOutline:
  case InterpolateUntouchedPointsThroughOutline+1:
    fprintf(stderr, "%s\n", "InterpolateUntouchedPointsThroughOutline");
    //TODO
    break;

  case DeltaExceptionP1:
    fprintf(stderr, "%s\n", "DeltaExceptionP1");
    std::abort();
  case DeltaExceptionP2:
    fprintf(stderr, "%s\n", "DeltaExceptionP2");
    std::abort();
  case DeltaExceptionP3:
    fprintf(stderr, "%s\n", "DeltaExceptionP3");
    std::abort();
  case DeltaExceptionC1:
    fprintf(stderr, "%s\n", "DeltaExceptionC1");
    std::abort();
  case DeltaExceptionC2:
    fprintf(stderr, "%s\n", "DeltaExceptionC2");
    std::abort();
  case DeltaExceptionC3:
    fprintf(stderr, "%s\n", "DeltaExceptionC3");
    std::abort();

  case DuplicateStackTop:
    fprintf(stderr, "%s\n", "DuplicateStackTop");
    stack.push_back(stack.back());
    break;
  case PopStack:
    fprintf(stderr, "%s\n", "PopStack");
    stack.pop_back();
    break;
  case ClearStack:
    fprintf(stderr, "%s\n", "ClearStack");
    stack.clear();
    break;
  case SwapTopTwo:
    fprintf(stderr, "%s\n", "SwapTopTwo");
    std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
    break;
  case GetStackSize:
    fprintf(stderr, "%s\n", "GetStackSize");
    stack.push_back(stack.size());
    break;
  case CopyIndexedElementToStack:
    fprintf(stderr, "%s\n", "CopyIndexedElementToStack");
    stack.back() = stack[stack.back()];
    break;
  case MoveIndexedToTopOfStack:
    fprintf(stderr, "%s\n", "MoveIndexedToTopOfStack");
  {
    size_t offset = stack.back();
    stack.pop_back();
    for (; offset < stack.size()-1; offset++) {
      std::swap(stack[offset], stack[offset+1]);
    }
  }
    break;
  case RollTopThreeStackElements:
    fprintf(stderr, "%s\n", "RollTopThreeStackElements");
  {
    uint32_t a = stack.back(); stack.pop_back();
    uint32_t b = stack.back(); stack.pop_back();
    uint32_t c = stack.back(); stack.pop_back();
    stack.push_back(b);
    stack.push_back(a);
    stack.push_back(c);
  }
    break;

  case IfTest:
    fprintf(stderr, "%s\n", "IfTest");
  {
    uint32_t v = stack.back(); stack.pop_back();
    if (v == 0) {
      ++curInstr;
      while (*curInstr != Else && *curInstr != EndIf) {
        if (*curInstr == IfTest) {
          SkipAcrossIf();
        }
        curInstr += nextInstructionSize();
      }
    }
  }
    break;
  case Else:
    fprintf(stderr, "%s\n", "Else");
    // when found in normal instruction flow
    while (*curInstr != EndIf) {
      if (*curInstr == IfTest) {
        SkipAcrossIf();
      }
      curInstr += nextInstructionSize();
    }
    break;
  case EndIf:
    fprintf(stderr, "%s\n", "EndIf");
    break;

  case JumpRelative:
    fprintf(stderr, "%s\n", "JumpRelative");
  {
    uint32_t e = stack.back(); stack.pop_back();
    if (e) {
      curInstr += (int32_t)stack.back();
      stack.pop_back();
    }
  }
    break;
  case Jump:
    fprintf(stderr, "%s\n", "Jump");
    curInstr += (int32_t)stack.back();
    stack.pop_back();
    break;
  case JumpRelativeInv:
    fprintf(stderr, "%s\n", "JumpRelativeInv");
  {
    uint32_t e = stack.back(); stack.pop_back();
    if (not e) {
      curInstr += (int32_t)stack.back();
      stack.pop_back();
    }
  }
    break;
  case LessThan:
    fprintf(stderr, "%s\n", "LessThan");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 < e2);
  }
    break;
  case LessThanOrEqual:
    fprintf(stderr, "%s\n", "LessThanOrEqual");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 <= e2);
  }
    break;
  case GreaterThan:
    fprintf(stderr, "%s\n", "GreaterThan");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 > e2);
  }
    break;
  case GreaterThanOrEqual:
    fprintf(stderr, "%s\n", "GreaterThanOrEqual");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 >= e2);
  }
    break;
  case Equal:
    fprintf(stderr, "%s\n", "Equal");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 == e2);
  }
    break;
  case NotEqual:
    fprintf(stderr, "%s\n", "NotEqual");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 != e2);
  }
    break;
  case Odd:
    fprintf(stderr, "%s\n", "Odd");
  {
    uint32_t a = stack.back(); stack.pop_back();
    stack.push_back(a); // TODO
  }
    break;
  case Even:
    fprintf(stderr, "%s\n", "Even");
  {
    uint32_t a = stack.back(); stack.pop_back();
    stack.push_back(a); // TODO
  }
    break;
  case LogicalAnd:
    fprintf(stderr, "%s\n", "LogicalAnd");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 and e2);
  }
    break;
  case LogicalOr:
    fprintf(stderr, "%s\n", "LogicalOr");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 or e2);
  }
    break;
  case LogicalNot:
    fprintf(stderr, "%s\n", "LogicalNot");
  {
    uint32_t e = stack.back(); stack.pop_back();
    stack.push_back(not e);
  }
    break;
  case Add:
    fprintf(stderr, "%s\n", "Add");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 + e2);
  }
    break;
  case Subtract:
    fprintf(stderr, "%s\n", "Subtract");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(e1 - e2);
  }
    break;
  case Divide:
    fprintf(stderr, "%s\n", "Divide");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(64 * uint64_t(e1) / e2);
  }
    break;
  case Multiply:
    fprintf(stderr, "%s\n", "Multiply");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(uint64_t(e1) * e2 / 64);
  }
    break;
  case Absolute:
    fprintf(stderr, "%s\n", "Absolute");
  {
    int32_t e = (int32_t)stack.back(); stack.pop_back();
    stack.push_back(std::abs(e));
  }
    break;
  case Negate:
    fprintf(stderr, "%s\n", "Negate");
  {
    uint32_t e = stack.back(); stack.pop_back();
    stack.push_back(-e);
  }
    break;
  case Floor:
    fprintf(stderr, "%s\n", "Floor");
  {
    uint32_t e = stack.back(); stack.pop_back();
    stack.push_back(e & 0xFFFFFFC0);
  }
    break;
  case Ceiling:
    fprintf(stderr, "%s\n", "Ceiling");
  {
    uint32_t e = stack.back(); stack.pop_back();
    if (e & 0x3F) e += 0x40;
    stack.push_back(e & 0xFFFFFFC0);
  }
    break;
  case Maximum:
    fprintf(stderr, "%s\n", "Maximum");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(std::max(e1, e2));
  }
    break;
  case Minimum:
    fprintf(stderr, "%s\n", "Minimum");
  {
    uint32_t e2 = stack.back(); stack.pop_back();
    uint32_t e1 = stack.back(); stack.pop_back();
    stack.push_back(std::min(e1, e2));
  }
    break;

  case Round:
  case Round+1:
  case Round+2:
    fprintf(stderr, "%s\n", "Round2");
  {
    uint32_t v = stack.back(); stack.pop_back();
    stack.push_back(RoundValue(v));
  }
    break;
  case Round+3:
    fprintf(stderr, "%s\n", "Round3 - invalid");
    std::abort();
  case NoRound:
  case NoRound+1:
  case NoRound+2:
    // We have no compensation, and no rounding, so do nothing.
    break;
  case NoRound+3:
    fprintf(stderr, "%s\n", "NoRound3 - invalid");
    std::abort();

  case FuncDef:
    fprintf(stderr, "%s\n", "FuncDef");
  {
    uint32_t n = stack.back(); stack.pop_back();
    functions[n] = curInstr;
    ++curInstr;
    while (*curInstr != FuncEnd) {
      curInstr += nextInstructionSize();
    }
  }
    break;
  case FuncEnd:
    fprintf(stderr, "%s\n", "FuncEnd");
    if (callstack.empty()) {
      std::abort();
    } else {
      curInstr = callstack.back();
      callstack.pop_back();
    }
    break;

  case InstrDef:
    fprintf(stderr, "%s\n", "InstrDef");
    std::abort();

  case CallFunc:
    fprintf(stderr, "%s\n", "CallFunc");
  {
    uint32_t n = stack.back(); stack.pop_back();
    callstack.push_back(curInstr);
    curInstr = functions[n];
  }
    break;
  case LoopAndCall:
    fprintf(stderr, "%s\n", "LoopAndCall");
  {
    // Using a tail recursion hack to do this
    uint32_t n = stack.back(); stack.pop_back();
    uint32_t count = stack.back(); stack.pop_back();
    if (count > 0) {
      callstack.push_back(curInstr);
      for (size_t x = 0; x < count-1; x++) {
        callstack.push_back(functions[n]);
      }
      curInstr = functions[n];
    }
  }
    break;

  case DebugCall:
    fprintf(stderr, "%s\n", "DebugCall");
    stack.pop_back();
    break;

  case GetInfo:
    fprintf(stderr, "%s\n", "GetInfo");
  {
    // Magic value that says "We're up to date, and we are not doing cleartype."
    uint32_t rv = 0x1f2a;
    uint32_t selector = stack.back(); stack.pop_back();
    uint32_t result = 0;
    if (selector & 1) result |= rv & 0xFF;
    if (selector & 2) result |= rv & 0x100;
    if (selector & 4) result |= rv & 0x200;
    if (selector & 8) result |= rv & 0x400;
    if (selector & 16) result |= rv & 0x800;
    if (selector & 32) result |= rv & 0x1000;
    if (selector & 64) result |= rv & 0x2000;
    if (selector & 128) result |= rv & 0x4000;
    if (selector & 256) result |= rv & 0x8000;
    if (selector & 512) result |= rv & 0x10000;
    if (selector & 1024) result |= rv & 0x20000;
    if (selector & 2048) result |= rv & 0x40000;
    if (selector & 4096) result |= rv & 0x80000;
    stack.push_back(result);
  }
    break;
  }
  ++curInstr;
}


