/**********************************************************************

  Audacity: A Digital Audio Editor

  Ruler.h

  Dominic Mazzoni

**********************************************************************/

#ifndef __AUDACITY_RULER__
#define __AUDACITY_RULER__

#include "wxPanelWrapper.h" // to inherit

#include <wx/colour.h> // member variable
#include <wx/pen.h> // member variable

class wxArrayString;
class wxDC;
class wxFont;

class NumberScale;
class TimeTrack;
class ZoomInfo;

class AUDACITY_DLL_API Ruler {
 public:

   enum RulerFormat {
      IntFormat,
      RealFormat,
      RealLogFormat,
      TimeFormat,
      LinearDBFormat,
   };

   //
   // Constructor / Destructor
   //

   Ruler();
   ~Ruler();

   //
   // Required Ruler Parameters
   //

   void SetBounds(int left, int top, int right, int bottom);

   // wxHORIZONTAL || wxVERTICAL
   void SetOrientation(int orient);

   // min is the value at (x, y)
   // max is the value at (x+width, y+height)
   // (at the center of the pixel, in both cases)
   void SetRange(double min, double max);

   // An overload needed for the special case of fisheye
   // min is the value at (x, y)
   // max is the value at (x+width, y+height)
   // hiddenMin, hiddenMax are the values that would be shown without the fisheye.
   // (at the center of the pixel, in both cases)
   void SetRange(double min, double max, double hiddenMin, double hiddenMax);

   //
   // Optional Ruler Parameters
   //

   // If twoTone is true, cause zero and positive numbers to appear black, negative in another color.
   void SetTwoTone(bool twoTone);

   // IntFormat, RealFormat, or TimeFormat
   void SetFormat(RulerFormat format);

   // Specify the name of the units (like "dB") if you
   // want numbers like "1.6" formatted as "1.6 dB".
   void SetUnits(const wxString &units);
   void SetDbMirrorValue( const double d ){ mDbMirrorValue = d ; };

   // Logarithmic
   void SetLog(bool log);

   // Minimum number of pixels between labels
   void SetSpacing(int spacing);

   // If this is true, the edges of the ruler will always
   // receive a label.  If not, the nearest round number is
   // labeled (which may or may not be the edge).
   void SetLabelEdges(bool labelEdges);

   // Makes a vertical ruler hug the left side (instead of right)
   // and a horizontal ruler hug the top (instead of bottom)
   void SetFlip(bool flip);

   // Set it to false if you don't want minor labels.
   void SetMinor(bool value);

   // Good defaults are provided, but you can override here
   void SetFonts(const wxFont &minorFont, const wxFont &majorFont, const wxFont &minorMinorFont);
   struct Fonts { wxFont *major, *minor, *minorMinor; };
   Fonts GetFonts() const
   { return { mMajorFont.get(), mMinorFont.get(), mMinorMinorFont.get() }; }

   // Copies *pScale if it is not NULL
   void SetNumberScale(const NumberScale *pScale);

   // The ruler will not draw text within this (pixel) range.
   // Use this if you have another graphic object obscuring part
   // of the ruler's area.  The values start and end are interpreted
   // relative to the Ruler's local coordinates.
   void OfflimitsPixels(int start, int end);

   //
   // Calculates and returns the maximum size required by the ruler
   //
   void GetMaxSize(wxCoord *width, wxCoord *height);


   // The following functions should allow a custom ruler setup:
   // autosize is a GREAT thing, but for some applications it's
   // useful the definition of a label array and label step by
   // the user.
   void SetCustomMode(bool value);
   // If this is the case, you should provide a wxString array of labels, start
   // label position, and labels step. The range eventually specified will be
   // ignored.
   void SetCustomMajorLabels(wxArrayString *label, size_t numLabel, int start, int step);
   void SetCustomMinorLabels(wxArrayString *label, size_t numLabel, int start, int step);

   void SetUseZoomInfo(int leftOffset, const ZoomInfo *zoomInfo);

   //
   // Drawing
   //

   // Note that it will not erase for you...
   void Draw(wxDC& dc);
   void Draw(wxDC& dc, const TimeTrack* timetrack);
   // If length <> 0, draws lines perpendiculars to ruler corresponding
   // to selected ticks (major, minor, or both), in an adjacent window.
   // You may need to use the offsets if you are using part of the dc for rulers, borders etc.
   void DrawGrid(wxDC& dc, int length, bool minor = true, bool major = true, int xOffset = 0, int yOffset = 0);

