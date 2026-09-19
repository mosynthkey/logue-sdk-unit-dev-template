# logue SDK Unit Template

[logue SDK](https://github.com/korginc/logue-sdk) の **単体 Unit** を作るためのテンプレートです。

このリポジトリを Fork し、Cursor などに作りたい音を伝えると、GitHub Actions が

- 実機用 Unit（NTS-1 mkII / NTS-3 / microKORG2）
- ブラウザ用シミュレータ（NTS-1 mkII / NTS-3）

をビルドします。`main` にマージすると GitHub Pages で試せて、NTS-1 mkII と NTS-3 には Web MIDI で直接送れます。microKORG2 は USB マスストレージでコピーします。

スターターは単純なオシレータ **Sine** です。置き換えて使ってください。

KORG 非公式です。logue-sdk のライセンスは KORG の BSD 3-Clause のままです。

## 使い方

1. このリポジトリを Fork する
2. GitHub の **Settings → Pages → Source** を **GitHub Actions** にする
3. Fork を clone して Cursor で開く（`--recurse-submodules` は使わない）

```bash
git clone https://github.com/<you>/logue-sdk-unit-dev-template.git
cd logue-sdk-unit-dev-template
git submodule update --init
git -C third_party/logue-sdk submodule update --init platform/ext/CMSIS
```

4. 作りたい Unit を伝える（例: 「NTS-1 mkII で動く Supersaw」「NTS-3 で使える Delay」）
5. 変更を `main` にマージする（または `main` に push する）
6. Actions が通ったら Pages の URL を開く
7. シミュレータで試し、NTS-1 mkII / NTS-3 には Send、microKORG2 は `.mk2unit` をダウンロードしてコピーする

Chrome または Edge が必要です（Web MIDI SysEx）。

## 構成

```text
plugin.json                 # 名前・説明・パラメータ
dsp/                        # 共通 DSP
targets/nts-1_mkii/         # NTS-1 mkII（osc）
targets/nts-3_kaoss/        # NTS-3（genericfx）
targets/microkorg2/         # microKORG2（osc）
web/index.html              # ターゲット切替・ダウンロード・実機送信・シミュレータ
```

DSP は 1 つだけです。機種ごとの違いは `targets/` の薄いアダプタに置きます。

このテンプレートは **プラグイン 1 個専用** です。カタログサイトではありません。

## ローカルビルド

```bash
git submodule update --init
git -C third_party/logue-sdk submodule update --init platform/ext/CMSIS

make unit GCC_BIN_PATH=/path/to/gcc-arm-none-eabi-10.3-2021.10/bin
make wasm EMCC_BIN_PATH=/path/to/emsdk/upstream/emscripten
make site
python3 scripts/serve.py dist
```

microKORG2 をビルドするには drumlogue / microKORG2 用 A7 ツールチェーンが必要です。CI では自動取得します。

`git submodule update --recursive` は使わないでください（emsdk が巨大です）。

## 実機

| 機種 | ファイル | モジュール | 送り方 |
| --- | --- | --- | --- |
| NTS-1 mkII | `.nts1mkiiunit` | osc（変更可） | Pages の Send、Chrome / Edge |
| NTS-3 | `.nts3unit` | genericfx のみ | Pages の Send、Chrome / Edge |
| microKORG2 | `.mk2unit` | osc（変更可） | USB マスストレージ（下記） |

### microKORG2 の入れ方

1. `.mk2unit` をダウンロードする（mkII の `.nts1mkiiunit` ではない）
2. 電源オフ → **FUNCTION 1** を押しながら電源オン → USB マスストレージ
3. `Units/Oscs/` 配下の空スロットにコピーする（1 スロットに 1 ファイル）
4. 取り外して **FUNCTION 5**、OSC ページで選ぶ

Web SysEx 送信は microKORG2 では使えません。

## Developer ID

`common/dev_id.h` の `UNIT_DEV_ID` はテンプレート用の仮 ID（`lutd`）です。公開するなら [developer_ids.md](https://github.com/korginc/logue-sdk/blob/master/developer_ids.md) で自分の FourCC を登録してください。

## License

BSD 3-Clause.
