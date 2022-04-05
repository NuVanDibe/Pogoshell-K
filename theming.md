# Unofficial Pogoshell v2.x Themes
There are many aspects to a Pogoshell theme. Along with providing the very foundation for the display of Pogoshell, there is a certain level of flexibility on just what is displayed and how. This is especially true in my unofficial fork, which includes some incompatibilities with earlier versions to extend the possible. This is an attempt to provide a complete guide on exactly what is necessary and possible with Pogoshell themes. So, please read through this if you're interested in creating your own Pogoshell theme.

## Manditory And Optional
The first thing to mention is that there are a list of elements that are absolutely manditory in any Pogoshell theme. There are also manditory files necessary for Pogoshell's various built-in plugins, but they're not listed here. An important thing to always remember is to include all files you need that aren't in the standard distribution.

- Manditory files
-- THEME/your_theme.theme
- Manditory named elements
-- "list" type LISTVIEW
-- "title" type TEXTBAR
-- "root" type TRICONTAINER
- Optional named elements
-- "status" type TEXTBAR
-- "mtitle" type TEXTBAR
-- "mflow" type TEXTFLOW
-- "msgbox" type TRICONTAINER
-- "dflow" type TEXTFLOW
-- "dtitle" type TEXTBAR
-- "dlgbox" type TRICONTAINER
-- "jpgviewer" type TYPEFACE
-- "textnormal" type TYPEFACE
-- "textbold" type TYPEFACE
-- "textemph" type TYPEFACE
-- "textbig" type TYPEFACE

## Elements
Each named element in a Pogoshell theme corresponds to a predefined component of the GUI. While most are optional, some optional elements predicate the need to include other optional components. Here is a run down of what each element requires and what function it performs.

##### Available Elements 

| NAME | Type | Description |
| ------ | ------ | ------ |
| root | TRICONTAINER | The root of each theme. This contains as manditory CHILD0 as "title" and CHILD1 as "list". CHILD2 can be optionally "status". |
| title | TEXTBAR | The main titlebar of Pogoshell. |
| list | LISTVIEW | The main list of Pogoshell. |
| status | TEXTBAR | The optional main status bar of Pogoshell. |
| msgbox | TRICONTAINER | The optional message box for displaying questions or helpful information. CHILD0 is optionally mtitle. CHILD1 is manditorily mflow. | 
| mtitle | TEXTBAR | The title bar of the optional message box. |
| mflow | TEXTFLOW | The text area for the optional message box. Note, including of msgbox makes this manditory. Also note, some attributes of mflow's TEXTFLOW are made into an automatically generated listview also associated with msgbox. |
| dlgbox | TRICONTAINER | The optional dialog box for displaying the Please wait... message. If this is excluded, list is used to display the Please wait... message instead. CHILD0 is optional dtitle. CHILD1 is manditorily dflow. |
| dtitle | TEXTBAR | The title bar for the optional dialog box. |
| dflow | TEXTFLOW | The text area for the optional dialog box. Notw, including of dlgbox makes this manditory. Also note, some attributes are ignored (specifically, COLOR2 and COLOR3). |
| jpgviewer | TYPEFACE | The typeface used for the zoom display in the [encrypted] jpeg viewer. If left unspecified, cnokia.font is used. |
| textnormal | TYPEFACE | The typeface used for textreader's normal text. If left unspecified, dungeon.font is used. |
textbold | TYPEFACE | The typeface used for textreader's bold text. If left unspecified, dungeon.font is used with the BOLD attribute set. |
| textemph | TYPEFACE | The typeface used for textreader's emphasis text. If left unspecified, dungeon.font is used. |
| textbig | TYPEFACE | The typeface used for textreader's big text. If left unspecified, dungeon12.font is used. |

## Types
There are several types that form the widget set for Pogoshell. Along with having specific properties, many have several configurable attributes that can greatly effect their appearance.

### TRICONTAINER
A vertically stacked container of up to three sub widgets.

##### TRICONTAINER Attributes
| Name | Function/Default Value |
| ---- | ---- |
| CHILD0 | The first sub widget. Defaults to empty. |
| CHILD1 | The second sub widget. Defaults to empty. |
| CHILD2 | The third sub widget. Defaults to empty. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |

