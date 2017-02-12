# Text Resources
Before rendering Text, you must generate and load a .font file containing the pre-rendered characters.
Any .ttf can be converted to a .font using the FontEncoder.

Using the command line, ```"FontEncoder.exe Arial.ttf -w 64 -h 64 Arial.font"``` will generate a .font binary containing 64x64 resolution characters.
The resulting file can then be loaded and used with the Text component to render text.

```cpp
auto font = Load<Font>("Fonts/Arial.font");
entity->Add<Text>("Hello World!", font);
```