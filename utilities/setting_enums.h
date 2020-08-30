#pragma once

namespace one_bit
{
  enum class UiMode : uint32_t
  {
    NONE = 0,
#ifdef USE_QT5
    QT = 1,
#endif
#ifdef USE_CEGUI
    CEGUI = 2,
#endif
  };

  enum class CropRegion : uint32_t
  {
    TOP_LEFT = 1,
    TOP,
    TOP_RIGHT,
    LEFT,
    CENTER,
    RIGHT,
    BOTTOM_LEFT,
    BOTTOM, 
    BOTTOM_RIGHT,
  };
}