### TEXTBAR
A horizontal bar of text. This will be horizontally truncated to fit the available space.

##### TEXTBAR Attributes
| Name | Function/Default Value |
| ---- | ---- |
| ALIGN | Specifies the text alignment (LEFT, CENTER, or RIGHT). Defaults to LEFT. |
| BACKDROP | An optional BACKDROP widget. If not specified, a dark gray color is used to fill the backdrop. |
| COLOR | Used to specify the text color. This is ignored if using a color font. The default color is Black. |
| FONT | Specifies the font to use to display text. By default no FONT is specified. (DEPRICATED) |
| MARGIN | Specifies the number of pixels buffer to allow on all sides. Defaults to 1. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |
| TEXT | Specifies the startup text. Defaults to blank. |
| TYPEFACE | Specifies the typeface to use to display text. By default no TYPEFACE is specified. |

### TEXTFLOW
A horizontal flow of text that will spill onto successive lines if not enough space is available. Note, some attributes (COLOR2 and COLOR3) are overloaded for the use of "mflow".

##### TEXTFLOW Attributes
| Name | Function/Default Value |
| ---- | ---- |
| ALIGN | Specifies the text alignment (LEFT, CENTER, or RIGHT). Defaults to LEFT. |
| BACKDROP | An optional BACKDROP widget. If not specified, a dark gray color is used to fill the backdrop. |
| COLOR0 | Used to specify the text color. This is ignored if using a color font. The default color is Black. |
| COLOR2 | Used for "mflow" to specify the list highlight bar text color. The default color is White. |
| COLOR3 | Used for "mflow" to specify the list highlight bar color. The default color is Blue. |
| FONT | Specifies the font to use to display text. By default no FONT is specified. (DEPRICATED) |
| MARGINDOWN | Used to specify the bottom margin. Defaults to 4. |
| MARGINLEFT | Used to specify the left margin. Defaults to 2. |
| MARGINRIGHT | Used to specify the right margin. Defaults to 2. |
| MARGINUP | Used to specify the top margin. Defaults to 4. |
| MARGINX | Used to specify the left/right margin. Defaults to 2. |
| MARGINY | Used to specify the top/bottom margin. Defaults to 4. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |
| TEXT | Specifies the startup text. Defaults to blank. |
| TYPEFACE | Specifies the typeface to use to display text. By default no TYPEFACE is specified. |

### LISTVIEW
Used to provide a list of selectable items. This and SCROLLBAR are the only types that don't ignore the alpha attribute of colors.

##### LISTVIEW Attributes
| Name | Function/Default Value |
| ---- | ---- |
| ALIGN0 | Used to specify the text alignment of column 1 (LEFT, CENTER, or RIGHT). Defaults to LEFT. |
| ALIGN1 | Used to specify the text alignment of column 2 (LEFT, CENTER, or RIGHT). If there is no column 2, this option is ignored. Defaults to LEFT. |
| ASSOCIATESTYLE | Specifies the coloring behavior related to file assocation and the file list. Options are PLAIN, COLORTEXT, COLORHIGHLIGHT, and COLORBOTH. PLAIN results in COLOR0 always being used. COLORTEXT results in a file association color being used when specified, otherwise defaulting to COLOR0, for normal text. COLORHIGHLIGHT results in a file association color being used when specified, otherwise defaulting to COLOR2, for highlighted text. COLORBOTH is a combination of COLORTEXT and COLORHIGHLIGHT. Defaults to COLORTEXT. |
| BACKDROP | An optional BACKDROP widget. If not specified, a dark gray color is used to fill the backdrop. |
| COLOR0 | Used to specify the non-highlighted text color. This is ignored if using a color font. ASSOCIATESTYLE might override this. The default color is Black. |
| COLOR2 | Used to specify the highlight bar text color. ASSOCIATESTYLE might override this. The default color is White. |
| COLOR3 | Used to specify the highlight bar color. This attributes alpha value isn't ignore. The default color is Blue. |
| COLWIDTH0 | Specifies in pixels the width of column 1. Defaults to 1. |
| COLWIDTH1 | Specifies in pixels the width of column 2. If there is no column 2, this option is ignored. Defaults to 1. |
| FONT | Specifies the font to use to display text. By default no FONT is specified. (DEPRICATED) |
| GRADIENTALIGN | Specify the location of the file assocation gradient bar (LEFT, CENTER, or RIGHT). LEFT or RIGHT draws to the left or right side of the screen. CENTER draws to whichever side the scrollbar would be on, towards the center; if no scrollbar is specified, the right side is used. Defaults to RIGHT. |
| GRADIENTSTYLE | Specifies the style of the file assocation gradient bar. Options are NONE, SIMPLE, or GRADIENT. NONE never shows the gradient bar. SIMPLE shows a simple segmented gradient bar. GRADIENT shows a blended gradient bar. Default is SIMPLE. |
| GRADIENTWIDTH | Specifies the width of the file assocation gradient bar. This width is subtracted equally from the left and right columns. Defaults to 2. |
| MARGINX | Used to specify the left/right margin. Defaults to 0. |
| MARGINY | Used to specify the top/bottom margin. Defaults to 0. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |
| SCROLLBAR | An optional SCROLLBAR widget. If no scrollbar is specified, none is drawn. |
| TRUNCATESTYLE | Specifies the truncation style to use to display text. Options are FULL and TRUNCATE. FULL displays the full text (up to the edge of the screen), regardless of whether it fits the column or not. TRUNCATE truncates to the column width set in COLWIDTH0/COLWIDTH1. Defaults to TRUNCATE. |
| TYPEFACE | Specifies the typeface to use to display text. By default no TYPEFACE is specified. |

