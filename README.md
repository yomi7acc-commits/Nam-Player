# NAM Player（学習用）

JUCEで作る、最小限のNAM（Neural Amp Modeler）プレイヤーです。
コードには `空欄①`〜`空欄⑦` があり、1つずつ埋めていくと完成します。

| ファイル | 役割 |
|---|---|
| `Source/PluginProcessor.h/.cpp` | 音の処理（空欄①〜⑥） |
| `Source/PluginEditor.h/.cpp` | 画面（空欄⑦） |
| `CMakeLists.txt` | ビルド設定（演習の対象外） |

## ビルド（空欄をすべて埋めたあと）

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target NamPlayer_Standalone
```

JUCEとNAM Coreは初回のcmake時に自動でダウンロードされます。
