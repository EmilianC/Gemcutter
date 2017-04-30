# Text Resources
Before rendering Text, you must generate and load a .font file containing the pre-rendered characters.
By default, any .ttf can be converted to a .font using the AssetManager.

```cpp
auto font = Load<Font>("Fonts/Arial.font");
entity->Add<Text>("Hello World!", font);
```