   // So we can have white ticks on black...
   void SetTickColour( const wxColour & colour)
   { mTickColour = colour; mPen.SetColour( colour );}

   // Force regeneration of labels at next draw time
   void Invalidate();

 private:
   void Update();
   void Update(const TimeTrack* timetrack);
   void FindTickSizes();
   void FindLinearTickSizes(double UPP);
   wxString LabelString(double d, bool major);

   void Tick(int pos, double d, bool major, bool minor);

   // Another tick generator for custom ruler case (noauto) .
   void TickCustom(int labelIdx, bool major, bool minor);

public:
   bool mbTicksOnly; // true => no line the length of the ruler
   bool mbTicksAtExtremes;
   wxRect mRect;

private:
   wxColour mTickColour;
   wxPen mPen;

   int          mMaxWidth, mMaxHeight;
   int          mLeft, mTop, mRight, mBottom, mLead;
   int          mLength;
   int          mLengthOld;
   wxDC        *mDC;

   std::unique_ptr<wxFont> mMinorFont, mMajorFont, mMinorMinorFont;
   bool         mUserFonts;

   double       mMin, mMax;
   double       mHiddenMin, mHiddenMax;

   double       mMajor;
   double       mMinor;

   int          mDigits;

   ArrayOf<int> mUserBits;
   ArrayOf<int> mBits;
   int          mUserBitLen;

   bool         mValid;

   class Label {
    public:
      double value;
      int pos;
      int lx, ly;
      wxString text;

      void Draw(wxDC &dc, bool twoTone, wxColour c) const;
   };

   int          mNumMajor;
   ArrayOf<Label> mMajorLabels;
   int          mNumMinor;
   ArrayOf<Label> mMinorLabels;
   int          mNumMinorMinor;
   ArrayOf<Label> mMinorMinorLabels;

   // Returns 'zero' label coordinate (for grid drawing)
   int FindZero(Label * label, int len);

   public:
   int GetZeroPosition();

   private:
   int          mOrientation;
   int          mSpacing;
   double       mDbMirrorValue;
   bool         mHasSetSpacing;
   bool         mLabelEdges;
   RulerFormat  mFormat;
   bool         mLog;
   bool         mFlip;
   bool         mCustom;
   bool         mbMinor;
   bool         mMajorGrid;      //  for grid drawing
   bool         mMinorGrid;      //         .
   int          mGridLineLength; //        end
   wxString     mUnits;
   bool         mTwoTone;
   const ZoomInfo *mUseZoomInfo;
   int          mLeftOffset;

   std::unique_ptr<NumberScale> mpNumberScale;
};

class AUDACITY_DLL_API RulerPanel final : public wxPanelWrapper {
   DECLARE_DYNAMIC_CLASS(RulerPanel)

 public:
   using Range = std::pair<double, double>;

   struct Options {
      bool log { false };
      bool flip { false };
      bool labelEdges { false };
      bool ticksAtExtremes { false };
      bool hasTickColour{ false };
      wxColour tickColour;

      Options() {}

      Options &Log( bool l )
      { log = l; return *this; }

      Options &Flip( bool f )
      { flip = f; return *this; }

      Options &LabelEdges( bool l )
      { labelEdges = l; return *this; }

      Options &TicksAtExtremes( bool t )
      { ticksAtExtremes = t; return *this; }

      Options &TickColour( const wxColour c )
      { tickColour = c; hasTickColour = true; return *this; }
   };

   RulerPanel(wxWindow* parent, wxWindowID id,
              wxOrientation orientation,
              const wxSize &bounds,
              const Range &range,
              Ruler::RulerFormat format,
              const wxString &units,
              const Options &options = {},
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);

   ~RulerPanel();

   void DoSetSize(int x, int y,
                  int width, int height,
                  int sizeFlags = wxSIZE_AUTO) override;

   void OnErase(wxEraseEvent &evt);
   void OnPaint(wxPaintEvent &evt);
   void OnSize(wxSizeEvent &evt);
   void SetTickColour( wxColour & c){ ruler.SetTickColour( c );}

   // We don't need or want to accept focus.
   bool AcceptsFocus() const override  { return false; }
   // So that wxPanel is not included in Tab traversal - see wxWidgets bug 15581
   bool AcceptsFocusFromKeyboard() const override { return false; }

 public:

   Ruler  ruler;

private:
    DECLARE_EVENT_TABLE()
};

#endif //define __AUDACITY_RULER__
