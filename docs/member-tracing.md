# メンバ変数の追跡：JUCE公式サンプルで設計の考え方を読む

対象: JUCE 8.0.10 の `examples/Plugins/AudioPluginDemo.h`
（初回ビルド後、`build/_deps/juce-src/examples/Plugins/` にあります。行番号はこの版のものです）

## 何のためにやるのか

クラスのメンバ変数を眺めるだけでは、なぜそれが置かれているのかは分かりません。
**「どの関数で準備され、どの関数で使われるか」** を追うと、置かれた理由が見えてきます。

オーディオプラグインの関数は、呼ばれるタイミングで大きく2つに分かれます。

- `prepareToPlay`：再生の直前に1回だけ呼ばれる。時間がかかってもよい。
- `processBlock`：音のかたまりごとに1秒間に何百回も呼ばれる。時間のかかる処理をすると音が途切れる。

そのため、各メンバを「`prepareToPlay` で何をされるか」「`processBlock` で何をされるか」で見比べると、
「重い準備は前もって、処理中は使うだけ」という設計が浮かび上がります。

## 追い方（3ステップ）

1. `private:`（このサンプルでは一部が `public:`）に並んでいるメンバを書き出す
2. メンバ名でファイル内を検索し、出てくる関数名をメモする
3. 「準備する場所」と「使う場所」がどこかで分類する

このサンプルの `processBlock`（255行目）は中身を `process()`（560行目）に任せていて、
`process()` がさらに `applyDelay()`・`applyGain()` を呼んでいます。
そのため、ここではこの3つもまとめて「`processBlock` 側」として扱います。

## 追跡結果

| メンバ | prepareToPlay 側（216行目〜） | processBlock 側 | ほかの場所 |
|---|---|---|---|
| `delayBufferFloat` / `delayBufferDouble` | `setSize (2, 12000)` で大きさを決める。続けて `reset()` で中身を0にする | `applyDelay` で読み書きするだけ | — |
| `delayPosition` | 触らない | `applyDelay` で「次にどこを読むか」を更新する | — |
| `synth` | `setCurrentPlaybackSampleRate` でサンプルレートを伝える | `renderNextBlock` で音を作らせる | コンストラクタ（`initialiseSynth`）で声部を作る |
| `keyboardState` | `reset()` で押鍵状態を消す | `processNextMidiBuffer` でMIDIを流す | 画面の鍵盤からも参照される |
| `state`（AudioProcessorValueTreeState） | 触らない | `getParameter("gain")` などで値を読むだけ | コンストラクタで作る。`get/setStateInformation` で保存と復元をする |
| `lastPosInfo`（SpinLockedPosInfo） | 触らない | `set()`：鍵が取れなければ書き込みをあきらめる | 画面のタイマーが `get()`：鍵が取れるまで待つ |
| `trackPropertiesLock` / `trackProperties` | 触らない | **触らない** | ホストから `updateTrackProperties` で書き込まれ、画面が読む。普通の鍵（CriticalSection）で待つ |

## 見えてくる4つの型

### 型1：入れ物は先に確保し、処理中は使うだけ
`delayBuffer` は `prepareToPlay` で大きさが決まり、`processBlock` 側では読み書きしかされません。
メモリ確保は時間が読めない処理なので、音声処理の外で済ませています。

→ このプロジェクトの `outputBuffer` と同じです（`prepareToPlay` で `resize`、`processBlock` では使うだけ）。

### 型2：重い部品は外で作り、prepareで条件を合わせ、processで使う
`synth` は、声部の生成をコンストラクタで済ませます。`prepareToPlay` ではサンプルレートを伝えるだけで、`processBlock` では音を作らせるだけです。

→ このプロジェクトの `model` と同じです。作るのは `loadModel()`（`get_dsp`）、条件を合わせるのは `Reset()`、使うのは `process()` です。
違いは、`model` がボタンを押したとき、つまり `prepareToPlay` の **後** にも作られる点です。
そのときにも `Reset()` に渡せるよう、`currentSampleRate` / `currentBlockSize` を覚えておく必要があります。

### 型3：音声処理をまたいで続きを覚える
`delayPosition` は、前のブロックをどこまで処理したかを次のブロックに引き継ぐための変数です。
`processBlock` が1回で終わらず何度も呼ばれるからこそ必要になります。

→ このプロジェクトでは、同じ役割をNAMモデルが内部で持っています（自分ではメンバとして書いていません）。

### 型4：スレッドをまたぐ受け渡しには鍵。ただし音声側は待たない
`lastPosInfo` の中には `SpinLock` があります。
音声処理側の `set()` は `ScopedTryLockType` を使い、鍵が取れなければ書き込みをあきらめます（330行目付近）。
画面側の `get()` は `ScopedLockType` を使い、鍵が取れるまで待ちます。

一方、`trackProperties` は `processBlock` から一切触られないので、待つタイプの普通の鍵（`CriticalSection`）で済ませています。
**音声処理が触るかどうかで、鍵の種類を使い分けている**ことが分かります。

→ このプロジェクトの `modelLock` と同じ形です。`processBlock` は `ScopedTryLockType` で「取れなければ素通し」、`loadModel` は `ScopedLockType` で待ちます。
データの向きだけが逆で、サンプルは「音声 → 画面」、このプロジェクトは「画面（ボタン）→ 音声」です。

## まとめ

このプロジェクトの `private:` に並んでいるメンバは、どれも公式サンプルのどれかの型に当てはまります。

| このプロジェクト | 型 | 公式サンプルで対応するもの |
|---|---|---|
| `outputBuffer` | 型1 | `delayBuffer` |
| `model` + `currentSampleRate` / `currentBlockSize` | 型2 | `synth` |
| （NAMモデルの内部状態） | 型3 | `delayPosition` |
| `modelLock` | 型4 | `lastPosInfo` の中の `SpinLock` |

別のプラグインのコードを読むときも、同じ3ステップで追えば、メンバがどの型に当たるかで置かれた理由を判断できます。