### ICONSET
Optionally provides the icons to be used with Pogoshell.

##### ICONSET Attributes
| Name | Function/Default Value |
| ---- | ---- |
| BITMAP | Specifies the .bm file that provides the icons. Defaults to undefined. |
| HEIGHT | The height of each individual icon. Note, this is fixed for all icons. Defaults to undefined. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |

### BACKDROP
Used to provide the backdrop for many widgets. This is rather useful for skinning purposes.

##### BACKDROP Attributes
| Name | Function/Default Value |
| ---- | ---- |
| BITMAP | Specifies the .bm file to be used with BITMAP STYLE. If STYLE is not BITMAP, then this is ignored. Defaults to unspecified. |
| BORDER | Used to specify the border width. COLOR0 and COLOR1 are used to provide the colors. Default is 0. |
| COLOR0 | Used to provide the top and left bevel colors. Defaults to White. |
| COLOR1 | Used to provide the bottom and right bevel colors. Defaults to Black. |
| COLOR2 | Use depends on STYLE. With SOLID, this color is used as a fill color. With HRANGE and VRANGE, this is used for the left and top colors, respectively. With BITMAP, this color is ignored. Defaults to Gray. |
| COLOR3 | Use depends on STYLE. With SOLID and BITMAP, this color is ignored. With HRANGE and VRANGE, this is used for the right and bottom colors, respectively. Defaults to Gray. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |
| STYLE | Used to specify the backdrop stop. SOLID provides a solid fill for the backdrop. HRANGE provides a horizontal gradient from COLOR2 to COLOR3. VRANGE provides a vertical gradient from COLOR2 to COLOR3. BITMAP uses BITMAP to provide the backdrop. |

### SCROLLBAR
Provides the scrollbar for listview.

