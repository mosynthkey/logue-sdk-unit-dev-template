# logue SDK Unit Template

[logue SDK](https://github.com/korginc/logue-sdk) の **単体 Unit** を作るためのテンプレートです。

このリポジトリを Fork し、Cursor などに作りたい音を伝えると、GitHub Actions が

- 実機用 Unit（NTS-1 mkII / NTS-3）
- ブラウザ用シミュレータ

をビルドします。`main` にマージすると GitHub Pages で試せて、NTS-1 mkII と NTS-3 には Web MIDI で直接送れます。

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

4. 作りたい Unit を伝える（例: 「TB-303 風のベース」「パッドで動くディレイ」）
5. 変更を `main` にマージする（または `main` に push する）
6. Actions が通ったら Pages の URL を開く
7. シミュレータで試し、USB 接続した NTS-1 mkII / NTS-3 に Send する

Chrome または Edge が必要です（Web MIDI SysEx）。

## 構成

```text
plugin.json                 # 名前・説明・パラメータ
dsp/                        # 共通 DSP
targets/nts-1_mkii/         # NTS-1 mkII（osc）
targets/nts-3_kaoss/        # NTS-3（genericfx）
web/index.html              # シミュレータへのリンクと実機送信
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

`git submodule update --recursive` は使わないでください（emsdk が巨大です）。

## 実機

| 機種 | ファイル | モジュール | 送り方 |
| --- | --- | --- | --- |
| NTS-1 mkII | `.nts1mkiiunit` | osc（変更可） | Pages の Send、Chrome / Edge |
| NTS-3 | `.nts3unit` | genericfx のみ | Pages の Send、Chrome / Edge |

## Developer ID

`common/dev_id.h` の `UNIT_DEV_ID` はテンプレート用の仮 ID（`lutd`）です。公開するなら [developer_ids.md](https://github.com/korginc/logue-sdk/blob/master/developer_ids.md) で自分の FourCC を登録してください。

## License

BSD 3-Clause.