##### SCROLLBAR Attributes
| Name | Function/Default Value |
| ---- | ---- |
| ALIGN | Specifies which side the scrollbar is on (LEFT or RIGHT). Default is RIGHT. |
| BAR | BACKDROP widget that provides for the apperance of the central bar section of the scrollbar. If left unspecified, no bar is displayed. |
| BARSTYLE | Used to specify the style of the BAR (NONE, MINIMALLEFT, MINIMALRIGHT, MINIMALBOTH, PARTIAL, ALWAYS). With NONE, BAR is never shown. With MINIMALLEFT, MINIMALRIGHT, and MINIMALBOTH, BAR is only shown if there are more list items than can be displayed on the screen at one time. MINIMALLEFT shifts any freed space to column 0 of its containing list. MINIMALRIGHT shifts any freed space to column 1 of its containing list. MINIMALBOTH shifts any freed space both both column 0 and column 1 equal. With PARTIAL, BAR is only shown if there are more list items than can be displayed on the screen at one time; any freed space is not shifted. With ALWAYS, BAR is always shown. If no BAR exists, this option is ignored. Default is ALWAYS. |
| COLOR | Used to provide the color of the trough if TROUGH isn't specified. This attribute's alpha value isn't ignored. | Defaults to an additive dark gray.
| MARGINTROUGHDOWN | Used to specify the bottom margin for the trough. Default is 1. |
| MARGINTROUGHLEFT | Used to specify the left margin for the trough. Default is 1. |
| MARGINTROUGHRIGHT | Used to specify the right margin for the trough. Default is 1. |
| MARGINTROUGHUP | Used to specify the top margin for the trough. Default is 1. |
| MARGINTROUGHX | Used to specify the left/right margin for the trough. This is the space between the list and the scrollbar trough. Default is 1. |
| MARGINTROUGHY | Used to specify the top/bottom margin for the trough. Default is 1. |
| MARGINBARDOWN | Used to specify the bottom margin for the bar. Default is 1. |
| MARGINBARLEFT | Used to specify the left margin for the bar. Default is 1. |
| MARGINBARRIGHT | Used to specify the right margin for the bar. Default is 1. |
| MARGINBARUP | Used to specify the top margin for the bar. Default is 1. |
| MARGINBARX | Used to specify the left/right margin for the trough. This is the space between the trough and the scrollbar trough. Default is 1. |
| MARGINBARY | Used to specify the top/bottom margin for the trough. Default is 1. |
| NAME | Provides the name of an instance of this widget. Defaults to blank. |
| TROUGH | Optional BACKDROP widget to provide for the apperance of the scollbar trough. If left unspecified, COLOR is used. |
| TROUGHBOTTOMBUTTON | Optional BACKDROP widget to provide for a button to display at the bottom of the trough. This is useful for containing a down arrow graphic. This is unanimated. If something other than STYLE=BITMAP for the specified BACKDROP is used, the button width and height is WIDTH. If left unspecified, no trough bottom button is displayed. |
| TROUGHTOPBUTTON | Optional BACKDROP widget to provide for a button to display at the top of the trough. This is useful for containing an up arrow graphic. This is unanimated. If something other than STYLE=BITMAP for the specified BACKDROP is used, the button with width and height is WIDTH. If left unspecified, no trough top button is displayed. |
| TROUGHSTYLE | Used to specify the style of the trough (NONE, MINIMALLEFT, MINIMALRIGHT, MINIMALBOTH, PARTIAL, ALWAYS). With NONE, the trough is never shown. With MINIMALLEFT, MINIMALRIGHT, and MINIMALBOTH, the trough is only shown if there are more list items than can be displayed on the screen at one time. MINIMALLEFT shifts any freed space to column 0 of its containing list. MINIMALRIGHT shifts any freed space to column 1 of its containing list. MINIMALBOTH shifts any freed space both both column 0 and column 1 equal. With PARTIAL, the trough is only shown if there are more list items than can be displayed on the screen at one time; any freed space is not shifted. With ALWAYS, the trough is always shown. Default is ALWAYS. |
| WIDTH | Used to specify the width of the scrollbar. This is the width of the displayed scrollbar. The specificed trough margin may make the total space used greater. Default is 10. |

### TYPEFACE
Provides a further qualified character representation. In short, BITMAP : BACKDROP :: FONT : TYPEFACE. This will replace FONT in other widgets in future versions, probably.

##### TYPEFACE Attributes
| Name | Function/Default Value |
| ---- | ---- |
| COLOR0 | Specifies the dropshadow color. This only takes effect if the STYLE includes DROPSHADOW. If left unspecified, black is used. |
| COLOR1 | Specifies the outline color. This only takes effect if the STYLE includes OUTLINE. If left unspecified, white is used. |
| FONT | Specifies the font to use to display text. By default no FONT is specified. (DEPRICATED) |
| STYLE | Specifies addition addtributes/style of the font. Possible attributes include BOLD, DROPSHADOW, and OUTLINE. Attributes can be separated by the pipe character (|). By default, none of the three attributes are used. |

### Color Alphas
Both LISTVIEW and SCROLLBAR offer attributes that include support for alpha colors. Alpha colors occupy the space in other color formats which would normally contain the alpha channel. In Pogoshell themes, however, this channel is used as a means of providing a variety of color options. First, this is the format of the color option with alpha: $AARRGGBB; without alpha, it is: $RRGGBB. AA specifies the "alpha channel". One can also use ADD, NEG, ADD_SHIFT1..5, AVG_SHIFT1..5, or AVG with an $RRGGBB value (eg. ADD|$ff0000). RR, GG, and BB specify the red, green, and blue channels respectively. Here is a simple table on the "alpha channel" and its effects.

##### Alpha Effects
| Alpha Value | Effect |
| ---- | ---- |
| None or 0x00 to 0x7A | $RRGGBB is used directly. |
| ADD_SHIFT1 to ADD_SHIFT5 or 0x7B to 0x7F | $RRGGBB is added to the background. Before adding, the backdrop color is shifted downward (once for 0x7F, twice for 0x7E, ..., five times for 0x7B). 0x7B is equivalent to 0x00-0x7A. Each channel maxes out at 0xFF. |
| AVG_SHIFT1 - AVG_SHIFT5, 0x81 - 0x85 | $RRGGBB is averaged with the background. Before averaging, the background color is shifted downward (once for 0x81, twice for 0x82, ..., five times for 0x85). 0x85 is equivalent to halving the given $RRGGBB color. |
| AVG or 0x80 or 0x86 to 0xFD | $RRGGBB and the background are averaged. |
| NEG or 0xFE | $RRGGBB is subtracted from the background. Each channel minimals out at 0x00. |
| ADD or 0xFF | $RRGGBB is added to the background. Each channel maxes out at 0xFF. |

### Gradient Bar
The gradient bar is a line used to represent, in color, all files in the current folder. With properly defined file assocation colors, this can be used to at a vaguely know the contents of a folder. File assocation colors are specifed in the form $RRGGBB in the pogo.cfg file, after the saver option (if present) but before the description (if present). Specifying a zero width gradient bar causes the gradient bar to not be displayed.

## Syntax Tree
```
S ->	CompleteWidgets
CompleteWidgets ->	"" | CompleteWidgets""CompleteWidget
CompleteWidget ->	CompleteTRICONTAINER | CompleteTEXTBAR | CompleteTEXTFLOW | CompleteLISTVIEW | CompleteICONSET | CompleteBACKDROP | CompleteSCROLLBAR | CompleteTYPEFACE
CompleteTRICONTAINER ->	"TRICONTAINER "TRICONTAINERAttributes"NAME="ValidName" "TRICONTAINERAttributes"\n"
CompleteTEXTBAR ->	"TEXTBAR "TEXTBARAttributes"NAME="ValidName" "TEXTBARAttributes"\n"
CompleteTEXTFLOW ->	"TEXTFLOW "TEXTFLOWAttributes"NAME="ValidName" "TEXTFLOWAttributes"\n"
CompleteLISTVIEW ->	"LISTVIEW "LISTVIEWAttributes"NAME="ValidName" "LISTVIEWAttributes"\n"
CompleteICONSET ->	"ICONSET "ICONSETAttributes"NAME="ValidName" "ICONSETAttributes"\n"
CompleteBACKDROP ->	"BACKDROP "BACKDROPAttributes"NAME="ValidName" "BACKDROPAttributes"\n"
CompleteSCROLLBAR ->	"SCROLLBAR "SCROLLBARAttributes"NAME="ValidName" "SCROLLBARAttributes"\n"
CompleteTYPEFACE ->	"TYPEFACE "TYPEFACEAttributes"NAME="ValidName" "TYPEFACEAttributes"\n"
TRICONTAINERAttributes ->	"" | TRICONTAINERAttributes""TRICONTAINERAttribute
TEXTBARAttributes ->	"" | TEXTBARAttributes""TEXTBARAttribute
TEXTFLOWAttributes ->	"" | TEXTFLOWAttributes""TEXTFLOWAttribute
LISTVIEWAttributes ->	"" | LISTVIEWAttributes""LISTVIEWAttribute
ICONSETAttributes ->	"" | ICONSETAttributes""ICONSETAttribute
BACKDROPAttributes ->	"" | BACKDROPAttributes""BACKDROPAttribute
SCROLLBARAttributes ->	"" | SCROLLBARAttributes""SCROLLBARAttribute
TYPEFACEAttributes ->	"" | TYPEFACEAttributes""TYPEFACEAttribute
TRICONTAINERAttribute ->	"CHILD0="ValidName" " | "CHILD1="ValidName" " | "CHILD2="ValidName" "
TEXTBARAttribute ->	"ALIGN="ValidTextAlignment" " | "BACKDROP="ValidName" " | "COLOR="ValidColor" " | "FONT=<"ValidName"> " | "MARGIN="ValidMargin" " | "TEXT=\""NoDoubleQuoteText"\" " | "TYPEFACE="ValidName" "
TEXTFLOWAttribute ->	"ALIGN="ValidTextAlignment" " | "BACKDROP="ValidName" " | "COLOR0="ValidColor" " | "COLOR2="ValidColor" " | "COLOR3="ValidAlphaColor" " | "FONT=<"ValidName"> " | "MARGINDOWN="ValidMargin" " | "MARGINLEFT="ValidMargin" " | "MARGINRIGHT="ValidMargin" " | "MARGINUP="ValidMargin" " | "MARGINX="ValidMargin" " | "MARGINY="ValidMargin" " | "TEXT=\""NoDoubleQuoteText"\" " | "TYPEFACE="ValidName" "
LISTVIEWAttribute ->	"ALIGN0="ValidTextAlignment" " | "ALIGN1="ValidTextAlignment" " | "ASSOCIATESTYLE="ValidAssociateStyle" " | "BACKDROP="ValidName" " | "COLOR0="ValidColor" " | "COLOR2="ValidColor" " | "COLOR3="ValidAlphaColor" " | "COLWIDTH0="ValidWidth" " | "COLWIDTH1="ValidWidth" " | "FONT=<"ValidName"> " | "GRADIENTALIGN="ValidAlign" " | "GRADIENTSTYLE="ValidGradientStyle" " | "GRADIENTWIDTH="ValidWidth" " | "MARGINX="ValidMargin" " | "MARGINY="ValidMargin" " | "SCROLLBAR="ValidName" " | "TRUNCATESTYLE="ValidTruncateStyle" " | "TYPEFACE="ValidName" "
ICONSETAttribute ->	"BITMAP=<"ValidName"> " | "HEIGHT="ValidHeight" "
BACKDROPAttribute ->	"BITMAP=<"ValidName"> " | "BORDER="ValidBorder" " | "COLOR0="ValidColor" " | "COLOR1="ValidColor" " | "COLOR2="ValidColor" " | "COLOR3="ValidColor" " | "STYLE="ValidBackdropStyle" "
SCROLLBARAttribute ->	"ALIGN="ValidSideAlignment" " | "BAR="ValidName" " | "BARSTYLE="ValidBarStyle" " | "COLOR="ValidAlphaColor" " | "MARGINTROUGHDOWN="ValidMargin" " | "MARGINTROUGHLEFT="ValidMargin" " | "MARGINTROUGHRIGHT="ValidMargin" " | "MARGINTROUGHUP="ValidMargin" " | "MARGINTROUGHX="ValidMargin" " | "MARGINTROUGHY="ValidMargin" " | "MARGINBARDOWN="ValidMargin" " | "MARGINBARLEFT="ValidMargin" " | "MARGINBARRIGHT="ValidMargin" " | "MARGINBARUP="ValidMargin" " | "MARGINBARX="ValidMargin" " | "MARGINBARY="ValidMargin" " | "TROUGH="ValidName" " | "TROUGHBOTTOMBUTTON="ValidName" " | "TROUGHTOPBUTTON="ValidName" " | "TROUGHSTYLE="ValidTroughStyle" " | "WIDTH="ValidWidth" "
TYPEFACEAttribute ->	"COLOR0="ValidColor" " | "COLOR1="ValidColor" " | "FONT=<"ValidName"> " | "STYLE="ValidTypefaceStyle" "
ValidTextAlignment ->	"LEFT" | "RIGHT" | "CENTER"
ValidSideAlignment ->	"LEFT" | "RIGHT"
ValidGradientStyle ->	"NONE" | "SIMPLE" | "GRADIENT"
ValidTruncateStyle ->	"FULL" | "TRUNCATE"
ValidAssociateStyle ->	"PLAIN" | "COLORTEXT" | "COLORHIGHLIGHT" | "COLORBOTH"
ValidBackdropStyle ->	"SOLID" | "HRANGE" | "VRANGE" | "BITMAP"
ValidBarStyle ->	ValidScrollbarStyle
ValidTroughStyle ->	ValidScrollbarStyle
ValidScrollbarStyle ->	"NONE" | "MINIMALLEFT" | "MINIMALRIGHT" | "MINIMALBOTH" | "PARTIAL" | "ALWAYS"
ValidTypefaceStyle ->	FontAttribute | FontAttribute"|"ValidTypefaceStyle
FontAttribute ->	"BOLD" | "OUTLINE" | "DROPSHADOW"
ValidWidth ->	ValidNumber | ValidSymbol
ValidMargin ->	ValidNumber | ValidSymbol
ValidColor ->	"$"X""X""X""X""X""X | ValidSymbol
ValidAlphaColor ->	"$"X""X""X""X""X""X""X""X | ValidSymbol
ValidNumber ->	ValidDigits""ValidDigit
ValidDigits ->	"" | ValidDigits""ValidDigit
ValidDigit ->	\x30 | \x31 | \x32 | \x33 | \x34 | \x35 | \x36 | \x37 | \x38 | \x39
X ->	ValidDigit | AtoF | atof
AtoF ->	\x41 | \x42 | \x43 | \x44 | \x45 | \x46
atof ->	\x61 | \x62 | \x63 | \x64 | \x65 | \x66
ValidSymbol ->	ValidName
ValidName ->	NonDigit""ValidChars
ValidChars ->	"" | ValidChars""ValidChar
ValidChar ->	NoDoubleQuoteLessThanGreaterThanChar | DoubleQuote
NonDigit ->	NoDoubleQuoteLessThanGreaterThanDigitChar | DoubleQuote | LessThan | GreaterThan
NoDoubleQuoteText ->	"" | NoDoubleQuoteText""NoDoubleQuoteChar
NoDoubleQuoteChar ->	NoDoubleQuoteLessThanGreaterThanChar | LessThan | GreaterThan
NoDoubleQuoteLessThanGreaterThanCharChar ->	NoDoubleQuoteLessThanGreaterThanDigitChar | Digit
NoDoubleQuoteLessThanGreaterThanDigitChar ->	SubDoubleQuoteChar | SuperDoubleQuoteSubDigitChar | SuperDigitSubLessThanChar | SuperLessThanSubGreaterThanChar | SuperGreaterThanSubAtoFChar | AtoF | SuperAtoFSubatofChar | atof | SuperatofChar
SubDoubleQuoteChar ->	\x01 | \x02 | \x03 | \x04 | \x05 | \x06 | \x07 | \x08 | \x09 | \x0A | \x0B | \x0C | \x0D | \x0E | \x0F | \x10 | \x11 | \x12 | \x13 | \x14 | \x15 | \x16 | \x17 | \x18 | \x19 | \x20 | \x21
SuperDoubleQuoteSubDigitChar ->	\x23 | \x24 | \x25 | \x26 | \x27 | \x28 | \x29 | \x2A | \x2B | \x2C | \x2D | \x2E | \x2F
SuperDigitSubLessThanChar ->	\x3A | \x3B
SuperLessThanSubGreaterThanChar -Gt;	\x3D
SuperGreaterThanSubAtoFChar ->	\x3F | \x40
SuperAtoFSubatofChar ->	\x47 | \x48 | \x49 | \x4A | \x4B | \x4C | \x4D | \x4E | \x4F | \x50 | \x51 | \x52 | \x53 | \x54 | \x55 | \x56 | \x57 | \x58 | \x59 | \x5A | \x5B | \x5C | \x5D | \x5E | \x5F | \x60
SuperatofChar ->	\x67 | \x68 | \x69 | \x6A | \x6B | \x6C | \x6D | \x6E | \x6F | \x70 | \x71 | \x72 | \x73 | \x74 | \x75 | \x76 | \x77 | \x78 | \x79 | \x7A | \x7B | \x7C | \x7D | \x7E | \x7F
LessThan ->	\x3C
GreaterThan ->	\x3E
DoubleQuote ->	\x22
```
